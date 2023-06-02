// This file defines a one-time translation from v0.9.8.1 (INI files).
//TODO- define mext version
// <FIRST_JSON_VERSION> uses a JSON file instead of a collection of INI files.
// The pal and tbl files are stored as arrays of bytes.

#include "stdafx.h"
#include "upgrade_functions.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "string_functions.h"
#include "FFH2Project.h"
#include "FFH2Project_IniToJson_Defs.hpp"
#include <fstream>
#include <sstream>

// Internal support routines
namespace {
	struct TypeMapping {
		std::string type;
		std::string format;
	};
	std::map<std::string, TypeMapping> typeMappings = {
		{ "int", { "int", "%d" }},
		{ "bool", { "bool", "a" }},
		{ "str", { "str", "" }},
		{ "addr", { "addr", "$%04X" }},
		{ "rgb", { "rgb", "0x%06X" }},
		{ "hex", { "hex", "0x%02X" }},
		{ "hex8", { "hex", "0x%02X" }},
		{ "hex16", { "hex", "0x%04X" }},
		{ "hex24", { "hex", "0x%06X" }},
		{ "hex32", { "hex", "0x%08X" }},
	};
	std::map<std::string, std::string> typeMapAliases = {
		{ "dec", "int" }, { "int8", "int" }, { "int16", "int" }, { "int24", "int" }, { "int32", "int" },
		{ "uint8", "int" }, { "uint16", "int" }, { "uint24", "int" }, { "uint32", "int" },
		{ "opcode", "addr" },
		{ "byte", "hex8" }, { "word", "hex16" }, { "bword", "hex24" }, { "dword", "hex32" }
	};

	void ResolveWorkingFile(FFH2Project& p, CString inipath, CString newprojectname);
	void ResolveStrings(CString inipath, FFH2Project& p);
	void ResolveEditorSettings(CString inipath, FFH2Project& p);
	void ResolveValues(CString inipath, FFH2Project& p);
	void ResolveDialogueSettings(CString inipath, FFH2Project& p);

	CString GetSubfilePath(CString inipath, CString key);
	void ReadTable(CString tablepath, std::array<std::string, 256>& table);
	void ReadPalette(CString palpath, nespalettearrays & palette, CString inipath);

	bytevector ReadBinary(CString palpath);
	std::vector<std::vector<std::int8_t>> ConvertBytes(CString inipath, CString section, CString key, int rows, int cols);

	std::string RemapEditorName(const stdstrstrmap& mappings, CString settingsname);
	mfcstringset ReadRefList(CString inipath);
	std::string GetBaseType(std::string type);
	TypeMapping GetTypeMapping(std::string type);
	std::string DeduceEditorSettingsTypeEx(std::string initype);
	std::string ConvertIniTypedTermEx(const std::string& type, const std::string& value);
	std::string FormatJsonValueForTypeEx(std::string key, std::string type, std::string format, std::string value);
}

namespace Upgrades
{
	bool IsIniFormat(CString projectpath)
	{
		auto ininame = Ini::ReadIni(projectpath, "PROJECT", "name", "");
		return !ininame.IsEmpty();
	}

	//TODO - implement semantic version compare function inetad of doing this going forward.
	const std::set<std::string> sourceVersions = { "9810" };

