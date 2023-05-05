#include "stdafx.h"
#include "FFH2Project.h"
#include "DataValueAccessor.h"
#include "FFH2Project_IniToJson_Defs.hpp"
#include "FFHDataValue_dva.hpp"
#include "ini_functions.h"
#include "io_functions.h"
#include "path_functions.h"
#include "std_collections_dva.hpp"
#include "string_conversions.hpp"
#include "string_functions.h"

#include <functional>
#include <fstream>
#include <iomanip>
#include <map>


FFH2Project::FFH2Project()
{
}

FFH2Project::~FFH2Project()
{
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

//TODO - move these to separate cpp files?
// Conversions to and from JSON (in this case, ordered_json)
// Declared externally in the json header above.

//=== FFHSettingValue
void to_json(ojson& j, const FFHSettingValue& p)
{
	auto jdata = FieldToJson<ojson>(p.type, p.data);
	j = ojson{
		{"type", p.type},
		{"format", p.format},
		{"data", jdata}
	};
}

void from_json(const ojson& j, FFHSettingValue& p)
{
	ojson jdata;
	j.at("type").get_to(p.type);
	j.at("format").get_to(p.format);
	j.at("data").get_to(jdata);
	p.data = JsonToField(jdata);
}

void to_json(ujson& j, const FFHSettingValue& p)
{
	auto jdata = FieldToJson<ujson>(p.type, p.data);
	j = ujson{
		{"type", p.type},
		{"format", p.format},
		{"data", jdata}
	};
}

void from_json(const ujson& j, FFHSettingValue& p)
{
	ujson jdata;
	j.at("type").get_to(p.type);
	j.at("format").get_to(p.format);
	j.at("data").get_to(jdata);
	p.data = JsonToField(jdata);
}


//=== FFHDataValue
void to_json(ojson& j, const FFHDataValue& p)
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

void from_json(const ojson& j, FFHDataValue& p)
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

void to_json(ujson& j, const FFHDataValue& p)
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

void from_json(const ujson& j, FFHDataValue& p)
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

	//TODO - LoadCartData(), need to add palette loading, Finger creation

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

const std::string* FFH2Project::GetTable(int index)
{
	ASSERT(index >= 0 && index < (int)session.textViewInDTE.size());
	if (index < 0 || index >= (int)session.textViewInDTE.size())
		throw std::runtime_error("Can't find a text table with index '" + std::to_string(index) + "'.");

	return data.tables[session.textViewInDTE[index]].data();
}

bool FFH2Project::ClearROM()
{
	ff1coredefs::DataValueAccessor d(*this);
	auto ROM_SIZE = d.get<int>("ROM_SIZE");
	if (ROM_SIZE == 0)
		throw std::runtime_error("ROM_SIZE shouldn't be 0");

	ROM.clear();
	ROM.resize(ROM_SIZE, 0);
	return true;
}

void FFH2Project::LoadROM()
{
	ASSERT(info.type == "rom");
	Io::load_binary(tomfc(WorkRomPath), ROM);
}

// ROM projects load these values from the Values file, whereas assembly projects use the constants.inc and variables.inc source files.
// This call only has an effect on ROM projects.
bool FFH2Project::UpdateVarsAndConstants()
{
	if (IsRom()) {
		const std::string fieldName = "ROMINIT_VARNAMES_SECTIONS";
		auto it = values.entries.find(fieldName);
		if (it != cend(values.entries)) {
			ff1coredefs::DataValueAccessor d(*this);
			// Get the main list of entries
			auto arr = d.get<std::vector<std::string>>(fieldName);
			for (const auto& a : arr) {
				auto itval = values.entries.find(a);
				if (itval == cend(values.entries))
					throw std::runtime_error("Value '" + a + "' is in the project file but missing from the entries collection.");

				auto sublist = d.get<std::vector<std::string>>(a);
				for (const auto& sub : sublist) {
					auto vsub = d.get<std::string>(sub);
					TRACE("subval is %s\n", vsub.c_str());

					auto subvalue = d.get<int>(sub);
					m_varmap[sub] = subvalue;
				}

				// ====
				//TODO - implement this and both rom and assembly should work as fully expected.
				// ====

				// This is a sublist of entry names:
				// grab each name and get its data as an int

				//auto sublist = d.get<std::vector<std::string>>(a);
				//for (const auto& sub : sublist) {
				//	if (value.empty())
				//		throw std::runtime_error("Cannot get referenced value '" + a + ".");
				//	//THROWEXCEPTION(std::runtime_error, "Value '" + std::string((LPCSTR)name) + "' is either missing or has an empty value property.");

				//	m_varmap[a] = std::stol(value);
				//}
				// ====
			}
		}
		//--
		//auto groups = mfcstrvec(ReadIni(ValuesPath, "ROMINIT_VARNAMES_SECTIONS", "value", ""));
		//for (const auto& group : groups) {
		//	auto names = mfcstrvec(ReadIni(ValuesPath, group, "value", ""));
		//	for (const auto& name : names) {
		//		auto value = ReadIni(ValuesPath, name, "value", "");
		//		if (value.IsEmpty())
		//			THROWEXCEPTION(std::runtime_error, "Value '" + std::string((LPCSTR)name) + "' is either missing or has an empty value property.");
		//		m_varmap[(LPCSTR)name] = to_int(value);
		//	}
		//}
	}
	return true;
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
