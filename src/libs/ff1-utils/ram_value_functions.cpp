#include "stdafx.h"
#include "ram_value_functions.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "type_support.h"
#include "FFHacksterProject.h"
#include <algorithm>

using namespace Ini;
using namespace Strings;
using namespace Types;

namespace Ramvalues
{

	namespace {

		CString DoLoadLabels(const ramvaluevector & values, int start, int last, CListBox * m_list, CComboBox * m_combo, bool showindex = false)
		{
			CString ret = "";
			if (m_list != nullptr) m_list->SetRedraw(FALSE);
			if (m_combo != nullptr) m_combo->SetRedraw(FALSE);

			bool single = (last - start) == 1;
			for (int co = start; co < last; co++) {
				CString temptext;
				if (showindex) temptext.AppendFormat("%02X: ", values[co].addr);
				temptext += values[co].name;

				CString text = temptext;
				if (co && !single) ret += "\\b";
				ret += text;
				if (m_list != nullptr) m_list->InsertString(-1, text);
				if (m_combo != nullptr) m_combo->InsertString(-1, text);
			}
			if (m_list != nullptr) m_list->SetRedraw(TRUE);
			if (m_combo != nullptr) m_combo->SetRedraw(TRUE);
			return ret;
		}

	}


	// Valid RAM address are expected to be in either $addr or 0xhex format.
	// Any other format (or blank) returns -1.
	int ReadRamAddress(CFFHacksterProject & proj, CString key)
	{
		auto strvalue = ReadIni(proj.ValuesPath, key, "value", "");
		if (strvalue.IsEmpty()) return -1;

		if (is_addr(strvalue))
			return addr(strvalue);
		if (is_hex(strvalue))
			return hex(strvalue);

		return -1;
	}

	ramvaluevector ReadRamValuesToVector(CFFHacksterProject & proj, const std::vector<CString> & sectionnames)
	{
		ramvaluevector vec;
		bool error = false;
		CString inifile = proj.ValuesPath;

		for (const auto & section : sectionnames) {
			// Attempts to read label, then desc, and defaults to section if it can't find the others
			//N.B. - this could be in either address or hex format (treasure chest sound values are in hex)
			auto strvalue = ReadIni(inifile, section, "value", CString());
			int addrvalue = is_addr(strvalue) ? addr(strvalue) : hex(strvalue);
			vec.push_back({ section, addrvalue });
		}

		if (error) vec.clear();
		return vec;
	}

	ramvaluevector ReadRamValuesToVector(CFFHacksterProject & proj, CString key)
	{
		auto namestring = ReadIni(proj.ValuesPath, key, "value", CString());
		auto names = split(namestring, " ");
		return ReadRamValuesToVector(proj, names);
	}


	namespace {
		const mfcstringvector vehiclelabels = {
			"ship_vis", "airship_vis", "bridge_vis", "has_canoe", "item_canoe" // included both canoe items since they're tied to the canoe in some way
		};
		const mfcstringvector mapspritelabels = {
			"ship_vis", "airship_vis", "bridge_vis", "canal_vis", "has_canoe"
		};
		const mfcstringvector orblabels = {
			"orb_fire", "orb_water", "orb_air", "orb_earth"
		};
		const mfcstringvector itemlabels = {
			"item_lute","item_crown","item_crystal","item_herb","item_mystickey",
			"item_tnt","item_adamant","item_slab","item_ruby","item_rod",
			"item_floater","item_chime","item_tail","item_cube","item_bottle",
			"item_oxyale",
			"item_canoe",
			"orb_fire","orb_water","orb_air",
			"orb_earth","item_tent","item_cabin","item_house","item_heal",
			"item_pure","item_soft"
		};
		const mfcstringvector mapanditemlabels = mapspritelabels + itemlabels;

		const mfcstringvector fanfarelabels = { "talkfanfare", "notalksnd" };

		const mfcstringvector canoelabels = { "has_canoe" };

		const mfcstringvector itemorcanoelabels = canoelabels + itemlabels;
	}


