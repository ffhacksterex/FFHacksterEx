// DlgEditStrings.cpp : implementation file
//

#include "stdafx.h"
#include "DlgEditStrings.h"
#include "afxdialogex.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "general_functions.h"
#include "ui_helpers.h"

using namespace Ini;
using namespace Ui;

// CDlgEditStrings dialog

IMPLEMENT_DYNAMIC(CDlgEditStrings, CFFBaseDlg)

CDlgEditStrings::CDlgEditStrings(CWnd* pParent, UINT helpid)
	: CFFBaseDlg(IDD_STRINGS, pParent, helpid)
{
}

CDlgEditStrings::~CDlgEditStrings()
{
}


BEGIN_MESSAGE_MAP(CDlgEditStrings, CFFBaseDlg)
	ON_BN_CLICKED(IDC_SAVE, &CDlgEditStrings::OnBnClickedSave)
	ON_LBN_SELCHANGE(IDC_LISTGROUPS, &CDlgEditStrings::OnLbnSelchangeListgroups)
	ON_NOTIFY(NM_CLICK, IDC_LISTELEMENTS, &CDlgEditStrings::OnNMClickListelements)
END_MESSAGE_MAP()


void CDlgEditStrings::InitMainList()
{
	m_elementlist.SetExtendedStyle(m_elementlist.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_elementlist.DeleteAllItems();
	while (m_elementlist.GetHeaderCtrl()->GetItemCount() > 0)
		m_elementlist.DeleteColumn(0);

	int col = 0;
	m_elementlist.InsertColumn(col++, "#", 0, 32);
	m_elementlist.InsertColumn(col++, "Description", 0, 576);
}

void CDlgEditStrings::LoadValues()
{
	CWaitCursor wait;

	CString inifile = m_proj->StringsPath;
	CString section;
	CString strslot;
	CString key;
	CString text;

	m_elementlist.DeleteAllItems();
	m_listgroups.GetText(m_curindex, section);

	RedrawScope redraw(&m_elementlist);

	auto countname = ReadIni(m_proj->ProjectPath, "STRINGCOUNTS", section, "");
	if (countname.IsEmpty()) THROWEXCEPTION(std::runtime_error, std::string("no count variable was specified for group ") + (LPCSTR)section);

	// Read this value from the ValuesPath
	auto count = ReadDec(m_proj->ValuesPath, countname, 0);
	if (count < 1) {
		CString err;
		err.Format("group '%s' count variable '%s' must be a positive value (not %d)", (LPCSTR)section, (LPCSTR)countname, count);
		THROWEXCEPTION(std::runtime_error, (LPCSTR)err);
	}

	CString strcount;
	strcount.Format("Count = %d (%s)", count, (LPCSTR)countname);
	m_countstatric.SetWindowText(strcount);

	for (int index = 0; index < count; ++index) {
		CString thiskey;
		CString theslot;
		thiskey.Format("%lu", index);
		theslot.Format("%lu", index + 1);
		auto thetext = ReadIni(inifile, section, thiskey, "");
		int rowindex = m_elementlist.InsertItem(index, theslot);
		m_elementlist.SetItemText(rowindex, 1, thetext);
		m_elementlist.SetItemData(rowindex, index);
	}
}

void CDlgEditStrings::StoreValues()
{
	CString inifile = m_proj->StringsPath;
	CString section;
	m_listgroups.GetText(m_curindex, section);

	RedrawScope redraw(&m_elementlist);

	for (int item = 0, count = m_elementlist.GetItemCount(); item < count; ++item) {
		auto rowindex = m_elementlist.GetItemData(item);
		CString desc = m_elementlist.GetItemText(item, 1);
		CString key;
		key.Format("%lu", rowindex);
		WriteIni(inifile, section, key, desc);
	}
}

bool CDlgEditStrings::SaveIni()
{
	if (SaveAction == nullptr)
		return true; // read-only dialog doesn't save, this isn't a failure

	StoreValues();
	bool saved = SaveAction();
	if (!saved)
		AfxMessageBox("Unable to save the changes.", MB_ICONERROR);
	return saved;
}

void CDlgEditStrings::PrepTextEdit(int item, int subitem)
{
	CRect client(0, 0, 0, 0);
	m_elementlist.GetClientRect(&client);
	CRect rcitem(0, 0, 0, 0);
	VERIFY(m_elementlist.GetSubItemRect(item, subitem, LVIR_BOUNDS, rcitem));
	if (rcitem.right > client.right) rcitem.right = client.right;

	m_elementlist.ClientToScreen(&rcitem); // it's actually a dialog that uses screen coords

	m_inplaceedit.OKFunc = nullptr;
	m_inplaceedit.ClearData();
	m_inplaceedit.SetPosition(rcitem);
	m_inplaceedit.ShowWindow(SW_SHOW);
}

void CDlgEditStrings::BeginTextEdit(int item, int subitem, StringTransformFunc xformfunc)
{
	PrepTextEdit(item, subitem);
	CString value = m_elementlist.GetItemText(item, subitem);
	m_inplaceedit.SetText(value, true);
	m_inplaceedit.OKFunc = [&, item, subitem, xformfunc](CString newtext) {
		CString xformtext = xformfunc(newtext);
		m_elementlist.SetItemText(item, subitem, xformtext);
		return true;
	};
}

void CDlgEditStrings::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTGROUPS, m_listgroups);
	DDX_Control(pDX, IDC_LISTELEMENTS, m_elementlist);
	DDX_Control(pDX, IDC_STATIC1, m_countstatric);
}

