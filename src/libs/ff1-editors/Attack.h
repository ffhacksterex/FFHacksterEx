#if !defined(AFX_ATTACK_H__819F5C22_126A_411C_BB13_B4818F8CD3CE__INCLUDED_)
#define AFX_ATTACK_H__819F5C22_126A_411C_BB13_B4818F8CD3CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Attack.h : header file
//

#include <EditorWithBackground.h>
class CFFHacksterProject;

/////////////////////////////////////////////////////////////////////////////
// CAttack dialog

class CAttack : public CEditorWithBackground
{
// Construction
public:
	CAttack(CWnd* pParent = nullptr);   // standard constructor

	int cur;

protected:
// Dialog Data
	enum { IDD = IDD_ATTACK };

	CClearButton	m_eff_effect;
	CClearButton	m_eff_damage;
	CClearEdit	m_unknown;
	CClearButton	m_target;
	CStrikeCheck	m_elem8;
	CStrikeCheck	m_elem7;
	CStrikeCheck	m_elem6;
	CStrikeCheck	m_elem5;
	CStrikeCheck	m_elem4;
	CStrikeCheck	m_elem3;
	CStrikeCheck	m_elem2;
	CStrikeCheck	m_elem1;
	CClearButton	m_effectbox;
	CStrikeCheck	m_eff8;
	CStrikeCheck	m_eff7;
	CStrikeCheck	m_eff6;
	CStrikeCheck	m_eff5;
	CStrikeCheck	m_eff4;
	CStrikeCheck	m_eff3;
	CStrikeCheck	m_eff2;
	CStrikeCheck	m_eff1;
	CStatic	m_damagetext;
	CClearEdit	m_damage;
	CClearListBox	m_attacklist;
	CClearButton m_savebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CCloseButton m_closebutton;
	CHelpbookButton m_helpbookbutton;

	int ATTACK_OFFSET = -1;
	int ATTACK_BYTES = -1;
	int BANK0A_OFFSET = -1;
	int MAGIC_OFFSET = -1;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	void LoadRom();
	void SaveRom();
	void LoadValues();
	void StoreValues();
	void ChangeEffect(bool);
	virtual void PaintClient(CDC & dc);

	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangeAttacklist();
	afx_msg void OnEffDamage();
	afx_msg void OnEffEffect();
};

#endif // !defined(AFX_ATTACK_H__819F5C22_126A_411C_BB13_B4818F8CD3CE__INCLUDED_)
