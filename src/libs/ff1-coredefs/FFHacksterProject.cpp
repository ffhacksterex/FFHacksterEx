#include "stdafx.h"
#include "FFHacksterProject.h"
#include "asmdll_impl.h"
#include "collection_helpers.h"
#include "general_functions.h"
#include "ini_functions.h"
#include "interop_assembly_interface.h"
#include "io_functions.h"
#include "path_functions.h"
#include "std_project_properties.h"
#include "type_support.h"
#include "upgrade_functions.h"
#include "AppSettings.h"

#include "AsmFiles.h"
#include "assembly_types.h"
#include "DATA.h"
#include "GameSerializer.h"
#include "IProgress.h"
#include <fstream>
#include <filesystem>

//STDFILESYS - see path_functions.cpp for more info
#if _MSVC_LANG  <= 201402L
namespace fsys = std::experimental::filesystem;
#else
namespace fsys = std::filesystem;
#endif

using namespace asmdll_impl;
using namespace Ini;
using namespace Types;

namespace // local helpers
{
	const auto DEFTRANSPARENTCOLOR = 0xFF00FF;
	const auto DEFTRANSPARENTCOLORREPLACEMENT = 0xE600E6;

	const int THIEFID = 1;
	const int NINJAID = 7;

	pair_result<CString> failure (CString cs)
	{
		return pair_result<CString>(false, cs);
	};

	template <int ROWS, int COLS>
	void DoWriteArray(unsigned char buffer[ROWS][COLS], CString inifile, CString section, CString key)
	{
		bytevector bytes;
		array_to_bytes<ROWS,COLS>(buffer, bytes);
		WriteIni(inifile, section, key, bytes);
	}

	template <int ROWS, int COLS>
	void DoReadArray(unsigned char buffer[ROWS][COLS], CString inifile, CString section, CString key)
	{
		auto bytes = ReadIni(inifile, section, key, bytevector());
		bytes_to_array<ROWS, COLS>(bytes, buffer);
	}

	void WriteFromDAT(CString inipath, CString section, const char * source, int count, int length, CString prefix = "")
	{
		CString pre(prefix);
		if (!pre.IsEmpty()) pre += ": ";

		for (int co = 0; co < count; ++co) {
			CString key;
			key.Format("%d", co);
			WriteIni(inipath, section, key, pre + source);
			source += length;
		}
	}

} // end namespace anonymous (local helpers)


const CString CFFHacksterProject::SETTINGS = "SETTINGS";
const CString CFFHacksterProject::CLASSES = "CLASSES";
const CString CFFHacksterProject::ENEMIES = "ENEMIES";
const CString CFFHacksterProject::MAGIC = "MAGIC";

const bool CFFHacksterProject::DEF_TEXTVIEWINDTE[20] = { false, false, false, false, false, true, false, false, true, false, true, true, true, true, true, 0 };

CFFHacksterProject::CFFHacksterProject()
	: m_projtype(ProjType::Unknown)
	, ROMSize(0)
{
	TextViewInDTE.resize(20, false);
}

CFFHacksterProject::~CFFHacksterProject()
{
	DeleteStandardTiles();
	Finger.DeleteImageList();
}

// Return the project version number from the project file.
// Returns -1 if it's not found.
int CFFHacksterProject::ReadProjectVersion(CString inifile)
{
	CString strver = ReadIni(inifile, "PROJECT", "version", "");
	if (strver.IsEmpty()) return -1;

	int version = dec(strver);
	return version;
}

void CFFHacksterProject::WriteProjectVersion(CString inifile, int version)
{
	CString strver = dec(version);
	WriteIni(inifile, "PROJECT", "version", strver);
}

// Loads file references and verifies the existence of require common files (excluding the assembly DLL).
pair_result<CString> CFFHacksterProject::LoadFileRefs(CString projectini)
{
	// Build a full path if the ini key specifies a filename, otherwise an empty string.
	auto GetIni = [&, this](CString section, CString key) {
		auto filename = ReadIni(ProjectPath, section, key, "");
		filename = Paths::GetFileName(filename); // for old projects that stored fullpaths
		return !filename.IsEmpty() ?
			//Project.ProjectFolder + "\\" + filename : //REFACTOR - verify that using Combine doesn't change behavior dealing with these paths
			Paths::Combine({ ProjectFolder, filename }) :
			"";
	};

	// Load all file ref strings
	ProjectPath = projectini;
	ProjectFolder = Paths::GetDirectoryPath(ProjectPath);

	ProjectTypeName = ReadIni(ProjectPath, PROJECTSECTION, "type", "");
	m_projtype = TextToProjType(ProjectTypeName);

	ProjectName = ReadIni(ProjectPath, PROJECTSECTION, "name", "");
	RevertGameDataPath = GetIni(PROJECTSECTION, "revert");
	WorkRomPath = GetIni(PROJECTSECTION, "workrom");

	ValuesPath = GetIni(FILESSECTION, FFHFILE_ValuesPath);
	RevertValuesPath = GetIni(FILESSECTION, FFHFILE_RevertValuesPath);
	StringsPath = GetIni(FILESSECTION, FFHFILE_StringsPath);
	EditorSettingsPath = GetIni(FILESSECTION, FFHFILE_EditorSettingsPath);
	PalettePath = GetIni(FILESSECTION, FFHFILE_PalettePath);
	StdTablePath = GetIni(FILESSECTION, FFHFILE_StdTablePath);
	DteTablePath = GetIni(FILESSECTION, FFHFILE_DteTablePath);
	DialoguePath = GetIni(FILESSECTION, FFHFILE_DialoguePath);

	PublishRomPath = GetIniRefDir(FFHREFDIR_Publish);
	AdditionalModulesFolder = GetIniRefDir(FFHREFDIR_AddlModFolder);
	AsmDLLPath = IsAsm() ? GetIniRefDir(FFHREFDIR_AsmDLLPath) : "";

	// Validate the existence of the required files, collecting an error message for the failures.
	CString msg;
	auto checkfunc = [&msg](CString key, CString filepath) {
		if (!Paths::FileExists(filepath))
			msg.AppendFormat("Can't find '%s' file '%s'\n", (LPCSTR)key, (LPCSTR)filepath);
	};

	if (!IsProjectTypeValid()) msg.AppendFormat("Project type '%s' is invalid\n", (LPCSTR)ProjectTypeName);
	checkfunc("Project", ProjectPath);
	checkfunc("revert", RevertGameDataPath);
	checkfunc(FFHFILE_ValuesPath, ValuesPath);
	checkfunc(FFHFILE_RevertValuesPath, RevertValuesPath);
	checkfunc(FFHFILE_StringsPath, StringsPath);
	checkfunc(FFHFILE_EditorSettingsPath, EditorSettingsPath);
	checkfunc(FFHFILE_PalettePath, PalettePath);
	checkfunc(FFHFILE_StdTablePath, StdTablePath);
	checkfunc(FFHFILE_DteTablePath, DteTablePath);
	checkfunc(FFHFILE_DialoguePath, DialoguePath);

	return { msg.IsEmpty(), msg };
}

