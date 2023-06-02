// DlgEditStrings.cpp : implementation file
//

#include "stdafx.h"
#include "DlgEditStrings.h"
#include <FFH2Project.h>
#include "general_functions.h"
#include "ini_functions.h"
#include "string_functions.h"
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
	m_elementlist.DeleteAllItems();

	auto section = m_projstrings.order[m_curindex];
	auto& entries = m_projstrings.entries[section];
	auto count = entries.size();
	if (count < 1)
		throw std::runtime_error("Project string group '" + section + "' has no entries.");

	CString strcount;
	strcount.Format("Count = %d", count);
	m_countstatric.SetWindowText(strcount);

	for (int index = 0; index < count; ++index) {
		CString theslot;
		theslot.Format("%lu", index + 1);
		int rowindex = m_elementlist.InsertItem(index, theslot);
		m_elementlist.SetItemText(rowindex, 1, entries[index].c_str());
		m_elementlist.SetItemData(rowindex, index);
	}
}

void CDlgEditStrings::StoreValues()
{
	auto section = m_projstrings.order[m_curindex];
	auto& entries = m_projstrings.entries[section];

	ASSERT((size_t)m_elementlist.GetItemCount() == entries.size());
	if ((size_t)m_elementlist.GetItemCount() != entries.size())
		throw std::runtime_error("Cannot save because entry count (" + std::to_string(entries.size()) +
			") and list count (" + std::to_string(entries.size()) + ") don't match.");

	for (auto index = 0; index < entries.size(); ++index) {
		auto rowindex = m_elementlist.GetItemData(index);
		CString desc = m_elementlist.GetItemText(index, 1);
		entries[index] = ffh::str::tostd(desc);
	}
}

bool CDlgEditStrings::Save()
{
	if (ReadOnly) return false;

	StoreValues();
	Proj2->strings = m_projstrings;
	return true;
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

	if (ReadOnly) {
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

	m_projstrings = Proj2->strings;

	InitMainList();

	for (const auto & name : m_projstrings.order)
	{
		CString csname = ffh::str::tomfc(name);
		m_listgroups.AddString(csname);
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
	if (Save())
		CFFBaseDlg::OnOK();
}


// CDlgEditStrings message handlers


void CDlgEditStrings::OnBnClickedSave()
{
	Save();
}

void CDlgEditStrings::OnLbnSelchangeListgroups()
{
	RedrawScope redraw(&m_elementlist);
	CWaitCursor wait;
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