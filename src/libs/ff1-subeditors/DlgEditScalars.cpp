// DlgEditScalars.cpp : implementation file
//

#include "stdafx.h"
#include "DlgEditScalars.h"
#include "DlgEditIniValue.h"
#include "InplaceArrayEdit.h"
#include <algorithm>
#include <iterator>
#include <FFHacksterProject.h>
#include <general_functions.h>
#include <ini_functions.h>
#include <path_functions.h>
#include <ram_value_functions.h>
#include <set_types.h>
#include <type_support.h>
#include <ui_helpers.h>
#include <vector_types.h>

using namespace Ini;
using namespace Ui;
using namespace Types;

#define ITEM_RESERVED 1  // Identifies a reserved item

// CDlgEditScalars dialog

IMPLEMENT_DYNAMIC(CDlgEditScalars, CFFBaseDlg)

CDlgEditScalars::CDlgEditScalars(CWnd* pParent, UINT helpid)
	: CFFBaseDlg(IDD_SCALARS, pParent, helpid)
	, UNGROUPED("(ungrouped)")
	, HexUppercase(false)
{
}

CDlgEditScalars::~CDlgEditScalars()
{
}

BEGIN_MESSAGE_MAP(CDlgEditScalars, CFFBaseDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SCALARS_BUTTON_SAVE, &CDlgEditScalars::OnClickedScalarsButtonSave)
	ON_LBN_SELCHANGE(IDC_SCALARS_LIST_GROUPS, &CDlgEditScalars::OnSelchangeGroupList)
	ON_NOTIFY(NM_CLICK, IDC_SCALARS_LISTELEMENTS, &CDlgEditScalars::OnClickElementList)
	ON_COMMAND(ID_ACCELERATOR_F2, &CDlgEditScalars::OnAcceleratorF2)
	ON_BN_CLICKED(IDC_SCALARS_BUTTON_ADD, &CDlgEditScalars::OnClickAdd)
	ON_BN_CLICKED(IDC_SCALARS_BUTTON_DEL, &CDlgEditScalars::OnClickDelete)
	ON_BN_CLICKED(IDC_SCALARS_BUTTON_REVERT, &CDlgEditScalars::OnClickRevert)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SCALARS_LISTELEMENTS, &CDlgEditScalars::OnCustomdrawScalarsListelements)
END_MESSAGE_MAP()

void CDlgEditScalars::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCALARS_LIST_GROUPS, m_listgroups);
	DDX_Control(pDX, IDC_SCALARS_LISTELEMENTS, m_elementlist);
	DDX_Control(pDX, IDC_SCALARS_BUTTON_LINKIMPORT, m_importlink);
}

BOOL CDlgEditScalars::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	if (m_proj == nullptr) {
		AfxMessageBox("A project must be provided to the Scalars dialog in order to edit scalar values.", MB_ICONERROR);
		EndDialog(IDABORT);
		return TRUE;
	}

	if (SaveAction == nullptr) {
		CString title;
		GetWindowText(title);
		title += " (Read-only)";
		SetWindowText(title);
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_SCALARS_BUTTON_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCANCEL)->ModifyStyle(0, BS_DEFPUSHBUTTON);
		m_importlink.ShowWindow(SW_HIDE);
	}
	
	//REMOVE - CCM_SETVERSION isn't working, text is still clipped in the list view
	//auto oldccm  = m_elementlist.SendMessage(CCM_SETVERSION, 5, 0);

	//REMOVE - font changes still suffer from the CCM_SETVERSION issue for Custom Draw.
	//// Create an italicized version of the font for use in the list view.
	//{
	//	LOGFONT lf = { 0 };
	//	GetFont()->GetLogFont(&lf);
	//	lf.lfItalic = TRUE;
	//	//lf.lfUnderline = TRUE; //N.B. - italic increases the font height slightly which clips the underline
	//	VERIFY(m_italicfont.CreateFontIndirect(&lf));
	//}

	//REMOVE - don't bother resizing the grid font.
	//N.B. - setting the font via the dialog editor resizes the window as well,
	//		so just resize it here with a relative font size.
	//{
	//	LOGFONT lf = { 0 };
	//	GetFont()->GetLogFont(&lf);
	//	//lf.lfHeight = lf.lfHeight * 105 / 100;
	//	auto points = 10;
	//	lf.lfHeight = Ui::PointsToHeight(points);
	//	if (m_font.CreateFontIndirectA(&lf)) {
	//		m_listgroups.SetFont(&m_font);
	//		m_elementlist.SetFont(&m_font);
	//	}
	//}

	SetWindowPos(nullptr, -1, -1, 960, 480, SWP_NOMOVE | SWP_NOZORDER);
	CenterToParent(this);
	m_inplaceedit.Create(m_inplaceedit.IDD, &m_elementlist);

	InitMainList();
	m_importlink.m_bDefaultClickProcess = TRUE;

	Reload();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditScalars::OnOK()
{
	if (SaveIni())
		CFFBaseDlg::OnOK();
}

