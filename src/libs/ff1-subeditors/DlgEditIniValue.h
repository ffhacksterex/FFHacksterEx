#pragma once

#include "afxwin.h"
#include <FFBaseDlg.h>

// CDlgEditIniValue dialog

class CDlgEditIniValue : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CDlgEditIniValue)

public:
	CDlgEditIniValue(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgEditIniValue();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDITINIVALUE };
#endif

	enum Disable { None = 0, Key = 1, Group = 2 };

	bool HexUppercase;

	CString Label;
	CString KeyName;
	CString Value;
	CString Type;
	CString GroupName;
	Disable Disable;

protected:
	CEdit m_keynameedit;
	CEdit m_valueedit;
	CEdit m_groupedit;
	CComboBox m_typecombo;

	CString m_prevtype;

	void LoadTypes();
	bool ValidateEntry();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()	
	afx_msg void OnCbnSelchangeCombo1();
	CStatic m_valuestatic;
};
