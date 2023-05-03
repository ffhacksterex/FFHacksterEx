#pragma once

#include "pair_result.h"
#include "collection_helpers.h"
#include <vector>
#include "conditional_build_symbols.h"
#include "core_exceptions.h"

class IProgress;
class AppSettings;

#define PROJECTSECTION             "PROJECT"
#define FILESSECTION               "FILES"
#define REFSECTION                 "REF"

// use CFFHacksterProject Get/SetIniFilePath to access these
#define FFHFILE_ValuesPath         "values"
#define FFHFILE_RevertValuesPath   "revertvals"
#define FFHFILE_StringsPath        "strings"
#define FFHFILE_EditorSettingsPath "editorsettings"
#define FFHFILE_PalettePath        "nespal"
#define FFHFILE_StdTablePath       "stdtable"
#define FFHFILE_DteTablePath       "dtetable"
#define FFHFILE_DialoguePath       "dialogue"

// use CFFHacksterProject Get/SetIniRefDir to access these
#define FFHREFDIR_AsmDLLPath       "asmdll"
#define FFHREFDIR_AddlModFolder    "addlmodpath"
#define FFHREFDIR_Publish          "publish"


class mark_deprecated CFFHacksterProject
{
	friend class ProjectLoader;
	friend class ProjectLoader_ini;
public:
	CFFHacksterProject();
	~CFFHacksterProject();

	static const int Version;

	static int ReadProjectVersion(CString inifile);
	static void WriteProjectVersion(CString inifile, int version);

	pair_result<CString> LoadFileRefs(CString projectini);

	bool SaveRefPaths();
	void LoadSharedSettings();
	void SaveSharedSettings();
	bool Clone(CString newprojectfile);

	bool IsRom() const;
	bool IsAsm() const;

	CString* GetTable(int index);
	bool ClearROM();
	void ReTintPalette();

	bool CanRevert() const;
	pair_result<CString>  ImportHacksterDAT(CString hacksterfile);
	pair_result<CString> Revert();
	pair_result<CString> Compile();
	bool Publish();
	//pair_result<CString> RevertValues(); //TODO - REMOVED FOR NOW, See CPP DEFINITION for more info
	bool LoadVariablesAndConstants(IProgress * progress = nullptr);
	bool UpdateVarsAndConstants();

	bool ShouldWarnAssemblyDllMismatch() const;

	CString GetContentFolder() const;

	CString GetIniFilePath(CString key) const;
	static CString GetIniFilePath(CString projectini, CString key);
	void SetIniFilePath(CString key, CString newpath);
	static void SetIniFilePath(CString projectini, CString key, CString newpath);

	CString DecodePathnameIfInKnownFolder(CString pathname) const;
	static CString DecodePathnameIfInKnownFolder(CString pathname, CString projectdir);
	CString EncodePathnameIfInKnownFolder(CString pathname) const;
	static CString EncodePathnameIfInKnownFolder(CString pathname, CString projectdir);

	CString GetIniRefDir(CString key);
	static CString GetIniRefDir(CString projectini, CString key);
	void SetIniRefDir(CString key, CString newvalue);
	static void SetIniRefDir(CString projectini, CString key, CString newvalue);

	static bool IsAsm(CString projectini);
	static bool RenameProjectFiles(CString projectpath, CString newprojectfile, CString srctitle, CString desttitle);
	static bool WriteStringCounts(CString projectini);

	CImageList & GetStandardTiles(size_t index, bool showrooms);
	CImageList & GetStandardTiles(size_t index, int showroomsindex);

	AppSettings* AppSettings = nullptr;

	std::vector<unsigned char> ROM;
	size_t ROMSize;

	CString ProjectFolder;
	CString ProjectPath;

	CString ProjectName;
	CString ProjectTypeName;
	CString RevertGameDataPath;      // only reverts game data using the backup assembly folder or ROM
	CString WorkRomPath;

	CString ValuesPath;
	CString RevertValuesPath;       // only reverts the values ini
	CString StringsPath;
	CString EditorSettingsPath;
	CString PalettePath;
	CString StdTablePath;
	CString DteTablePath;
	CString DialoguePath;

	// REFDIR paths
	// In the INI file, these can be fullpaths or ref placeholder paths (e.g. *APP*\filename.ext)
	// At runtime, they're always full paths.
	//TODO - make these private (or remove the altogether) and use Set/GetIniRefDir
	CString PublishRomPath;
	CString AsmDLLPath;
	CString AdditionalModulesFolder;

	bool SuppressWarnOnAssemblyMismatch = false;

	stdstrintmap m_varmap;

	enum { TILEMAPS = 61 };
	enum { FOLLOWUPS = 0x80 };
	enum { DEFTINT_VARIANT = 32 };

	COLORREF Palette[9][65];        //64 NES colors (plus transparency)... 9 sets
	                                // 0 = normal
	                                // 1 = tint black
	                                // 2 = tint blue
	                                // 3 = tint green
	                                // 4 = tint red
	                                // 5 = tint blue-green
	                                // 6 = tint purple
	                                // 7 = tint yellow
	                                // 8 = tint white
	CString Tables[2][256];         //Standard and DTE tables
	CImageList Finger;

	CImageList m_overworldtiles;
	bool OK_overworldtiles = false;
	std::vector<bool> OK_tiles;

	BYTE TeleportFollowup[FOLLOWUPS][3] = { 0 };
	BYTE curFollowup = 0;
	BYTE maxFollowup = 0;

	boolvector TextViewInDTE;
	bool ShowLastClick = false;
	bool DrawDomainGrid = false;
	BYTE TintVariant = DEFTINT_VARIANT;
	BYTE TintTiles[9][128] = { 0 };
	BYTE SmartTools[19][9] = { 0 };

private:
	static const CString SETTINGS;
	static const CString CLASSES;
	static const CString ENEMIES;
	static const CString MAGIC;

	static const bool DEF_TEXTVIEWINDTE[20];

	enum ProjType { Unknown = -1, None = 0, Rom, Asm };
	ProjType m_projtype;

	std::vector<CImageList*> m_vstandardtiles;

	static void LoadTable(CString table[256], FILE* file);
	static ProjType TextToProjType(CString text);

	std::vector<CString> ReadIniLabels(CString inifile, CString section);
	void WriteIniLabels(const std::vector<CString>& labels, CString inifile, CString section, CString keyprefix);

	bool IsProjectTypeValid() const;
	CString LoadCartData();
	void DeleteStandardTiles();
	void LoadFinger();
};
