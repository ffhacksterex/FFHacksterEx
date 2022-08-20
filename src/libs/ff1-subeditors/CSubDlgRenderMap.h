#pragma once

#include "CSubDlgRenderMapStatic.h"

// CSubDlgRenderMap dialog

class CSubDlgRenderMap : public CDialogEx
{
	DECLARE_DYNAMIC(CSubDlgRenderMap)

public:
	CSubDlgRenderMap(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSubDlgRenderMap();

	BOOL CreateOverControl(CWnd* pParent, CWnd* pOver);
	void SetRenderState(const sRenderMapState& state);
	const sRenderMapState& GetRenderState() const;
	void InvalidateFrame();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUBDLG_RENDERMAP };
#endif

protected:
	CPoint m_rmbasept;

	// Internal implementation
	BOOL init();
	int handle_scroll(UINT nBar, UINT nSBCode, UINT nPos);
	void handle_sizing();
	void handle_sizing(int clientx, int clienty);

	// Controls
	CSubDlgRenderMapStatic m_frame;

	// Overrides and message handlers
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PreSubclassWindow();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