	std::string IniToJson(CString inipath)
	{
		std::string oldver = (LPCSTR)Ini::ReadIni(inipath, "PROJECT", "version", "");
		if (sourceVersions.find(oldver) == cend(sourceVersions))
			throw std::runtime_error("Upgrades must come from version v0.9.8.1: this file is " + oldver + ".");

		const std::string FIRST_JSON_VERSION = "0.9.8.2"; //TODO - define this version, maybe save 0.9.9.0 for the ASM change?

		FFH2Project p;
		p.ffheader.name = Ini::ReadIni(inipath, "PROJECT", "name", "");
		p.ffheader.version = FIRST_JSON_VERSION;
		p.ffheader.pragma = "forceReset";

		p.info.type = Ini::ReadIni(inipath, "PROJECT", "type", "");
		p.info.publishTo = Ini::ReadIni(inipath, "REF", "publish", "");
		p.info.additionalModulesPath = Ini::ReadIni(inipath, "REF", "addlmodpath", "");
		p.info.asmdll = Ini::ReadIni(inipath, "REF", "asmdll", "");
		ResolveWorkingFile(p, inipath, ("new - " + p.ffheader.name).c_str());

		p.session.suppressWarnOnAssemblyMismatch = Ini::ReadIniBool(inipath, "SETTINGS", "SuppressWarnOnAssemblyMismatch", false);
		p.session.textViewInDTE = Ini::ReadIni(inipath, "SETTINGS", "TextViewInDTE", std::vector<bool>(20, false));
		p.session.showLastClick = Ini::ReadIniBool(inipath, "SETTINGS", "ShowLastClick", false);
		p.session.drawDomainGrid = Ini::ReadIniBool(inipath, "SETTINGS", "DrawDomainGrid", false);
		p.session.tintVariant = atol(Ini::ReadIni(inipath, "SETTINGS", "TintVariant", "32"));
		p.session.tintTiles = ConvertBytes(inipath, "SETTINGS", "TintTiles", 9, 128);
		p.session.smartTools = ConvertBytes(inipath, "SETTINGS", "SmartTools", 19, 9);

		ResolveStrings(inipath, p);
		ResolveEditorSettings(inipath, p);
		ResolveValues(inipath, p);
		ResolveDialogueSettings(inipath, p);

		auto stdtblpath = GetSubfilePath(inipath, "stdtable");
		ReadTable(stdtblpath, p.tables[0]);
		auto dtetblpath = GetSubfilePath(inipath, "dtetable");
		ReadTable(dtetblpath, p.tables[1]);
		auto palpath = GetSubfilePath(inipath, "nespal");
		ReadPalette(palpath, p.palette, inipath);

		// Write out the object as JSON
		std::string ext = (LPCSTR)Paths::GetFileExtension(inipath);
		CString newext;
		if (ext == ".ff1rom") newext = ".rom.ffh";
		else if (ext == ".ff1asm") newext = ".asm.ffh";
		else throw std::runtime_error("Can't upgrade unsupported old extension '" + ext + "', expected .f1rom or .ff1asm");

		std::string projectpath = (LPCSTR)Paths::ReplaceExtension(inipath, newext);

		ojson j = p;
		std::ofstream ofs(projectpath);
		ofs << std::setw(1) << j;
		return projectpath;
	}
}

namespace {

	void ResolveWorkingFile(FFH2Project& p, CString inipath, CString newprojectname)
	{
		//[PROJECT]
		//revert=UpgadeTest.rom.clean
		//workrom=UpgadeTest.work.nes

		//DEVNOTE - no check is made or action taken if these files don't exist.
		p.info.cleanfile = Ini::ReadIni(inipath, "PROJECT", "revert", newprojectname + ".clean");
		p.info.workrom = Ini::ReadIni(inipath, "PROJECT", "workrom", newprojectname + ".work.nes");
	}

	void ResolveStrings(CString inipath, FFH2Project& p)
	{
		// Go through each group and copy its strings into the array

		auto strfname = Ini::ReadIni(inipath, "FILES", "strings", "");
		auto stringpath = Paths::ReplaceFileName(inipath, strfname);
		auto keys = Ini::ReadIniKeyNames(inipath, "STRINGCOUNTS");

		for (const auto& key : keys) {
			auto cslabel = key;
			cslabel.MakeLower();
			auto label = ffh::str::tostd(key);

			if (p.strings.entries.find(label) != cend(p.strings.entries))
				throw std::domain_error("Project strings cannot add a duplicate label named '" + label + "'.");

			p.strings.order.push_back(label);

			auto valkeys = Ini::ReadIniKeyNames(stringpath, key);
			auto& v = (p.strings.entries[label] = std::vector<std::string>());
			v.reserve(valkeys.size());
			for (const auto& vk : valkeys) {
				auto value = Ini::ReadIni(stringpath, key, vk, "");
				v.push_back((LPCSTR)value);
			}
		}
	}

