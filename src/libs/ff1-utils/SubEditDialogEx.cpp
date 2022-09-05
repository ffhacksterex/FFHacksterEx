#include "stdafx.h"
#include "SubEditDialogEx.h"

IMPLEMENT_DYNAMIC(CSubEditDialogEx, CDialogEx)

CSubEditDialogEx::CSubEditDialogEx(UINT idres, CWnd* parent)
	: CDialogEx(idres, parent)
{
}

CSubEditDialogEx::CSubEditDialogEx(LPCTSTR tszres, CWnd* parent)
	: CDialogEx(tszres, parent)
{
}

CSubEditDialogEx::~CSubEditDialogEx()
{
}

INT_PTR CSubEditDialogEx::DoModal()
{
	try {
		return CDialogEx::DoModal();
	}
	catch (std::exception& ex) {
		CString msg;
		msg.Format("Subeditor aborted with the following error:\n%s", ex.what());
		AfxMessageBox(msg);
		return IDABORT;
	}
}