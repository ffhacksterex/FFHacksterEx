#pragma once

namespace ODUtils
{

	bool HasAllStyles(UINT objectstyles, UINT checkfor);
	bool HasAnyStyles(UINT objectstyles, UINT checkfor);
	bool HasAtLeastOneOf(UINT flags, std::initializer_list<UINT> flagstocheck);

	void UpdateParentRect(CWnd* pwnd);

	POINT SetBrushOrigin(CDC & dc, CWnd * pwnd);
	HBRUSH GetODCtlColorBrush(CWnd * pwnd, CDC * pDC);
}
