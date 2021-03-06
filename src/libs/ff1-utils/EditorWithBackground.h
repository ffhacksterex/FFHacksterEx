#pragma once

#include "BackgroundMixin.h"
#include "SubBanner.h"
#include "SaveableDialog.h"
class CFFHacksterProject;

// CEditorWithBackground dialog

class CEditorWithBackground : public CSaveableDialog, public CBackgroundMixin
{
	DECLARE_DYNAMIC(CEditorWithBackground)

public:
	CEditorWithBackground(UINT nID, CWnd* pParent = nullptr);   // standard constructor
	CEditorWithBackground(UINT nID, UINT uResImage, CWnd* pParent = nullptr);
	virtual ~CEditorWithBackground();

	CFFHacksterProject* Project = nullptr;

protected:
	CSubBanner m_banner;

	virtual void PaintBanner(CDC & dc);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
