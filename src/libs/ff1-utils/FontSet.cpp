#include "stdafx.h"
#include "FontSet.h"

FontSet::FontSet()
{
}

FontSet::~FontSet()
{
}

void FontSet::Clear()
{
	m_bannerfont.DeleteObject();
	m_subbannerfont.DeleteObject();
	m_actionfont.DeleteObject();
	m_normalfont.DeleteObject();
}

namespace {
	BOOL LoadFont(CFont& f, CString face, int points)
	{
		//REFACTOR - unclear error behavior
		// If the font face doesn't exist, it won't fail, but a default font from other internal LOGFONT attributes.
		// So check the font face returned to see if it matches.
		if (!f.CreatePointFont(points, face)) {
			ErrorHere << "Failed to load point font '" << (LPCSTR)face << "' " << points / 10.0f << " pt" << std::endl;
			return FALSE;
		}
		return TRUE;
	};

	BOOL LoadFontIndirect(HDC hdc, CFont& f, CString face, int points, int weight = FW_NORMAL)
	{
		LOGFONT lf = { 0 };
		lf.lfHeight = -MulDiv(points / 10, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		lf.lfWeight = weight;
		strncpy(lf.lfFaceName, face, LF_FACESIZE);
		//REFACTOR - unclear error behavior
		// If the font face doesn't exist, it won't fail, but a default font from other internal LOGFONT attributes.
		// So check the font face returned to see if it matches. Likely need EnumFontFamiliesEx for this (and loadFont below).
		if (!f.CreateFontIndirect(&lf)) {
			ErrorHere << "Failed to load indirect font '" << (LPCSTR)face << "' " << points / 10.0f << " pt" << " weight " << weight << std::endl;
			return FALSE;
		}
		return TRUE;
	};
}

void FontSet::SetDefaults()
{
	auto hdc = GetDC(NULL);

	//REMOVE - do this in a later release
	//static const auto loadFontIfNot = [](CString desiredFace, CFont& f, CString face, int points) {
	//	LOGFONT lf = { 0 };
	//	f.GetLogFont(&lf);
	//	if (desiredFace == lf.lfFaceName)
	//		return TRUE;

	//	f.DeleteObject();
	//	return LoadFont(f, face, points);
	//};

	LoadFontIndirect(hdc, m_bannerfont, "Verdana", 360, FW_BOLD);
	LoadFont(m_subbannerfont, "Verdana", 140);
	LoadFont(m_actionfont, "Verdana", 120);
	LoadFont(m_normalfont, "Tahoma", 80);
	//TODO - Tahoma isn't installed by default on pre-Vista systems.
	//loadFontIfNot("Tahoma", m_normalfont, "Microsoft Sans Serif", 80);

	ReleaseDC(NULL, hdc);
}

CFont* FontSet::GetBannerFont() { return m_bannerfont.GetSafeHandle() != NULL ? &m_bannerfont : nullptr; }
CFont* FontSet::GetSubbannerFont() { return m_subbannerfont.GetSafeHandle() != NULL ? &m_subbannerfont : nullptr; }
CFont* FontSet::GetActionFont() { return m_actionfont.GetSafeHandle() != NULL ? &m_actionfont : nullptr; }
CFont* FontSet::GetNormalFont() { return m_normalfont.GetSafeHandle() != NULL ? &m_normalfont : nullptr; }