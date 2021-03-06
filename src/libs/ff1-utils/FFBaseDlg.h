#pragma once

#include "ff1.colors.h"
#include <rc_functions.h>

// CFFBaseDlg dialog

class CFFBaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFFBaseDlg)

public:
	CFFBaseDlg(UINT nID, CWnd* pParent = nullptr, UINT helpid = 0);   // standard constructor
	virtual ~CFFBaseDlg();

	bool PreloadControlMappings = false;

	virtual INT_PTR DoModal();
	bool InitFailed() const;
	UINT GetDlgId() const;

private:
	UINT m_idd = 0;
	UINT m_helpid = 0;

protected:
	bool m_initfailed = false;
	Rc::dialogmappings m_dlgmappings;

	BOOL EndAndReturn(int result, CString message);
	BOOL FailInitAndReturn(int result, CString message);

	CWnd* GetControlByName(CString controlname);

	// The original help ID is private; descendants can ignore it and override GetHelpID() as desired
	// to use a different ID.
	virtual UINT GetHelpID() const { return m_helpid; }
	virtual void InitFonts();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnBnClickedHelpbook();
	afx_msg void OnBnClickedCancel2();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
