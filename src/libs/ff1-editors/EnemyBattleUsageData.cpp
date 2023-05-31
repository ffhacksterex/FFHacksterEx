#include "stdafx.h"
#include "EnemyBattleUsageData.h"
#include <FFH2Project.h>
#include "ini_functions.h"
#include "common_symbols.h"
#include "editor_label_functions.h"
#include "editors_common.h"
#include <cnv_primitives.h>
#include <collection_helpers.h>
#include <string_functions.h>
#include <ValueDataAccessor.h>
#include <vda_std_collections.h>
#include <dialogue_helpers.h>

using ffh::acc::ValueDataAccessor;

namespace {
	CString DefBattleFormatter2(FFH2Project& proj, const sUseData& u);
	CString DefEnemyFormatter2(FFH2Project& proj, const sUseData& u);
}

EnemyBattleUsageData::EnemyBattleUsageData()
{
}

EnemyBattleUsageData::~EnemyBattleUsageData()
{
}

void EnemyBattleUsageData::SetProject(CFFHacksterProject& proj)
{
	FFH_SWITCH_TO_FFH2;
}

void EnemyBattleUsageData::SetProject(FFH2Project& proj)
{
	Proj2 = &proj;
	Clear();

	ValueDataAccessor d(*Proj2);
	BANK_SIZE = d.get<int>("BANK_SIZE");
	MAP_COUNT = d.get<int>("MAP_COUNT");
	MAP_OFFSET = d.get<int>("MAP_OFFSET");
	MAP_PTRADD = d.get<int>("MAP_PTRADD");
	MAPTILESET_ASSIGNMENT = d.get<int>("MAPTILESET_ASSIGNMENT");
	BATTLEDOMAIN_OFFSET = d.get<int>("BATTLEDOMAIN_OFFSET");
	BATTLEPROBABILITY_OFFSET = d.get<int>("BATTLEPROBABILITY_OFFSET");
	BATTLE_OFFSET = d.get<int>("BATTLE_OFFSET");
	BATTLE_BYTES = d.get<int>("BATTLE_BYTES");
	SPRITE_COUNT = d.get<int>("SPRITE_COUNT");
	TALKROUTINEPTRTABLE_OFFSET = d.get<int>("TALKROUTINEPTRTABLE_OFFSET");
	TALKROUTINEPTRTABLE_BYTES = d.get<int>("TALKROUTINEPTRTABLE_BYTES");
	TALKROUTINEPTRTABLE_PTRADD = d.get<int>("TALKROUTINEPTRTABLE_PTRADD");
	TALKROUTINEDATA_OFFSET = d.get<int>("TALKROUTINEDATA_OFFSET");
	TALKROUTINEDATA_BYTES = d.get<int>("TALKROUTINEDATA_BYTES");
	TILESET_TILEDATA = d.get<int>("TILESET_TILEDATA");
	TILESET_COUNT = d.get<int>("TILESET_COUNT");

	const auto& rom = Proj2->ROM;

	m_probabilities.resize(8, 0); //TODO - currently no INI value to make this configurable
	auto offset = BATTLEPROBABILITY_OFFSET;
	for (auto i = 0; i < 64; i++)
		m_probabilities[rom[offset + i]] += 1;

	MapTalkRoutines();
	MapSpikedSquares();
}

void EnemyBattleUsageData::Clear()
{
	m_probabilities.clear();
	m_usedata.clear();
	m_usedatarefs.clear();
	m_talkmap.clear();
	m_tset_tileindexmap.clear();
	m_tmap_spikedsqs.clear();
}

void EnemyBattleUsageData::Reset(bool keepProbabilities)
{
	if (!keepProbabilities) m_probabilities.clear();
	m_usedata.clear();
	m_usedatarefs.clear();
}

