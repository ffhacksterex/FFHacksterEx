#pragma once
//TODO - rename this file to FFH2Project_json.h

#include "FFH2Project.h"
#include "nlohmann/nlohmann-integration.hpp"

//=== Enums
NLOHMANN_JSON_SERIALIZE_ENUM(ProjectEditorModuleEntryType, {
	{Editor, "editor"},
	{Subeditor, "subeditor"}
	});


//=== FFHSettingValue
extern void to_json(ojson& j, const FFHSettingValue& p);
extern void from_json(const ojson& j, FFHSettingValue& p);
extern void to_json(ujson& j, const FFHSettingValue& p);
extern void from_json(const ujson& j, FFHSettingValue& p);


//=== FFHDataValue
extern void to_json(ojson& j, const FFHDataValue& p);
extern void from_json(const ojson& j, FFHDataValue& p);
extern void to_json(ujson& j, const FFHDataValue& p);
extern void from_json(const ujson& j, FFHDataValue& p);


//=== Ordered macro declarations
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectHeader, name, version, pragma);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectInfo, type, publishTo, additionalModulesPath, cleanfile, workrom);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectStrings, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectSession,
	suppressWarnOnAssemblyMismatch, textViewInDTE,
	showLastClick, drawDomainGrid, tintVariant,
	tintTiles, smartTools);

EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectData, palette, tables);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectEditorModuleEntry, id, slotName, sourcePath, type, settings);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectEditorModules, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectValues, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueElemTypeDesc, name, sizeInBytes, hardcoded, desc);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueLabel, label, desc);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueElement, type, hexoffset, paramindex, comment);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueTalkHandler, desc, bankaddr, elements);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueTalkHandlers, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogue, valueTypes, labels, handlers);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, FFH2Project, ffheader, info, strings, session, data, modules, values, dialogue);


//=== Unordered macro declarations
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectHeader, name, version, pragma);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectInfo, type);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectStrings, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectSession,
	suppressWarnOnAssemblyMismatch, textViewInDTE,
	showLastClick, drawDomainGrid, tintVariant,
	tintTiles, smartTools);

EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectData, palette, tables);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectEditorModuleEntry, id, slotName, sourcePath, type, settings);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectEditorModules, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectValues, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueElemTypeDesc, name, sizeInBytes, hardcoded, desc);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueLabel, label, desc);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueElement, type, hexoffset, paramindex, comment);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueTalkHandler, desc, bankaddr, elements);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueTalkHandlers, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogue, valueTypes, labels, handlers);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, FFH2Project, ffheader, info, strings, session, data, modules, values, dialogue);

