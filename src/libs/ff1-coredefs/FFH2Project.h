#pragma once

#define SWITCH_OLDFFH_PTR_CHECK(p) if ((p) != nullptr) throw std::runtime_error(__FUNCTION__ ": Switch editor\nfrom CFFHacksterProject\nto FFH2Project (Proj2)");
#define MUST_SPECIFY_PROJECT(proj,edName) if (proj == nullptr) std::runtime_error(std::string(edName) + " must specify a project.")

#ifndef THROW_FFPROJECT_ERROR
	#include <string>
	inline void ThrowOldFfProjectError(std::string filename, int line, std::string function) \
	{ throw std::runtime_error(filename + "(" + std::to_string(line) + "): " + function + " - switch to using FFH2Project"); }

	#define THROW_FFPROJECT_ERROR ThrowOldFfProjectError(__FILE__, __LINE__, __FUNCTION__)
#endif

#define FFH_THROW_OLD_FFHACKSTERPROJ(p) SWITCH_OLDFFH_PTR_CHECK((p))
#define FFH_THROW_NULL_PROJECT(proj,edName) MUST_SPECIFY_PROJECT((proj),(edName))
#define FFH_SWITCH_TO_FFH2 THROW_FFPROJECT_ERROR

#include "FFHSetting.h"
#include "FFHValue.h"
#include "FFHImages.h"
#include "pair_result.h"
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <array>
class AppSettings;

struct ProjectHeader
{
	std::string name;
	std::string version;
	std::string pragma;
};

struct ProjectInfo
{
	std::string type;
	std::string publishTo;
	std::string asmdll;
	std::string additionalModulesPath;
	std::string cleanfile;
	std::string workrom;
};

struct ProjectStrings
{
	std::vector<std::string> order;
	std::map<std::string, std::vector<std::string>> entries;
};

struct ProjectSession
{
	bool suppressWarnOnAssemblyMismatch = false;;
	std::vector<bool> textViewInDTE;
	bool showLastClick = true;
	bool drawDomainGrid = true;
	int tintVariant = 32;
	std::vector<std::vector<std::int8_t>> tintTiles;
	std::vector<std::vector<std::int8_t>> smartTools;
};

enum ProjectEditorModuleEntryType
{
	Editor = 0, Subeditor
};

struct ProjectEditorModuleEntry //TODO- rename, maybe ProjectExtensionEntry?
{
	std::string id;
	std::string slotName; //TODO - rename this to just name?
	std::string sourcePath;
	ProjectEditorModuleEntryType type;
	std::map<std::string, FFHSetting> settings;

	FFHSetting& GetSetting(const std::string& name);
};

struct ProjectEditorModules
{
	std::vector<std::string> order;
	std::map<std::string, ProjectEditorModuleEntry> entries;
};

struct ProjectValues
{
	std::map<std::string, FFHValue> entries;
};

struct ProjectDialogueElemTypeDesc
{
	std::string name;
	int sizeInBytes;
	bool hardcoded = false;
	std::string desc;
};

struct ProjectDialogueLabel
{
	std::string label;
	std::string desc;
};

struct ProjectDialogueElement
{
	std::string type;
	std::string hexoffset;  // offset from the handler's bankaddr in hex, 0x12ABC //TODO - rename to bank offset and type as int
	int paramindex;         // -1 if hardcoded
	std::string comment;

	bool isHardcoded() const { return type.find("hc") == 0; }
};

struct ProjectDialogueTalkHandler
{
	std::string name;
	std::string desc;
	std::string bankaddr;   // addr format, e.g. $12AB //TODO - change to int and define to_json/from_json for this struct
	std::vector<ProjectDialogueElement> elements;
};

struct ProjectDialogueTalkHandlers
{
	std::vector<std::string> order;
	std::map<std::string, ProjectDialogueTalkHandler> entries;
};

struct ProjectDialogue
{
	std::vector<ProjectDialogueElemTypeDesc> valueTypes;
	std::map<std::string, ProjectDialogueLabel> labels;
	ProjectDialogueTalkHandlers handlers;
};

//TODO - not used yet
//struct AsmFilePair
//{
//	std::string originalpath;
//	std::string workingpath;
//	bool keep = false; // by default, changes will not be kept
//};
//using AsmFileSet = std::map<std::string, AsmFilePair>; // maps shortname to filepath

using nespalettearrays = std::array<std::array<unsigned long, 65>, 9>; //DEVNOTE - use unsigned long instead of COLORREF here?

//struct FFHImages
//{
//public:
//	FFHImages();
//	~FFHImages();
//	FFHImages(const FFHImages& rhs);
//	FFHImages& operator=(const FFHImages& rhs);
//
//	operator CImageList& ();
//
//	std::unique_ptr<CImageList> images;
//};

class FFH2Project
{
public:
	FFH2Project();
	~FFH2Project();

	void Load(std::string projectpath);
	void Save(std::string altenateprojectpath = "");

	// === Non-serialized members (Runtime-only)
	std::string ProjectPath;
	std::string ProjectFolder;
	std::string WorkRomPath;
	AppSettings* AppSettings = nullptr;
	std::map<std::string, int> m_varmap;
	//TODO - wrap/hide these imagelists if possible
	//--
	FFHImages Finger; //TODO - either move this out or add a copy ctor...

	//TODO - do these images lists have to be part of the project? or can they live in the respective editors and
	//		initialize on edit? Overworld tiles does exactly that every time it loads.
	std::vector<FFHImages> m_vstandardtiles;
	FFHImages m_overworldtiles;
	//--

	enum { FOLLOWUPS = 0x80 };
	BYTE TeleportFollowup[FOLLOWUPS][3] = { 0 };
	BYTE curFollowup = 0;
	BYTE maxFollowup = 0;
	std::vector<bool> OK_tiles;
	bool OK_overworldtiles = false;

	std::vector<unsigned char> ROM;
	//AsmFileSet AsmFiles; //TODO - not used yet

	//TODO - make these a separate class, e.g. FFH2ProjectData? ROM and ASM files aren't serialized with it
	// === Serialized members
	ProjectHeader ffheader;
	ProjectInfo info;
	ProjectStrings strings;
	ProjectSession session;
	nespalettearrays palette;
	std::array<std::array<std::string, 256>, 2> tables;
	ProjectEditorModules modules;
	ProjectValues values;
	ProjectDialogue dialogue;

	// Methods
	bool IsRom() const;
	bool IsAsm() const;
	std::string* GetTable(int index);
	bool ClearROM();
	void LoadROM();
	void SaveROM();
	bool UpdateVarsAndConstants();
	void ReTintPalette();

	FFHValue& GetValue(const std::string& name);
	ProjectEditorModuleEntry& GetModule(const std::string& name);
	ProjectDialogueTalkHandler& GetHandler(const std::string& name);

	CImageList& GetStandardTiles(size_t index, bool showrooms);
	CImageList& GetStandardTiles(size_t index, int showroomsindex);

private:
	void LoadFinger();
	void InitMapVars();
	void DeleteStandardTiles();
};
