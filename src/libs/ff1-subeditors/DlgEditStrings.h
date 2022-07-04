#pragma once

#include "resource_subeditors.h"
#include "vector_types.h"
#include "function_types.h"
#include "InplaceEdit.h"
#include "afxwin.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;

// CDlgEditStrings dialog

class CDlgEditStrings : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CDlgEditStrings)

public:
	CDlgEditStrings(CWnd* pParent = nullptr, UINT helpid = 0);   // standard constructor
	virtual ~CDlgEditStrings();

	CFFHacksterProject * m_proj = nullptr;
	BoolAction SaveAction = nullptr;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STRINGS };
#endif
	CBorderedListBox m_listgroups;
	CBorderedListCtrl m_elementlist;
	CInplaceEdit m_inplaceedit;
	int m_curindex = -1;

protected:
	CStatic m_countstatric;

	void InitMainList();
	void LoadValues();
	void StoreValues();
	bool SaveIni();

	void PrepTextEdit(int item, int subitem);
	void BeginTextEdit(int item, int subitem, StringTransformFunc xformfunc);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedSave();
	afx_msg void OnLbnSelchangeListgroups();
	afx_msg void OnNMClickListelements(NMHDR *pNMHDR, LRESULT *pResult);
};