bool CFFHacksterProject::SaveRefPaths()
{
	try {
		SetIniRefDir(FFHREFDIR_Publish, PublishRomPath);
		SetIniRefDir(FFHREFDIR_AddlModFolder, AdditionalModulesFolder);
		if (IsAsm())
			SetIniRefDir(FFHREFDIR_AsmDLLPath, AsmDLLPath);
		return true;
	}
	catch (std::exception & ex) {
		ErrorHere << "Unable to save refpaths due to an exception: " << ex.what() << std::endl;
	}
	catch (...) {
		ErrorHere << "Unable to save refpaths due to an unexpected exception" << std::endl;
	}
	return false;
}

void CFFHacksterProject::LoadEditorSettings()
{
	SuppressWarnOnAssemblyMismatch = ReadIniBool(ProjectPath, SETTINGS, "SuppressWarnOnAssemblyMismatch", SuppressWarnOnAssemblyMismatch);

	TextViewInDTE = ReadIni(ProjectPath, SETTINGS, "TextViewInDTE", boolvector());
	for (size_t st = TextViewInDTE.size(); st < _countof(DEF_TEXTVIEWINDTE); ++st) TextViewInDTE.push_back(DEF_TEXTVIEWINDTE[st]);

	ShowLastClick = ReadIniBool(ProjectPath, SETTINGS, "ShowLastClick", ShowLastClick);
	DrawDomainGrid = ReadIniBool(ProjectPath, SETTINGS, "DrawDomainGrid", DrawDomainGrid);
	TintVariant = (BYTE)dec(ReadIni(ProjectPath, SETTINGS, "TintVariant", dec(TintVariant)));
	DoReadArray<9, 128>(TintTiles, ProjectPath, SETTINGS, "TintTiles");
	DoReadArray<19, 9>(SmartTools, ProjectPath, SETTINGS, "SmartTools");
}

void CFFHacksterProject::SaveEditorSettings()
{
	// edited in Project Settings dialog
	WriteIniBool(ProjectPath, SETTINGS, "SuppressWarnOnAssemblyMismatch", SuppressWarnOnAssemblyMismatch);

	// edited in the Text editor
	WriteIni(ProjectPath, SETTINGS, "TextViewInDTE", TextViewInDTE);

	// edited in OW and STD map editors
	WriteIniBool(ProjectPath, SETTINGS, "ShowLastClick", ShowLastClick);
	WriteIniBool(ProjectPath, SETTINGS, "DrawDomainGrid", DrawDomainGrid);
	WriteIni(ProjectPath, SETTINGS, "TintVariant", dec(TintVariant));
	DoWriteArray<9, 128>(TintTiles, ProjectPath, SETTINGS, "TintTiles");
	DoWriteArray<19, 9>(SmartTools, ProjectPath, SETTINGS, "SmartTools");
}

bool CFFHacksterProject::Clone(CString newprojectfile)
{
	try {
		// Copy all files into the new project folder
		CString destfolder = Paths::GetDirectoryPath(newprojectfile);
		fsys::copy((LPCSTR)ProjectFolder, (LPCSTR)destfolder, fsys::copy_options::recursive | fsys::copy_options::overwrite_existing);

		// Now rename the top level files that have the source project title
		CString srctitle = Paths::GetFileStem(ProjectPath);
		CString desttitle = Paths::GetFileStem(newprojectfile);
		auto tempprojectpath = destfolder + "\\" + Paths::GetFileName(ProjectPath);
		return RenameProjectFiles(destfolder, tempprojectpath, srctitle, desttitle);
	}
	catch (std::exception ex) {
		ErrorHere << "Failed to clone project " << (LPCSTR)ProjectPath << " to " << (LPCSTR)newprojectfile << nl
				<< "        exception: " << ex.what() << std::endl;
	}
	return false;
}

bool CFFHacksterProject::IsRom() const
{
	return m_projtype == ProjType::Rom;
}

bool CFFHacksterProject::IsAsm() const
{
	return m_projtype == ProjType::Asm;
}

CString * CFFHacksterProject::GetTable(int index)
{
	ASSERT(index >= 0 && index < (int)TextViewInDTE.size());
	if (index < 0 || index >= (int)TextViewInDTE.size())
		return nullptr;

	return Tables[TextViewInDTE[index]];
}

bool CFFHacksterProject::ClearROM()
{
	if (ROMSize == 0)
		throw std::runtime_error("ROMSize shouldn't be 0");

	ROM.clear();
	ROM.resize(ROMSize, 0);
	return true;
}

//STATIC
void CFFHacksterProject::LoadTable(CString table[256], FILE* file)
{
	BYTE buffer[0x1000] = { 0 };
	CString text;
	CString linebreak;
	linebreak.Format("%c%c", 0x0D, 0x0A);
	fread(buffer, 1, 0x1000, file);
	fclose(file);
	text.Format("%s", buffer);

	CString ex;
	int find;
	int value;
	bool end = 0;
	while (!end) {
		ex = "";
		find = text.Find('=');
		if (find == -1) return;

		value = StringToInt_HEX(text.Left(find));
		text = text.Right(text.GetLength() - find - 1);
		find = text.Find(linebreak);
		if (find == -1) {
			find = text.GetLength();
			end = 1;
		}
		ex = text.Left(find);
		text = text.Right(text.GetLength() - find - 2);
		if (value < 256)
			table[value] = ex;
	}
}

