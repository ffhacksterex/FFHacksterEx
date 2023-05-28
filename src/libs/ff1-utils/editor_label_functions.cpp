#include "stdafx.h"
#include "editor_label_functions.h"
#include "editor_label_defaults.h"
#include "std_assembly.h"
#include "general_functions.h"
#include "ini_functions.h"
#include <string_conversions.hpp>
#include "type_support.h"
#include "FFHacksterProject.h"
#include <FFH2Project.h>
#include <algorithm>
#include <ValueDataAccessor.h>
#include <vda_std_collections.h>

using ffh::str::tomfc;
using ffh::str::tostd;

using namespace Ini;
using namespace std_assembly::shared;
using namespace Types;

namespace Editorlabels
{

	// ################################################################
	// INTERNAL HELPERS

	namespace {
		const dataintnodevector fanfarevalues{ { "don't play", ASM_FALSE },{ "play", ASM_TRUE },{ "ignore", ASM_IGNORE } };

		// Internal function that actually loads the list and/or combo and returns the formatted string in one step.
		dataintnodevector DoLoadLabels(const std::vector<CString> & strings, int start, int count, bool showindex = false)
		{
			dataintnodevector dvec;
			const int last = start + count - 1;
			for (int co = start; co <= last; co++) {
				CString text;
				if (showindex) text.AppendFormat("%02X: ", co);
				text += strings[co];
				dvec.push_back({text, co});
			}
			return dvec;
		}

		CString FormatIniLabel(CString stringsini, CString section, int index, bool showindex)
		{
			CString key;
			key.Format("%lu", index);

			CString value;
			if (showindex) value.Format("%02X: ", index);
			value.Append(ReadIni(stringsini, section, key, ""));

			return value;
		}

		dataintnodevector LoadIniLabels(CFFHacksterProject & proj, CString section, bool showindex)
		{
			dataintnodevector labels;

			// The string count name mappings are in the project.
			// The counts are in the values file.
			CString countname = ReadIni(proj.ProjectPath, "STRINGCOUNTS", section, "");

			if (countname.IsEmpty()) RAISEEXCEPTION(std::runtime_error, "group %s did not specify a count name", (LPCSTR)section);

			int count = ReadDec(proj.ValuesPath, countname);
			if (count < 1) {
				RAISEEXCEPTION(std::runtime_error, "group '%s' specfied a count name '%s', but that value must be > 0", (LPCSTR)section, (LPCSTR)countname);
			}

			for (int index = 0; index < count; ++index) {
				auto label = FormatIniLabel(proj.StringsPath, section, index, showindex);
				labels.push_back({ label, index });
			}

			if (count != (int)labels.size()) {
				RAISEEXCEPTION(std::runtime_error,
					"count mismatch: group '%s' expects count %d, but read %d items", (LPCSTR)section, count, labels.size());
			}

			return labels;
		}
		
		CString FormatLabel(const std::string & group, const std::string & value, int index, bool showindex)
		{
			CString key;
			key.Format("%lu", index);

			CString csvalue;
			if (showindex) csvalue.Format("%02X: ", index);
			csvalue.Append(value.c_str());
			return csvalue;
		}

		dataintnode LoadIniLabel(CFFHacksterProject & proj, CString section, int index, bool showindex)
		{
			// The string count name mappings are in the project.
			// The counts are in the values file.
			CString countname = ReadIni(proj.ProjectPath, "STRINGCOUNTS", section, "");

			if (countname.IsEmpty()) RAISEEXCEPTION(std::runtime_error, "group %s did not specify a count name", (LPCSTR)section);

			int count = ReadDec(proj.ValuesPath, countname);
			if (index >= count) {
				RAISEEXCEPTION(std::runtime_error,
					"group '%s' specfied a count name '%s', but requested an index (%d) higher than the count (%d)",
					(LPCSTR)section, (LPCSTR)countname, index, count);
			}

			auto value = FormatIniLabel(proj.StringsPath, section, index, showindex);
			return{ value, index };
		}