	void ResolveEditorSettings(CString inipath, FFH2Project& p)
	{
		// The editorsettings file contains two sets of sections:
		// [EDITOR-someeditor]
		// name=someeditor
		// modulepath=...
		// 
		// [SOMEEDITOR]
		// key1=27
		// key2=Some text.
		// ...

		// This function creates an editor entry by stripping "EDITOR-" from the module section name,
		// then adds the keys from the corresponding editor settings section to it.
		// "someeditor": {
		//   "settings": {
		//     "key1": { "type": "int", "format": "", "data": 27 },
		//     "key2": { "type": "str", "format": "", "data": "Some text." }
		//   }
		// }

		static const char* const modmarker = "EDITOR-";
		static const size_t modlen = strlen(modmarker);
		static auto isMod = [](const CString name) { return name.Find(modmarker) == 0; };
		static auto isNotMod = [](const CString name) { return name.Find(modmarker) != 0; };

		auto edpath = GetSubfilePath(inipath, "editorsettings");
		mfcstringvector modnames, stgnames;

		// Collect the names of the editor modules and editor settings into separate lists
		{
			auto allsects = Ini::ReadIniSectionNames(edpath);
			std::copy_if(cbegin(allsects), cend(allsects), std::back_inserter(modnames), isMod);
			std::copy_if(cbegin(allsects), cend(allsects), std::back_inserter(stgnames), isNotMod);
		}

		// Add the modules to the editors collection first
		for (const auto& sect : modnames) {
			// Remove the "EDITOR-" portion to form the name of the editor property
			std::string modkey = (LPCSTR)sect;
			modkey.erase(0, modlen);
			if (modkey.empty())
				throw std::runtime_error("Module names must not be empty.");

			// If it's not already there, add the entry and its order.
			if (p.modules.entries.find(modkey) == end(p.modules.entries)) {
				p.modules.order.push_back(modkey);
				p.modules.entries[modkey] = {};
			}

			//NOTE: The extension's display name is not specified in these settings since
			//	it's supplied at runtime by the extension itself.
			auto& themod = p.modules.entries[modkey];
			themod.type = ProjectEditorModuleEntryType::Editor;
			themod.id = modkey;
			themod.slotName = (LPCSTR)Ini::ReadIni(edpath, sect, "name", "");
			themod.sourcePath = (LPCSTR)Ini::ReadIni(edpath, sect, "modulepath", "");

			if (themod.slotName.empty())
				throw std::runtime_error(modkey + " module must not have a blank name.");
			if (themod.sourcePath.empty())
				throw std::runtime_error(modkey + " module must not have a blank module path.");
		}

		// Some editor module section names might not match the editor settings section name.
		// A builtin example is module [EDITOR-battles] vs settings [BATTLE].
		// Provide a mapping from the settings name to the new editor property name.
		//TODO - put this into a file (add it as a template) to allow custom mappings
		//		without having to rebuild the app. Could be JSON or JsonLines, but avoid INI.
		std::map<std::string, std::string> namemapHandleToSection = { { "battle", "battles" } };
		std::vector<std::string> mismatches;

		// Add the editor settings to the editors created above.
		for (CString mfcstgname : stgnames) {
			// Remap known mismatches of module vs settings section names
			auto stgname = RemapEditorName(namemapHandleToSection, mfcstgname);

			// If this setting name exist in the modules collection, add its settings now.
			// e.g. add keys from INI section [ARMOR] to editor "armor".
			// the name mappings above allow adding keys from section [BATTLE] to editor "battles".
			auto it = p.modules.entries.find(stgname);
			if (it != cend(p.modules.entries)) {
				auto& stg = it->second.settings;
				auto keys = Ini::ReadIniKeyNames(edpath, mfcstgname);
				for (const auto& key : keys) {
					// We have to deduce the type from the value since editor settings doesn't specify it.
					std::string value = (LPCSTR)Ini::ReadIni(edpath, mfcstgname, key, "");
					auto initype = DeduceEditorSettingsTypeEx(value);
					auto tmap = GetTypeMapping(initype);
					auto formattedValue = FormatJsonValueForTypeEx((LPCSTR)key, tmap.type, tmap.format, value);
					auto& s = (stg[(LPCSTR)key] = {});
					s.name = (LPCSTR)key;
					s.type = tmap.type;
					s.format = tmap.format;
					s.data = ojson::parse(formattedValue).dump(2);
				}
			}
			else {
				LogHere << "Can't find matching module for editor settings section: " << stgname << std::endl;
				mismatches.push_back("Can't find matching module for editor settings section: " + stgname);
			}
		}

		if (!mismatches.empty()) {
			std::ostringstream ostr;
			for (const auto& m : mismatches) ostr << "\n" << m;
			throw std::runtime_error("Found some unmatched editor settings:" + ostr.str());
		}
	}

