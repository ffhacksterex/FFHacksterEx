#if !defined(AFX_NEWLABEL_H__21BAFDE5_8220_4EC8_AEC3_CE42F4375214__INCLUDED_)
#define AFX_NEWLABEL_H__21BAFDE5_8220_4EC8_AEC3_CE42F4375214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewLabel.h : header file
//

#include "resource_subeditors.h"
#include <function_types.h>
#include <FFBaseDlg.h>
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CNewLabel dialog

class CNewLabel : public CFFBaseDlg
{
// Construction
public:
	CNewLabel(CWnd* pParent = nullptr);   // standard constructor
	int length;

// Dialog Data
	enum { IDD = IDD_NEWLABEL };
	CString	m_label;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

CString ChangeLabel(CFFHacksterProject & proj, int maxlength, CString label, IniWriteFunc func, int arid, CListBox* m_list, CComboBox* m_combo);

#endif // !defined(AFX_NEWLABEL_H__21BAFDE5_8220_4EC8_AEC3_CE42F4375214__INCLUDED_)
