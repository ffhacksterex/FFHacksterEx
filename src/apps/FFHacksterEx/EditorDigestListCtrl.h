#pragma once

#include <Editors2.h>
class CFFHacksterProject;

enum { COL_NAME, COL_STAT, COL_DISP, COL_LEV, COL_SRC, COL_FOLDER };

// CEditorDigestListCtrl

class CEditorDigestListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CEditorDigestListCtrl)

public:
	CEditorDigestListCtrl();
	virtual ~CEditorDigestListCtrl();

	CFFHacksterProject * Project = nullptr;
	Editors2::sEditorLinkVector * Links = nullptr;

	CFont * GetBoldFont();
	Editors2::sEditorLink & GetLink(int item);

protected:
	CFont m_font;
	CFont m_boldfont;
	CString m_tempimagepath;

	bool IsTransparent() const;
	void DoDrawItem(CDC & dc, LPDRAWITEMSTRUCT pds);

	virtual void DrawItem(LPDRAWITEMSTRUCT pds);
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT pmi);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};