//STATIC
mfcstringset CDlgEditScalars::ReadRefList(CString inipath)
{
	mfcstringset resnames;
	auto reskeys = Ini::ReadIniKeyNames(inipath, "RESERVED_LISTS");
	for (const auto & key : reskeys) {
		auto thenames = Ini::ReadIni(inipath, key, "value", mfcstringvector());
		for (const auto & name : thenames) {
			resnames.insert(name).second;
		}
	}
	WarnHere << std::flush;
	return resnames;
}

void CDlgEditScalars::Reload()
{
	// Load all scalar nodes and their groups
	SetRedraw(FALSE);
	m_scalarnodes.clear();

	auto sections = ReadIniSectionNames(m_proj->ValuesPath);
	auto refnames = ReadRefList(m_proj->ValuesPath);

	std::for_each(begin(sections), end(sections), [&](const CString& name) {
		if (!ReadIniRefonly(m_proj->ValuesPath, name)) {
			auto group = ReadIni(m_proj->ValuesPath, name, "group", "");
			auto reserved = refnames.find(name) != cend(refnames) ||
				ReadIniReserved(m_proj->ValuesPath, name);
			//m_scalarnodes.push_back(Node{ name, group, reserved }); //VISTA+ - actually C++17 vs. pre-17 issue
			auto n = Node();
			n.name = name;
			n.group = group;
			n.reserved = reserved;
			m_scalarnodes.push_back(n);
		}
	});

	if (GroupNames.empty())
		ShowAll();
	else if (GroupNames.size() == 1 && !Exclude)
		ShowSingleGroup(GroupNames[0]);
	else
		ShowGroups(GroupNames, Exclude);

	m_elementlist.SetColumnWidth(COL_TYPE, LVSCW_AUTOSIZE_USEHEADER);
	SetRedraw(TRUE);
	Invalidate();
}

bool CDlgEditScalars::SaveIni()
{
	if (SaveAction == nullptr)
		return true; // read-only dialog doesn't save, this isn't a failure

	StoreValues();
	bool saved = SaveAction();
	if (!saved)
		AfxMessageBox("Unable to save the changes.", MB_ICONERROR);
	return saved;
}

void CDlgEditScalars::ShowAll()
{
	// Load all groups simply means exclude none
	std::set<CString> uniquegroups;
	mfcstringvector groupstoload = { "All" };
	for (const auto & node : m_scalarnodes) {
		if (uniquegroups.find(node.group) == end(uniquegroups)) {
			groupstoload.push_back(node.group);
			uniquegroups.insert(node.group);
		}
	}
	LoadGroups(groupstoload);
}

