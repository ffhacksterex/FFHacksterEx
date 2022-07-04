#pragma once

#include "FFBaseDlg.h"

class CSaveableDialog : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CSaveableDialog)
public:
	CSaveableDialog(UINT nID, CWnd* pParent);
	virtual ~CSaveableDialog();

protected:
	virtual bool DoSave();
	virtual void StoreValues();
	virtual void SaveRom();

	virtual void OnSave();
	virtual void OnOK();
};
