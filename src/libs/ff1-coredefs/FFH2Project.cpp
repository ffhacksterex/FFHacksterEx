#include "stdafx.h"
#include "FFH2Project.h"
#include "DataValueAccessor.h"
#include "FFH2Project_IniToJson_Defs.hpp"
#include "GameSerializer.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "path_functions.h"
#include "AsmFiles.h"
#include "dva_primitives.h"
#include "dva_std_collections.h"
#include "string_functions.h"

#include <functional>
#include <fstream>
#include <iomanip>
#include <map>

using ffh::fda::DataValueAccessor;
using ffh::str::tomfc;
using ffh::str::tostd;

// === Structure members

FFHSetting& ProjectEditorModuleEntry::GetSetting(const std::string& name)
{
	auto it = settings.find(name);
	if (it == cend(settings))
		throw std::runtime_error("Module '" + this->slotName + "' does not contain a setting named " + name + ".");
	return it->second;
}


// === FFH2Project

FFH2Project::FFH2Project()
{
}

FFH2Project::~FFH2Project()
{
	DeleteStandardTiles();
}

//TODO - move these to a utility hpp, json_utility.h/cpp
namespace {
	#define REFDIR_APP "*APP*"
	#define REFDIR_CWD "*CWD*"
	#define REFDIR_PRJ "*PRJ*"

	static std::set<std::string> stringTypes = { "str", "hex", "addr", "rgb" };

	void ThrowOnFailure(const std::string& name, std::function<void()> func);
	
	template <typename jclass>
	jclass FieldToJson(const std::string & type, const std::string & field)
	{
		// Surround with dquotes if the field is:
		// 1) a string type and
		// 2) is either empty or doesn't starts with a dquote
		jclass jdata;
		if ((stringTypes.find(type) != cend(stringTypes)) && (field.empty() || field[0] != '"'))
			jdata = jclass::parse("\"" + field + "\"");
		else
			jdata = jclass::parse(field);
		return jdata;
	}

	template <typename jclass>
	std::string JsonToField(const jclass& jdata)
	{
		// If the input type is string, it implies that the recipient is also.
		//TODO - we could also check the type against stringTypes and throw here if needed.
		if (jdata.type() == ojson::value_t::string)
			return jdata.get<std::string>();

		return jdata.dump(2);
	}
}

void FFH2Project::Load(std::string projectpath)
{
	LogHere << "Loading JSON project to " << projectpath << std::endl;

	std::ifstream ifs(projectpath);
	auto j = ojson::parse(ifs);
	*this = j.get<FFH2Project>();

	ProjectPath = projectpath;
	ProjectFolder = Paths::GetDirectoryPath(projectpath);
	WorkRomPath = Paths::Combine({ tomfc(ProjectFolder), tomfc(this->info.workrom) });

	// Fixup the filepaths, e.g. *APP, *PRJ, *CWD must be translated to actual paths
	//	let the editors handle the translation at time of edit.
	//	This must be done (here and in Save below) before assembly processing will work.
	static const auto fixUp = [](const FFH2Project& proj, std::string& origpath)
	{
		// *app to C:/APPS/FFHacksterEx
		CString newvalue = tomfc(origpath);
		std::string directory = Paths::GetDirectoryPath(origpath);
		if (directory == REFDIR_APP) {
			newvalue = Paths::Combine({ Paths::GetProgramFolder(), Paths::GetFileName(origpath.c_str()) });
		}
		else if (directory == REFDIR_CWD) {
			newvalue = Paths::Combine({ Paths::GetCWD(), Paths::GetFileName(origpath.c_str()) });
		}
		else if (directory == REFDIR_PRJ) {
			newvalue = Paths::Combine({ proj.ProjectFolder.c_str(), Paths::GetFileName(origpath.c_str()) });
		}
		origpath = tostd(newvalue);
	};

	fixUp(*this, this->info.additionalModulesPath);
	fixUp(*this, this->info.asmdll);
	fixUp(*this, this->info.publishTo);
	for (auto& entry : this->modules.entries) {
		fixUp(*this, entry.second.sourcePath);
	}

	ClearROM();

	//TODO - from LoadCartData(), need to add palette loading

	Finger.Create(16, 16);
	LoadFinger();
	InitMapVars();

	LogHere << "  -> finished loading JSON project to " << projectpath << std::endl;
}