bool EnemyBattleUsageData::UpdateUseData(int keyindex, EnemyUsageDataIncluder includer, EnemyUsageDataFormatter formatter)
{
	if (Proj2 == nullptr) {
		ErrorHere << "Can't generate use data without a project." << std::endl;
		throw std::runtime_error("Can't generate use data without a project.");
	}
	else if (includer == nullptr) {
		ErrorHere << "Can't generate use data without an includer." << std::endl;
		throw std::runtime_error("Can't generate use data without an includer.");
	}
	else if (formatter == nullptr) {
		ErrorHere << "Can't generate use data without an formatter." << std::endl;
		throw std::runtime_error("Can't generate use data without an formatter.");
	}

	const auto& rom = Proj2->ROM;

	// Overworld domain random encounters
	for (auto dom = 0; dom < 64; ++dom) {
		auto y = dom / 8;
		auto x = dom % 8;
		auto ref = (y << 3) + x;
		ref = BATTLEDOMAIN_OFFSET + (ref << 3);
		for (auto co = 0; co < 8; ++co) {
			const auto rawbattleid = rom[ref + co];
			// INCLUSION CONDITION
			if (includer(keyindex, rawbattleid)) {
				const auto battle = (rawbattleid & 0x7F);
				sUseData u = {};
				u.type = UseDataType::Overworld;
				u.mapid = dom;
				u.x = x;
				u.y = y;
				u.battleid = battle;
				u.formation = (rawbattleid & 0x80) ? 2 : 1;
				u.slot = co;
				u.chance = m_probabilities[co];
				m_usedata.push_back(u);
				// TEXT FORMATTER
				//TODO - while it compiles, m_usedatarefs must change to FFH2Project before this will ctually work
				m_usedatarefs.push_back({ formatter(*Proj2, m_usedata.back()), (int)m_usedata.size() - 1 });
			}
		}
	}

	// Standard maps random encounters
	for (auto imap = 0; imap < MAP_COUNT; ++imap) {
		auto offset = BATTLEDOMAIN_OFFSET + 0x200 + (imap << 3);
		for (auto co = 0; co < 8; ++co, ++offset) {
			const auto rawbattleid = rom[offset];
			// INCLUSION CONDITION
			if (includer(keyindex, rawbattleid)) {
				const auto battle = (rawbattleid & 0x7F);
				sUseData u = {};
				u.type = UseDataType::StdMap;
				u.mapid = imap;
				u.battleid = battle;
				u.formation = (rawbattleid & 0x80) ? 2 : 1;
				u.slot = co;
				u.chance = m_probabilities[co];
				m_usedata.push_back(u);
				m_usedatarefs.push_back({ formatter(*Proj2, m_usedata.back()), (DWORD)m_usedata.size() - 1 });
			}
		}
	}

	// Spiked squares (Standard map)
	for (auto imap = 0; imap < MAP_COUNT; ++imap) {
		auto iter = m_tmap_spikedsqs.find(imap);
		if (iter != cend(m_tmap_spikedsqs)) {
			for (const auto& ssq : iter->second) {
				const auto rawbattleid = ssq.rawbattleid;
				// INCLUSION CONDITION
				if (includer(keyindex, rawbattleid)) {
					const auto battle = (rawbattleid & 0x7F);
					sUseData u = {};
					u.type = UseDataType::SpikedSquare;
					u.mapid = imap;
					u.x = ssq.x;
					u.y = ssq.y;
					u.battleid = battle;
					u.formation = (rawbattleid & 0x80) ? 2 : 1;
					u.slot = -1;
					u.chance = 0;
					m_usedata.push_back(u);
					m_usedatarefs.push_back({ formatter(*Proj2, m_usedata.back()), (DWORD)m_usedata.size() - 1 });
				}
			}
		}
	}

	// Dialogue-initiated battles (Standard map)
	for (auto isp = 0; isp < SPRITE_COUNT; ++isp) {
		int romoffset = TALKROUTINEPTRTABLE_OFFSET + (isp * TALKROUTINEPTRTABLE_BYTES);
		int bankaddr = rom[romoffset] + (rom[romoffset + 1] << 8);
		auto iter = m_talkmap.find(bankaddr);
		if (iter != cend(m_talkmap)) {
			const auto& routine = iter->second;
			ASSERT(routine.bankaddr == bankaddr);
			for (auto i = 0u; i < routine.elements.size(); ++i) {
				const auto& elem = routine.elements[i];
				auto rawbattleid = -1;
				//TODO - change to map a delem instead of the components of parts
				if (elem.type == "battle") {
					auto offset = TALKROUTINEDATA_OFFSET + (isp * 4) + elem.param;
					rawbattleid = rom[offset];
				}
				else if (elem.type == "hcbattle") {
					rawbattleid = elem.resolvedValue;
				}
				// INCLUSION CONDITION
				if (includer(keyindex, rawbattleid)) {
					const auto battle = (rawbattleid & 0x7F);
					sUseData u = {};
					u.type = UseDataType::SpriteDialogue;
					u.mapid = isp;
					u.battleid = battle;
					u.formation = (rawbattleid & 0x80) ? 2 : 1;
					u.slot = -1;
					u.chance = 0;
					m_usedata.push_back(u);
					m_usedatarefs.push_back({ formatter(*Proj2, m_usedata.back()), (DWORD)m_usedata.size() - 1 });
				}
			}
		}
	}

	return true;
}