void CDlgEditScalars::ShowSingleGroup(CString groupname)
{
	// No need to load group names from the file
	// Only load the values for this group
	// Hide the list box
	// Set element list left edge to list box left edge
	// Set window title to refer to this group

	m_listgroups.AddString(groupname);
	m_listgroups.ShowWindow(SW_HIDE);
	CRect rcgroup(0, 0, 0, 0);
	CRect rcelem(0, 0, 0, 0);
	m_listgroups.GetWindowRect(&rcgroup);
	m_elementlist.GetWindowRect(&rcelem);
	rcelem.left = rcgroup.left;
	ScreenToClient(&rcelem);
	m_elementlist.MoveWindow(&rcelem);

	CString title;
	GetWindowText(title);
	title += " for group " + groupname;
	SetWindowText(title);

	mfcstringvector groupstoload = { groupname };
	LoadGroups(groupstoload);
}

void CDlgEditScalars::ShowGroups(const mfcstringvector & groupnames, bool exclude)
{
	std::vector<CString> groupstoload;
	std::set<CString> uniquegroups;
	if (exclude) {
		// Load only the groups that are NOT in the list, no dupes
		for (const auto & node : m_scalarnodes) {
			auto iter = std::find(begin(groupnames), end(groupnames), node.group);
			if (iter == end(groupnames) && uniquegroups.find(node.group) == end(uniquegroups)) {
				groupstoload.push_back(node.group);
				uniquegroups.insert(node.group);
			}
		}
		// Include the pseudo group "All"
		groupstoload.push_back("All");
	}
	else {
		// load only these group names into the list, no dupes
		for (const auto & node : m_scalarnodes) {
			auto iter = std::find(begin(groupnames), end(groupnames), node.group);
			if (iter != end(groupnames) && uniquegroups.find(node.group) == end(uniquegroups)) {
				groupstoload.push_back(node.group);
				uniquegroups.insert(node.group);
			}
		}
	}

	LoadGroups(groupstoload);
}

void CDlgEditScalars::LoadGroups(const mfcstringvector & groupnames)
{
	m_listgroups.SetRedraw(FALSE);
	m_listgroups.ResetContent();
	for (auto group : groupnames) {
		//REFACTOR - not sure if VERSION group is used anymore
		if (group.CompareNoCase("VERSION") == 0) continue; // always exclude anything in the Version group
		if (group.IsEmpty()) group = UNGROUPED;
		m_listgroups.AddString(group);
	}

	if (m_listgroups.GetCount() > 0) {
		m_listgroups.SetCurSel(0);
		OnSelchangeGroupList();
		m_listgroups.SetFocus();
	}

	if (m_listgroups.GetCount() < 2) {
		m_elementlist.SetFocus();
		m_elementlist.SetSelectionMark(0);
		m_elementlist.SetItemState(0, LVIS_SELECTED, LVIF_STATE);
	}

	m_listgroups.SetRedraw(TRUE);
	m_listgroups.Invalidate();
}

void CDlgEditScalars::RemoveScalarNode(CString nodename)
{
	auto iter = std::find_if(begin(m_scalarnodes), end(m_scalarnodes), [nodename](const auto & node) { return nodename == node.name; });
	if (iter != end(m_scalarnodes))
		m_scalarnodes.erase(iter);
}

auto CDlgEditScalars::FindScalarNode(CString nodename) -> Node *
{
	// return a pointer to the node
	auto iter = std::find_if(begin(m_scalarnodes), end(m_scalarnodes), [nodename](const auto & node) { return nodename == node.name; });
	return iter != end(m_scalarnodes) ? &(*iter) : nullptr;
}

