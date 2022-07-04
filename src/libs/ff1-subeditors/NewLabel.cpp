// NewLabel.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "NewLabel.h"
#include <FFHacksterProject.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewLabel dialog


CNewLabel::CNewLabel(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(CNewLabel::IDD, pParent)
{
	m_label = _T("");
}


void CNewLabel::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LABEL, m_label);
	if (length != -1)
		DDV_MaxChars(pDX, m_label, length);
}


BEGIN_MESSAGE_MAP(CNewLabel, CFFBaseDlg)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

CString ChangeLabel(CFFHacksterProject & proj, int maxlength, CString label, IniWriteFunc func, int arid, CListBox * m_list, CComboBox * m_combo)
{
	CNewLabel dlg;
	dlg.length = maxlength;
	dlg.m_label = label;
	if (dlg.DoModal() == IDOK) {
		int temp;
		label = dlg.m_label;
		func(proj, arid, label);
		if (m_combo != nullptr) {
			temp = m_combo->GetCurSel();
			m_combo->DeleteString(arid);
			m_combo->InsertString(arid, label);
			m_combo->SetCurSel(temp);
		}
		if (m_list != nullptr) {
			temp = m_list->GetCurSel();
			m_list->DeleteString(arid);
			m_list->InsertString(arid, label);
			m_list->SetCurSel(temp);
		}
	}
	return label;
}