	void ResolveValues(CString inipath, FFH2Project& p)
	{
		auto valpath = GetSubfilePath(inipath, "values");

		// values have the following values, some are optional
			//[VALUE_NAME]
			//value=0x40010
			//desc=Expected size of the ROM being edited
			//label = Some label text
			//type=bword
			//group=system
			//reserved=true
			//ishidden=true

		// also might need to interpret the following
			//[RESERVED_LISTS]
			//refonly=true
			//ROMINIT_VARNAMES_SECTIONS=1
			//RAM_NAMES=1
			//EDITOR_NAME_FILTER=1
			//[RESERVED_FILTERS]
			//refonly=true
			//PREFIX_FILTER=1

		// Ref only values are NOT displayed on the values edito panel.
		// The RESERVED_LISTS and RESERVED_FILTERS indicate which sections to exclude.
		// The idea was that an expert mode could be used to access these values.
		// These are effectively std::map<std::string, int> where
		//		key = section name to treat as refonly
		//		value = 1 (it's a placeholder)

		// RESERVED_LISTS and RESERVED_FILTERS are locked values
		std::set<CString> exclusions = { "RESERVED_LISTS", "RESERVED_FILTERS" };
		auto refnames = ReadRefList(valpath);
		auto sections = Ini::ReadIniSectionNames(valpath);
		for (const auto& section : sections)
		{
			if (exclusions.find(section) != cend(exclusions)) continue;

			FFHValue& f = (p.values.entries[(LPCSTR)section] = {});
			f.name = (LPCSTR)section;
			f.desc = Ini::ReadIni(valpath, section, "desc", "");
			f.label = Ini::ReadIni(valpath, section, "label", "");
			f.group = Ini::ReadIni(valpath, section, "group", "");
			//TODO - replace readonly and internal with locked?
			f.readonly = Ini::ReadIniBool(valpath, section, "reserved", false);
			f.hidden = Ini::ReadIniBool(valpath, section, "ishidden", false);
			f.internal = (refnames.find(section) == cend(refnames));

			// This differs from EditorSettings in that the type here is explicitly specified.
			std::string value = (LPCSTR)Ini::ReadIni(valpath, section, "value", "");
			std::string initype = (LPCSTR)Ini::ReadIni(valpath, section, "type", "");
			if (initype == "byte[]")
				initype = initype;
			auto tmap = GetTypeMapping(initype);
			auto formattedValue = FormatJsonValueForTypeEx((LPCSTR)section, tmap.type, tmap.format, value);
			f.type = tmap.type;
			f.format = tmap.format;
			f.data = formattedValue;
		}
	}

