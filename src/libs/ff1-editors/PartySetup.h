#pragma once

#include <EditorWithBackground.h>
class CFFHacksterProject;

// CPartySetup dialog

class CPartySetup : public CEditorWithBackground
{
	DECLARE_DYNAMIC(CPartySetup)

public:
	CPartySetup(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPartySetup();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARTYSETUP };
#endif

protected:
	int CLASS_COUNT = -1;
	int BANK0A_OFFSET = -1;
	unsigned int NEWPARTYCLASSCOUNT_OFFSET = (unsigned int)-1;
	unsigned int NEWPARTYCLASSINC_OFFSET = (unsigned int)-1;
	unsigned int PTYGEN_OFFSET = (unsigned int)-1;

	CComboBox m_classcombo1;
	CComboBox m_classcombo2;
	CComboBox m_classcombo3;
	CComboBox m_classcombo4;
	CClearEdit m_newgamecountedit;
	CButton m_fixedpartycheck;
	CCloseButton m_closebutton;
	CClearButton m_savebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CHelpbookButton m_helpbookbutton;

	void LoadRom();
	virtual void SaveRom();
	virtual void StoreValues();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
