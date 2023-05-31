#pragma once

#include <vector>
#include <map>

class CFFHacksterProject;
class FFH2Project;

enum UseDataType { Overworld = 0, StdMap = 1, SpikedSquare = 2, SpriteDialogue = 3, UseDataType_Count };

struct sUseData {
	UseDataType type;
	size_t mapid;
	size_t battleid;
	int slot;
	size_t formation;
	size_t chance;
	int x, y;
};


using EnemyUsageDataFormatter = std::function<CString(FFH2Project& proj, const sUseData& use)>;
using EnemyUsageDataIncluder = std::function<bool(int keyindex, int battleindex)>;

class EnemyBattleUsageData
{
public:
	EnemyBattleUsageData();
	~EnemyBattleUsageData();

public:
	void SetProject(CFFHacksterProject& proj);
	void SetProject(FFH2Project& proj);
	void Clear();
	void Reset(bool keepProbabilities = true);
	bool UpdateUseData(int keyindex, EnemyUsageDataIncluder includer, EnemyUsageDataFormatter formatter);

	void UpdateTileset(int tilesetindex);
	void UpdateMapSpikedSquaresForTileset(int tilesetindex);

	std::vector<int> m_probabilities;
	std::vector<sUseData> m_usedata;
	std::vector<std::pair<CString, DWORD>> m_usedatarefs;

private:
	void MapTalkRoutines();
	void MapSpikedSquares();
	int ReadBankAddr(int ptrtbloffset, int ptrwidth, int index);

	void ReindexTileset(int tileset);
	void UpdateMapSpikedSquares(int mapindex);

	CFFHacksterProject* Project = nullptr;
	FFH2Project* Proj2 = nullptr;

	int BANK_SIZE = -1;
	int MAP_COUNT = -1;
	int MAP_OFFSET = -1;
	int MAP_PTRADD = -1;
	int MAPTILESET_ASSIGNMENT = -1;
	int BATTLEDOMAIN_OFFSET = -1;
	int BATTLEPROBABILITY_OFFSET = -1;
	int BATTLE_OFFSET = -1;
	int BATTLE_BYTES = -1;
	int SPRITE_COUNT = -1;
	int TALKROUTINEPTRTABLE_OFFSET = -1;
	int TALKROUTINEPTRTABLE_BYTES = -1;
	int TALKROUTINEPTRTABLE_PTRADD = -1;
	int TALKROUTINEDATA_OFFSET = -1;
	int TALKROUTINEDATA_BYTES = -1;
	int TILESET_TILEDATA = -1;
	int TILESET_COUNT = -1;
	const char* const BIN_OBJECTDATA = "bin\\0E_95D5_objectdata.bin";

	struct sUseDataTalkElement {
		CString type;
		int param;
		CString comment;
		int resolvedValue;
	};
	struct sUseDataTalkRoutine {
		CString name;
		CString desc;
		int bankaddr;
		std::vector<sUseDataTalkElement> elements;
	};
	using talkroutinemap = std::map<int, sUseDataTalkRoutine>;

	using tset_tileindexmap = std::map<int, std::map<int, int>>;
	struct sUseDataSpikedSquare {
		int mapid;
		int x;
		int y;
		int rawbattleid;
	};

	using spikedsquarevector = std::vector<sUseDataSpikedSquare>;
	using tmap_spikedsquares = std::map<int, spikedsquarevector>;

	talkroutinemap m_talkmap;
	tset_tileindexmap m_tset_tileindexmap;
	tmap_spikedsquares m_tmap_spikedsqs;
};
