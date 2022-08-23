#pragma once

#include <afx.h>

// Interface to abstract access to the overworld and standard maps.
class ICoordMap
{
public:
	ICoordMap() {}
	virtual ~ICoordMap() {}

	virtual void UpdateTeleportLabel(int areaid, int type) = 0;
	virtual void Cancel(int context) = 0;
	virtual POINT GetLastClick() = 0;
	virtual int GetCurMap() = 0;
	virtual void TeleportHere(int mapindex, int x, int y) = 0;

	virtual void DoHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) = 0;
	virtual void DoVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) = 0;
	virtual void SendKeydown(WPARAM wparam, LPARAM lparam) {}
};
