#if !defined(AFX_WEPMAGGRAPHIC_H__2724C844_E3E9_4D32_8DEA_23529A62EDCB__INCLUDED_)
#define AFX_WEPMAGGRAPHIC_H__2724C844_E3E9_4D32_8DEA_23529A62EDCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WepMagGraphic.h : header file
//

#include "resource_subeditors.h"
#include "DRAW_STRUCT.h"
#include <SaveableDialog.h>
class CFFHacksterProject;
class FFH2Project;

/////////////////////////////////////////////////////////////////////////////
// CWepMagGraphic dialog

class CWepMagGraphic : public CSaveableDialog
{
// Construction
public:
	CWepMagGraphic(CWnd* pParent = nullptr);   // standard constructor

	FFH2Project* Proj2 = nullptr;
	CFFHacksterProject * Project = nullptr;
	bool IsWeapon;
	BYTE paletteref;
	short graphic;

	BYTE PicFormation[8];
	BYTE palette[4];
	BYTE bitmap[0x200];
	DRAW_STRUCT draw;

	void SetCaption(CString caption);

// Dialog Data
	enum { IDD = IDD_WEPMAG_GRAPHIC };
	CButton	m_weapon;
	CButton	m_magic;

	CString m_caption = "";
	int WEAPONMAGICGRAPHIC_OFFSET = -1;
	int BINBANK09GFXDATA_OFFSET = -1;

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
	void LoadRom();
	virtual void SaveRom();
	virtual void StoreValues();

	// Generated message map functions
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnExportbitmap();
	afx_msg void OnImportbitmap();
};

#endif // !defined(AFX_WEPMAGGRAPHIC_H__2724C844_E3E9_4D32_8DEA_23529A62EDCB__INCLUDED_)