//STATIC
auto CFFHacksterProject::TextToProjType(CString text) -> ProjType
{
	if (text == "rom") return ProjType::Rom;
	if (text == "asm") return ProjType::Asm;
	if (text == "") return ProjType::None;
	return ProjType::Unknown;
}

std::vector<CString> CFFHacksterProject::ReadIniLabels(CString inifile, CString section)
{
	std::vector<CString> labels;

	// The string count name mappings are in the project.
	// The counts are in the values file.
	CString countname = ReadIni(ProjectPath, "STRINGCOUNTS", section, "");

	if (countname.IsEmpty()) THROWEXCEPTION(std::runtime_error, "group " + std::string((LPCSTR)section) + " did not specify a count name");
	int count = ReadDec(ValuesPath, countname);
	if (count < 1) {
		CString err;
		err.Format("group '%s' specfied a count name '%s', but that value must be > 0", (LPCSTR)section, (LPCSTR)countname);
		THROWEXCEPTION(std::runtime_error, (LPCSTR)err);
	}

	for (int index = 0; index < count; ++index) {
		CString key;
		key.Format("%lu", index);
		CString value = ReadIni(inifile, section, key, "");
		labels.push_back(value);
	}

	if (count != (int)labels.size()) {
		CString err;
		err.Format("count mismatch: group '%s' expects count %d, but read %d items", (LPCSTR)section, count, labels.size());
		THROWEXCEPTION(std::runtime_error, (LPCSTR)err);
	}

	return labels;
}

void CFFHacksterProject::WriteIniLabels(const std::vector<CString>& labels, CString inifile, CString section, CString keyprefix)
{
	// Format key-value pairs.
	CString sectiontext;
	for (size_t st = 0, length = labels.size(); st < length; ++st) {
		sectiontext.AppendFormat("%s%lu=%s\n", (LPCSTR)keyprefix, st, (LPCSTR)labels[st]);
	}

	// Replace newlines with null chars to fit the expectations of WritePrivateProfileSection.
	sectiontext.Replace('\n', '\0');
	WritePrivateProfileSection(section, sectiontext, inifile);
}

bool CFFHacksterProject::IsProjectTypeValid() const
{
	return m_projtype == ProjType::Rom || m_projtype == ProjType::Asm;
}

CString CFFHacksterProject::LoadCartData()
{
	const BYTE DEF_SMARTTOOLS[19][9] = {
		{ 3,4,5,19,20,21,35,36,37 },
		{ 6,7,8,22,23,24,38,39,40 },
		{ 16,17,18,32,33,34,48,49,51 },
		{ 23,23,23,24,24,24,28,28,28 },
		{ 23,23,23,24,24,24,29,29,29 },
		{ 0,1,2,3,4,5,6,7,8 },
		{ 0,1,2,3,94,5,6,7,8 },
		{ 0,1,2,3,4,5,6,7,8 },
		{ 0,1,2,3,46,5,6,7,8 },
		{ 0,1,2,3,4,5,6,7,8 },
		{ 0,1,2,3,70,5,6,7,8 },
		{ 0,1,2,3,4,5,6,7,8 },
		{ 0,1,2,3,45,5,6,7,8 },
		{ 0,1,2,3,4,5,6,7,8 },
		{ 0,1,2,3,83,5,6,7,8 },
		{ 0,1,2,3,4,5,6,7,8 },
		{ 0,1,2,3,73,5,6,7,8 },
		{ 0,1,2,3,4,5,6,7,8 },
		{ 0,1,2,3,85,5,6,7,8 }
	};

	//first... boot up the palette.
	FILE* file = fopen(PalettePath, "rb");
	BYTE pal[0xC0];
	BYTE rgb[3];
	if (file == nullptr)
		return "Error loading palette...\nImages will not display correctly";
	
	fread(pal, 1, 0xC0, file);
	fclose(file);

	int TRANSPARENTCOLOR = ReadRgb(ValuesPath, "TRANSPARENTCOLOR", DEFTRANSPARENTCOLOR);
	int TRANSPARENTCOLORREPLACEMENT = ReadRgb(ValuesPath, "TRANSPARENTCOLORREPLACEMENT", DEFTRANSPARENTCOLORREPLACEMENT);

	int co1, co;
	for (co = 0, co1 = 0; co < 64; co++, co1 += 3) {
		rgb[0] = pal[co1]; rgb[1] = pal[co1 + 1]; rgb[2] = pal[co1 + 2];
		Palette[0][co] = RGB(rgb[0], rgb[1], rgb[2]);
		if (Palette[0][co] == (COLORREF)TRANSPARENTCOLOR)
			Palette[0][co] = TRANSPARENTCOLORREPLACEMENT;
	}
	for (co = 0; co < 9; co++)
		Palette[co][0x40] = TRANSPARENTCOLOR;

	//okay... break out the 'finger' ImageList
	//N.B. - reloads might not release the imagelist control, so do it here
	Finger.DeleteImageList();
	Finger.Create(16, 16, ILC_COLOR16 + ILC_MASK, 2, 0);
	LoadFinger();

	//next... load the standard map tile showroom flags and imagelists
	int MAP_COUNT = ReadDec(ValuesPath, "MAP_COUNT");
	OK_overworldtiles = 0;
	OK_tiles.resize(MAP_COUNT, false);
	m_vstandardtiles.resize(MAP_COUNT * 2);
	for (size_t st = 0; st < m_vstandardtiles.size(); ++st) m_vstandardtiles[st] = new CImageList();

	//now... let's get our Tables
	file = fopen(StdTablePath, "rb");
	if (file == nullptr)
		return "Error loading Standard Table...\nSome text will not display correctly";

	LoadTable(Tables[0], file);

	file = fopen(DteTablePath, "rb");
	if (file == nullptr)
		return "Error loading DTE Table...\nSome text will not display correctly";

	LoadTable(Tables[1], file);

	// Final prep, set defaults (the caller will read the settings if this isn't a new project)
	ReTintPalette();

	TextViewInDTE.resize(sizeof(DEF_TEXTVIEWINDTE), false);
	for (size_t st = 0; st < _countof(DEF_TEXTVIEWINDTE); ++st) TextViewInDTE[st] = DEF_TEXTVIEWINDTE[st];

	ShowLastClick = false;
	DrawDomainGrid = false;
	TintVariant = DEFTINT_VARIANT;
	memcpy(SmartTools, DEF_SMARTTOOLS, sizeof(DEF_SMARTTOOLS));
	memset(TintTiles, 0, sizeof(TintTiles));

	return CString();
}