void EnemyBattleUsageData::UpdateTileset(int tilesetindex)
{
	ReindexTileset(tilesetindex);
}

void EnemyBattleUsageData::UpdateMapSpikedSquaresForTileset(int tilesetindex)
{
	const auto& rom = Proj2->ROM;

	for (auto imap = 0; imap < MAP_COUNT; ++imap) {
		auto iset = rom[MAPTILESET_ASSIGNMENT + imap];
		if (iset == tilesetindex)
			UpdateMapSpikedSquares(imap);
	}
}

void EnemyBattleUsageData::MapTalkRoutines()
{
	//TODO - change the map to refer to the handlers directly
	//		instead of copies of creating piecemeal copies.
	const auto& rom = Proj2->ROM;
	
	auto v = ffh::acc::ValueDataAccessor(*Proj2);
	auto sections = Proj2->dialogue.handlers.order;
	for (const auto& section : sections)
	{
		const auto& handler = Proj2->GetHandler(section);
		auto bankaddr = ffh::cnv::hex(handler.bankaddr);
		if (m_talkmap.find(bankaddr) != cend(m_talkmap)) continue;

		sUseDataTalkRoutine routine;
		routine.name = handler.name.c_str();
		routine.desc = handler.desc.c_str();
		routine.bankaddr = bankaddr;

		for (const auto & delem : handler.elements)
		{
			CString marker = ffh::str::tomfc(delem.type);
			if (marker == "hcbattle" || marker == "battle") {
				// hardcoded: battle ID is at the offset specified by parts[1]
				// battle ID is in the param specified by parts[1]
				auto hardcoded = delem.isHardcoded();
				sUseDataTalkElement elem = { 0 };
				elem.type = marker;

				elem.param = hardcoded ? ffh::cnv::hex(delem.hexoffset) : delem.paramindex;
				elem.comment = ffh::str::tomfc(delem.comment);

				// We can resolve the hardcode now sicne it won't change.
				// We can't resolve parameterized until we loop through the elements.
				if (hardcoded) {
					auto offset = TALKROUTINEPTRTABLE_PTRADD + bankaddr + elem.param;
					elem.resolvedValue = rom[offset];
				}
				routine.elements.push_back(elem);
			}
		}
		m_talkmap[bankaddr] = routine;
	}
}

void EnemyBattleUsageData::MapSpikedSquares()
{
	// For each map, build a map of tileset ID to battle ID for all spiked battles.
	// Don't map random envounters.
	// It's a random battle if the high bit is set (i.e. value >= 0x80).
	//DEVNOTE - without a ROM/assembly hack, the above means formation 2 can't be used,
	//	as battles 0x80 through 0xFF will be interpreted as a random encounter.
	for (auto iset = 0; iset < TILESET_COUNT; ++iset) {
		ReindexTileset(iset);
	}

	for (auto imap = 0; imap < MAP_COUNT; ++imap) {
		UpdateMapSpikedSquares(imap);
	}
}

