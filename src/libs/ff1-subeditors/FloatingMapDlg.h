#pragma once

#include <afxpanedialog.h>
#include <DrawingToolButton.h>
#include "CSubDlgRenderMap.h"
#include <memory>

#define WMA_SHOWFLOATINGMAP ((WM_APP) + 3)

struct sMapDlgButton {
	UINT resid;
	CString restype;
	int param;
};


// CFloatingMapDlg dialog

class CFloatingMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFloatingMapDlg)

public:
	CFloatingMapDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFloatingMapDlg();

	void SetRenderState(const sRenderMapState& state);
	bool SetButtons(const std::vector<sMapDlgButton> & buttons);
	void InvalidateMap();

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
	BYTE mousedown = 0;

	// Implementation
	void handle_sizing(int clientx, int clienty);
	void handle_close();

	// Controls
	CSubDlgRenderMap m_subdlg;
	std::vector<std::shared_ptr<CDrawingToolButton>> m_buttons;

	CStatic m_subdlgover;
	CStatic m_buttonanchor;

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
};