void CFFHacksterProject::ReTintPalette()
{
	int TRANSPARENTCOLOR = ReadRgb(ValuesPath, "TRANSPARENTCOLOR", DEFTRANSPARENTCOLOR);
	int TRANSPARENTCOLORREPLACEMENT = ReadRgb(ValuesPath, "TRANSPARENTCOLORREPLACEMENT", DEFTRANSPARENTCOLORREPLACEMENT);

	bool tinter[9][3] = { 1,1,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,1,1,0,1,1,1,0,1,1,1 };
	int co2, co, c;
	int rgb[3];
	int temp;
	for (co2 = 1; co2 < 9; co2++)
	{
		for (co = 0; co < 0x40; co++)
		{
			rgb[0] = Palette[0][co] & 0xFF;
			rgb[1] = (Palette[0][co] >> 8) & 0xFF;
			rgb[2] = (Palette[0][co] >> 16) & 0xFF;

			for (c = 0; c < 3; c++) {
				if (tinter[co2][c]) {
					rgb[c] += TintVariant;
					if (rgb[c] > 0xFF) rgb[c] = 0xFF;
				}
				else {
					rgb[c] -= TintVariant;
					if (rgb[c] < 0) rgb[c] = 0;
				}
			}

			temp = (rgb[2] << 16) + (rgb[1] << 8) + rgb[0];
			if (temp == TRANSPARENTCOLOR) temp = TRANSPARENTCOLORREPLACEMENT;

			Palette[co2][co] = temp;
		}
	}
}

bool CFFHacksterProject::CanRevert() const
{
	return Paths::FileExists(RevertGameDataPath);
}

pair_result<CString> CFFHacksterProject::ImportHacksterDAT(CString hacksterfile)
{
	CString errmsg;

	if (errmsg.IsEmpty()) {
		if (!Paths::FileExists(ProjectPath))
			errmsg.Format("cannot find the project file '%s'", (LPCSTR)ProjectPath);
	}

	std::ifstream ifs;
	ifs.open((LPCSTR)hacksterfile, std::ios::binary | std::ios::ate);
	if (!ifs.is_open())
		errmsg.Format("unable to open the DAT file '%s'", (LPCSTR)hacksterfile);

	DATA dat = { 0 };

	if (errmsg.IsEmpty()) {
		auto length = (size_t)ifs.tellg();
		if (length != sizeof(DATA))
			errmsg.Format("Expected the DAT file to have as least %lu bytes, but the file instead has %lu", sizeof(DATA), length);
	}

	if (errmsg.IsEmpty()) {
		ifs.seekg(0);
		ifs.read((char*)&dat, sizeof(DATA));
		if (memcmp(dat.Header, "FFH1", 4) != 0) {
			errmsg.Format("FFHackster DAT format not recpgnized for the file '%s'", (LPCSTR)hacksterfile);
		}
		else {
			TextViewInDTE.resize(TextViewInDTE.size(), false);
			for (size_t st = 0; st < _countof(dat.TextViewInDTE); ++st) TextViewInDTE[st] = dat.TextViewInDTE[st];

			ShowLastClick = dat.ShowLastClick;
			DrawDomainGrid = dat.DrawDomainGrid;
			TintVariant = dat.TintVariant;
			memcpy(TintTiles, dat.TintTiles, sizeof(TintTiles));
			memcpy(SmartTools, dat.SmartTools, sizeof(SmartTools));
		}
	}

	if (errmsg.IsEmpty()) {
		CString stringsini = this->StringsPath;
		if (!Paths::FileExists(stringsini)) {
			errmsg.Format("cannot write DAT strings to non-existent strings file '%s'", (LPCSTR)stringsini);
		}
		else {
			WriteFromDAT(stringsini, "WEPMAGLABELS", *dat.WepMagLabels, DATA::WEAPONMAGICGRAPHIC_COUNT, DATA::WEAPONMAGICGRAPHICLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "AILABELS", *dat.AILabels, DATA::AI_COUNT, DATA::AILABEL_SIZE + 1);
			WriteFromDAT(stringsini, "SHOPLABELS", *dat.ShopLabels, DATA::SHOP_COUNT, DATA::SHOPLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "PATTERNTABLELABELS", *dat.BattlePatternTableLabels, DATA::BATTLEPATTERNTABLE_COUNT, DATA::BATTLEPATTERNTABLELABEL_SIZE + 1);
			WriteFromDAT(stringsini, "BACKDROPLABELS", *dat.BackdropLabels, DATA::BACKDROP_COUNT, DATA::BACKDROPLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "ONTELEPORTLABELS", *dat.ONTeleportLabels, DATA::ONTELEPORT_COUNT, DATA::TELEPORTLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "NNTELEPORTLABELS", *dat.NNTeleportLabels, DATA::NNTELEPORT_COUNT, DATA::TELEPORTLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "NOTELEPORTLABELS", *dat.NOTeleportLabels, DATA::NOTELEPORT_COUNT, DATA::TELEPORTLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "TREASURELABELS", *dat.TCLabels, DATA::TREASURE_COUNT, DATA::TCLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "STDMAPLABELS", *dat.MapLabels, DATA::MAP_COUNT, DATA::MAPLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "TILESETLABELS", *dat.TilesetLabels, DATA::TILESET_COUNT, DATA::TILESETLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "SPRITELABELS", *dat.SpriteLabels, DATA::SPRITE_COUNT, DATA::SPRITELABEL_SIZE + 1);
			WriteFromDAT(stringsini, "SPRITEGRAPHICLABELS", *dat.SpriteGraphicLabels, DATA::MAPSPRITEGRAPHIC_COUNT, DATA::SPRITEGRAPHICLABEL_SIZE + 1);
			WriteFromDAT(stringsini, "BATTLELABELS", *dat.BattleLabels, DATA::BATTLE_COUNT, DATA::BATTLELABEL_SIZE + 1);
			WriteFromDAT(stringsini, "DIALOGUELABELS", *dat.TextLabels, DATA::DIALOGUE_COUNT, DATA::TEXTLABEL_SIZE + 1);
		}
	}

	return { errmsg.IsEmpty(), errmsg };
}