	CString GetItemFallbackName(CFFHacksterProject & proj, CString name, int itemaddr)
	{
		if (isemptyorwhitespace(name)) {
			auto iter = find_by_data(proj.m_varmap, itemaddr);
			if (iter != cend(proj.m_varmap)) {
				name = iter->first.c_str();
				auto altname = ReadIni(proj.ValuesPath, name, "label", "");
				if (altname.IsEmpty()) altname = ReadIni(proj.ValuesPath, name, "desc", "");
				if (!altname.IsEmpty()) name = altname;
			}
		}
		return name;
	}


	namespace {

		dataintnodevector DoLoadEntries(const ramvaluevector & values, int start, int count, bool showindex = false)
		{
			dataintnodevector dvec;
			int last = start + count - 1;
			for (int co = start; co <= last; co++) {
				CString text;
				if (showindex) text.AppendFormat("%04X: ", values[co].addr);
				text += values[co].name;
				dvec.push_back({text, values[co].addr });
			}
			return dvec;
		}

		dataintnodevector LoadEntries(CFFHacksterProject & proj, bool showindex, const std::vector<CString> & sectionnames)
		{
			auto values = ReadRamValuesToVector(proj, sectionnames);
			return DoLoadEntries(values, 0, (int)values.size(), showindex);
		}

		// Find the label by the supplied address, as these values are not strictly contiguous
		dataintnode LoadEntry(CFFHacksterProject & proj, int value, bool showindex, const std::vector<CString> & sectionnames)
		{
			// short list, just use a linear search
			auto values = ReadRamValuesToVector(proj, sectionnames);
			auto iter = std::find_if(begin(values), end(values), [value](const ramvalue & rv) { return rv.addr == value;});

			if (iter == cend(values)) return{ "", -1 };

			CString strname;
			if (showindex) strname.Format("%04X: ", iter->addr);
			strname += iter->name;
			return { strname, iter->addr };
		}

	} // end namespace unnamed (LoadEntries helpers)


	void AdjustRamItemEntries(CFFHacksterProject & proj, dataintnodevector & nodes, CString indexformat, int adjustby)
	{
		for (auto & node : nodes) {
			// adjust the value if it's nonzero
			if (adjustby != 0) node.value += adjustby;

			if (isemptyorwhitespace(node.name)) {
				// the name is empty, generate a new name
				auto iter = find_by_data(proj.m_varmap, node.value);
				CString newname;
				if (iter != cend(proj.m_varmap)) {
					// use the label, description, or ramvalue name (in that order of preference)
					newname = ReadIni(proj.ValuesPath, newname, "label", "");
					if (newname.IsEmpty())
						newname = ReadIni(proj.ValuesPath, newname, "desc", "");
					if (newname.IsEmpty())
						newname = iter->first.c_str();
				}

				// If there's still no name, this is actually an error condition (the ramvalue label was blank or missing)
				// But for now, create a temporary name using the value.
				if (newname.IsEmpty())
					newname.Format("Item_%04X", node.value);

				node.name = newname;
			}

			// adjust the label if the indexformat is defined
			if (!indexformat.IsEmpty()) {
				// strip the leading value and colon if present, as well as any space before the existing name
				int sep = node.name.Find(':');
				if (sep != -1) node.name.TrimRight().Delete(0, sep + 1);
				// now format the number and prepend it
				CString newprefix;
				newprefix.Format(indexformat + ": ", node.value);
				node.name.Insert(0, newprefix);
			}
		}
	}


	// ### STRINGS

	dataintnodevector LoadMapSpriteEntries(CFFHacksterProject& proj, bool showindex)
	{
		//FUTURE - this loads the section name instead of the label, see if we can load the label.
		return LoadEntries(proj, showindex, mapspritelabels);
	}

	dataintnodevector LoadDialogSoundEntries(CFFHacksterProject & proj, bool showindex)
	{
		return LoadEntries(proj, showindex, fanfarelabels);
	}

	dataintnodevector LoadCanoeEntries(CFFHacksterProject & proj, bool showindex)
	{
		return LoadEntries(proj, showindex, canoelabels);
	}

	dataintnodevector LoadItemOrCanoeEntries(CFFHacksterProject & proj, bool showindex)
	{
		return LoadEntries(proj, showindex, itemorcanoelabels);
	}


	// ### SINGLE STRINGS

	dataintnode LoadMapSpriteEntry(CFFHacksterProject & proj, int value, bool showindex)
	{
		return LoadEntry(proj, value, showindex, mapspritelabels);
	}

} // end namespace Ramvalues