BOOL CDlgEditStrings::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	if (SaveAction == nullptr) {
		CString title;
		GetWindowText(title);
		title += " (Read-only)";
		SetWindowText(title);
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCALARS_BUTTON_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCANCEL)->ModifyStyle(0, BS_DEFPUSHBUTTON);
	}

	SetWindowPos(nullptr, -1, -1, 960, 480, SWP_NOMOVE | SWP_NOZORDER);
	CenterToParent(this);
	m_inplaceedit.Create(m_inplaceedit.IDD, &m_elementlist);

	InitMainList();

	auto sections = ReadIniSectionNames(m_proj->StringsPath);
	for (auto name : sections)
	{
		m_listgroups.AddString(name);
	}

	if (m_listgroups.GetCount() > 0) {
		m_listgroups.SetCurSel(0);
		OnLbnSelchangeListgroups();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditStrings::OnOK()
{
	if (SaveIni())
		CFFBaseDlg::OnOK();
}


// CDlgEditStrings message handlers


void CDlgEditStrings::OnBnClickedSave()
{
	SaveIni();
}

void CDlgEditStrings::OnLbnSelchangeListgroups()
{
	try {
		if (m_curindex != -1) StoreValues();
		m_curindex = m_listgroups.GetCurSel();
		LoadValues();
	}
	catch (std::exception & ex) {
		CString err;
		err.Format("Caught an exception while loading group %d:\n%s.", m_curindex, ex.what());
		AfxMessageBox(err);
		EndDialog(IDABORT);
	}
	catch (...) {
		CString err;
		err.Format("Caught an unknown exception while loading group %d.", m_curindex);
		AfxMessageBox(err);
		EndDialog(IDABORT);
	}
}

void CDlgEditStrings::OnNMClickListelements(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pnm = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (0 <= pnm->iItem && pnm->iItem < m_elementlist.GetItemCount()) {
		CString value;
		switch (pnm->iSubItem) {
		case 1:
			value = m_elementlist.GetItemText(pnm->iItem, pnm->iSubItem);
			BeginTextEdit(pnm->iItem, pnm->iSubItem, [](CString text) { return text;});
			break;
		}
	}
	*pResult = 0;
}