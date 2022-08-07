#pragma once

#define WMA_SHOWFLOATINGMAP ((WM_APP) + 3)

// CFloatingMapDlg dialog

class CFloatingMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFloatingMapDlg)

public:
	CFloatingMapDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFloatingMapDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FLOATING_MAP };
#endif

protected:
	void HandleClose();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnClose();
};
