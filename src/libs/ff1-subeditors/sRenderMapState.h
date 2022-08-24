#pragma once

class IMapEditor;
class CFFHacksterProject;

struct sRenderMapState
{
	BYTE* pmousedown = nullptr;

	// Initialize-only (treat as immutable)
	CFFHacksterProject* project = nullptr;
	IMapEditor* owner = nullptr;
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
	const std::vector<CPoint>* Sprite_Coords = nullptr;
	const std::vector<bool>* Sprite_InRoom = nullptr;
	const std::vector<bool>* Sprite_StandStill = nullptr;
	const std::vector<short>* Sprite_Value = nullptr;

	size_t SPRITE_COUNT = (size_t)-1;
	size_t SPRITE_PICASSIGNMENT = (size_t)-1;

	bool IsValid() const;
};
