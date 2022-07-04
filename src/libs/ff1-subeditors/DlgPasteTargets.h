#pragma once

#include "resource_subeditors.h"
#include <vector>
#include "afxwin.h"
#include <FFBaseDlg.h>

struct PasteTarget
{
	CString caption;
	bool checked;
	CString warning;
};

// CDlgPasteTargets dialog

class CDlgPasteTargets : public CFFBaseDlg
{
	DECLARE_DYNAMIC(CDlgPasteTargets)

public:
	CDlgPasteTargets(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgPasteTargets();

	CString Title;
	void SetTargets(const std::vector<PasteTarget> & targets);
	bool IsChecked(int index) const;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PASTETARGETS };
#endif

protected:
	std::vector<PasteTarget> m_targets;
	std::vector<bool> m_checked;
	std::vector<CButton*> m_pastechecks;
	std::vector<CStatic*> m_infostatics;

	std::vector<CButton*> GetChecks();
	void DestroyChecks();

	CToolTipCtrl m_tooltips;
	CButton m_pastetarget1;

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
};
