#pragma once
//TODO - rename this file to FFH2Project_json.h

#include "FFH2Project.h"
#include "nlohmann/nlohmann-integration.hpp"

//=== Enums
NLOHMANN_JSON_SERIALIZE_ENUM(ProjectEditorModuleEntryType, {
	{Editor, "editor"},
	{Subeditor, "subeditor"}
	});


//=== FFHSetting
extern void to_json(ojson& j, const FFHSetting& p);
extern void from_json(const ojson& j, FFHSetting& p);
extern void to_json(ujson& j, const FFHSetting& p);
extern void from_json(const ujson& j, FFHSetting& p);


//=== FFHValue
extern void to_json(ojson& j, const FFHValue& p);
extern void from_json(const ojson& j, FFHValue& p);
extern void to_json(ujson& j, const FFHValue& p);
extern void from_json(const ujson& j, FFHValue& p);


//=== Ordered macro declarations
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectHeader, name, version, pragma);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectInfo, type, publishTo, asmdll, additionalModulesPath, cleanfile, workrom);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectStrings, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectSession,
	suppressWarnOnAssemblyMismatch, textViewInDTE,
	showLastClick, drawDomainGrid, tintVariant,
	tintTiles, smartTools);

EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectEditorModuleEntry, id, slotName, sourcePath, type, settings);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectEditorModules, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectValues, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueElemTypeDesc, name, sizeInBytes, hardcoded, desc);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueLabel, label, desc);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueElement, type, hexoffset, paramindex, comment);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueTalkHandler, name, desc, bankaddr, elements);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogueTalkHandlers, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, ProjectDialogue, valueTypes, labels, handlers);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ojson, FFH2Project, ffheader, info, strings, session, palette, tables, modules, values, dialogue);


//=== Unordered macro declarations
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectHeader, name, version, pragma);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectInfo, type, publishTo, asmdll, additionalModulesPath, cleanfile, workrom);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectStrings, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectSession,
	suppressWarnOnAssemblyMismatch, textViewInDTE,
	showLastClick, drawDomainGrid, tintVariant,
	tintTiles, smartTools);

EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectEditorModuleEntry, id, slotName, sourcePath, type, settings);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectEditorModules, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectValues, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueElemTypeDesc, name, sizeInBytes, hardcoded, desc);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueLabel, label, desc);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueElement, type, hexoffset, paramindex, comment);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueTalkHandler, desc, bankaddr, elements);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogueTalkHandlers, order, entries);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, ProjectDialogue, valueTypes, labels, handlers);
EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ujson, FFH2Project, ffheader, info, strings, session, palette, tables, modules, values, dialogue);