void FFH2Project::Save(std::string alternateprojectpath)
{
	auto& projpath = alternateprojectpath.empty() ? this->ProjectPath : alternateprojectpath;
	LogHere << " Saving JSON project to: " << projpath << std::endl;

	// Fixup the filepaths, e.g. filepaths must be translated back to *APP, *PRJ, *CWD
	const auto fixUp = [](const FFH2Project& proj, std::string& origpath)
	{
		// C:/APPS/FFHacksterEx to *app
		CString newvalue = tomfc(origpath);
		CString directory = Paths::GetDirectoryPath(newvalue);
		if (directory == Paths::GetProgramFolder()) {
			newvalue = Paths::Combine({ REFDIR_APP, Paths::GetFileName(newvalue) });
		}
		else if (directory == Paths::GetCWD()) {
			newvalue = Paths::Combine({ REFDIR_CWD, Paths::GetFileName(newvalue) });
		}
		else if ((directory == tomfc(proj.ProjectFolder)) || directory == ".") {
			newvalue = Paths::Combine({ REFDIR_PRJ, Paths::GetFileName(newvalue) });
		}

		// Encoded paths also use forward slashes instead of backslashes.
		//TODO - for the moment, use MFC as a shortcut, rewrite either with std::string::replace or std::regexp
		newvalue.Replace('\\', '/');
		origpath = tostd(newvalue);
	};

	// Make the changes to a temp clone.
	// If an exception occurs, we don't have to do a piecewise rollback.
	FFH2Project clone = *this;
	fixUp(clone, clone.info.additionalModulesPath);
	fixUp(clone, clone.info.asmdll);
	fixUp(clone, clone.info.publishTo);
	for (auto& entry : clone.modules.entries) {
		fixUp(clone, entry.second.sourcePath);
	}

	std::ostringstream ostr;
	ojson oj = clone;
	ostr << oj.dump(2);

	std::string s = ostr.str();
	std::ofstream ofs(projpath);
	ofs << s;
	LogHere << "  -> finished saving JSON project to " << projpath << std::endl;

	// The save worked, update *this with the changes.
	*this = clone;
}

bool FFH2Project::IsRom() const
{
	return info.type == "rom";
}

bool FFH2Project::IsAsm() const
{
	return info.type == "asm";
}

std::string* FFH2Project::GetTable(int index)
{
	ASSERT(index >= 0 && index < (int)session.textViewInDTE.size());
	if (index < 0 || index >= (int)session.textViewInDTE.size())
		throw std::runtime_error("Can't find a text table with index '" + std::to_string(index) + "'.");

	return tables[session.textViewInDTE[index]].data();
}

void FFH2Project::InitMapVars()
{
	DataValueAccessor d(*this);
	auto MAP_COUNT = d.get<int>("MAP_COUNT");
	OK_overworldtiles = false;
	OK_tiles.resize(MAP_COUNT, false);

	DeleteStandardTiles();
	m_vstandardtiles.resize(MAP_COUNT * 2);
}

bool FFH2Project::ClearROM()
{
	DataValueAccessor d(*this);
	auto ROM_SIZE = d.get<int>("ROM_SIZE");
	if (ROM_SIZE == 0)
		throw std::runtime_error("ROM_SIZE shouldn't be 0");

	ROM.clear();
	ROM.resize(ROM_SIZE, 0);
	return true;
}

void FFH2Project::LoadROM()
{
	ASSERT(info.type == "rom"); //TODO - throw instead?
	Io::load_binary(tomfc(WorkRomPath), ROM);
}

void FFH2Project::SaveROM()
{
	ASSERT(info.type == "rom"); //TODO - throw instead?
	Io::save_binary(tomfc(WorkRomPath), ROM);
}

