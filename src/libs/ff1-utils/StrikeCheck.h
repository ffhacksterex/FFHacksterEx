#pragma once

#include "ClearButton.h"

// CStrikeCheck

class CStrikeCheck : public CClearButton
{
	DECLARE_DYNAMIC(CStrikeCheck)

public:
	CStrikeCheck();
	virtual ~CStrikeCheck();

	static BOOL RenderAsCheckbox;
	static COLORREF SharedCheckedColor;
	static CFont* SharedCheckedFont;

	void SetOverrideColor(COLORREF color = 0x80000000);
	void SetOverrideFont(CFont * font);

protected:
	int m_checkstate;

	COLORREF UncheckedColor = RGB(0, 0, 0);
	CFont * UncheckedFont = nullptr;
	bool UseOverriddenColor = false;
	bool UseOverriddenFont = false;

	void Toggle();
	bool IsOwnerDraw() const;

	virtual void DrawItem(LPDRAWITEMSTRUCT pdi);
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnSetCheck(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnGetCheck(WPARAM wparam, LPARAM lparam);
};