	void ResolveDialogueSettings(CString inipath, FFH2Project& p)
	{
		auto dlgpath = GetSubfilePath(inipath, "dialogue");

		auto vtypekeys = Ini::ReadIniKeyNames(dlgpath, "VALUETYPES");
		p.dialogue.valueTypes.reserve(vtypekeys.size());
		for (const auto& vkey : vtypekeys) {
			std::string cvtype = (LPCSTR)Ini::ReadIni(dlgpath, "VALUETYPES", vkey, "");
			auto parts = Strings::split(cvtype, "|");

			//TODO - develop a warning data payload instead, maybe use nlohmann::json for this?
			if (parts.size() != 2 || parts[0].empty() || parts[1].empty()) {
				//throw std::runtime_error("Malformed valuetype cannot be converted: " + vkey);
				continue;
			}

			p.dialogue.valueTypes.push_back({});
			ProjectDialogueElemTypeDesc& elem = p.dialogue.valueTypes.back();
			elem.name = vkey;
			elem.sizeInBytes = atol(parts[0].c_str()) / 8;
			elem.hardcoded = vkey.Find("hc") == 0;
			elem.desc = parts[1];
		}

		const auto labelSections = { "Label_TalkJumpTable", "Label_EndParsingMarker" };
		for (const auto& sect : labelSections) {
			ProjectDialogueLabel& lbl = (p.dialogue.labels[(LPCSTR)sect] = {});
			lbl.label = (LPCSTR)Ini::ReadIni(dlgpath, sect, "value", "");
			lbl.desc = (LPCSTR)Ini::ReadIni(dlgpath, sect, "desc", "");
		}

		const std::set<CString> excludedLabels = { "VALUETYPES", "Label_TalkJumpTable", "Label_EndParsingMarker" };
		auto talksections = Ini::ReadIniSectionNames(dlgpath);
		for (const auto& sect : talksections) {
			std::string name = (LPCSTR)sect;
			if (excludedLabels.find(sect) != cend(excludedLabels))
				continue;
			if (p.dialogue.handlers.entries.find(name) != cend(p.dialogue.handlers.entries))
				throw std::runtime_error("Duplicate dialog handler entry '" + name + "' is not allowed.");

			p.dialogue.handlers.order.push_back(name);

			ProjectDialogueTalkHandler& talk = (p.dialogue.handlers.entries[name] = {});
			talk.name = name;
			talk.desc = (LPCSTR)Ini::ReadIni(dlgpath, sect, "desc", "");
			talk.bankaddr = (LPCSTR)Ini::ReadIni(dlgpath, sect, "bankaddr", "");

			// start at "elem0", loop up until there no element is found (elem1, elem2, etc.)
			const int TALKELEM_EXPECTED_FIELD_COUNT = 4;
			CString kidx;
			std::string kelem = "init";
			for (size_t idx = 0; ; ++idx) {
				kidx.Format("elem%d", idx);
				//TODO - implement ProjectDialogueElement
				kelem = (LPCSTR)Ini::ReadIni(dlgpath, sect, kidx, "");
				if (kelem.empty())
					break;

				//talk.elements.push_back(kelem);
				//-
				auto parts = Strings::split(kelem, "|");
				if (parts.size() != 4)
					throw std::runtime_error(
						std::string((LPCSTR)("Malformed dialogue element '" + kidx + "/" + kidx + " had "))
						+ std::to_string(parts.size())
						+ " fields instead of the expected " + std::to_string(TALKELEM_EXPECTED_FIELD_COUNT) + ".");
				talk.elements.push_back({});
				auto& talkelem = talk.elements.back();
				talkelem.type = parts[0];
				talkelem.hexoffset = parts[1];
				talkelem.paramindex = atol(parts[2].c_str());
				talkelem.comment = parts[3];
			}
		}
	}

