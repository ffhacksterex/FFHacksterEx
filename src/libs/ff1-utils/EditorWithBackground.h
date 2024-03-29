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
	UINT m_oddefid = 0; // OwnerDraw buttons and BS_DEFPUSHBUTTON are
						// mutually exclusive, so using them as default
	                    // buttons (e.g. IDOK) doesn't work.
						// This is a quick per-dialog workaround.
						//FUTURE - instead, update CClearButton to store
						//	the BS_DEFPUSHBUTTON state internally and
						//	check it everywhere that's needed.
						//	That's a lot more work than the above workaround.

	virtual void PaintBanner(CDC & dc);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void SetODDefButtonID(UINT id);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnGetDefId(WPARAM wparam, LPARAM lparam);
};
