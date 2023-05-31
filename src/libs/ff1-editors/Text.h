#pragma once
// Text.h : header file

#include <EditorWithBackground.h>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CText dialog

class CText : public CEditorWithBackground
{
	// Construction
public:
	CText(CWnd* pParent = nullptr);   // standard constructor

	int cur_main; //N.B. this is a context value (WEAPON, ARMOR, etc.), not a listbox or array index
	int cur_text; // index of currently selected text sublist box item
	bool viewDTE;
	int ptroffset;
	int ptradd;
	int count;
	int kab;
	int textstart;
	int textend;
	int ptrtblstart;
	int ptrtblend;
	bool madechange;

protected:
	// Overrides
	virtual void LoadOffsets();
	virtual void LoadRom();
	virtual void SaveRom();
	virtual void LoadValues();
	virtual void StoreValues();
	virtual CString GetInfoText();

	// Text behavior overridables
	virtual bool ShowCurrentTextItemIndex() const;

	std::vector<int> m_editcontexts;
	std::vector<CString> m_editcontextnames;

	int BASICTEXT_PTRADD = -1;
	int BASICTEXT_OFFSET = -1;
	int BASICTEXT_COUNT = -1;
	int ITEM_COUNT = -1;
	int WEAPON_COUNT = -1;
	int ARMOR_COUNT = -1;
	int GOLDITEM_COUNT = -1;
	int MAGIC_COUNT = -1;
	int CLASS_COUNT = -1;
	int ATTACKTEXT_OFFSET = -1;
	int ATTACKTEXT_PTRADD = -1;
	int ATTACK_COUNT = -1;
	int ENEMYTEXT_OFFSET = -1;
	int ENEMYTEXT_PTRADD = -1;
	int ENEMY_COUNT = -1;
	int DIALOGUE_OFFSET = -1;
	int DIALOGUE_PTRADD = -1;
	int DIALOGUE_COUNT = -1;
	int BATTLEMESSAGETEXT_OFFSET = -1;
	int BATTLEMESSAGETEXT_PTRADD = -1;
	int BATTLEMESSAGE_COUNT = -1;
	int BASICTEXT_START = -1;
	int BASICTEXT_END = -1;
	int ATTACKTEXT_START = -1;
	int ATTACKTEXT_END = -1;
	int ENEMYTEXT_START = -1;
	int ENEMYTEXT_END = -1;
	int DIALOGUE_START = -1;
	int DIALOGUE_END = -1;
	int BATTLEMESSAGETEXT_START = -1;
	int BATTLEMESSAGETEXT_END = -1;
	int BATTLEMESSAGE_OFFSET = -1;
	int NOTHINGHAPPENS_OFFSET = -1;
	int BANK0A_OFFSET = -1;

	int INTROTEXT_OFFSET = -1;
	int INTROTEXT_START = -1;
	int INTROTEXT_END = -1;

	int SHOPTEXTPTR_OFFSET = -1;
	int SHOPTEXT_OFFSET = -1;
	int SHOPTEXT_PTRADD = -1;
	int SHOPTEXT_START = -1;
	int SHOPTEXT_END = -1;
	int SHOPTEXT_COUNT = -1;

	int STATUSMENUTEXTPTR_OFFSET = -1;
	int STATUSMENUTEXT_OFFSET = -1;
	int STATUSMENUTEXT_PTRADD = -1;
	int STATUSMENUTEXT_START = -1;
	int STATUSMENUTEXT_END = -1;
	int STATUSMENUTEXT_COUNT = -1;

	int STORYTEXTPTR_OFFSET = -1;
	int STORYTEXT_OFFSET = -1;
	int STORYTEXT_PTRADD = -1;
	int STORYTEXT_START = -1;
	int STORYTEXT_END = -1;
	int STORYTEXT_COUNT = -1;

	unsigned int PROLOGUELASTPAGE_OFFSET = (unsigned int)-1;
	unsigned int EPILOGUELASTPAGE_OFFSET = (unsigned int)-1;

	int CREDITTEXTPTR_OFFSET = -1;
	int CREDITTEXT_OFFSET = -1;
	int CREDITTEXT_PTRADD = -1;
	int CREDITTEXT_START = -1;
	int CREDITTEXT_END = -1;
	int CREDITTEXT_COUNT = -1;

	void FindKAB();
	void ResetTextList();
	void UpdateTextList();
	int ConvertText(BYTE*, CString);
	void UpdateSpinChange(int newpos);
	bool CanShowDialogueEdit(int context);
	int GetOffset(int theptroffset, int theptradd, int index);

	// Dialog Data
	enum { IDD = IDD_TEXT };
	CStatic m_listindexstatic;
	CClearButton m_findbutton;
	CClearEdit m_findedit;
	CClearButton	m_changeptrbutton;
	CClearButton	m_newslotbutton;
	CClearButton	m_editlabelbutton;
	CStatic	m_kab;
	CClearEdit	m_pointer;
	CButton	m_dte;
	CBorderedEdit	m_textmultilinebox;
	CClearEdit	m_textbox;
	CBorderedListBox	m_textlist;
	CBorderedListBox	m_mainlist;
	CSpinButtonCtrl m_spinPrologue;
	CStatic m_staticPrologue;
	CStatic m_staticEpilogue;
	CClearButton m_deleteslotbutton;
	CStatic m_staticScrWidth;
	CStatic m_staticScrLine;
	CStatic m_staticPpuAddr;
	CClearEdit m_editPpuAddr;
	CSubBanner m_banner;
	CCloseButton m_closebutton;
	CInfoButton m_infobutton;
	CHelpbookButton m_helpbookbutton;
	CClearButton m_update;
	CClearButton m_save;
	CClearButton m_ok;
	CClearButton m_cancel;

	// ClassWizard generated virtual function overrides
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangeMainlist();
	afx_msg void OnSelchangeTextlist();
	afx_msg void OnEditlabel();
	afx_msg void OnDte();
	afx_msg void OnUpdate();
	afx_msg void OnUpdateTextbox();
	afx_msg void OnUpdatePointer();
	afx_msg void OnChangeptr();
	afx_msg void OnDeleteslot();
	afx_msg void OnNewslot();
	afx_msg void OnSpinPrologueDeltaPos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedInfo2();
	afx_msg void OnBnClickedTextBtnFind();
};
