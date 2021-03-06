#pragma once

#include "BackgroundMixin.h"
#include "FFBaseDlg.h"
#include "SubBanner.h"
class AppSettings;

// CDialogWithBackground dialog

class CDialogWithBackground : public CFFBaseDlg, public CBackgroundMixin
{
	DECLARE_DYNAMIC(CDialogWithBackground)

public:
	CDialogWithBackground(UINT IDD, CWnd* pParent = nullptr);   // standard constructor
	CDialogWithBackground(UINT IDD, UINT nResid, CWnd* pParent = nullptr);
	virtual ~CDialogWithBackground();

	AppSettings * AppStgs = nullptr;

protected:
	CSubBanner m_banner;

	virtual void PaintBanner(CDC & dc);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