	CString GetSubfilePath(CString inipath, CString key)
	{
		auto strfname = Ini::ReadIni(inipath, "FILES", key, "");
		if (strfname.IsEmpty())
			throw std::domain_error("Cannot find subfile '" + std::string((LPCSTR)key) +
				"' for " + std::string((LPCSTR)inipath));

		auto stringpath = Paths::ReplaceFileName(inipath, strfname);
		return stringpath;
	}

	void ReadTable(CString tablepath, std::array<std::string, 256>& table)
	{
		std::ifstream ifs((LPCSTR)tablepath);
		while (ifs && ifs.good() && !ifs.eof()) {
			std::string key, value;

			// Space is a legit value (e.g. index 0xFF is a single space).
			std::getline(ifs, key, '=');
			std::getline(ifs, value);
			auto index = strtoul(key.c_str(), nullptr, 16);
			table[index] = value;
		}
	}

	void ReadPalette(CString palpath, nespalettearrays& palette, CString inipath)
	{
		auto valpath = GetSubfilePath(inipath, "values");
		const int DEFTRANSPARENTCOLOR = 0xFF00FF;
		const int DEFTRANSPARENTCOLORREPLACEMENT = 0xE600E6;
		int TRANSPARENTCOLOR = Ini::ReadRgb(valpath, "TRANSPARENTCOLOR", DEFTRANSPARENTCOLOR);
		int TRANSPARENTCOLORREPLACEMENT = Ini::ReadRgb(valpath, "TRANSPARENTCOLORREPLACEMENT", DEFTRANSPARENTCOLORREPLACEMENT);

		// The destination palatte is a fixed size 2d array (and NOT a jagged array).
		auto pal = ReadBinary(palpath);
		int co1, co;
		BYTE rgb[3] = {0};
		for (co = 0, co1 = 0; co < 64; co++, co1 += 3) {
			rgb[0] = pal[co1]; rgb[1] = pal[co1 + 1]; rgb[2] = pal[co1 + 2];
			palette[0][co] = RGB(rgb[0], rgb[1], rgb[2]);
			if (palette[0][co] == (COLORREF)TRANSPARENTCOLOR)
				palette[0][co] = TRANSPARENTCOLORREPLACEMENT;
		}
		for (co = 0; co < 9; co++)
			palette[co][0x40] = TRANSPARENTCOLOR;
	}

	bytevector ReadBinary(CString palpath)
	{
		std::ifstream ifs;
		ifs.open((LPCSTR)palpath, std::ios_base::binary | std::ios_base::ate);
		auto len = ifs.tellg();
		bytevector v(len);
		auto buf = address(v);
		ifs.seekg(0);
		ifs.read((char*)buf, len);
		return v;
	}

	std::vector<std::vector<std::int8_t>> ConvertBytes(CString inipath, CString section, CString key, int rows, int cols)
	{
		auto bytes = Ini::ReadIni(inipath, section, key, bytevector());
		auto res = std::vector<std::vector<std::int8_t>>(rows);
		size_t idx = 0;
		for (auto& v : res) {
			v.resize(cols);
			for (int i = 0; i < cols; ++i) {
				v[i] = bytes[idx];
				++idx;
			}
		}
		return res;
	}

	// Map from an MFC string containing settings name to a std::string containing editor name.
	std::string RemapEditorName(const stdstrstrmap& mappings, CString settingsname)
	{
		settingsname.MakeLower();
		std::string stgname = (LPCSTR)settingsname;
		{
			auto it = mappings.find(stgname);
			if (it != cend(mappings))
				stgname = it->second;
		}
		return stgname;
	}

	mfcstringset ReadRefList(CString inipath)
	{
		mfcstringset resnames;
		std::vector<CString> sections = { "RESERVED_LISTS", "RESERVED_FILTERS" };
		for (const auto& section : sections) {
			auto reskeys = Ini::ReadIniKeyNames(inipath, section);
			for (const auto& key : reskeys) {
				auto thenames = Ini::ReadIni(inipath, key, "value", mfcstringvector());
				for (const auto& name : thenames) {
					resnames.insert(name);
				}
			}
		}
		return resnames;
	}

