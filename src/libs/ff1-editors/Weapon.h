#pragma once
// Weapon.h : header file

#include "EditorWithBackground.h"
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CWeapon dialog

class CWeapon : public CEditorWithBackground
{
// Construction
public:
	CWeapon(CWnd* pParent = nullptr);   // standard constructor

	int cur;
	int cur_pal;
	CRect rcPalette;
	CRect rcFinger;
	CRect rcGraphic;

protected:
	CImageList m_graphics;
	void ResetGraphicList();

	void LoadRom();
	virtual void SaveRom();
	void LoadValues();
	virtual void StoreValues();
	virtual void PaintClient(CDC & dc);

// Dialog Data
	//{{AFX_DATA(CWeapon)
	enum { IDD = IDD_WEAPON };
	CClearButton	m_editgfx;
	CClearListBox	m_weaponlist;
	CClearEdit	m_critrate;
	CComboBox	m_spellcast;
	CClearEdit	m_price;
	CClearEdit	m_hit;
	CComboBox	m_gfx;
	CClearEdit	m_damage;
	CClearButton m_editlabelbutton;
	CStrikeCheck	m_elem8;
	CStrikeCheck	m_elem7;
	CStrikeCheck	m_elem6;
	CStrikeCheck	m_elem5;
	CStrikeCheck	m_elem3;
	CStrikeCheck	m_elem4;
	CStrikeCheck	m_elem2;
	CStrikeCheck	m_elem1;
	CStrikeCheck	m_use9;
	CStrikeCheck	m_use8;
	CStrikeCheck	m_use7;
	CStrikeCheck	m_use6;
	CStrikeCheck	m_use5;
	CStrikeCheck	m_use4;
	CStrikeCheck	m_use3;
	CStrikeCheck	m_use2;
	CStrikeCheck	m_use12;
	CStrikeCheck	m_use11;
	CStrikeCheck	m_use10;
	CStrikeCheck	m_use1;
	CStrikeCheck	m_cat8;
	CStrikeCheck	m_cat7;
	CStrikeCheck	m_cat6;
	CStrikeCheck	m_cat5;
	CStrikeCheck	m_cat4;
	CStrikeCheck	m_cat3;
	CStrikeCheck	m_cat2;
	CStrikeCheck	m_cat1;
	CCloseButton m_closebutton;
	CClearButton m_savebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CHelpbookButton m_helpbookbutton;

	int CLASS_COUNT = -1;
	int WEAPON_COUNT = -1;
	int WEAPON_OFFSET = -1;
	int WEAPON_BYTES = -1;
	int WEAPONPRICE_OFFSET = -1;
	unsigned int WEAPONPERMISSIONS_OFFSET = (unsigned int)-1;

	int WEAPONMAGICGRAPHIC_OFFSET = -1;
	int BANK0A_OFFSET = -1;
	int BINBANK09GFXDATA_OFFSET = -1;
	int BINPRICEDATA_OFFSET = -1;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangeWeaponlist();
	afx_msg void OnEditlabel();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeGfx();
	afx_msg void OnEditgfx();
};