// ROM projects load these values from the Values file, whereas assembly projects use the constants.inc and variables.inc source files.
// This call only has an effect on ROM projects.
bool FFH2Project::UpdateVarsAndConstants()
{
	if (IsRom()) {
		const std::string fieldName = "ROMINIT_VARNAMES_SECTIONS";
		auto it = values.entries.find(fieldName);
		if (it != cend(values.entries)) {
			DataValueAccessor d(*this);
			// Get the main list of entries
			auto arr = d.get<std::vector<std::string>>(fieldName);
			for (const auto& a : arr) {
				auto itval = values.entries.find(a);
				if (itval == cend(values.entries))
					throw std::runtime_error("Value '" + a + "' is in the project file but missing from the entries collection.");

				auto sublist = d.get<std::vector<std::string>>(a);
				for (const auto& sub : sublist) {
					//--
					//TODO - remove, just tracing string form of the value here
					auto vsub = d.str(sub);
					TRACE("subval '%s' = %s\n", sub.c_str(), vsub.c_str());
					//--

					auto subvalue = d.get<int>(sub);
					m_varmap[sub] = subvalue;
				}
			}
		}
	}
	return true;
}

void FFH2Project::ReTintPalette()
{
	ffh::fda::DataValueAccessor d(*this);
	int TRANSPARENTCOLOR = d.get<int>("TRANSPARENTCOLOR");
	int TRANSPARENTCOLORREPLACEMENT = d.get<int>("TRANSPARENTCOLORREPLACEMENT");

	bool tinter[9][3] = { 1,1,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,1,1,0,1,1,1,0,1,1,1 };
	int co2, co, c;
	int rgb[3];
	int temp;
	for (co2 = 1; co2 < 9; co2++)
	{
		for (co = 0; co < 0x40; co++)
		{
			rgb[0] = palette[0][co] & 0xFF;
			rgb[1] = (palette[0][co] >> 8) & 0xFF;
			rgb[2] = (palette[0][co] >> 16) & 0xFF;

			for (c = 0; c < 3; c++) {
				if (tinter[co2][c]) {
					rgb[c] += session.tintVariant;
					if (rgb[c] > 0xFF) rgb[c] = 0xFF;
				}
				else {
					rgb[c] -= session.tintVariant;
					if (rgb[c] < 0) rgb[c] = 0;
				}
			}

			temp = (rgb[2] << 16) + (rgb[1] << 8) + rgb[0];
			if (temp == TRANSPARENTCOLOR) temp = TRANSPARENTCOLORREPLACEMENT;

			palette[co2][co] = temp;
		}
	}
}

FFHValue& FFH2Project::GetValue(const std::string& name)
{
	//TODO - replace this copypasta with ffh::uti::find
	auto it = values.entries.find(name);
	if (it == cend(values.entries))
		throw std::runtime_error("Project values collection does not contain a value named " + name + ".");
	return it->second;
}

ProjectEditorModuleEntry& FFH2Project::GetModule(const std::string& name)
{
	auto it = modules.entries.find(name);
	if (it == cend(modules.entries))
		throw std::runtime_error("Project modules collection does not contain a module named " + name + ".");
	return it->second;
}

ProjectDialogueTalkHandler& FFH2Project::GetHandler(const std::string& name)
{
	auto it = dialogue.handlers.entries.find(name);
	if (it == cend(dialogue.handlers.entries))
		throw std::runtime_error("Project dialogue handler collection does not contain a handler named " + name + ".");
	return it->second;
}

CImageList& FFH2Project::GetStandardTiles(size_t index, bool showrooms)
{
	// Stored as staggered, so stdmap 0 uses index 0 (outside) and 1 (showrooms)
	//[0-60] [0-121] 0 => 0,1   60=> 120,121
	ASSERT(((index * 2) + showrooms) < m_vstandardtiles.size());
	int showroomsindex = showrooms ? 1 : 0;
	return m_vstandardtiles[(index * 2) + showroomsindex].get();
}

