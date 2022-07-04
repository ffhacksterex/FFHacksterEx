// CDataListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DataListDialog.h"
#include "afxdialogex.h"
#include "ui_helpers.h"

// CDataListDialog dialog

#define IDD IDD_DATALIST

IMPLEMENT_DYNAMIC(CDataListDialog, CDialogEx)

CDataListDialog::CDataListDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD, pParent)
{
}

CDataListDialog::~CDataListDialog()
{
}

BOOL CDataListDialog::CreateModeless(CWnd* parent)
{
	auto p = iif(parent, GetDesktopWindow());
	return CDialogEx::Create(IDD, p);
	//return CDialogEx::Create(IDD, parent);
}

void CDataListDialog::Load(const std::vector<std::pair<CString, DWORD>>& data)
{
	try {
		Ui::RedrawScope redraw(&m_listbox);
		m_listbox.ResetContent();
		for (const auto& d : data) {
			auto index = m_listbox.AddString(d.first);
			m_listbox.SetItemData(index, d.second);
		}
	}
	catch (std::exception& ex) {
		CString msg;
		msg.Format("Unable to load the data list: %s", ex.what());
		AfxMessageBox(msg);
	}
}

void CDataListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX, m_listbox);
}


BEGIN_MESSAGE_MAP(CDataListDialog, CDialogEx)
END_MESSAGE_MAP()


// CDataListDialog message handlers

