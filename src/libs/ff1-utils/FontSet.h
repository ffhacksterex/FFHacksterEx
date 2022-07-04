#pragma once

class FontSet
{
public:
	FontSet();
	~FontSet();

	void Clear();
	void SetDefaults();

	CFont* GetBannerFont();
	CFont* GetSubbannerFont();
	CFont* GetActionFont();
	CFont* GetNormalFont();

private:
	CFont m_bannerfont;
	CFont m_subbannerfont;
	CFont m_actionfont;
	CFont m_normalfont;
};