pair_result<CString> CFFHacksterProject::Revert()
{
	if (IsRom()) {
		// Just copy the reversion ROM over the current one
		if (!Paths::FileCopy(RevertGameDataPath, WorkRomPath))
			return failure("Failed to copy the reversion ROM; the original working ROM remains intact.");

		Io::MakeWritable(WorkRomPath);
	}
	else {
		// Try to avoid the state of 'deleted the current data, but also failed to extract the reverted data'
		CString asmdir(ProjectFolder + "\\asm");
		CString dirtemp(asmdir + "-tmp");
		if (Paths::DirExists(dirtemp))
			return failure("Unable to prepare temp storage for data extraction.");

		// Move asmdir to dirtemp, then extract the archive to asmdir
		if (!Paths::DirMove(asmdir, dirtemp))
			return failure("Unable to back up current data, the extraction process has been halted.");
		if (!Io::Unzip(RevertGameDataPath, asmdir)) {
			Paths::DirDelete(asmdir);
			if (Paths::DirMove(dirtemp, asmdir))
				return failure("Failed to extract reversion data; the original data remains intact.");
			return failure("Failed to extract reversion data, but couldn't move current data back to its original folder.\n"
				"It is now at " + dirtemp);
		}

		// New data is extracted, remove the temp folder
		if (!Paths::DirDelete(dirtemp))
			return failure("Reversion data was extracted, but the temp folder couldn't be removed:\n" + dirtemp);

		Io::MakeWritable(asmdir);
	}

	// Now revert the values file as well
	return RevertValues();
}

pair_result<CString> CFFHacksterProject::Compile()
{
	if (IsAsm()) {
		// The working ROM has to be compiled before being published.
		// 1) compile to ff1.nes in the asm folder
		// 2) ensure that ff1.nes is ROM_SIZE
		// 3) copy ff1.nes to WorkRomPath

		// The ROM_SIZE is a positive scalar, but if it's not properly read, it will either be
		// 0 (not specified) or MININT (incorrectly formatted hex value).
		int ROM_SIZE = ReadHex(ValuesPath, "ROM_SIZE");
		if (ROM_SIZE <= 0) {
			auto modalresult = AfxMessageBox("The ROM size is not defined in th eproject scalars.\n"
				"Do you want to continue by using the FF1 default size of 262,160 bytes?", MB_OKCANCEL | MB_ICONQUESTION);
			if (modalresult == IDCANCEL)
				return failure(""); // cancelled by the user, no message needed

			ROM_SIZE = 262160;
		}

		// Remove the file, so a failure to recreate it won't leave the old file intact.
		CString ff1(ProjectFolder + "\\asm\\ff1.nes");
		Paths::FileDelete(ff1);

		// Before performing the batch, move the STD and DTE tables into the asm folder
		//N.B. - while these aren't included in the compilation process, if someone zips the asm folder
		//		after compiling, then the asm folder should have the same tables as its parent.
		CString asmdir(ProjectFolder + "\\asm\\");
		if (!Paths::FileCopy(StdTablePath, asmdir + "table_standard.tbl"))
			return failure("couldn't copy the Standard table into the compilation folder.");
		if (!Paths::FileCopy(DteTablePath, asmdir + "table_dte.tbl"))
			return failure("couldn't copy the DTE table into the compilation folder.");

		// Now execute the batch file that builds the project.
		CString workingdir(ProjectFolder + "\\asm");
		CString cmd;
		cmd.Format(R"("ca65.exe" "ld65.exe" ff1.nes %d)", ROM_SIZE);
		Io::ExecuteFile(workingdir, "build.bat", cmd, 10000);

		// Ensure the file was created with the correct size.
		int filesize = Paths::FileSize(ff1);
		if (filesize != ROM_SIZE) {
			CString err;
			err.Format("expected the compiler ROM to be %lu bytes, but it's %lu instead.", ROM_SIZE, filesize);
			return failure(err);
		}

		// Copy the file to the working ROM path.
		if (!Paths::FileCopy(ff1, WorkRomPath))
			return failure("failed to copy the compiled ROM to '" + WorkRomPath + "'.");

		return pair_result<CString>(true, CString());
	}

	return pair_result<CString>(false, CString()); // nothing to do, but no error so no message
}

bool CFFHacksterProject::Publish()
{
	if (WorkRomPath == PublishRomPath)
		return true;
	return Paths::FileCopy(WorkRomPath, PublishRomPath);
}

pair_result<CString> CFFHacksterProject::RevertValues()
{
	//N.B. - ROM and ASM values files are unified into one file (previously were romvals and asmvals).
	auto srcfile = GetIniFilePath(FFHFILE_RevertValuesPath);
	if (!Paths::FileExists(srcfile))
		srcfile = Paths::Combine({ Paths::GetProgramFolder(), "FFHacksterEx.values.template" });
	if (!Paths::FileExists(srcfile))
		return pair_result<CString>(false, "Unable to locate a values reversion or template source file.");

	auto destfile = GetIniFilePath(FFHFILE_ValuesPath);
	if (!Paths::FileCopy(srcfile, destfile))
		return pair_result<CString>(false, "Unable to revert the values because the file copy failed.\nNo changes were made.");
	Io::MakeWritable(destfile);

	// If a temp file is being used, update that as well.
	auto tempfile = this->ValuesPath;
	if (tempfile != destfile) {
		if (!Paths::FileCopy(srcfile, tempfile))
			THROWEXCEPTION(std::runtime_error,
				"The original values file reverted,\nbut the working copy failed to revert due to an internal error.\n"
				"Restart the values editor to load the reverted values.");
		Io::MakeWritable(tempfile);
	}

	return pair_result<CString>(true, "Revert succeeded.");
}

