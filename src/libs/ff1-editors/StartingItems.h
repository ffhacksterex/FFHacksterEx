#pragma once

#include <EditorWithBackground.h>
#include <vector>

// CStartingItems dialog

class CStartingItems : public CEditorWithBackground
{
	DECLARE_DYNAMIC(CStartingItems)

public:
	CStartingItems(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CStartingItems();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STARTINGITEMS };
#endif

protected:
	virtual void LoadOffsets();
	virtual void LoadRom();
	virtual void SaveRom();
	virtual void LoadValues();
	virtual void StoreValues();

	int HASCANOE_OFFSET;
	int SHIPVIS_OFFSET = -1;
	int AIRSHIPVIS_OFFSET = -1;
	int BRIDGEVIS_OFFSET = -1;
	int CANALVIS_OFFSET = -1;
	int BRIDGESCENE_OFFSET;
	int STARTINGITEMS_OFFSET;
	int STARTINGCONSUMABLES_OFFSET;
	int BANK00_OFFSET;
	int BANK0A_OFFSET;

	std::vector<int> m_itemids;
	std::vector<int> m_comsumeids;
	std::vector<int> m_consumelabelids;

	CComboBox m_comboBridge;
	CButton m_checkHasCanoe;
	CButton m_checkHasShip;
	CButton m_checkHasAirship;
	CButton m_checkHasBridge;
	CButton m_checkHasCanal;
	CClearEdit m_comumableedit1;
	CClearEdit m_comumableedit2;
	CClearEdit m_comumableedit3;
	CClearEdit m_comumableedit4;
	CClearEdit m_comumableedit5;
	CClearEdit m_comumableedit6;
	CCloseButton m_closebutton;
	CClearButton m_savebutton;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;
	CHelpbookButton m_helpbookbutton;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};
