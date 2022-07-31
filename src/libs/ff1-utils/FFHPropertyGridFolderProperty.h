#pragma once

#include <afxpropertygridctrl.h>

class CFFHPropertyGridFolderProperty :
    public CMFCPropertyGridFileProperty
{
	DECLARE_DYNAMIC(CFFHPropertyGridFolderProperty)
public:
	CFFHPropertyGridFolderProperty(const CString& strName, const CString& strFolderName, DWORD_PTR dwData = 0, LPCTSTR lpszDescr = NULL);
	CFFHPropertyGridFolderProperty(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CFFHPropertyGridFolderProperty();

public:
	virtual void OnClickButton(CPoint point);
};

