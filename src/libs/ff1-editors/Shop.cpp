// Shop.cpp : implementation file
//
#include "stdafx.h"
#include "Shop.h"
#include <AsmFiles.h>
#include <collection_helpers.h>
#include <editor_label_functions.h>
#include <EntriesLoader.h>
#include <FFHacksterProject.h>
#include <GameSerializer.h>
#include <general_functions.h>
#include <imaging_helpers.h>
#include <ini_functions.h>
#include <io_functions.h>
#include <string_functions.h>
#include <ui_helpers.h>
#include "NewLabel.h"

using namespace Editorlabels;
using namespace Ini;
using namespace Io;
using namespace Ui;
using namespace Imaging;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShop dialog

CShop::CShop(CWnd* pParent /*= nullptr */)
	: CEditorWithBackground(CShop::IDD, pParent)
{
}

void CShop::DoDataExchange(CDataExchange* pDX)
{
	CEditorWithBackground::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STARTINGGOLD, m_startinggold);
	DDX_Control(pDX, IDC_PRICE5, m_price5);
	DDX_Control(pDX, IDC_PRICE4, m_price4);
	DDX_Control(pDX, IDC_PRICE3, m_price3);
	DDX_Control(pDX, IDC_PRICE2, m_price2);
	DDX_Control(pDX, IDC_PRICE1, m_price1);
	DDX_Control(pDX, IDC_SHOPTYPE, m_shoptype);
	DDX_Control(pDX, IDC_SHOP, m_shop);
	DDX_Control(pDX, IDC_REMOVE5, m_remove5);
	DDX_Control(pDX, IDC_REMOVE4, m_remove4);
	DDX_Control(pDX, IDC_REMOVE3, m_remove3);
	DDX_Control(pDX, IDC_REMOVE2, m_remove2);
	DDX_Control(pDX, IDC_REMOVE1, m_remove1);
	DDX_Control(pDX, IDC_PRICETEXT, m_pricetext);
	DDX_Control(pDX, IDC_PRICE, m_price);
	DDX_Control(pDX, IDC_POINTER, m_pointer);
	DDX_Control(pDX, IDC_KAB, m_kab);
	DDX_Control(pDX, IDC_ITEM5, m_item5);
	DDX_Control(pDX, IDC_ITEM4, m_item4);
	DDX_Control(pDX, IDC_ITEM3, m_item3);
	DDX_Control(pDX, IDC_ITEM2, m_item2);
	DDX_Control(pDX, IDC_ITEM1, m_item1);
	DDX_Control(pDX, IDC_CHANGEPTR, m_changeptr);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}


BEGIN_MESSAGE_MAP(CShop, CEditorWithBackground)
	ON_LBN_SELCHANGE(IDC_SHOP, OnSelchangeShop)
	ON_LBN_SELCHANGE(IDC_SHOPTYPE, OnSelchangeShoptype)
	ON_BN_CLICKED(IDC_REMOVE1, OnRemove1)
	ON_BN_CLICKED(IDC_REMOVE2, OnRemove2)
	ON_BN_CLICKED(IDC_REMOVE3, OnRemove3)
	ON_BN_CLICKED(IDC_REMOVE4, OnRemove4)
	ON_BN_CLICKED(IDC_REMOVE5, OnRemove5)
	ON_EN_UPDATE(IDC_POINTER, OnUpdatePointer)
	ON_BN_CLICKED(IDC_CHANGEPTR, OnChangeptr)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_DELETESLOT, OnDeleteslot)
	ON_BN_CLICKED(IDC_NEWSLOT, OnNewslot)
	ON_CBN_SELCHANGE(IDC_ITEM1, OnSelchangeItem1)
	ON_CBN_SELCHANGE(IDC_ITEM2, OnSelchangeItem2)
	ON_CBN_SELCHANGE(IDC_ITEM3, OnSelchangeItem3)
	ON_CBN_SELCHANGE(IDC_ITEM4, OnSelchangeItem4)
	ON_CBN_SELCHANGE(IDC_ITEM5, OnSelchangeItem5)
	ON_BN_CLICKED(IDC_EDITLABEL, OnEditlabel)
	ON_WM_PAINT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CShop message handlers