CImageList& FFH2Project::GetStandardTiles(size_t index, int showroomsindex)
{
	// Stored as staggered, so stdmap 0 uses index 0 (outside) and 1 (showrooms)
	//[0-60] [0-121] 0 => 0,1   60=> 120,121
	ASSERT(((index * 2) + showroomsindex) < m_vstandardtiles.size());
	return m_vstandardtiles[(index * 2) + showroomsindex].get();
}

void FFH2Project::DeleteStandardTiles()
{
	m_vstandardtiles.clear();
}

void FFH2Project::LoadFinger()
{
	Finger.Empty();

	CBitmap bmp;
	CPaintDC dc(AfxGetMainWnd());
	CDC mDC; mDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, 32, 16);
	auto oldbmp = mDC.SelectObject(&bmp);

	try {
		ffh::fda::DataValueAccessor s(*this);
		auto FINGERGRAPHIC_OFFSET = s.get<int>("FINGERGRAPHIC_OFFSET");
		auto CHARBATTLEPALETTE_OFFSET = s.get<int>("CHARBATTLEPALETTE_OFFSET");
		ASSERT(FINGERGRAPHIC_OFFSET > 0);
		ASSERT(CHARBATTLEPALETTE_OFFSET > 0);

		if (IsRom()) {
			// use live ROM as expected
			LoadROM();
		}
		else if (IsAsm()) {
			// Read the Finger data into ROM
			auto BINBANK09GFXDATA_OFFSET = s.get <int>("BINBANK09GFXDATA_OFFSET");
			ASSERT(BINBANK09GFXDATA_OFFSET > 0);
			GameSerializer ser(*this, AfxGetMainWnd());
			ser.LoadAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
			ser.LoadInline(ASM_0F, { { asmlabel, LUT_BATTLESPRITEPALETTES,{ (unsigned int)CHARBATTLEPALETTE_OFFSET } } });
		}
		else {
			ErrorHere << "Unsupported project type can't load the finger icon" << std::endl;
		}

		auto& THISROM = ROM;

		BYTE coY, coX, bigY, bigX;
		int offset = FINGERGRAPHIC_OFFSET;
		BYTE line;
		BYTE pal[4] = { 0x35,
			THISROM[CHARBATTLEPALETTE_OFFSET + 13],
			THISROM[CHARBATTLEPALETTE_OFFSET + 14],
			THISROM[CHARBATTLEPALETTE_OFFSET + 15] };
		BYTE pixel[2][2] = { 0,2,1,3 };
		for (bigY = 0; bigY < 2; bigY++) {
			for (bigX = 0; bigX < 2; bigX++, offset += 8) {
				for (coY = 0; coY < 8; coY++, offset += 1) {
					for (coX = 0, line = 0x80; coX < 8; coX++, line >>= 1) {
						mDC.SetPixelV((bigX << 3) + coX, ((bigY << 3) + coY),
							palette[0][pal[pixel
							[(THISROM[offset] & line) != 0]
						[(THISROM[offset + 8] & line) != 0]]]);
						mDC.SetPixelV(16 + (bigY << 3) + coY, 15 - ((bigX << 3) + coX),
							palette[0][pal[pixel
							[(THISROM[offset] & line) != 0]
						[(THISROM[offset + 8] & line) != 0]]]);
					}
				}
			}
		}

		mDC.SelectObject(oldbmp);
		Finger.get().Add(&bmp, palette[0][pal[0]]);
	}
	catch (...) {
		ErrorHere << "Unknown exception caught while trying to load the finger icon" << std::endl;
	}

	mDC.DeleteDC();
	bmp.DeleteObject();
}


// LOCAL HELPER IMPLEMENTATIONS
namespace // unnamed
{

	void ThrowOnFailure(const std::string& name, std::function<void()> func)
	{
		try {
			func();
		}
		catch (nlohmann::json_abi_v3_11_2::detail::parse_error& jex) {
			std::string what = jex.what();
			auto i = what.find("parse error");
			if (i != std::string::npos) what.erase(0, i);
			throw std::runtime_error(name + " JSON " + what);
		}
		catch (std::exception& ex) {
			throw std::runtime_error("Exception parsing property '" + name + "': " + std::string(ex.what()));
		}
		catch (...) {
			throw std::runtime_error("Unknown exception parsing property '" + name + "'.");
		}
	}

} // end namespace // unnamed



