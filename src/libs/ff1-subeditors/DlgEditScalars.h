#pragma once

#include "resource_subeditors.h"
#include "function_types.h"
#include "vector_types.h"
#include "set_types.h"
#include "InplaceEdit.h"
#include "InplaceCheck.h"
#include "ListCtrlHideColumns.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;

// CDlgEditScalars dialog

class CDlgEditScalars : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CDlgEditScalars)

	using ListCtrlClass =  CListCtrlHideColumns; // CListCtrl

public:
	CDlgEditScalars(CWnd* pParent = nullptr, UINT helpid = 0);   // standard constructor
	virtual ~CDlgEditScalars();

	CFFHacksterProject * m_proj = nullptr;
	BoolAction SaveAction = nullptr;
	mfcstringvector GroupNames;
	bool Exclude = false;
	bool EditLabels = false;
	bool HexUppercase = false;
	bool ShowBuiltinGroups = true;

protected:
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCALARS };
#endif
	enum { COL_INDEX, COL_NAME, COL_TYPE, COL_VALUE, COL_LABELORDESC, COL_DESC, COUNT_COL };

	struct Node {
		CString name;
		CString group;
		bool reserved = false;
	};
	using nodevector = std::vector<Node>;

	const CString UNGROUPED;

	CBorderedListBox m_listgroups;
	ListCtrlClass m_elementlist;
	CMFCLinkCtrl m_importlink;
	CInplaceEdit m_inplaceedit;
	CInplaceCheck m_inplacecheck;
	CButton m_showhiddencheck;
	CFont m_italicfont;
	CFont m_font;

	CString m_inibackup;
	nodevector m_scalarnodes;

	int m_curindex = -1;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	static mfcstringset ReadRefList(CString inipath);
	void Reload();
	bool SaveIni();
	void ShowAll();
	void ShowSingleGroup(CString groupname);
	void ShowGroups(const mfcstringvector & groupnames, bool exclude);
	void LoadGroups(const mfcstringvector & groupnames);
	void RemoveScalarNode(CString nodename);
	Node * FindScalarNode(CString nodename);

	void InitMainList();
	void LoadValues();
	void StoreValues();
	CString PromptForImportFile(CWnd * parent);

	void PrepTextEdit(int item, int subitem);
	void BeginTextEdit(int item, int subitem, StringTransformFunc intfunc);

	void PrepCheckEdit(int item, int subitem);
	void BeginCheckEdit(int item, int subitem, StringTransformFunc xformfunc);

	void Edit(CString type, int item, int subitem);
	void DoIntEdit(CString type, int item, int subitem, int intwidth);
	void DoArrayEdit(CString type, int item, int subitem);
	void DoColorEdit(int item, int subitem);
	void DoTextEdit(int item, int subitem);
	void DoCheckEdit(int item, int subitem);

	CString ValidateAddrIntAndUpdate(CString type, int item, int subitem, int bytewidth, CString text);
	CString ValidateDecIntAndUpdate(CString type, int item, int subitem, int bytewidth, CString text);
	CString ValidateHexIntAndUpdate(CString type, int item, int subitem, int bytewidth, CString text);

	CString HexCase(int value, int bytewidth);
	CString AddrCase(int value, int bytewidth);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnClickedScalarsButtonSave();
	afx_msg void OnSelchangeGroupList();
	afx_msg void OnClickElementList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAcceleratorF2();
	afx_msg void OnClickAdd();
	afx_msg void OnClickDelete();
	//afx_msg void OnClickRevert(); // see ff1-coredefs/FFHacksterProject.cpp RevertValues definition for more info
	afx_msg void OnCustomdrawScalarsListelements(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedCheckShowhidden();
};
