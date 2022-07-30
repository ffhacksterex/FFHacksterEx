#if !defined(AFX_ABOUT_H__1D008DC7_8F53_448E_A939_2191DC29646B__INCLUDED_)
#define AFX_ABOUT_H__1D008DC7_8F53_448E_A939_2191DC29646B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// About.h : header file
//

#include "ClearStatic.h"
#include "ClearButton.h"
#include "DialogWithBackground.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CAbout dialog

class CAbout : public CDialogWithBackground
{
// Construction
public:
	CAbout(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	enum { IDD = IDD_ABOUT };
	const CString STRIDD = "IDD_ABOUT";

protected:
	void HandleLink(LPNMHDR phdr);

	CClearStatic m_releasestatic;
	CClearStatic m_label1static;
	CClearStatic m_label2static;
	CClearStatic m_details1static;
	CClearStatic m_details2static;
	CClearStatic m_pdstatic;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton = { IDPNG_CLOSESM, "PNG" };
	CClearStatic m_clearlabel3;
	CClearStatic m_cleardetail3;
	CClearStatic m_disassemblystatic;
	CClearStatic m_progtitlestatic;
	CLinkCtrl m_linkdisch;
	CLinkCtrl m_linkpaulygon;
	CLinkCtrl m_linkgamefaqs;
	CLinkCtrl m_7ziplicense;
	CLinkCtrl m_linkiconmonstr;

	HICON m_hIcon;

	virtual void PaintClient(CDC & dc);

// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnNMClickSyslinkDisch(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMReturnSyslinkDisch(NMHDR* pNMHDR, LRESULT* pResult);
};

#endif // !defined(AFX_ABOUT_H__1D008DC7_8F53_448E_A939_2191DC29646B__INCLUDED_)