void CDlgEditScalars::InitMainList()
{
	m_elementlist.SetRedraw(FALSE);
	m_elementlist.SetExtendedStyle(m_elementlist.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_elementlist.DeleteAllItems();
	while (m_elementlist.GetHeaderCtrl()->GetItemCount() > 0)
		m_elementlist.DeleteColumn(0);

	m_elementlist.InsertColumn(COL_INDEX, "#", 0, 16);
	m_elementlist.InsertColumn(COL_NAME, "Name", 0, 16);
	m_elementlist.InsertColumn(COL_TYPE, "Type", 0, 16);
	m_elementlist.InsertColumn(COL_VALUE, "Value", 0, 16);
	m_elementlist.InsertColumn(COL_LABELORDESC, "Labels", 0, 16);
	m_elementlist.InsertColumn(COL_DESC, "Description", 0, 16);

	const int ramwidths[COUNT_COL] = { 32, 128, 40, 192, 192, 288 };
	const int valwidths[COUNT_COL] = { 32, 208, 40, 192, 0, 384 };
		static_assert(_countof(ramwidths) == _countof(valwidths), "ramwidths and valwidths arrays must be the same lrngth");
	const int * widths = EditLabels ? ramwidths : valwidths;
	for (int i = 0; i < COUNT_COL; ++i) m_elementlist.SetColumnWidth(i, widths[i]);

	m_elementlist.SetRedraw(TRUE);
}

void CDlgEditScalars::LoadValues()
{
	m_curindex = m_listgroups.GetCurSel();

	// Get all names for the current group.
	CString groupname;
	m_listgroups.GetText(m_curindex, groupname);
	if (groupname == UNGROUPED) groupname = "";

	CString inipath = m_proj->ValuesPath;
	int linenumber = 0;
	bool showall = groupname == "All";

	m_elementlist.SetRedraw(FALSE);
	m_elementlist.DeleteAllItems();
	for (const auto & node : m_scalarnodes) {
		if (node.group == groupname || showall) {
			CString value = ReadIni(inipath, node.name, "value", "");
			CString type = ReadIni(inipath, node.name, "type", "");
			CString desc = ReadIni(inipath, node.name, "desc", "");
			CString number;
			number.Format("%d", linenumber + 1);

			int index = m_elementlist.InsertItem(linenumber, number);
			int itemdata = node.reserved ? ITEM_RESERVED : 0;
			m_elementlist.SetItemData(index, itemdata);
			m_elementlist.SetItemText(index, COL_NAME, node.name);
			m_elementlist.SetItemText(index, COL_VALUE, value);
			m_elementlist.SetItemText(index, COL_TYPE, type);
			if (EditLabels) {
				CString label = ReadIni(inipath, node.name, "label", "");
				m_elementlist.SetItemText(index, COL_LABELORDESC, label);
				m_elementlist.SetItemText(index, COL_DESC, desc);
			}
			else {
				m_elementlist.SetItemText(index, COL_DESC, desc);
			}

			++linenumber;
		}
	}
	m_elementlist.SetRedraw(TRUE);
}

void CDlgEditScalars::StoreValues()
{
	CString groupname;
	m_listgroups.GetText(m_curindex, groupname);
	if (groupname == UNGROUPED) groupname = "";

	CString inipath = m_proj->ValuesPath;

	m_listgroups.SetRedraw(FALSE);
	for (int item = 0, count = m_elementlist.GetItemCount(); item < count; ++item) {
		CString section = m_elementlist.GetItemText(item, COL_NAME);
		CString type = m_elementlist.GetItemText(item, COL_TYPE);
		CString value = m_elementlist.GetItemText(item, COL_VALUE);
		CString label, desc;
		if (EditLabels) {
			desc = m_elementlist.GetItemText(item, COL_DESC);
			label = m_elementlist.GetItemText(item, COL_LABELORDESC);
			WriteIni(inipath, section, "label", label);
		}
		else {
			desc = m_elementlist.GetItemText(item, COL_DESC);
		}
		WriteIni(inipath, section, "value", value);
		WriteIni(inipath, section, "desc", desc);
	}
	m_listgroups.SetRedraw(TRUE);
}

CString CDlgEditScalars::PromptForImportFile(CWnd * parent)
{
	CFileDialog dlg(TRUE, nullptr, nullptr, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | UiTemp::COMMON_OFN,
		"Supported FF1 Values files (*.ff1rom;*.ff1asm,*.values)|*.ff1rom;*.ff1asm;*.values|"
		"FF1 Projects (*.ff1rom;*.ff1asm)|*.ff1rom;*.ff1asm|"
		"FF1 Values file (*.values)|*.values|"
		"|", parent);

	auto modalresult = dlg.DoModal();
	if (modalresult == IDOK)
		return dlg.GetPathName();
	return "";
}

void CDlgEditScalars::PrepTextEdit(int item, int subitem)
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

void CDlgEditScalars::BeginTextEdit(int item, int subitem, StringTransformFunc xformfunc)
{
	PrepTextEdit(item, subitem);
	CString value = m_elementlist.GetItemText(item, subitem);
	m_inplaceedit.SetText(value, true);
	m_inplaceedit.OKFunc = [&, this, item, subitem, xformfunc](CString newtext) {
		try {
			CString xformtext = xformfunc(newtext);
			m_elementlist.SetItemText(item, subitem, xformtext);
			return true;
		}
		catch (std::exception & ex) {
			ErrorHere << "BeginEditText failed to save the text for item " << item << " subitem " << subitem << ": " << ex.what() << std::endl;
			AfxMessageBox(ex.what(), MB_ICONERROR);
		}
		catch (...) {
			ErrorHere << "BeginEditText failed to save the text for item " << item << " subitem " << subitem << std::endl;
			AfxMessageBox("An unexpected exception prevented the inplace edit.", MB_ICONERROR);
		}
		return false;
	};
}

void CDlgEditScalars::Edit(CString type, int item, int subitem)
{
	bool foundeditor = false;

	if (has(mfcstringset{ "str[]", "byte[]", "bool[]" }, type)) {
		DoArrayEdit(type, item, subitem);
		foundeditor = true;
	}
	else if (type == "rgb") {
		DoColorEdit(item, subitem);
		foundeditor = true;
	}
	else if (type == "str") {
		DoTextEdit(item, subitem);
		foundeditor = true;
	}
	else if (IsIntegralType(type)) {
		auto bytewidth = GetTypeByteWidth(type);
		DoIntEdit(type, item, subitem, bytewidth);
		foundeditor = true;
	}

	if (!foundeditor) AfxMessageBox("No editor was found for the type '" + type + "'.", MB_ICONERROR);
}

void CDlgEditScalars::DoIntEdit(CString type, int item, int subitem, int bytewidth)
{
	ASSERT(IsIntegralType(type));

	bool foundeditor = false;
	auto ints = GetDecConvertibleTypes();
	auto hexs = GetHexConvertibleTypes();
	auto addrs = GetAddrConvertibleTypes();
	if (has(addrs, type))
	{
		foundeditor = true;
		BeginTextEdit(item, subitem, [=](CString text) {
			return ValidateAddrIntAndUpdate(type, item, subitem, bytewidth, text);
		});
	}
	else if (has(ints, type))
	{
		foundeditor = true;
		BeginTextEdit(item, subitem, [=](CString text) {
			return ValidateDecIntAndUpdate(type, item, subitem, bytewidth, text);
		});
	}
	else if (has(hexs, type))
	{
		foundeditor = true;
		BeginTextEdit(item, subitem, [=](CString text) {
			return ValidateHexIntAndUpdate(type, item, subitem, bytewidth, text);
		});
	}

	if (!foundeditor)
		AfxMessageBox("No int editor was found for the type '" + type + "'.", MB_ICONERROR);
}

void CDlgEditScalars::DoArrayEdit(CString type, int item, int subitem)
{
	CInplaceArrayEdit dlg(this);

	if (type == "str[]") {
		dlg.ArrayText = m_elementlist.GetItemText(item, subitem);
	}
	else if (type == "byte[]") {
		dlg.ArrayText = m_elementlist.GetItemText(item, subitem);
	}
	else if (type == "bool[]") {
		dlg.ArrayText = m_elementlist.GetItemText(item, subitem);
	}

	if (!dlg.ArrayText.IsEmpty()) {
		dlg.Type = type;
		dlg.Caption = "Edit " + m_elementlist.GetItemText(item, 1);
		dlg.HexUppercase = HexUppercase;
		if (dlg.DoModal() == IDOK) {
			m_elementlist.SetItemText(item, subitem, dlg.ArrayText);
		}
	}
	else {
		AfxMessageBox("No array editor was found for the type '" + type + "'.", MB_ICONERROR);
	}
}

void CDlgEditScalars::DoColorEdit(int item, int subitem)
{
	auto text = m_elementlist.GetItemText(item, subitem);
	DWORD color = strtoul(text, nullptr, 16);
	CColorDialog dlg(color, 0, this);
	if (dlg.DoModal() == IDOK) {
		DWORD newcolor = dlg.GetColor();
		auto bytewidth = GetTypeByteWidth("rgb");
		m_elementlist.SetItemText(item, subitem, HexCase(newcolor, bytewidth));
	}
}

void CDlgEditScalars::DoTextEdit(int item, int subitem)
{
	BeginTextEdit(item, subitem, [](CString text) { return text; });
}

//DEVNOTE - to get total control over this display, type traits would have to be introduced.
//	these would define range and output format and could be retireved from a map (or INI) by type name.
//	for now, don't worry about it.
CString CDlgEditScalars::ValidateAddrIntAndUpdate(CString type, int item, int subitem, int bytewidth, CString text)
{
	UNREFERENCED_PARAMETER(item);
	UNREFERENCED_PARAMETER(subitem);

	// $12AB
	auto retval = ValidateAddrInt(type, bytewidth, text, [this,bytewidth](long long value) { return AddrCase((int)value, bytewidth);});
	if (!retval.result) throw std::invalid_argument((LPCSTR)retval.value);
	return retval.value;
}

//DEVNOTE - to get total control over this display, type traits would have to be introduced.
//	these would define range and output format and could be retireved from a map (or INI) by type name.
//	for now, don't worry about it.
CString CDlgEditScalars::ValidateDecIntAndUpdate(CString type, int item, int subitem, int bytewidth, CString text)
{
	UNREFERENCED_PARAMETER(item);
	UNREFERENCED_PARAMETER(subitem);

	auto retval = ValidateDecInt(type, bytewidth, text, [](long long value) { return dec((int)value); });
	if (!retval.result) throw std::invalid_argument((LPCSTR)retval.value);
	return retval.value;
}

//DEVNOTE - to get total control over this display, type traits would have to be introduced.
//	these would define range and output format and could be retireved from a map (or INI) by type name.
//	for now, don't worry about it.
CString CDlgEditScalars::ValidateHexIntAndUpdate(CString type, int item, int subitem, int bytewidth, CString text)
{
	UNREFERENCED_PARAMETER(item);
	UNREFERENCED_PARAMETER(subitem);

	// 0x1234ABCD
	auto retval = ValidateHexInt(type, bytewidth, text, [this, bytewidth](long long value) { return HexCase((int)value, bytewidth); });
	if (!retval.result) throw std::invalid_argument((LPCSTR)retval.value);
	return retval.value;
}

CString CDlgEditScalars::HexCase(int value, int bytewidth)
{
	return hex_upper(hex(value, bytewidth * 2), HexUppercase);
}

CString CDlgEditScalars::AddrCase(int value, int bytewidth)
{
	return addr_upper(addr(value, bytewidth * 2), HexUppercase);
}


// CDlgEditScalars message handlers

void CDlgEditScalars::OnDestroy()
{
	CFFBaseDlg::OnDestroy();
}

void CDlgEditScalars::OnClickedScalarsButtonSave()
{
	SaveIni();
}

void CDlgEditScalars::OnSelchangeGroupList()
{
	CWaitCursor wait;
	if (m_curindex != -1) StoreValues();
	LoadValues();
}

void CDlgEditScalars::OnClickElementList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pnm = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (0 <= pnm->iItem && pnm->iItem < m_elementlist.GetItemCount()) {
		switch (pnm->iSubItem) {
		case COL_VALUE:
			{
				auto type = m_elementlist.GetItemText(pnm->iItem, COL_TYPE);
				Edit(type, pnm->iItem, pnm->iSubItem);
			}
			break;
		case COL_LABELORDESC:
			{
				BeginTextEdit(pnm->iItem, pnm->iSubItem, [](CString text) { return text;});
			}
			break;
		case COL_DESC:
			{
				BeginTextEdit(pnm->iItem, pnm->iSubItem, [](CString text) { return text;});
			}
			break;
		}
	}
	*pResult = 0;
}

