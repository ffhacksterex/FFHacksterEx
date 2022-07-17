// InplaceArrayEdit.cpp : implementation file
//

#include "stdafx.h"
#include "InplaceArrayEdit.h"
#include "afxdialogex.h"
#include <general_functions.h>
#include <type_support.h>
#include <ui_helpers.h>

using namespace Types;
using namespace Ui;

// CInplaceArrayEdit dialog

IMPLEMENT_DYNAMIC(CInplaceArrayEdit, CDialogEx)

CInplaceArrayEdit::CInplaceArrayEdit(CWnd* pParent /*= nullptr */)
	: CDialogEx(IDD_INPLACEARRAYEDIT, pParent)
	, HexUppercase(false)
{
}

CInplaceArrayEdit::~CInplaceArrayEdit()
{
}

void CInplaceArrayEdit::Edit(int item)
{
	if (Type == "str[]") {
		auto rcitem = GetSubitemRect(m_list, item, 1);
		auto value = m_list.GetItemText(item, 1);
		m_inplaceedit.InitEdit(rcitem, value);
		m_inplaceedit.OKFunc = [&, this, item](CString newtext) {
			if (newtext.IsEmpty())
				return Fail("Blank entries are not allowed.");

			m_list.SetItemText(item, 1, newtext);
			return true;
		};
	}
	else if (Type == "bool[]") {
		auto rcitem = GetSubitemRect(m_list, item, 1);
		auto value = m_list.GetItemText(item, 1);
		m_inplacecheck.InitEdit(rcitem, "", value == "true");
		m_inplacecheck.OKFunc = [this,item](bool checked) {
			m_list.SetItemText(item, 1, checked ? "true" : "false");
			return true;
		};
	}
	else if (Type == "byte[]") {
		auto rcitem = GetSubitemRect(m_list, item, 1);
		auto value = m_list.GetItemText(item, 1);
		m_inplaceedit.InitEdit(rcitem, value);
		m_inplaceedit.OKFunc = [&, this, item](CString newtext) {
			if (newtext.IsEmpty())
				return Fail("Blank entries are not allowed.");

			auto strtext = m_inplaceedit.GetText();

			CString singletype = "byte";
			int bytewidth = GetTypeByteWidth(singletype);
			auto retval = ValidateHexInt(singletype, 1, strtext, [this, bytewidth](long long value) { return hex_upper(hex((int)value, bytewidth * 2), HexUppercase); });
			if (retval.result) m_list.SetItemText(item, 1, retval.value);
			if (!retval) AfxMessageBox(retval.value);
			return retval.result;
		};
	}
}

void CInplaceArrayEdit::Add(int item)
{
	auto strindex = std::to_string(item);
	auto slot = m_list.InsertItem(item, strindex.c_str());
	m_list.SetItemText(slot, 1, NewValueText);
	Edit(slot);

	RedrawScope redraw(&m_list);
	RenumberList(m_list, 0, 1, slot);
	m_list.SetItemState(slot, LVIS_SELECTED, LVIS_SELECTED);
	m_list.SetSelectionMark(slot);

	m_list.SetFocus();
}

void CInplaceArrayEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CInplaceArrayEdit, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CInplaceArrayEdit::OnClickList)
	ON_BN_CLICKED(IDC_BUTTON1, &CInplaceArrayEdit::OnClickAdd)
	ON_BN_CLICKED(IDC_BUTTON2, &CInplaceArrayEdit::OnClickDelete)
	ON_BN_CLICKED(IDC_BUTTON3, &CInplaceArrayEdit::OnClickAppend)
END_MESSAGE_MAP()


// CInplaceArrayEdit message handlers

namespace {
	mfcstringvector bools_to_mfcstrs(const boolvector & bools);
	mfcstringvector bytes_to_mfcstrs(const bytevector & bytes);
	boolvector mfcstrs_to_bools(const mfcstringvector & strs);
	bytevector mfcstrs_to_bytes(const mfcstringvector & strs);

	mfcstringvector bools_to_mfcstrs(const boolvector & bools)
	{
		mfcstringvector strs;
		for (auto b : bools) strs.push_back(b ? "true" : "false");
		return strs;
	}

	mfcstringvector bytes_to_mfcstrs(const bytevector & bytes)
	{
		mfcstringvector strs;
		CString fmt;
		for (auto b : bytes) {
			fmt.Format("0x%02X", b);
			strs.push_back(fmt);
		}
		return strs;
	}

