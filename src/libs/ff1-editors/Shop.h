#if !defined(AFX_SHOP_H__5A98EF79_B54B_4344_94AD_CC5725AA9099__INCLUDED_)
#define AFX_SHOP_H__5A98EF79_B54B_4344_94AD_CC5725AA9099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Shop.h : header file
//

#include <EditorWithBackground.h>
class CFFHacksterProject;
class CEntriesLoader;

/////////////////////////////////////////////////////////////////////////////
// CShop dialog

class CShop : public CEditorWithBackground
{
// Construction
public:
	CShop(CWnd* pParent = nullptr);   // standard constructor

	CEntriesLoader* Enloader = nullptr;

	int cur_type;
	int cur;
	int shopsize;
	int kab;

	int ptroffset;
	int numitems;

protected:
	virtual int GetItemPriceOffset(int index);

	void LoadRom();
	void SaveRom();
	void LoadValues();
	void StoreValues();

	void FindKAB();
	void Remove(int);
	void Add(int);
	void LoadPrice(int);
	void StorePrice(int);
	void SelChangeItem(int);

	CSubBanner m_banner;
	CClearComboBox* m_item[5];
	CButton* m_remove[5];
	CEdit* m_itprice[5];
	int m_curitem[5];

	dataintnodevector m_shopitems;

	int SHOP_START = -1;
	int SHOP_OFFSET = -1;
	int SHOP_PTRADD = -1;
	int SHOP_END = -1;
	int SHOP_MAXDATALENGTH = -1;
	int STARTINGGOLD_OFFSET = -1;
	int ITEMPRICE_OFFSET = -1;

	int BANK00_OFFSET = -1;
	int BANK0A_OFFSET = -1;
	int BINBANK09GFXDATA_OFFSET = -1;
	int BINPRICEDATA_OFFSET = -1;
	int BINSHOPDATA_OFFSET = -1;

// Dialog Data
	enum { IDD = IDD_SHOP };
	CClearEdit	m_startinggold;
	CClearEdit	m_price5;
	CClearEdit	m_price4;
	CClearEdit	m_price3;
	CClearEdit	m_price2;
	CClearEdit	m_price1;
	CBorderedListBox	m_shoptype;
	CBorderedListBox	m_shop;
	CButton	m_remove5;
	CButton	m_remove4;
	CButton	m_remove3;
	CButton	m_remove2;
	CButton	m_remove1;
	CStatic	m_pricetext;
	CClearEdit	m_price;
	CClearEdit	m_pointer;
	CStatic	m_kab;
	CClearComboBox	m_item5;
	CClearComboBox	m_item4;
	CClearComboBox	m_item3;
	CClearComboBox	m_item2;
	CClearComboBox	m_item1;
	CButton	m_changeptr;
	CCloseButton m_closebutton;
	CHelpbookButton m_helpbookbutton;

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangeShop();
	afx_msg void OnSelchangeShoptype();
	afx_msg void OnRemove1();
	afx_msg void OnRemove2();
	afx_msg void OnRemove3();
	afx_msg void OnRemove4();
	afx_msg void OnRemove5();
	afx_msg void OnUpdatePointer();
	afx_msg void OnChangeptr();
	afx_msg void OnDeleteslot();
	afx_msg void OnNewslot();
	afx_msg void OnSelchangeItem1();
	afx_msg void OnSelchangeItem2();
	afx_msg void OnSelchangeItem3();
	afx_msg void OnSelchangeItem4();
	afx_msg void OnSelchangeItem5();
	afx_msg void OnEditlabel();
	afx_msg void OnPaint();
};

#endif // !defined(AFX_SHOP_H__5A98EF79_B54B_4344_94AD_CC5725AA9099__INCLUDED_)