void CDlgEditScalars::OnAcceleratorF2()
{
	//FUTURE - implement a current column index and allow F2 to inplace edit the current record for that column index.
}

void CDlgEditScalars::OnClickAdd()
{
	CString groupname;
	m_listgroups.GetText(m_curindex, groupname);

	CDlgEditIniValue dlg;
	dlg.Label = "Fill in details for the new value";
	dlg.GroupName = groupname;
	dlg.HexUppercase = HexUppercase;
	dlg.Disable = CDlgEditIniValue::Disable::Group;
	if (dlg.DoModal() == IDOK) {
		auto prevnode = FindScalarNode(dlg.KeyName);
		if (prevnode == nullptr) {
			WriteGroupedValue(m_proj->ValuesPath, dlg.KeyName, dlg.Value, dlg.GroupName);
			WriteIni(m_proj->ValuesPath, dlg.KeyName, "type", dlg.Type);
			//m_scalarnodes.push_back(Node{ dlg.KeyName, dlg.GroupName }); //VISTA+ - REALLY c++17 vs. pre-17 issue
			auto n = Node();
			n.name = dlg.KeyName;
			n.group = dlg.GroupName;
			n.reserved = false;
			m_scalarnodes.push_back(n);
			OnSelchangeGroupList();

			// For now, we always append
			m_elementlist.SetFocus();
			m_elementlist.SetSelectionMark(m_elementlist.GetItemCount() - 1);
			m_elementlist.SetItemState(m_elementlist.GetSelectionMark(), LVIS_SELECTED, LVIS_SELECTED);
		}
		else {
			CString dupegroupname = !prevnode->group.IsEmpty() ? prevnode->group : "(ungrouped)";
			CString cs;
			cs.Format("The node %s can't be added, it aleady exists in group '%s'", (LPCSTR)dlg.KeyName, (LPCSTR)dupegroupname);
			AfxMessageBox(cs, MB_ICONSTOP);
		}
	}
}