		void WriteIniLabel(CFFHacksterProject & proj, CString section, int index, CString newvalue)
		{
			// The string count name mappings are in the project.
			// The counts are in the values file.
			CString countname = ReadIni(proj.ProjectPath, "STRINGCOUNTS", section, "");

			if (countname.IsEmpty()) RAISEEXCEPTION(std::runtime_error, "group %s did not specify a count name", (LPCSTR)section);

			int count = ReadDec(proj.ValuesPath, countname);
			if (index >= count) {
				RAISEEXCEPTION(std::runtime_error,
					"group '%s' specfied a count name '%s', but requested an index (%d) higher than the count (%d)",
					(LPCSTR)section, (LPCSTR)countname, index, count);
			}

			CString key;
			key.Format("%lu", index);
			WriteIni(proj.StringsPath, section, key, newvalue);
		}
	}



	// ################################################################
	// COLLECTION READERS

	dataintnodevector LoadWepMagicLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "WEPMAGLABELS", showindex); }
	dataintnodevector LoadAILabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "AILABELS", showindex); }
	dataintnodevector LoadShopLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "SHOPLABELS", showindex); }
	dataintnodevector LoadBattlePatternTableLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "PATTERNTABLELABELS", showindex); }
	dataintnodevector LoadBackdropLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "BACKDROPLABELS", showindex); }
	dataintnodevector LoadONTeleportLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "ONTELEPORTLABELS", showindex); }
	dataintnodevector LoadNNTeleportLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "NNTELEPORTLABELS", showindex); }
	dataintnodevector LoadNOTeleportLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "NOTELEPORTLABELS", showindex); }
	dataintnodevector LoadTreasureLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "TREASURELABELS", showindex); }
	dataintnodevector LoadMapLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "STDMAPLABELS", showindex); }
	dataintnodevector LoadTilesetLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "TILESETLABELS", showindex); }
	dataintnodevector LoadSpriteLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "SPRITELABELS", showindex); }
	dataintnodevector LoadSpriteGraphicLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "SPRITEGRAPHICLABELS", showindex); }
	dataintnodevector LoadBattleLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "BATTLELABELS", showindex); }

	dataintnodevector LoadBattleLabelsEx(CFFHacksterProject& proj, bool form2only, bool showindex)
	{
		auto labels = LoadIniLabels(proj, "BATTLELABELS", false);
		dataintnodevector v;
		if (!form2only) {
			for (const auto& d : labels) {
				CString name;
				if (showindex) name.Format("%02X: ", d.value);
				name.AppendFormat("%s", d.name);
				dataintnode dx(name, d.value);
				v.push_back(dx);
			}
		}
		for (const auto& d : labels) {
			auto value = d.value | 0x80;
			CString name;
			if (showindex) name.Format("%02X: ", value);
			name.AppendFormat("%s (F2)", d.name);
			dataintnode dx(name, value);
			v.push_back(dx);
		}
		return v;
	}

	dataintnodevector LoadTextLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "DIALOGUELABELS", showindex); }
	dataintnodevector LoadSpecialTileLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "SPECIALTILELABELS", showindex); }
	dataintnodevector LoadMiscCoordLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "MISCCOORDLABELS", showindex); }
	dataintnodevector LoadAilEffectLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "AILEFFECTLABELS", showindex); }

	dataintnodevector LoadElementLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "ELEMENTLABELS", showindex); }

	dataintnodevector LoadEnemyCategoryLabels(CFFHacksterProject & proj, bool showindex) { return LoadIniLabels(proj, "ENEMYCATEGORYLABELS", showindex); }
	dataintnodevector LoadArmorTypes(CFFHacksterProject& proj, bool showindex) { return LoadIniLabels(proj, "ARMORTYPELABELS", showindex); }

	dataintnodevector LoadSMTeleportLabels(CFFHacksterProject & proj, bool showindex)
	{
		// The entry values must progress contiguously, so simply adding the entry vectors together won't work.
		const int NNTELEPORT_COUNT = ReadDec(proj.ValuesPath, "NNTELEPORT_COUNT");
		const int NOTELEPORT_COUNT = ReadDec(proj.ValuesPath, "NOTELEPORT_COUNT");
		if (NNTELEPORT_COUNT != -1 && NOTELEPORT_COUNT != -1) {
			auto entries = LoadNNTeleportLabels(proj, showindex) + LoadNOTeleportLabels(proj, false);
			if (entries.size() == (size_t)(NNTELEPORT_COUNT + NOTELEPORT_COUNT)) {
				for (size_t st = NNTELEPORT_COUNT; st < entries.size(); ++st) {
					auto & entry = entries[st];
					entry.value += NNTELEPORT_COUNT;
					if (showindex) {
						CString pre;
						pre.Format("%02X: ", st);
						entry.name.Insert(0, pre);
					}
				}
				return entries;
			}
		}
		return dataintnodevector();
	}

	dataintnodevector LoadOnScreenSpriteLabels(CFFHacksterProject & proj, bool showindex)
	{
		int count = dec(ReadIni(proj.ValuesPath, CString("MAPSPRITE_COUNT"), "value", ""));
		if (count == 0) return dataintnodevector();

		// Create a temp list in vector<CString> form, and pass that to DoLoadLabels.
		std::vector<CString> sprites;
		CString text;
		for (int co = 0; co < count; co++) {
			text.Format("Sprite: %d", co);
			sprites.push_back(text);
		}
		return DoLoadLabels(sprites, 0, count, showindex);
	}

	dataintnodevector LoadFanfareLabels(CFFHacksterProject & proj, bool showindex)
	{
		UNREFERENCED_PARAMETER(proj);
		UNREFERENCED_PARAMETER(showindex);
		return fanfarevalues; //N.B.   this list is hardcoded, at least for now
	}

	dataintnodevector LoadFanfareTrueFalseLabels(CFFHacksterProject & proj, bool showindex)
	{
		UNREFERENCED_PARAMETER(proj);
		UNREFERENCED_PARAMETER(showindex);
		return { fanfarevalues[ASM_FALSE], fanfarevalues[ASM_TRUE] };
	}



	// ################################################################
	// SINGLE ENTRY READERS

	dataintnode LoadWepMagicLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "WEPMAGLABELS", index, showindex); }
	dataintnode LoadAILabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "AILABELS", index, showindex); }
	dataintnode LoadShopLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "SHOPLABELS", index, showindex); }
	dataintnode LoadBattlePatternTableLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "PATTERNTABLELABELS", index, showindex); }
	dataintnode LoadBackdropLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "BACKDROPLABELS", index, showindex); }
	dataintnode LoadONTeleportLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "ONTELEPORTLABELS", index, showindex); }
	dataintnode LoadNNTeleportLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "NNTELEPORTLABELS", index, showindex); }
	dataintnode LoadNOTeleportLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "NOTELEPORTLABELS", index, showindex); }
	dataintnode LoadTreasureLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "TREASURELABELS", index, showindex); }
	dataintnode LoadMapLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "STDMAPLABELS", index, showindex); }
	dataintnode LoadTilesetLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "TILESETLABELS", index, showindex); }
	dataintnode LoadSpriteLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "SPRITELABELS", index, showindex); }
	dataintnode LoadSpriteGraphicLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "SPRITEGRAPHICLABELS", index, showindex); }
	dataintnode LoadBattleLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "BATTLELABELS", index, showindex); }

	dataintnode LoadBattleLabelEx(CFFHacksterProject& proj, int index, bool showindex)
	{
		// In the original game, LoadIniLabel throws since formation 2 battles have a higher index than the BATTLELABELS values.
		// So mask out the hight bit, then build the node here.
		auto d = LoadIniLabel(proj, "BATTLELABELS", index & 0x7F, false);
		CString name;
		if (showindex) name.Format("%02X: ", index);
		name.AppendFormat("%s", d.name);
		if (index >= 0x80) name.Append(" (F2)");
		dataintnode dx(name, d.value);
		return dx;
	}

	dataintnode LoadTextLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "DIALOGUELABELS", index, showindex); }
	dataintnode LoadSpecialTileLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "SPECIALTILELABELS", index, showindex); }
	dataintnode LoadMiscCoordLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "MISCCOORDLABELS", index, showindex); }
	dataintnode LoadAilEffectLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "AILEFFECTLABELS", index, showindex); }
	dataintnode LoadElementLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "ELEMENTLABELS", index, showindex); }
	dataintnode LoadEnemyCategoryLabel(CFFHacksterProject & proj, int index, bool showindex) { return LoadIniLabel(proj, "ENEMYCATEGORYLABELS", index, showindex); }
	dataintnode LoadArmorTypeLabel(CFFHacksterProject& proj, int index, bool showindex) { return LoadIniLabel(proj, "ARMORTYPELABELS", index, showindex); }

	dataintnode LoadSMTeleportLabel(CFFHacksterProject & proj, int index, bool showindex)
	{
		// The ranges are successive, so if index is in the NO range, then add the NN count to it.
		const int NNTELEPORT_COUNT = ReadDec(proj.ValuesPath, "NNTELEPORT_COUNT");
		ASSERT(NNTELEPORT_COUNT > 0);
		if (index < NNTELEPORT_COUNT)
			return LoadNNTeleportLabel(proj, showindex);

		auto entry = LoadNOTeleportLabel(proj, false);
		entry.value += NNTELEPORT_COUNT;
		if (showindex) {
			CString pre;
			pre.Format("%02X: ", index);
			entry.name.Insert(0, pre);
		}
		return entry;
	}

	dataintnode LoadOnScreenSpriteLabel(CFFHacksterProject & proj, int index, bool showindex)
	{
		int count = dec(ReadIni(proj.ValuesPath, CString("MAPSPRITE_COUNT"), "value", ""));

		ASSERT(count > 0);
		ASSERT(index < count);
		if (count > 0 && index < count) {
			CString text;
			if (showindex) text.AppendFormat("%02X: ", index);
			text.AppendFormat("Sprite: %d", index);
			return { text, index};
		}
		return{ "", -1 };
	}

	dataintnode LoadFanfareLabel(CFFHacksterProject & proj, int index, bool showindex)
	{
		UNREFERENCED_PARAMETER(proj);
		UNREFERENCED_PARAMETER(showindex);
		if (index < ASM_FALSE && index > ASM_TRUE) index = ASM_IGNORE;
		return fanfarevalues[index];
	}



	// ################################################################
	// SINGLE ENTRY WRITERS

	void WriteWepMagicLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "WEPMAGLABELS", index, newvalue); }
	void WriteAILabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "AILABELS", index, newvalue); }
	void WriteShopLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "SHOPLABELS", index, newvalue); }
	void WriteBattlePatternTableLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "PATTERNTABLELABELS", index, newvalue); }
	void WriteBackdropLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "BACKDROPLABELS", index, newvalue); }
	void WriteONTeleportLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "ONTELEPORTLABELS", index, newvalue); }
	void WriteNNTeleportLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "NNTELEPORTLABELS", index, newvalue); }
	void WriteNOTeleportLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "NOTELEPORTLABELS", index, newvalue); }
	void WriteTreasureLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "TREASURELABELS", index, newvalue); }
	void WriteMapLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "STDMAPLABELS", index, newvalue); }
	void WriteTilesetLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "TILESETLABELS", index, newvalue); }
	void WriteSpriteLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "SPRITELABELS", index, newvalue); }
	void WriteSpriteGraphicLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "SPRITEGRAPHICLABELS", index, newvalue); }
	void WriteBattleLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "BATTLELABELS", index, newvalue); }
	void WriteTextLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "DIALOGUELABELS", index, newvalue); }
	void WriteSpecialTileLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "SPECIALTILELABELS", index, newvalue); }
	void WriteMiscCoordLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "MISCCOORDLABELS", index, newvalue); }
	void WriteAilEffectLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "AILEFFECTLABELS", index, newvalue); }
	void WriteElementLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "ELEMENTLABELS", index, newvalue); }
	void WriteEnemyCategoryLabel(CFFHacksterProject & proj, int index, CString newvalue) { WriteIniLabel(proj, "ENEMYCATEGORYLABELS", index, newvalue); }
	void WriteArmorTypeLabel(CFFHacksterProject& proj, int index, CString newvalue) { WriteIniLabel(proj, "ARMORTYPELABELS", index, newvalue); }

} // end namespace Editorlabels (CFFHacksterProject)