BOOL CShop::OnInitDialog() 
{
	CEditorWithBackground::OnInitDialog();

	//REFACTOR - I'm not sure what's wrong in the dialog template, but a resizing bordeer keeps gettingn added even thought the
	//		dialog editor says it's a thin border. There's a lot of padding on the right and bottom margins.
	//		I don't have time to look into it now, so instead I just live with the padding and remove the thickframe manually.
	//this->ModifyStyle(WS_THICKFRAME | WS_OVERLAPPED, 0);

	try {
		if (Project == nullptr) throw std::runtime_error("No project was specified for this editor");
		if (Enloader == nullptr) throw std::runtime_error("No entry loader was specified for this editor");

		LoadRom();

		CString shoptypes[7] = { "Weapon","Armor","White","Black","Clinic","Inn","Item" };

		int co;
		for (co = 0; co < 7; co++)
			m_shoptype.InsertString(co, shoptypes[co]);

		m_item[0] = &m_item1;
		m_item[1] = &m_item2;
		m_item[2] = &m_item3;
		m_item[3] = &m_item4;
		m_item[4] = &m_item5;
		m_remove[0] = &m_remove1;
		m_remove[1] = &m_remove2;
		m_remove[2] = &m_remove3;
		m_remove[3] = &m_remove4;
		m_remove[4] = &m_remove5;
		m_itprice[0] = &m_price1;
		m_itprice[1] = &m_price2;
		m_itprice[2] = &m_price3;
		m_itprice[3] = &m_price4;
		m_itprice[4] = &m_price5;

		auto treasureitems = Enloader->LoadTreasureItemEntries(*Project);
		for (co = 0; co < 5; co++) {
			LoadCombo(*m_item[co], treasureitems);
		}

		m_banner.Set(this, COLOR_BLACK, COLOR_GREEN, "Shops");

		cur_type = -1;
		m_shoptype.SetCurSel(0);
		OnSelchangeShoptype();

		FindKAB();
	}
	catch (std::exception & ex) {
		return Ui::AbortFail(this, CString("Failed to load game data:\n") + ex.what());
	}
	catch (...) {
		return Ui::AbortFail(this, "An unexpected exception prevented game data from loading.");
	}

	return TRUE;
}

int CShop::GetItemPriceOffset(int index)
{
	int offset = ITEMPRICE_OFFSET + (m_curitem[index] << 1);
	return offset;
}

