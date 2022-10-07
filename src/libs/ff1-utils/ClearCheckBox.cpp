// ClearCheckBox.cpp : implementation file
//

#include "stdafx.h"
#include "ClearCheckBox.h"


// CClearCheckBox

IMPLEMENT_DYNAMIC(CClearCheckBox, CButton)

CClearCheckBox::CClearCheckBox()
{
}

CClearCheckBox::~CClearCheckBox()
{
}


BEGIN_MESSAGE_MAP(CClearCheckBox, CButton)
END_MESSAGE_MAP()


// CClearCheckBox message handlers

BOOL CClearCheckBox::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_RETURN) {
			::DispatchMessage(pMsg);
			auto pdlg = DYNAMIC_DOWNCAST(CDialog, GetParent());
			auto defid = (pdlg != nullptr) ? pdlg->GetDefID() : 0;

			// avoid the dozen reserved dialog control IDs (IDOK, etc.)
			auto pctl = defid > 12 ? pdlg->GetDlgItem(defid) : nullptr;
			if (defid > 0)
				pdlg->PostMessage(WM_COMMAND, MAKEWPARAM(defid, BN_CLICKED), (LPARAM)pctl);
			return true;
		}
		break;
	}
	return CButton::PreTranslateMessage(pMsg);
}
