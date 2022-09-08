#pragma once

// CSubDlgDynaButtons dialog

class CSubDlgDynaButtons : public CDialogEx
{
	DECLARE_DYNAMIC(CSubDlgDynaButtons)

public:
	CSubDlgDynaButtons(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSubDlgDynaButtons();

	BOOL CreateOverControl(CWnd* pctltoreplace);
	void UseFixedButtonSize(CSize size);
	void UseDynamicButtonSize();
	void SetButtonFont(CFont* font);
	void Add(UINT id, int eventindex, CString text);
	void Preload(UINT id, int eventindex, CString text);
	void ClearButtons();

	void UpdateLayout();
	void SuppressLayout(bool suppress);
	bool SuppressingLayout() const;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUB_DYNABUTTONS };
#endif

protected:
	bool m_supresslayout = false;
	bool m_usedynabuttonsize = true;
	CSize m_fixedbuttonsize;
	CFont* m_buttonfont = nullptr;
	CPoint m_buttonorg;
	std::vector<CWnd*> m_buttons;

	CWnd* get_last_child();
	void handle_size();
	void handle_size(int clientx, int clienty);
	void slide_buttons(int amount);

	// Virtual overrides and message handlers
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnFfttmShowDesc(WPARAM wparam, LPARAM lparam);
	afx_msg void OnNmRclickActionButton(UINT id, NMHDR* pNotify, LRESULT* result);
	afx_msg void OnBnClickedActionButton(UINT id);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