	std::string GetBaseType(std::string type)
	{
		auto sqbr = type.find('[');
		auto basetype = sqbr == std::string::npos ? type : type.substr(0, sqbr);
		return basetype;
	}

	TypeMapping GetTypeMapping(std::string type)
	{
		// Array formatting is fixed, so the type format applies to the elements.
		// if it's an array, each element is treated like it's base type.
		//DEVNOTE - if this becomes a problem, then specify an empty string as the array format,
		//		which forces each array element to use the default format for the array's base type.
		auto basetype = GetBaseType(type);
		auto isarray = type.find("[]") != std::string::npos;
		auto it = typeMapAliases.find(basetype);
		if (it != cend(typeMapAliases))
			basetype = it->second;
		auto itmap = typeMappings.find(basetype);
		if (itmap == cend(typeMappings))
			throw std::runtime_error("No settings type mapping found for type name '" + type + "'.");

		// Make a copy and replace the type if it's an array
		return !isarray
			? itmap->second
			: TypeMapping{ itmap->second.type + "[]", itmap->second.format };
	}

	// Editor settings don't store the type, so deduce the initype from the value.
	// The settings use a limited set of types, so the checks can be relatively small.
	std::string DeduceEditorSettingsTypeEx(std::string value)
	{
		if (value == "true" || value == "false")
			return "bool";

		auto first = !value.empty() ? value.front() : EOF;
		if (isdigit(first))
			return "int";

		if ((value.find("0x") == 0) || first == '$')
			return "hex";
		return "str";
	}

	std::string ConvertIniTypedTermEx(const std::string& initype, const std::string& value)
	{
		// Types that can be directly parsed into JSON
		if (initype == "int" || initype == "bool")
			return value;

		// The rest need to be quoted as strings
		if (initype == "str")
			return '"' + value + '"';
		if (initype == "rgb")
			return '"' + value + '"';
		if (initype == "hex")
			return '"' + value + '"';
		if (initype == "addr")
			return '"' + value + '"';

		throw std::runtime_error("Unsupported destination type '" + initype + "' cannot convert this value to JSON: " + value);
	}

	std::string FormatJsonValueForTypeEx(std::string key, std::string type, std::string format, std::string value)
	{
		// Simple types that can be directly parsed by json and aren't arrays can be returned immediately
		auto sqbr = type.find('[');
		auto basetype = sqbr == std::string::npos ? type : type.substr(0, sqbr);

		bool asis = basetype == "int" || basetype == "bool";
		bool isarray = type.find("[]") != std::string::npos;

		// Otherwise, we'll stream other types to keep the logic simple and in one place.
		std::string rawvalue = value;
		if (isarray) {
			// remove the array wrapper from the value since we've already recorded the type
			auto curly = value.find('{');
			auto rcurly = value.rfind('}');
			if (curly == std::string::npos || rcurly == std::string::npos)
				throw std::runtime_error("Key " + key + "failed to translate from '" + type + "' to a JSON value.");

			rawvalue = value.substr(curly + 1, (rcurly - curly) - 1);
		}

		if (type == "str") {
			// it's a string, return the entire thing surrounded by dquotes
			return '"' + value + '"';
		}

		// This is a series of space-separated values
		std::ostringstream ostr;
		if (isarray) ostr << "[";
		std::istringstream istr(rawvalue);
		istr >> std::ws;

		bool multi = false;
		while (istr && istr.good() && !istr.eof()) {
			std::string term;
			istr >> term >> std::ws;
			auto convterm = ConvertIniTypedTermEx(basetype, term);
			ostr << (multi ? ", " : "") << convterm;
			multi = true;
		}
		if (isarray) ostr << "]";
		auto result = ostr.str();
		return result;
	}

} // end namespace (unnamed)