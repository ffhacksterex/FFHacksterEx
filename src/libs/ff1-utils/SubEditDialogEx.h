#pragma once
#include <afxdialogex.h>

class CSubEditDialogEx : public CDialogEx
{
	DECLARE_DYNAMIC(CSubEditDialogEx)
public:
	CSubEditDialogEx(UINT idres, CWnd* parent = nullptr);
	CSubEditDialogEx(LPCTSTR tszres, CWnd* parent = nullptr);
	virtual ~CSubEditDialogEx();

protected:
	virtual INT_PTR DoModal();
};
