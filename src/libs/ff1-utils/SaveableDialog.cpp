#include "stdafx.h"
#include "SaveableDialog.h"

IMPLEMENT_DYNAMIC(CSaveableDialog, CFFBaseDlg)

CSaveableDialog::CSaveableDialog(UINT nID, CWnd * pParent)
	: CFFBaseDlg(nID, pParent)
{
}

CSaveableDialog::~CSaveableDialog()
{
}

bool CSaveableDialog::DoSave()
{
	try {
		CWaitCursor wait;
		this->StoreValues();
		this->SaveRom();
		return true;
	}
	catch (std::exception & ex) {
		AfxMessageBox("Unable to save ROM:\n" + CString(ex.what()), MB_ICONERROR);
	}
	catch (...) {
		AfxMessageBox("Unable to save ROM.", MB_ICONERROR);
	}
	return false;
}

void CSaveableDialog::StoreValues()
{
}

void CSaveableDialog::SaveRom()
{
}

void CSaveableDialog::OnSave()
{
	this->DoSave();
}

void CSaveableDialog::OnOK()
{
	if (this->DoSave())
		CFFBaseDlg::OnOK();
}