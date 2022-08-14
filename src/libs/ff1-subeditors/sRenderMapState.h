#pragma once

class CFFHacksterProject;

struct sRenderMapState
{
	CFFHacksterProject* project = nullptr;
	CWnd* owner = nullptr;
	const bool* showrooms = nullptr;
	const CPoint* ptLastClick = nullptr;
	const CRect* rcToolRect = nullptr;
	const int* cur_map = nullptr;
	const int* cur_tile = nullptr;
	const int* cur_tool = nullptr;
	const BYTE* DecompressedMap = nullptr;
	CSize mapdims;
	CSize tiledims;

	CImageList* m_sprites = nullptr;
	const std::vector<CPoint>* Sprite_Coords = nullptr;  // MAPSPRITE_COUNT;
	const std::vector<bool>* Sprite_InRoom = nullptr;    // MAPSPRITE_COUNT;
	const std::vector<bool>* Sprite_StandStill = nullptr;// MAPSPRITE_COUNT;
	const std::vector<short>* Sprite_Value = nullptr;    // MAPSPRITE_COUNT;

	size_t SPRITE_COUNT = (size_t)-1;
	size_t SPRITE_PICASSIGNMENT = (size_t)-1;

	bool IsValid() const;
};