void CDlgEditScalars::OnClickDelete()
{
	CString groupname;
	m_listgroups.GetText(m_curindex, groupname);
	if (groupname.IsEmpty()) return;

	int mark = m_elementlist.GetSelectionMark();
	if (mark == -1) {
		AfxMessageBox("Please select a value from the list to the right.", MB_ICONERROR);
		return;
	}

	CString valuename = m_elementlist.GetItemText(mark, 1);

	auto itemdata = m_elementlist.GetItemData(mark);
	if (itemdata & 1) {
		CString msg;
		msg.Format("Can't delete reserved value '%s'.", (LPCSTR)valuename);
		AfxMessageBox(msg);
		return;
	}

	CString message;
	message.Format("Remove the value %s from group %s?", (LPCSTR)valuename, (LPCSTR)groupname);
	auto result = AfxMessageBox(message, MB_OKCANCEL | MB_ICONQUESTION);
	if (result == IDOK) {
		// Values are stored as sections, so remove the section.
		m_elementlist.DeleteItem(mark);
		WritePrivateProfileString(valuename, nullptr, nullptr, m_proj->ValuesPath);
		RemoveScalarNode(valuename);
		OnSelchangeGroupList();
	}
}

void CDlgEditScalars::OnClickRevert()
{
	auto modalresult = AfxMessageBox("Do you really want to revert all values to their initial states?\n"
		"You will lose all customizations you've made to them.", MB_OKCANCEL | MB_ICONQUESTION);
	if (modalresult == IDOK) {
		try {
			auto result = m_proj->RevertValues();
			if (!result) AfxMessageBox("The attempt to revert the project values failed:\n" + result.value, MB_ICONERROR);
			else {
				m_curindex = -1;
				m_listgroups.SetCurSel(m_curindex);
				Reload();
			}
		}
		catch (std::exception& ex) {
			AfxMessageBox(ex.what());
			EndDialog(IDABORT);
		}
	}
}