bool CFFHacksterProject::LoadVariablesAndConstants(IProgress * progress)
{
	// Load the variables and constants. If this fails, then many editors become unusable.
	m_varmap.clear();
	if (IsAsm()) {
		GameSerializer gs(*this);
		return gs.ReadAllVariables(this->ProjectFolder, m_varmap, progress);
	}
	if (IsRom()) {
		return UpdateVarsAndConstants();
	}

	throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::initializing, "unknown project types can't load vars/constants");
}

// ROM projects load these values from the Values file, whereas assembly projects use the constants.inc and variables.inc source files.
// This call only has an effect on ROM projects.
bool CFFHacksterProject::UpdateVarsAndConstants()
{
	if (IsRom()) {
		auto groups = mfcstrvec(ReadIni(ValuesPath, "ROMINIT_VARNAMES_SECTIONS", "value", ""));
		for (auto group : groups) {
			auto names = mfcstrvec(ReadIni(ValuesPath, group, "value", ""));
			for (auto name : names) {
				auto value = ReadIni(ValuesPath, name, "value", "");
				if (value.IsEmpty())
					THROWEXCEPTION(std::runtime_error, "Value '" + std::string((LPCSTR)name) + "' is either missing or has an empty value property.");
				m_varmap[(LPCSTR)name] = to_int(value);
			}
		}
	}
	return true;
}

bool CFFHacksterProject::ShouldWarnAssemblyDllMismatch() const
{
	ASSERT(AppSettings != nullptr);
	if (AppSettings == nullptr)
		throw std::exception("Project can't check Asm Warning without access to App settings.");

	return AppSettings->WarnAssemblyDllCompatibility && !SuppressWarnOnAssemblyMismatch;
}

CString CFFHacksterProject::GetContentFolder() const
{
	if (IsRom())
		return ProjectFolder; // ROM is contained directly in the folder
	if (IsAsm())
		return Paths::Combine({ ProjectFolder, "asm" }); // asm data is contained in the asm subfolder
	
	std::string sprojtype = (LPCSTR)ProjectTypeName;
	THROWEXCEPTION(std::runtime_error, "unsupported project type '" + sprojtype + "' doesn't have a defined content folder");
}


// Used with the Get/SetIniFilePath functions below
namespace {
	const mfcstringset inipath_allowedkeys{
		FFHFILE_ValuesPath, FFHFILE_RevertValuesPath, FFHFILE_StringsPath, FFHFILE_EditorSettingsPath,
		FFHFILE_PalettePath, FFHFILE_StdTablePath, FFHFILE_DteTablePath, FFHFILE_DialoguePath
	};
}

CString CFFHacksterProject::GetIniFilePath(CString key) const
{
	//REFACTOR - change this to ProjectFolder instead of ProjectPath?
	//		If so, update the static version to compensate.
	return CFFHacksterProject::GetIniFilePath(ProjectPath, key);
}

//STATIC
CString CFFHacksterProject::GetIniFilePath(CString projectini, CString key)
{
	if (inipath_allowedkeys.find(key) == cend(inipath_allowedkeys))
		THROWEXCEPTION(std::invalid_argument, "can't call " __FUNCTION__ " with key " + std::string((LPCSTR)key));

	auto filename = ReadIni(projectini, FILESSECTION, key, "");
	filename = Paths::GetFileName(filename); // for old projects that stored fullpaths
	return !filename.IsEmpty() ?
		Paths::Combine({ Paths::GetDirectoryPath(projectini), filename }) :
		"";
}

void CFFHacksterProject::SetIniFilePath(CString key, CString newpath)
{
	CFFHacksterProject::SetIniFilePath(ProjectPath, key, newpath);
}

//STATIC
void CFFHacksterProject::SetIniFilePath(CString projectini, CString key, CString newpath)
{
	if (inipath_allowedkeys.find(key) == cend(inipath_allowedkeys))
		THROWEXCEPTION(std::invalid_argument, "can't call " __FUNCTION__ " with key " + std::string((LPCSTR)key));

	ASSERT(!fsys::path((LPCSTR)newpath).has_parent_path());
	if (fsys::path((LPCSTR)newpath).has_parent_path())
		THROWEXCEPTION(std::invalid_argument, "FILES paths must be relative paths, " + std::string((LPCSTR)newpath) + " is not relative");

	WriteIni(projectini, FILESSECTION, key, newpath);
}


// Used with the Encode/Decode functions below
namespace {
	const mfcstringset refdir_allowedkeys{ FFHREFDIR_AsmDLLPath, FFHREFDIR_AddlModFolder, FFHREFDIR_Publish };

	#define REFDIR_APP "*APP*"
	#define REFDIR_CWD "*CWD*"
	#define REFDIR_PRJ "*PRJ*"
}

CString CFFHacksterProject::DecodePathnameIfInKnownFolder(CString pathname) const
{
	return CFFHacksterProject::DecodePathnameIfInKnownFolder(pathname, this->ProjectFolder);
}

//STATIC
CString CFFHacksterProject::DecodePathnameIfInKnownFolder(CString filename, CString projectdir)
{
	// We map the encoded root of the path to an actual folder if necessary
	CString newvalue = filename;
	CString directory = Paths::GetDirectoryPath(filename);
	if (directory == REFDIR_APP) {
		newvalue = Paths::Combine({ Paths::GetProgramFolder(), Paths::GetFileName(filename) });
	}
	else if (directory == REFDIR_CWD) {
		newvalue = Paths::Combine({ Paths::GetCWD(), Paths::GetFileName(filename) });
	}
	else if (directory == REFDIR_PRJ) {
		newvalue = Paths::Combine({ projectdir, Paths::GetFileName(filename) });
	}
	return newvalue;
}

