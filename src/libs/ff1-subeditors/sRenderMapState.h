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

	bool IsValid() const;
};