int EnemyBattleUsageData::ReadBankAddr(int ptrtbloffset, int ptrwidth, int index)
{
	const auto& rom = Proj2->ROM;

	int romoffset = ptrtbloffset + (index * ptrwidth);
	int value = rom[romoffset] + (rom[romoffset + 1] << 8);
	return value;
}

void EnemyBattleUsageData::ReindexTileset(int tileset)
{
	const auto& rom = Proj2->ROM;

	std::map<int, int> tile_battlemap;
	for (auto itile = 0; itile < 128; ++itile) {
		auto offset = TILESET_TILEDATA + (tileset << 8) + (itile << 1);
		BYTE temp = rom[offset];
		BYTE byte2 = rom[offset + 1];
		if (temp == 0x0A && byte2 < 0x80) {
			auto btlid = byte2;
			tile_battlemap[itile] = btlid;
		}
	}
	m_tset_tileindexmap[tileset] = tile_battlemap;
}

void EnemyBattleUsageData::UpdateMapSpikedSquares(int mapindex)
{
	auto& rom = Proj2->ROM;

	// If there are no spiked squares on the tileset, then skip any map tied to it.
	auto iset = rom[MAPTILESET_ASSIGNMENT + mapindex];
	auto& tset = m_tset_tileindexmap[iset];
	if (tset.empty())
		return;

	// We have to decompress the map or the coords will never be correct.
	// Then walk through the decompressed map and snag tiles mapped to our battle.
	BYTE DecompressedMap[0x40][0x40] = { 0 };
	Maps::DecompressMap(rom, mapindex, MAP_OFFSET, MAP_PTRADD, DecompressedMap);

	spikedsquarevector vspikes;
	for (auto r = 0; r < 64; ++r) {
		for (auto c = 0; c < 64; ++c) {
			auto tile = DecompressedMap[r][c];
			auto iter = tset.find(tile);
			if (iter != cend(tset)) {
				sUseDataSpikedSquare ssq = {};
				ssq.mapid = mapindex;
				ssq.x = c;
				ssq.y = r;
				ssq.rawbattleid = iter->second;
				vspikes.push_back(ssq);
			}
		}
	}

	m_tmap_spikedsqs[mapindex] = vspikes;
}

namespace {
	static const CString UseDataTypeNames[] = { "Overworld", "StdMap", "SpikedSquare", "SpriteDialogue" };

	// === FFH2Project implementation

	CString GetMapName(FFH2Project& proj, const sUseData& u)
	{
		CString	mapname;
		switch (u.type)
		{
		case UseDataType::Overworld:
		{
			auto y = u.mapid / 8;
			auto x = u.mapid % 8;
			mapname.Format("OVW %d,%d", x, y);
			break;
		}
		case UseDataType::StdMap:
		{
			mapname.Format("STD %s", Labels2::LoadMapLabel(proj, (int)u.mapid, true).name);
			break;
		}
		case UseDataType::SpikedSquare:
		{
			mapname.Format("Spiked %s at %d,%d", Labels2::LoadMapLabel(proj, (int)u.mapid, true).name, u.x, u.y);
			break;
		}
		case UseDataType::SpriteDialogue:
		{
			mapname.Format("Sprite %s", Labels2::LoadSpriteLabel(proj, (int)u.mapid, true).name);
			break;
		}
		default:
			throw std::runtime_error("Unknown UseData type " + std::to_string((int)u.type));
		}
		return mapname;
	}

	CString DefBattleFormatter2(FFH2Project& proj, const sUseData& u)
	{
		auto mapname = GetMapName(proj, u);
		CString fmt;
		fmt.Format("%s \tSlot %d \tBtl %02X \tF%d \t%d/64", mapname, u.slot, u.battleid, u.formation, u.chance);
		return fmt;
	}

	CString DefEnemyFormatter2(FFH2Project& proj, const sUseData& u)
	{
		auto mapname = GetMapName(proj, u);
		CString fmt;
		fmt.Format("%s \tSlot %d \tBtl %02X \tF%d \t%d/64", mapname, u.slot, u.battleid, u.formation, u.chance);
		return fmt;
	}
}