namespace Editorlabels
{
	namespace ffh2
	{
		namespace {
			dataintnodevector LoadLabels(FFH2Project& proj, CString section, bool showindex)
			{
				dataintnodevector labels;

				std::string group = tostd(section);
				auto it = proj.strings.entries.find(group);
				if (it == cend(proj.strings.entries))
					RAISEEXCEPTION(std::runtime_error, "Failed to load labels, unable to find label group '%s'.", group.c_str());

				const auto& entries = it->second;
				auto count = entries.size();
				if (count < 1)
					RAISEEXCEPTION(std::runtime_error, "No entires found for label group '%s'.", group.c_str());

				for (auto index = 0; index < count; ++index) {
					const auto& entry = entries[index];
					auto label = FormatLabel(group, entry, index, showindex);
					labels.push_back({ label, index });
				}

				//TODO - would have to introduce a mapping to do this count check, e.g. map [BATTLE_COUNT] => proj.strings.entries["battlelabels"]
				//		then retrieve BATTLE_COUNT and check that it equals proj.strings.entries["battlelabels"].size()

				//if (count != (int)labels.size()) {
				//	RAISEEXCEPTION(std::runtime_error,
				//		"count mismatch: group '%s' expects count %d, but read %d items", (LPCSTR)section, count, labels.size());
				//}

				return labels;
			}

