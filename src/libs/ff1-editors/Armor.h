#if !defined(AFX_ARMOR_H__7A92585B_823D_4EA7_A12F_EA5E0EF2156E__INCLUDED_)
#define AFX_ARMOR_H__7A92585B_823D_4EA7_A12F_EA5E0EF2156E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Armor.h : header file
//

#include <EditorWithBackground.h>
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CArmor dialog

class CArmor : public CEditorWithBackground
{
// Construction
public:
	CArmor(CWnd* pParent = nullptr);   // standard constructor

	int cur;

protected:
	void LoadRom();
	virtual void SaveRom();
	void LoadValues();
	virtual void StoreValues();

// Dialog Data
	enum { IDD = IDD_ARMOR };
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
	CClearEdit	m_price;
	CClearEdit	m_evade;
	CStrikeCheck	m_def8;
	CStrikeCheck	m_def7;
	CStrikeCheck	m_def6;
	CStrikeCheck	m_def5;
	CStrikeCheck	m_def4;
	CStrikeCheck	m_def3;
	CStrikeCheck	m_def2;
	CStrikeCheck	m_def1;
	CClearEdit	m_def;
	CClearComboBox	m_spellcast;
	CClearListBox	m_armorlist;
	CClearStatic m_static1;
	CClearStatic m_static2;
	CClearStatic m_static3;
	CClearStatic m_static4;
	CCloseButton m_closebutton;
	CClearButton m_savebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CHelpbookButton m_helpbookbutton;

	int CLASS_COUNT = -1;
	int ARMORTEXT_OFFSET = -1;
	int BASICTEXT_PTRADD = -1;
	int ARMOR_COUNT = -1;
	int ATTACKTEXT_OFFSET = -1;
	int ATTACKTEXT_PTRADD = -1;
	int ATTACK_COUNT = -1;
	int MAGICTEXT_OFFSET = -1;
	int MAGIC_COUNT = -1;
	int CLASSTEXT_OFFSET = -1;
	int ARMOR_OFFSET = -1;
	int ARMOR_BYTES = -1;
	int ARMORPRICE_OFFSET = -1;
	unsigned int ARMORPERMISSIONS_OFFSET = (unsigned int)-1;

	int BANK0A_OFFSET = -1;
	int BINPRICEDATA_OFFSET = -1;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSelchangeArmorlist();
};

#endif // !defined(AFX_ARMOR_H__7A92585B_823D_4EA7_A12F_EA5E0EF2156E__INCLUDED_)
