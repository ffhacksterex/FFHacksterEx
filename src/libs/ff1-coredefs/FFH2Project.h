#pragma once

#define SWITCH_OLDFFH_PTR_CHECK(p) if ((p) != nullptr) throw std::runtime_error(__FUNCTION__ ": Switch editor\nfrom CFFHacksterProject\nto FFH2Project (Proj2)");

//#define STOP_USING_CFFHACKSTERPROJ throw std::runtime_error(__FILE__ "(" + std::to_string(__LINE__) + "): " __FUNCTION__ " - switch to using FFH2Project")
//#define MUST_SPECIFY_PROJECT(ed) std::runtime_error(std::string(ed) + " must specify a project.")

#define MUST_SPECIFY_PROJECT(proj,edName) if (proj == nullptr) std::runtime_error(std::string(edName) + " must specify a project.")


#include "FFHSettingValue.h"
#include "FFHDataValue.h"
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
	std::string slotName;
	std::string sourcePath;
	ProjectEditorModuleEntryType type;
	std::map<std::string, FFHSettingValue> settings;
};

struct ProjectEditorModules
{
	std::vector<std::string> order;
	std::map<std::string, ProjectEditorModuleEntry> entries;
};

struct ProjectValues
{
	std::map<std::string, FFHDataValue> entries;
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
	std::string hexoffset;  // offset from the handler's bankaddr in hex, 0x12ABC
	int paramindex;         // -1 if hardcoded
	std::string comment;
};

struct ProjectDialogueTalkHandler
{
	std::string desc;
	std::string bankaddr;   // addr format, e.g. $12AB
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

	// Non-serialized members (Runtime-only)
	std::string ProjectPath;
	std::string ProjectFolder;
	std::string WorkRomPath;
	AppSettings* AppSettings = nullptr;
	std::map<std::string, int> m_varmap;
	//TODO - wrap/hide these if possible
	//--
	FFHImages Finger; //TODO - either move this out or add a copy ctor...
	std::vector<CImageList*> m_vstandardtiles;
	//--

	std::vector<unsigned char> ROM;
	//AsmFileSet AsmFiles; //TODO - not used yet

	//TODO - make these a separate class, e.g. FFH2ProjectData? ROM and ASM files aren't serialized with it
	// Serialized members
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
	const std::string* GetTable(int index);
	bool ClearROM();
	void LoadROM();
	bool UpdateVarsAndConstants();

private:
	void LoadFinger();
	void DeleteStandardTiles();
};