void CShop::LoadRom()
{
	Project->ClearROM();

	SHOP_START = ReadHex(Project->ValuesPath, "SHOP_START");
	SHOP_OFFSET = ReadHex(Project->ValuesPath, "SHOP_OFFSET");
	SHOP_PTRADD = ReadHex(Project->ValuesPath, "SHOP_PTRADD");
	SHOP_END = ReadHex(Project->ValuesPath, "SHOP_END");
	SHOP_MAXDATALENGTH = ReadHex(Project->ValuesPath, "SHOP_MAXDATALENGTH");
	STARTINGGOLD_OFFSET = ReadHex(Project->ValuesPath, "STARTINGGOLD_OFFSET");
	ITEMPRICE_OFFSET = ReadHex(Project->ValuesPath, "ITEMPRICE_OFFSET");

	BANK00_OFFSET = ReadHex(Project->ValuesPath, "BANK00_OFFSET");
	BANK0A_OFFSET = ReadHex(Project->ValuesPath, "BANK0A_OFFSET");
	BINPRICEDATA_OFFSET = ReadHex(Project->ValuesPath, "BINPRICEDATA_OFFSET");
	BINSHOPDATA_OFFSET = ReadHex(Project->ValuesPath, "BINSHOPDATA_OFFSET");

	// Now load the data
	if (Project->IsRom()) {
		load_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		GameSerializer ser(*Project);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.LoadAsmBin(BANK_00, BANK00_OFFSET);
		ser.LoadAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.LoadAsmBin(BIN_SHOPDATA, BINSHOPDATA_OFFSET);
		ser.LoadAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CShop::SaveRom()
{
	if (Project->IsRom()) {
		save_binary(Project->WorkRomPath, Project->ROM);
	}
	else if (Project->IsAsm()) {
		GameSerializer ser(*Project);
		// Instead of writing to the entire buffer, just write to the parts we need
		ser.SaveAsmBin(BANK_00, BANK00_OFFSET);
		ser.SaveAsmBin(BANK_0A, BANK0A_OFFSET);
		ser.SaveAsmBin(BIN_SHOPDATA, BINSHOPDATA_OFFSET);
		ser.SaveAsmBin(BIN_PRICEDATA, BINPRICEDATA_OFFSET);
	}
	else {
		throw bad_ffhtype_exception(EXCEPTIONPOINT, exceptop::reading, (LPCSTR)Project->ProjectTypeName);
	}
}

void CShop::OnSelchangeShop() 
{
	if(cur != -1) StoreValues();

	cur = m_shop.GetCurSel();

	LoadValues();
}

void CShop::OnSelchangeShoptype() 
{
	if(cur_type != -1) StoreValues();

	cur_type = m_shoptype.GetCurSel();
	while(m_shop.GetCount()) m_shop.DeleteString(0);
	for(int co = 0; co < 10;co++)
		m_shop.InsertString(co, LoadShopLabel(*Project, co + (cur_type * 10)).name);
	cur = 0;
	m_shop.SetCurSel(0);

	LoadValues();
}

void CShop::FindKAB()
{
	int thispointer;
	int largestpointer = SHOP_START;
	int ptrref = 0;
	int co;

	for(co = SHOP_OFFSET; co < SHOP_START; co++){
		thispointer = Project->ROM[co] + (Project->ROM[co + 1] << 8) + SHOP_PTRADD;
		if(thispointer < SHOP_END && thispointer > largestpointer){
			largestpointer = thispointer;
			ptrref = co;
	}}
	if(ptrref == 0)
		kab = SHOP_END - SHOP_START;
	else if(ptrref >= 0x38362 && ptrref < 0x3838A){
		//if it's in these bounds... it is a Inn/Clinic... reserve 2 bytes, no break
		kab = SHOP_END - largestpointer - 2;}
	else{
		//if we got here... a normal shop is at the end.  determine it's size and
		// reserve 1 extra byte (for the break)
		for(co = largestpointer; Project->ROM[co]; co++);
		kab = SHOP_END - co - 1;}

	CString text; text.Format("%d",kab);
	m_kab.SetWindowText(text);
}

void CShop::LoadValues()
{
	ptroffset = (cur_type * 20) + (cur << 1) + SHOP_OFFSET;
	int offset = Project->ROM[ptroffset] + (Project->ROM[ptroffset + 1] << 8) + SHOP_PTRADD;

	CString text;
	text.Format("%X",offset);
	m_pointer.SetWindowText(text);

	bool hasprice = (cur_type == 4) || (cur_type == 5);
	m_price.ShowWindow(hasprice);
	m_pricetext.ShowWindow(hasprice);
	int temp, co;

	if(hasprice){
		// It's a single-price shop (Inn, Clinic), so hide the item controls
		// and load just that price.
		for(co = 0; co < 5; co++){
			m_item[co]->ShowWindow(0);
			m_remove[co]->ShowWindow(0);
			m_itprice[co]->ShowWindow(0);}
		temp = Project->ROM[offset] + (Project->ROM[offset + 1] << 8);
		text.Format("%d",temp);
		m_price.SetWindowText(text);
		numitems = -1;}
	else{
		// It's a multi-item shop.
		// Get the number of non-zero items for this shop, but cap it to 5.
		for(temp = 0; Project->ROM[offset + temp]; temp++);
		if(temp > 5) temp = 5;

		text = "Remove Item";
		for(co = 0; co < temp; co++){
			m_curitem[co] = Project->ROM[offset + co] - 1;
			m_item[co]->SetCurSel(m_curitem[co]);

			m_item[co]->ShowWindow(1);
			m_remove[co]->SetWindowText(text);
			m_remove[co]->ShowWindow(1);
			m_itprice[co]->ShowWindow(1);
			LoadPrice(co);}
		for(co = temp; co < 5; co++){
			m_curitem[co] = -1;
			m_item[co]->ShowWindow(0);
			m_remove[co]->ShowWindow(0);
			m_itprice[co]->ShowWindow(0);}
		if(temp < 5){
			text = "Add Item";
			m_remove[temp]->SetWindowText(text);
			m_remove[temp]->ShowWindow(1);}
		numitems = temp;
	}

	temp = Project->ROM[STARTINGGOLD_OFFSET] + (Project->ROM[STARTINGGOLD_OFFSET + 1] << 8) + (Project->ROM[STARTINGGOLD_OFFSET + 2] << 16);
	text.Format("%d",temp);
	m_startinggold.SetWindowText(text);

	m_changeptr.EnableWindow(FALSE);
}

void CShop::StoreValues()
{
	int offset = Project->ROM[ptroffset] + (Project->ROM[ptroffset + 1] << 8) + SHOP_PTRADD;
	bool hasprice = (cur_type == 4) || (cur_type == 5);
	CString text;
	int temp;

	if (hasprice) {
		m_price.GetWindowText(text); temp = StringToInt(text);
		if (temp > 0xFFFF) temp = 0xFFFF;
		Project->ROM[offset] = temp & 0xFF;
		Project->ROM[offset + 1] = (BYTE)(temp >> 8);
	}
	else {
		for (int co = 0; co < numitems; co++) {
			StorePrice(co);
			auto itemdata = m_item[co]->GetCurSel();

			Project->ROM[offset + co] = (BYTE)(itemdata + 1);
		}
	}

	m_startinggold.GetWindowText(text);
	temp = StringToInt(text); if(temp > 0xFFFFFF) temp = 0xFFFFFF;
	Project->ROM[STARTINGGOLD_OFFSET    ] = temp & 0xFF; temp >>= 8;
	Project->ROM[STARTINGGOLD_OFFSET + 1] = temp & 0xFF; temp >>= 8;
	Project->ROM[STARTINGGOLD_OFFSET + 2] = temp & 0xFF;
}

void CShop::LoadPrice(int co)
{
	int offset = GetItemPriceOffset(co);
	int temp = Project->ROM[offset] + (Project->ROM[offset + 1] << 8);
	CString text; text.Format("%d",temp);
	m_itprice[co]->SetWindowText(text);
}

void CShop::StorePrice(int co)
{
	if(m_curitem[co] == -1) return;
	int offset = GetItemPriceOffset(co);
	CString text;
	m_itprice[co]->GetWindowText(text); int temp = StringToInt(text);
	if(temp > 0xFFFF) temp = 0xFFFF;
	Project->ROM[offset] = temp & 0xFF;
	Project->ROM[offset + 1] = (BYTE)(temp >> 8);
}

void CShop::OnRemove1() 
{Remove(0);}
void CShop::OnRemove2() 
{Remove(1);}
void CShop::OnRemove3() 
{Remove(2);}
void CShop::OnRemove4() 
{Remove(3);}
void CShop::OnRemove5() 
{Remove(4);}

void CShop::Remove(int id)
{
	StoreValues();
	if(id == numitems){
		Add(id);
		return;}

	numitems -= 1;
	int thisptr = Project->ROM[ptroffset] + (Project->ROM[ptroffset + 1] << 8) + SHOP_PTRADD;
	if(thisptr < SHOP_START || thisptr >= SHOP_END){
		Project->ROM[thisptr + id] = 0x00;
		return;}

	int temp;
	int co;
	//shift all greater pointers left
	for (co = SHOP_OFFSET; co < SHOP_START; co += 2) {
		temp = Project->ROM[co] + (Project->ROM[co + 1] << 8) + SHOP_PTRADD;
		if (temp > thisptr && temp < SHOP_END) {
			temp -= SHOP_PTRADD + 1;
			Project->ROM[co] = temp & 0xFF;
			Project->ROM[co + 1] = (BYTE)(temp >> 8);
		}
	}

	//shift all data to the left
	for(co = thisptr + id; co < SHOP_END - 1; co++)
		Project->ROM[co] = Project->ROM[co + 1];

	LoadValues();
	FindKAB();
}

void CShop::Add(int id)
{
	StoreValues();
	int thisptr = Project->ROM[ptroffset] + (Project->ROM[ptroffset + 1] << 8) + SHOP_PTRADD;
	int temp;
	if(thisptr < SHOP_START || thisptr >= SHOP_END){
		if(AfxMessageBox("This data is not within standard boundaries.\n"
			"Increasing the size of this data may result in overwriting something.\nAre you sure you want to continue?",
			MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;
		Project->ROM[thisptr + id] = 1;
		Project->ROM[thisptr + id + 1] = 0;}
	else{
		if(kab < 1){
			AfxMessageBox("Don't have enough KAB.\nCould not increase shop size.", MB_ICONERROR);
			return;}

		//shift larger pointers to the right
		int co;
		for (co = SHOP_OFFSET; co < SHOP_START; co += 2) {
			temp = Project->ROM[co] + (Project->ROM[co + 1] << 8) + SHOP_PTRADD;
			if (temp > thisptr && temp < SHOP_END) {
				temp -= SHOP_PTRADD - 1;
				Project->ROM[co] = temp & 0xFF;
				Project->ROM[co + 1] = (BYTE)(temp >> 8);
			}
		}

		//shift all the data to the right
		for(co = SHOP_END - 2; co >= thisptr + id; co--)
			Project->ROM[co + 1] = Project->ROM[co];

		//insert a non-zero number in the new slot
		Project->ROM[thisptr + id] = 1;
	}

	LoadValues();
	FindKAB();
}

void CShop::OnUpdatePointer() 
{m_changeptr.EnableWindow(1);}

void CShop::OnChangeptr() 
{
	StoreValues();

	CString text;
	m_pointer.GetWindowText(text);

	int temp = StringToInt_HEX(text) - SHOP_PTRADD;

	if (temp < 0 || temp > SHOP_MAXDATALENGTH) {
		text.Format("Pointer is out of range.\nMust be between %X-%X.", SHOP_PTRADD, SHOP_PTRADD + SHOP_MAXDATALENGTH);
		AfxMessageBox(text);
		LoadValues();
		FindKAB();
		m_pointer.SetFocus();
		return;
	}

	Project->ROM[ptroffset] = temp & 0xFF;
	Project->ROM[ptroffset + 1] = (BYTE)(temp >> 8);

	LoadValues();
	FindKAB();
	return;
}

void CShop::OnDeleteslot() 
{
	int offset = Project->ROM[ptroffset] + (Project->ROM[ptroffset + 1] << 8) + SHOP_PTRADD;
	if(offset < SHOP_START || offset > SHOP_END){
		AfxMessageBox("The slot is not in the standard shop boundaries.\nCannot remove slot.", MB_ICONERROR);
		return;}

	int largestptr = SHOP_START;
	int thisptr;
	int co;

	for(co = SHOP_OFFSET; co < SHOP_START; co += 2){
		thisptr = Project->ROM[co] + (Project->ROM[co + 1] << 8) + SHOP_PTRADD;
		if(thisptr > largestptr && thisptr < SHOP_END)
			largestptr = thisptr;}

	if(largestptr == offset){
		AfxMessageBox("This is the last slot.\nCannot delete the last slot.", MB_ICONERROR);
		return;}
		
	if(AfxMessageBox("Really delete this slot?",MB_YESNO | MB_ICONQUESTION) == IDNO) return;

	int shift = numitems;
	if(numitems == -1) shift = 2;
	for(co = SHOP_OFFSET; co < SHOP_START; co += 2){
		thisptr = Project->ROM[co] + (Project->ROM[co + 1] << 8) + SHOP_PTRADD;
		if(thisptr > offset){
			thisptr -= SHOP_PTRADD + shift;
			Project->ROM[co] = thisptr & 0xFF;
			Project->ROM[co + 1] = (BYTE)(thisptr >> 8);
		}
	}

	for(co = offset + shift; co < SHOP_END; co++)
		Project->ROM[co - shift] = Project->ROM[co];

	FindKAB();
	LoadValues();
}

void CShop::OnNewslot() 
{
	int minkab = 1;
	if((cur_type == 4) || (cur_type == 5)) minkab = 2;

	if(kab < minkab){
		AfxMessageBox("Not enough KAB.\nCannot create a new slot.", MB_ICONERROR);
		return;}

	int newptr = SHOP_END - kab;

	for(minkab--; minkab >= 0; minkab--)
		Project->ROM[newptr + minkab] = 0;

	newptr -= SHOP_PTRADD;
	Project->ROM[ptroffset] = newptr & 0xFF;
	Project->ROM[ptroffset + 1] = (BYTE)(newptr >> 8);


	FindKAB();
	LoadValues();
}

void CShop::OnSelchangeItem1() 
{SelChangeItem(0);}
void CShop::OnSelchangeItem2() 
{SelChangeItem(1);}
void CShop::OnSelchangeItem3() 
{SelChangeItem(2);}
void CShop::OnSelchangeItem4() 
{SelChangeItem(3);}
void CShop::OnSelchangeItem5() 
{SelChangeItem(4);}

void CShop::SelChangeItem(int co)
{
	if(m_curitem[co] != -1) StorePrice(co);
	m_curitem[co] = (int)m_item[co]->GetItemData(m_item[co]->GetCurSel());
	LoadPrice(co);
}

void CShop::OnEditlabel() 
{
	ChangeLabel(*Project, -1, LoadShopLabel(*Project, (cur_type * 10) + cur), WriteShopLabel, cur, &m_shop, nullptr);
}

void CShop::OnPaint()
{
	CPaintDC dc(this);
	//REFACTOR - see OnInitDialogg for details. To work around the right-side padding move this right a bit.
	m_banner.Render(dc, 8, 8);
}