void CDlgEditScalars::OnCustomdrawScalarsListelements(NMHDR* pNMHDR, LRESULT* pResult)
{
	auto pnm = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	auto& nmcd = pnm->nmcd;
	*pResult = CDRF_DODEFAULT;

	switch (nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
	{
		//*pResult = (nmcd.lItemlParam & ITEM_RESERVED) ? CDRF_NOTIFYSUBITEMDRAW : CDRF_DODEFAULT;
		if (nmcd.lItemlParam & ITEM_RESERVED) {
			pnm->clrText = GetSysColor(COLOR_HOTLIGHT);
			pnm->clrTextBk = GetSysColor(COLOR_HIGHLIGHTTEXT);
			*pResult = CDRF_NEWFONT;
		}
		else {
			*pResult = CDRF_DODEFAULT;
		}
		break;
	}
	//case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
	//{
	//	// For reserved values, draw the name in italics with the hotlight color.
	//	auto isres1 = (nmcd.lItemlParam & ITEM_RESERVED) && (pnm->iSubItem == 1);
	//	auto color = isres1 ? GetSysColor(COLOR_HOTLIGHT) : GetSysColor(COLOR_WINDOWTEXT);
	//	auto back = isres1 ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOW);
	//	pnm->clrText = color;
	//	pnm->clrTextBk = back;
	//	*pResult = CDRF_NEWFONT;
	//	break;
	//}
	}

	//switch (nmcd.dwDrawStage)
	//{
	//case CDDS_PREPAINT:
	//	*pResult = CDRF_NOTIFYITEMDRAW;
	//	break;
	//case CDDS_ITEMPREPAINT:
	//	*pResult = (nmcd.lItemlParam & ITEM_RESERVED) ? CDRF_NOTIFYSUBITEMDRAW : CDRF_DODEFAULT;
	//	break;
	//case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
	//{
	//	// For reserved values, draw the name in italics with the hotlight color.
	//	auto isres1 = (nmcd.lItemlParam & ITEM_RESERVED) && (pnm->iSubItem == 1);
	//	auto font = isres1 ? &m_italicfont : GetFont();
	//	auto color = isres1 ? GetSysColor(COLOR_HOTLIGHT) : GetSysColor(COLOR_WINDOWTEXT);
	//	pnm->clrText = color;
	//	SelectObject(nmcd.hdc, font->GetSafeHandle());
	//	*pResult = CDRF_NEWFONT;
	//	break;
	//}
	//}

	//if (nmcd.dwDrawStage == CDDS_PREPAINT) {
	//	*pResult = CDRF_NOTIFYITEMDRAW;
	//}
	//else if (nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
	//	*pResult = CDRF_NOTIFYSUBITEMDRAW;
	//}
	//else if (nmcd.dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT)) {
	//	// For reserved values, draw the name in italics with the hotlight color.
	//	auto isres1 = (nmcd.lItemlParam & ITEM_RESERVED) && (pnm->iSubItem == 1);
	//	auto font = isres1 ? &m_italicfont : GetFont();
	//	font = GetFont();
	//	auto color = isres1 ? GetSysColor(COLOR_HOTLIGHT) : GetSysColor(COLOR_WINDOWTEXT);
	//	pnm->clrText = color;
	//	SelectObject(nmcd.hdc, font->GetSafeHandle());
	//	*pResult = CDRF_NEWFONT;
	//}
}