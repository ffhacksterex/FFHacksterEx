#include "stdafx.h"
#include "FFHPropertyGridFolderProperty.h"
#include "ui_prompts.h"

IMPLEMENT_DYNAMIC(CFFHPropertyGridFolderProperty, CMFCPropertyGridFileProperty)

CFFHPropertyGridFolderProperty::CFFHPropertyGridFolderProperty(const CString& strName, const CString& strFolderName, DWORD_PTR dwData, LPCTSTR lpszDescr)
	: CMFCPropertyGridFileProperty(strName, strFolderName, dwData, lpszDescr)
{
}

CFFHPropertyGridFolderProperty::CFFHPropertyGridFolderProperty(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt, DWORD dwFileFlags, LPCTSTR lpszFilter, LPCTSTR lpszDescr, DWORD_PTR dwData)
	: CMFCPropertyGridFileProperty(strName, bOpenFileDialog, strFileName, lpszDefExt, dwFileFlags, lpszFilter, lpszDescr, dwData)
{
}

CFFHPropertyGridFolderProperty::~CFFHPropertyGridFolderProperty()
{
}

void CFFHPropertyGridFolderProperty::OnClickButton(CPoint point)
{
	if (!m_bIsFolder) {
		CMFCPropertyGridFileProperty::OnClickButton(point);
		return;
	}

	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();

	CString strPath(m_varValue.bstrVal);
	BOOL bUpdate = FALSE;

	// This app isn't set up to use the shell manager,
	// which also lacks the customizations we're using,
	// so use our UI prompt function instead.
	auto result = Ui::PromptForFolder(AfxGetMainWnd(), "Select a folder", strPath);
	if (result) {
		bUpdate = true;
		strPath = result.value;
	}

	if (bUpdate)
	{
		if (m_pWndInPlace != NULL)
			m_pWndInPlace->SetWindowText(strPath);
		m_varValue = (LPCTSTR)strPath;
	}
	m_bButtonIsDown = FALSE;
	Redraw();

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}
