#pragma once
#include "pair_result.h"
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <array>

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

struct ProjectData
{
	std::vector<unsigned char> palette;
	std::array<std::array<std::string, 256>, 2> tables;
};

enum ProjectEditorModuleEntryType
{
	Editor = 0, Subeditor
};

struct FFHSettingValue
{
	std::string type;
	std::string format;
	std::string data;
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

struct FFHDataValue {
	std::string type;
	std::string format;
	std::string data;
	std::string label;
	std::string desc;
	std::string group;
	bool internal = false;
	bool readonly = false;
	bool hidden = false;
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

class FFH2Project
{
public:
	FFH2Project();
	~FFH2Project();

	void Load(CString filepath);

	ProjectHeader ffheader; //TODO - rename to ffhHeader, might help for version scraping
	ProjectInfo info;
	ProjectStrings strings;
	ProjectSession session;
	ProjectData data;
	ProjectEditorModules modules;
	ProjectValues values;
	ProjectDialogue dialogue;
};
