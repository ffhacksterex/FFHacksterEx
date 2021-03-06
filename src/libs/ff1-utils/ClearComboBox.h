#pragma once

// CClearComboInbeddedEdit

class CClearComboInbeddedEdit : public CEdit
{
	DECLARE_DYNAMIC(CClearComboInbeddedEdit)
public:
	CClearComboInbeddedEdit();
	virtual ~CClearComboInbeddedEdit();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};



// CClearComboBox

class CClearComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CClearComboBox)

public:
	CClearComboBox();
	virtual ~CClearComboBox();

protected:
	void SubclassEdit();

	// Dialog controls
	CClearComboInbeddedEdit m_item;

	// Message handlers
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCbnDropdown();
	afx_msg void OnCbnCloseup();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSetCurSel(WPARAM wparam, LPARAM lparam);
};