			dataintnode LoadLabel(FFH2Project& proj, CString section, int index, bool showindex)
			{
				std::string group = tostd(section);
				auto it = proj.strings.entries.find(group);
				if (it == cend(proj.strings.entries))
					RAISEEXCEPTION(std::runtime_error, "Failed to load label, unable to find label group '%s'.", group.c_str());

				const auto& entries = it->second;
				auto count = entries.size();
				if (count < 1)
					RAISEEXCEPTION(std::runtime_error, "No entires found for label group '%s'.", group.c_str());

				const auto& entry = entries[index];
				auto label = FormatLabel(group, entry, index, showindex);
				return { label, index };
			}
		}

		// COLLECTIONS

		dataintnodevector LoadAilEffectLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "aileffectlabels", showindex); }
		dataintnodevector LoadAILabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "ailabels", showindex); }
		dataintnodevector LoadArmorTypes(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "armortypelabels", showindex); }
		dataintnodevector LoadBackdropLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "backdroplabels", showindex); }
		dataintnodevector LoadBattleLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "battlelabels", showindex); }

		dataintnodevector LoadBattleLabelsEx(FFH2Project& proj, bool form2only, bool showindex)
		{
			auto labels = LoadLabels(proj, "battlelabels", showindex);
			dataintnodevector v;
			if (!form2only) {
				for (const auto& d : labels) {
					CString name;
					if (showindex) name.Format("%02X: ", d.value);
					name.AppendFormat("%s", d.name);
					dataintnode dx(name, d.value);
					v.push_back(dx);
				}
			}
			for (const auto& d : labels) {
				auto value = d.value | 0x80;
				CString name;
				if (showindex) name.Format("%02X: ", value);
				name.AppendFormat("%s (F2)", d.name);
				dataintnode dx(name, value);
				v.push_back(dx);
			}
			return v;
		}

		dataintnodevector LoadBattlePatternTableLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "patterntablelabels", showindex); }
		dataintnodevector LoadElementLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "elementlabels", showindex); }
		dataintnodevector LoadEnemyCategoryLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "enemycategorylabels", showindex); }

		dataintnodevector LoadFanfareLabels(FFH2Project& proj, bool showindex)
		{
			UNREFERENCED_PARAMETER(proj);
			UNREFERENCED_PARAMETER(showindex);
			return fanfarevalues; //N.B.   this list is hardcoded, at least for now
		}

		dataintnodevector LoadFanfareTrueFalseLabels(FFH2Project& proj, bool showindex)
		{
			UNREFERENCED_PARAMETER(proj);
			UNREFERENCED_PARAMETER(showindex);
			return { fanfarevalues[ASM_FALSE], fanfarevalues[ASM_TRUE] };
		}

		dataintnodevector LoadMapLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "stdmaplabels", showindex); }
		dataintnodevector LoadMiscCoordLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "misccoordlabels", showindex); }
		dataintnodevector LoadNNTeleportLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "nnteleportlabels", showindex); }
		dataintnodevector LoadNOTeleportLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "noteleportlabels", showindex); }
		dataintnodevector LoadONTeleportLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "onteleportlabels", showindex); }

		dataintnodevector LoadOnScreenSpriteLabels(FFH2Project& proj, bool showindex)
		{
			ffh::acc::ValueDataAccessor d(proj);
			int count = d.get<int>("MAPSPRITE_COUNT");
			if (count == 0) return dataintnodevector();

			// Create a temp list in vector<CString> form, and pass that to DoLoadLabels.
			std::vector<CString> sprites;
			CString text;
			for (int co = 0; co < count; co++) {
				text.Format("Sprite: %d", co);
				sprites.push_back(text);
			}
			return DoLoadLabels(sprites, 0, count, showindex);
		}

		dataintnodevector LoadShopLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "shoplabels", showindex); }
		dataintnodevector LoadSpecialTileLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "specialtilelabels", showindex); }
		dataintnodevector LoadSpriteGraphicLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "spritegraphiclabels", showindex); }
		dataintnodevector LoadSpriteLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "spritelabels", showindex); }
		dataintnodevector LoadTextLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "dialoguelabels", showindex); }
		dataintnodevector LoadTilesetLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "tilesetlabels", showindex); }
		dataintnodevector LoadTreasureLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "treasurelabels", showindex); }
		dataintnodevector LoadWepMagicLabels(FFH2Project& proj, bool showindex) { return LoadLabels(proj, "wepmaglabels", showindex); }

		// SINGLES

		dataintnode LoadAILabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "ailabels", index, showindex); }
		dataintnode LoadAilEffectLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "aileffectlabels", index, showindex); }
		dataintnode LoadArmorTypeLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "armortypelabels", index, showindex); }
		dataintnode LoadBackdropLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "backdroplabels", index, showindex); }
		dataintnode LoadBattleLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "battlelabels", index, showindex); }

		dataintnode LoadBattleLabelEx(FFH2Project& proj, int index, bool showindex)
		{
			// In the original game, LoadLabel throws since formation 2 battles have a higher index than the BATTLELABELS values.
			// So mask out the hight bit, then build the node here.
			auto d = LoadLabel(proj, "battlelabels", index & 0x7F, false);
			CString name;
			if (showindex) name.Format("%02X: ", index);
			name.AppendFormat("%s", d.name);
			if (index >= 0x80) name.Append(" (F2)");
			dataintnode dx(name, d.value);
			return dx;
		}

		dataintnode LoadBattlePatternTableLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "patterntablelabels", index, showindex); }
		dataintnode LoadElementLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "elementlabels", index, showindex); }
		dataintnode LoadEnemyCategoryLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "enemycategorylabels", index, showindex); }

		dataintnode LoadFanfareLabel(FFH2Project& proj, int index, bool showindex)
		{
			UNREFERENCED_PARAMETER(proj);
			UNREFERENCED_PARAMETER(showindex);
			if (index < ASM_FALSE && index > ASM_TRUE) index = ASM_IGNORE;
			return fanfarevalues[index];
		}

		dataintnode LoadMapLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "stdmaplabels", index, showindex); }
		dataintnode LoadMiscCoordLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "misccoordlabels", index, showindex); }
		dataintnode LoadNNTeleportLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "nnteleportlabels", index, showindex); }
		dataintnode LoadNOTeleportLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "noteleportlabels", index, showindex); }
		dataintnode LoadONTeleportLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "onteleportlabels", index, showindex); }

		dataintnode LoadOnScreenSpriteLabel(FFH2Project& proj, int index, bool showindex)
		{
			ffh::acc::ValueDataAccessor d(proj);
			int count = d.get<int>("MAPSPRITE_COUNT");

			ASSERT(count > 0);
			ASSERT(index < count);
			if (count > 0 && index < count) {
				CString text;
				if (showindex) text.AppendFormat("%02X: ", index);
				text.AppendFormat("Sprite: %d", index);
				return { text, index };
			}
			return{ "", -1 };
		}

		dataintnode LoadShopLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "shoplabels", index, showindex); }
		dataintnode LoadSpecialTileLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "specialtilelabels", index, showindex); }
		dataintnode LoadSpriteLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "spritelabels", index, showindex); }
		dataintnode LoadSpriteGraphicLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "spritegraphiclabels", index, showindex); }
		dataintnode LoadTextLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "dialoguelabels", index, showindex); }
		dataintnode LoadTilesetLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "tilesetlabels", index, showindex); }
		dataintnode LoadTreasureLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "treasurelabels", index, showindex); }
		dataintnode LoadWepMagicLabel(FFH2Project& proj, int index, bool showindex) { return LoadLabel(proj, "wepmaglabels", index, showindex); }

	} // end namespace ffh2
} // end namespace Editorlabels (CFFH2Project)