	boolvector mfcstrs_to_bools(const mfcstringvector & strs)
	{
		boolvector bools;
		for (auto s : strs) bools.push_back(s == "true" ? true : false);
		return bools;
	}

	bytevector mfcstrs_to_bytes(const mfcstringvector & strs)
	{
		bytevector bytes;
		for (auto s : strs) bytes.push_back((unsigned char)strtoul(s, nullptr, 16));
		return bytes;
	}
}

BOOL CInplaceArrayEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	mfcstringvector strings;
	try {
		if (Type == "str[]")
			strings = mfcstrvec(ArrayText);
		else if (Type == "bool[]")
			strings = bools_to_mfcstrs(boolvec(ArrayText));
		else if (Type == "byte[]")
			strings = bytes_to_mfcstrs(bytevec(ArrayText));
	}
	catch (std::exception & ex) {
		return AbortInitDialog(this, CString("Error loading inplace array data:\n") + ex.what());
	}

	if (!Caption.IsEmpty()) SetWindowText(Caption);
	if (GetParent() != nullptr) m_list.SetFont(GetParent()->GetFont());

	m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "Index");
	m_list.InsertColumn(1, "Value");
	for (int st = 0; st < (int)strings.size(); ++st) {
		auto strindex = std::to_string(st + 1);
		auto index = m_list.InsertItem(st, strindex.c_str());
		m_list.SetItemText(index, 1, strings[st]);
	}

	AutosizeAllColumns(m_list);

	m_inplacecombo.Create(m_inplacecombo.IDD, &m_list);
	m_inplaceedit.Create(m_inplaceedit.IDD, &m_list);
	m_inplaceedit.SetOwner(&m_list);
	m_inplacecheck.Create(m_inplacecheck.IDD, &m_list);
	m_inplacecheck.SetOwner(&m_list);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CInplaceArrayEdit::OnOK()
{
	try {
		size_t empties = 0;
		mfcstringvector strings;
		for (int i = 0; i < m_list.GetItemCount(); ++i) {
			auto value = m_list.GetItemText(i, 1);
			if (value.IsEmpty()) ++empties;
			else strings.push_back(value);
		}

		if (empties > 0) {
			CString msg;
			msg.Format("Removed %lu blank entries.", empties);
			AfxMessageBox(msg, MB_ICONINFORMATION);
		}

		if (Type == "str[]") {
			ArrayText = mfcstrvec(strings);
		}
		else if (Type == "bool[]") {
			ArrayText = boolvec(mfcstrs_to_bools(strings));
		}
		else if (Type == "byte[]") {
			ArrayText = bytevec(mfcstrs_to_bytes(strings));
		}
	}
	catch (std::exception & ex) {
		AfxMessageBox((CString("Unable to save the array:\n") + ex.what()), MB_ICONERROR);
	}

	CDialogEx::OnOK();
}

void CInplaceArrayEdit::OnClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pnm = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	Edit(pnm->iItem);
	*pResult = 0;
}

void CInplaceArrayEdit::OnClickAdd()
{
	//N.B. - relies on this list control being single-selection
	auto index = m_list.GetSelectionMark();
	if (index == -1) index = m_list.GetItemCount();
	Add(index);
}

void CInplaceArrayEdit::OnClickDelete()
{
	auto selpos = m_list.GetFirstSelectedItemPosition();
	if (selpos == nullptr) {
		AfxMessageBox("Please select an item to remove.", MB_ICONEXCLAMATION);
	}
	else {
		RedrawScope redraw(&m_list);
		intvector vindexes;
		while (selpos) {
			auto index = m_list.GetNextSelectedItem(selpos);
			vindexes.push_back(index);
		}

		// Now remove the items in reverse order
		for (size_t st = vindexes.size(); st > 0; --st) {
			auto delindex = vindexes[st - 1];
			m_list.DeleteItem(delindex);
		}
		RenumberList(m_list, 0, 1, vindexes.front());

		if (m_list.GetItemCount() > 0) {
			auto newsel = vindexes.front();
			if (newsel >= m_list.GetItemCount()) newsel = m_list.GetItemCount() - 1;
			m_list.SetSelectionMark(newsel);
			m_list.SetItemState(newsel, LVIS_SELECTED, LVIS_SELECTED);
		}

		m_list.SetFocus();
	}
}

void CInplaceArrayEdit::OnClickAppend()
{
	Add(m_list.GetItemCount());
}