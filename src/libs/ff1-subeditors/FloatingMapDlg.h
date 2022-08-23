#pragma once

#include <afxpanedialog.h>
#include <DrawingToolButton.h>
#include "CSubDlgRenderMap.h"
#include <memory>

//DUPE - WMA_SHOWFLOATINGMAP
#define WMA_SHOWFLOATINGMAP ((WM_APP) + 3)		// WPARAM: 1 = show, 0 = hide


// CFloatingMapDlg dialog

class CFloatingMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFloatingMapDlg)

public:
	CFloatingMapDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFloatingMapDlg();

	BOOL Init(const sRenderMapState& state, const std::vector<sMapDlgButton>& buttons);
	void InvalidateMap();
	void UpdateControls();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FLOATING_MAP };
#endif

protected:
	CFFHacksterProject* cart; //TODO - remove and replace with Project

	CFFHacksterProject* Project = nullptr;
	CPoint m_rmbasept;
	CRect rcMap;
	CPoint ScrollOffset;

	// Implementation
	void UpdateToolIndex();
	bool set_render_state(const sRenderMapState& state);
	bool set_buttons(const std::vector<sMapDlgButton>& buttons);
	void handle_sizing(int clientx, int clienty);
	void handle_close();
	CRect get_sizer_rect(bool client);

	// Controls
	CSubDlgRenderMap m_subdlg;
	std::vector<std::shared_ptr<CDrawingToolButton>> m_buttons;

	CStatic m_subdlgover;
	CStatic m_buttonanchor;
	CButton m_customizebutton;

	// Overrides and handlers
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnDrawToolBnClick(WPARAM wparam, LPARAM lparam);
	afx_msg void OnClickedCustomtool();
	afx_msg void OnClickedButtonImportMap();
	afx_msg void OnClickedButtonExportMap();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnPaint();
};