//TODO - move these external functions to a separate cpp file(s)? if so, FieldToJson and JsonToField must be visible
// Conversions to and from JSON (in this case, ordered_json)
// Declared externally in the json header above.

//=== FFHSetting extern functions //TODO- move these out of this class?
void to_json(ojson& j, const FFHSetting& p)
{
	auto jdata = FieldToJson<ojson>(p.type, p.data);
	j = ojson{
		{"type", p.type},
		{"format", p.format},
		{"data", jdata}
	};
}

void from_json(const ojson& j, FFHSetting& p)
{
	ojson jdata;
	j.at("type").get_to(p.type);
	j.at("format").get_to(p.format);
	j.at("data").get_to(jdata);
	p.data = JsonToField(jdata);
}

void to_json(ujson& j, const FFHSetting& p)
{
	auto jdata = FieldToJson<ujson>(p.type, p.data);
	j = ujson{
		{"type", p.type},
		{"format", p.format},
		{"data", jdata}
	};
}

void from_json(const ujson& j, FFHSetting& p)
{
	ujson jdata;
	j.at("type").get_to(p.type);
	j.at("format").get_to(p.format);
	j.at("data").get_to(jdata);
	p.data = JsonToField(jdata);
}


//=== FFHValue extern functions //TODO- move these out of this class?
void to_json(ojson& j, const FFHValue& p)
{
	// Coming back to JSON, we have to rely on the incoming object to tell us
	// when to put dquotes around strings.
	// e.g., hex integers will throw exceptions if 0x12AB isn't a quoted string.
	ThrowOnFailure(p.name, [&]() {
		auto jdata = FieldToJson<ojson>(p.type, p.data);
		j = ojson();
		j["name"] = p.name;
		j["label"] = p.label;
		j["desc"] = p.desc;
		j["group"] = p.group;
		j["internal"] = p.internal;
		j["readonly"] = p.readonly;
		j["hidden"] = p.hidden;
		j["type"] = p.type;
		j["format"] = p.format;
		j["data"] = jdata;
		});
}

void from_json(const ojson& j, FFHValue& p)
{
	ThrowOnFailure(p.name, [&]() {
		ojson jdata;
		j.at("name").get_to(p.name);
		j.at("label").get_to(p.label);
		j.at("desc").get_to(p.desc);
		j.at("group").get_to(p.group);
		j.at("internal").get_to(p.internal);
		j.at("readonly").get_to(p.readonly);
		j.at("hidden").get_to(p.hidden);
		j.at("type").get_to(p.type);
		j.at("format").get_to(p.format);
		j.at("data").get_to(jdata);
		p.data = JsonToField(jdata);
		});
}

void to_json(ujson& j, const FFHValue& p)
{
	ThrowOnFailure(p.name, [&]() {
		auto jdata = FieldToJson<ujson>(p.type, p.data);
		j = ujson();
		j["name"] = p.name;
		j["label"] = p.label;
		j["desc"] = p.desc;
		j["group"] = p.group;
		j["internal"] = p.internal;
		j["readonly"] = p.readonly;
		j["hidden"] = p.hidden;
		j["type"] = p.type;
		j["format"] = p.format;
		j["data"] = jdata;
		});
}

void from_json(const ujson& j, FFHValue& p)
{
	ThrowOnFailure(p.name, [&]() {
		ujson jdata;
		j.at("name").get_to(p.name);
		j.at("label").get_to(p.label);
		j.at("desc").get_to(p.desc);
		j.at("group").get_to(p.group);
		j.at("internal").get_to(p.internal);
		j.at("readonly").get_to(p.readonly);
		j.at("hidden").get_to(p.hidden);
		j.at("type").get_to(p.type);
		j.at("format").get_to(p.format);
		j.at("data").get_to(jdata);
		p.data = JsonToField(jdata);
		});
}