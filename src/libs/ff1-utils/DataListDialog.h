#pragma once
#include <vector>

// CDataListDialog dialog

class CDataListDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDataListDialog)

public:
	CDataListDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDataListDialog();

	BOOL CreateModeless(CWnd* parent = nullptr);
	void Load(const std::vector<std::pair<CString,DWORD>> & data);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATALIST };
#endif

protected:
	CListBox m_listbox;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
