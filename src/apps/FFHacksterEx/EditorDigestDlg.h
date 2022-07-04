#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include <EditorWithBackground.h>
#include <ClearListCtrl.h>
#include <InplaceCombo.h>
#include <Editors2.h>
#include "EditorDigestListCtrl.h"
class CFFHacksterProject;

// CEditorDigestDlg dialog

class CEditorDigestDlg : public CEditorWithBackground
{
	DECLARE_DYNAMIC(CEditorDigestDlg)

public:
	CEditorDigestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditorDigestDlg();

	CString AppFolder;
	Editors2::sEditorInfoVector EditorInfos;

// Dialog Data
	enum { IDD = IDD_EDITORDIGEST2 };

protected:
	Editors2::sEditorLinkVector m_links;

	virtual void InitFonts();

	void UpdateColumnWidth(int col);
	void UpdateRow(int item);
	void Edit(int item, int subitem);
	Editors2::sEditorLink & GetLink(int item);

	CFont m_boldfont;
	CSubBanner m_banner;
	CInplaceCombo m_inplacecombo;

	CEditorDigestListCtrl m_editorlist;
	CClearStatic m_reorderstatic;
	CClearStatic m_infostatic;
	CCloseButton m_closebutton;
	CClearButton m_upbutton = { IDPNG_ARROWUP, "PNG" };
	CClearButton m_downbutton = { IDPNG_ARROWDOWN, "PNG" };
	CHelpbookButton m_helpbookbutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNmClickEditorList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnClickedEdBtnUp();
	afx_msg void OnClickedEdBtnDown();	
	afx_msg void OnClickedSelectBuiltin();
	CClearButton m_selbuiltinsbutton;
};