CString CFFHacksterProject::EncodePathnameIfInKnownFolder(CString pathname) const
{
	return CFFHacksterProject::EncodePathnameIfInKnownFolder(pathname, this->ProjectFolder);
}

//STATIC
// Replaces known path fragments with REFDIR placeholders and converts the path to forward slashes.
// e.g. if the app is in the folder C:\Tools\FFHacksterEx, then the path
//		C:\Tools\FFhacksterEx\ff1asm-disch.dll
//		becomes
//		*APP*/ff1asm-disch.dll
CString CFFHacksterProject::EncodePathnameIfInKnownFolder(CString filename, CString projectdir)
{
	// We map the volume/directory portion of the path to a known encoded placeholder
	CString newvalue = filename;
	CString directory = Paths::GetDirectoryPath(filename);
	if (directory == Paths::GetProgramFolder()) {
		newvalue = Paths::Combine({ REFDIR_APP, Paths::GetFileName(newvalue) });
	}
	else if (directory == Paths::GetCWD()) {
		newvalue = Paths::Combine({ REFDIR_CWD, Paths::GetFileName(newvalue) });
	}
	else if (directory == projectdir || directory == ".") {
		newvalue = Paths::Combine({ REFDIR_PRJ, Paths::GetFileName(newvalue) });
	}

	// Encoded paths also use forward slashes instead of backslashes.
	newvalue.Replace('\\', '/');
	return newvalue;
}

CString CFFHacksterProject::GetIniRefDir(CString key)
{
	return CFFHacksterProject::GetIniRefDir(ProjectPath, key);
}

//STATIC
CString CFFHacksterProject::GetIniRefDir(CString projectini, CString key)
{
	if (refdir_allowedkeys.find(key) == cend(refdir_allowedkeys))
		THROWEXCEPTION(std::invalid_argument, "can't call " __FUNCTION__ " with key " + std::string((LPCSTR)key));

	auto filename = ReadIni(projectini, REFSECTION, key, "");
	auto mappedpath = DecodePathnameIfInKnownFolder(filename, Paths::GetDirectoryPath(projectini));
	return mappedpath;
}

void CFFHacksterProject::SetIniRefDir(CString key, CString newvalue)
{
	CFFHacksterProject::SetIniRefDir(ProjectPath, key, newvalue);
}

//STATIC
void CFFHacksterProject::SetIniRefDir(CString projectini, CString key, CString newvalue)
{
	if (refdir_allowedkeys.find(key) == cend(refdir_allowedkeys))
		THROWEXCEPTION(std::invalid_argument, "can't call " __FUNCTION__ " with key " + std::string((LPCSTR)key));

	auto mappedpath = EncodePathnameIfInKnownFolder(newvalue, Paths::GetDirectoryPath(projectini));
	WriteIni(projectini, REFSECTION, key, mappedpath);
}

//STATIC
bool CFFHacksterProject::IsAsm(CString projectini)
{
	auto strtype = ReadIni(projectini, PROJECTSECTION, "type", "");
	auto projtype = TextToProjType(strtype);
	return projtype == ProjType::Asm;
}

//STATIC
bool CFFHacksterProject::RenameProjectFiles(CString projectpath, CString projectfile, CString origsrctitle, CString origdesttitle)
{
	try {
		// Since multi-dot extensions are possible, append a dot to srctitle and desttitle.
		// That should help avoid misnaming multi-dot filenames.
		auto srctitle = origsrctitle + '.';
		auto desttitle = origdesttitle + '.';

		// In the project file, update old project file references to the new project title
		auto RetitleKey = [srctitle, desttitle](CString ini, CString section, CString key) {
			auto oldvalue = ReadIni(ini, section, key, "");
			if (oldvalue.IsEmpty() || oldvalue.Find(srctitle) == -1) return;

			//DEVNOTE - don't call Paths::ReplaceFileTitle here; it doesn't handle multi-dot extensions well in this case.
			auto newvalue = oldvalue;
			newvalue.Replace(srctitle, desttitle);
			WriteIni(ini, section, key, newvalue);
		};

		//FUTURE - if publish refers to the source folder, update it to the dest folder?
		WriteIni(projectfile, PROJECTSECTION, "name", origdesttitle);
		RetitleKey(projectfile, PROJECTSECTION, "revert");
		RetitleKey(projectfile, PROJECTSECTION, "workrom");

		RetitleKey(projectfile, FILESSECTION, FFHFILE_ValuesPath);
		RetitleKey(projectfile, FILESSECTION, FFHFILE_RevertValuesPath);
		RetitleKey(projectfile, FILESSECTION, FFHFILE_StringsPath);
		RetitleKey(projectfile, FILESSECTION, FFHFILE_EditorSettingsPath);
		RetitleKey(projectfile, FILESSECTION, FFHFILE_PalettePath);
		RetitleKey(projectfile, FILESSECTION, FFHFILE_StdTablePath);
		RetitleKey(projectfile, FILESSECTION, FFHFILE_DteTablePath);
		RetitleKey(projectfile, FILESSECTION, FFHFILE_DialoguePath);

		// If the publish path refers to the old filetitle, change it to the new.
		// We don't what the old project was, so don't bother messing wtith the folder.
		{
			auto newpublish = CFFHacksterProject::GetIniRefDir(projectfile, FFHREFDIR_Publish);
			newpublish.Replace(srctitle, desttitle);
			CFFHacksterProject::SetIniRefDir(projectfile, FFHREFDIR_Publish, newpublish);
		}

		// Now rename all of the files to match the new file title
		for (auto & entry : fsys::directory_iterator((LPCSTR)projectpath)) {
			if (!fsys::is_regular_file(entry.status())) continue;

			CString srcfilepath = entry.path().u8string().c_str();
			//REFACTOR - use ci_support for this instead? if so, add CString support to the ci namespace in ci_support.h
			if (srcfilepath.Find(srctitle) == -1) continue;

			//DEVNOTE - don't call Paths::ReplaceFileTitle here; it doesn't handle multi-dot extensions well in this case.
			CString destfilepath = srcfilepath;
			destfilepath.Replace(srctitle, desttitle);
			Paths::FileRename(srcfilepath, destfilepath);
		}

		//FUTURE - remove read-only flags from all files in the directory automatically?

		return true;
	}
	catch (std::exception ex) {
		ErrorHere << "Failed to clone project " << (LPCSTR)projectpath << " to " << (LPCSTR)projectfile << nl
				<< "        exception: " << ex.what() << std::endl;
	}
	return false;
}

//STATIC
bool CFFHacksterProject::WriteStringCounts(CString projectini)
{
	const CString section = "STRINGCOUNTS";
	WriteIni(projectini, section, "WEPMAGLABELS", "WEAPONMAGICGRAPHIC_COUNT");
	WriteIni(projectini, section, "AILABELS", "AI_COUNT");
	WriteIni(projectini, section, "SHOPLABELS", "SHOP_COUNT");
	WriteIni(projectini, section, "PATTERNTABLELABELS", "BATTLEPATTERNTABLE_COUNT");
	WriteIni(projectini, section, "BACKDROPLABELS", "BATTLEBACKDROP_COUNT");
	WriteIni(projectini, section, "ONTELEPORTLABELS", "ONTELEPORT_COUNT");
	WriteIni(projectini, section, "NNTELEPORTLABELS", "NNTELEPORT_COUNT");
	WriteIni(projectini, section, "NOTELEPORTLABELS", "NOTELEPORT_COUNT");
	WriteIni(projectini, section, "TREASURELABELS", "TREASURE_COUNT");
	WriteIni(projectini, section, "STDMAPLABELS", "MAP_COUNT");
	WriteIni(projectini, section, "TILESETLABELS", "TILESET_COUNT");
	WriteIni(projectini, section, "SPRITELABELS", "SPRITE_COUNT");
	WriteIni(projectini, section, "SPRITEGRAPHICLABELS", "MAPSPRITEGRAPHIC_COUNT");
	WriteIni(projectini, section, "BATTLELABELS", "BATTLE_COUNT");
	WriteIni(projectini, section, "DIALOGUELABELS", "DIALOGUE_COUNT");
	WriteIni(projectini, section, "SPECIALTILELABELS", "SPECIAL_COUNT");
	WriteIni(projectini, section, "MISCCOORDLABELS", "MISCCOORDOFFSET_COUNT");
	WriteIni(projectini, section, "OOBMAGICLABELS", "OOBMAGIC_COUNT");
	WriteIni(projectini, section, "AILEFFECTLABELS", "AILEFFECT_COUNT");
	WriteIni(projectini, section, "ELEMENTLABELS", "ELEMENT_COUNT");
	WriteIni(projectini, section, "ENEMYCATEGORYLABELS", "ENEMYCATEGORY_COUNT");
	return true;
}

CImageList & CFFHacksterProject::GetStandardTiles(size_t index, bool showrooms)
{
	// Stored as staggered, so stdmap 0 uses index 0 (outside) and 1 (showrooms)
	//[0-60] [0-121] 0 => 0,1   60=> 120,121
	ASSERT(((index * 2) + showrooms) < m_vstandardtiles.size());
	return *m_vstandardtiles[(index * 2) + showrooms];

}

CImageList & CFFHacksterProject::GetStandardTiles(size_t index, int showroomsindex)
{
	// Stored as staggered, so stdmap 0 uses index 0 (outside) and 1 (showrooms)
	//[0-60] [0-121] 0 => 0,1   60=> 120,121
	ASSERT(((index * 2) + showroomsindex) < m_vstandardtiles.size());
	return *m_vstandardtiles[(index * 2) + showroomsindex];
}

void CFFHacksterProject::DeleteStandardTiles()
{
	for (auto * ptile : m_vstandardtiles) delete ptile;
	m_vstandardtiles.clear();
}

void CFFHacksterProject::LoadFinger()
{
	while (Finger.GetImageCount() > 0) Finger.Remove(0);

	CBitmap bmp;
	CPaintDC dc(AfxGetMainWnd());
	CDC mDC; mDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, 32, 16);
	auto oldbmp = mDC.SelectObject(&bmp);

	try {
		auto FINGERGRAPHIC_OFFSET = Ini::ReadHex(ValuesPath, "FINGERGRAPHIC_OFFSET");
		auto CHARBATTLEPALETTE_OFFSET = (unsigned int)Ini::ReadHex(ValuesPath, "CHARBATTLEPALETTE_OFFSET");
		ASSERT(FINGERGRAPHIC_OFFSET > 0);
		ASSERT(CHARBATTLEPALETTE_OFFSET > 0);

		if (IsRom()) {
			// use live ROM as expected
		}
		else if (IsAsm()) {
			// Read the Finger data into ROM
			auto BINBANK09GFXDATA_OFFSET = ReadHex(ValuesPath, "BINBANK09GFXDATA_OFFSET");
			ASSERT(BINBANK09GFXDATA_OFFSET > 0);
			GameSerializer ser(*this, AfxGetMainWnd());
			ser.LoadAsmBin(BIN_BANK09GFXDATA, BINBANK09GFXDATA_OFFSET);
			ser.LoadInline(ASM_0F, { { asmlabel, LUT_BATTLESPRITEPALETTES,{ CHARBATTLEPALETTE_OFFSET } } });
		}
		else {
			ErrorHere << "Unsupported project type can't load the finger icon" << std::endl;
		}

		auto & THISROM = ROM;

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
							Palette[0][pal[pixel
							[(THISROM[offset] & line) != 0]
						[(THISROM[offset + 8] & line) != 0]]]);
						mDC.SetPixelV(16 + (bigY << 3) + coY, 15 - ((bigX << 3) + coX),
							Palette[0][pal[pixel
							[(THISROM[offset] & line) != 0]
						[(THISROM[offset + 8] & line) != 0]]]);
					}
				}
			}
		}

		mDC.SelectObject(oldbmp);
		Finger.Add(&bmp, Palette[0][pal[0]]);
	}
	catch (...) {
		ErrorHere << "Unknown exception caught while trying to load the finger icon" << std::endl;
	}

	mDC.DeleteDC();
	bmp.DeleteObject();
}