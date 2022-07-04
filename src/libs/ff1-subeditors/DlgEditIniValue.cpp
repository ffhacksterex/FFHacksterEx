// DlgEditIniValue.cpp : implementation file
//

#include "stdafx.h"
#include "DlgEditIniValue.h"
#include "afxdialogex.h"
#include "resource_subeditors.h"
#include <general_functions.h>
#include <set_types.h>
#include <string_functions.h>
#include <type_support.h>
#include <ui_helpers.h>
#include <vector_types.h>

using namespace Strings;
using namespace Types;
using namespace Ui;


// CDlgEditIniValue dialog

IMPLEMENT_DYNAMIC(CDlgEditIniValue, CFFBaseDlg)

CDlgEditIniValue::CDlgEditIniValue(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(IDD_EDITINIVALUE, pParent)
	, HexUppercase(false)
	, Disable(Disable::None)
{

}

CDlgEditIniValue::~CDlgEditIniValue()
{
}

void CDlgEditIniValue::LoadTypes()
{
	const auto & types = Types::GetUserCreatableTypes();
	m_typecombo.ResetContent();
	for (auto type : types) {
		m_typecombo.AddString(type);
	}
	m_typecombo.SetCurSel(0);
}

bool CDlgEditIniValue::ValidateEntry()
{
	auto text = GetControlText(m_valueedit);
	auto type = GetControlText(m_typecombo);
	CString failmsg;

	//FUTURE - translate the value text box instead of inserting a default/zero value
	auto ints = Types::GetDecConvertibleTypes();
	auto hexs = Types::GetHexConvertibleTypes();
	auto addrs = Types::GetAddrConvertibleTypes();
	if (type == "rgb") {
		text = "0xFF00FF"; // defaults to hot pink
	}
	else if (type == "str[]") {
		text = "str[]{ }";
	}
	else if (type == "byte[]") {
		text = "byte[]{ }";
	}
	else if (type == "bool[]") {
		text = "bool[]{ }";
	}
	else if (has(ints, type)) {
		text = "0";
		int bytewidth = GetTypeByteWidth(type);
		auto retval = ValidateDecInt(type, bytewidth, text, [](long long value) { return dec((int)value); });
		if (!retval.result)
			failmsg = retval.value;
		else
			text = retval.value;
	}
	else if (has(hexs, type)) {
		text = "0x0";
		int bytewidth = GetTypeByteWidth(type);
		auto retval = Types::ValidateHexInt(type, bytewidth, text, [this, bytewidth](long long value) { return hex_upper(hex((int)value, bytewidth * 2), HexUppercase); });
		if (!retval.result)
			failmsg = retval.value;
		else
			text = retval.value;
	}
	else if (has(addrs, type)) {
		text = "$0";
		int bytewidth = GetTypeByteWidth(type);
		auto retval = Types::ValidateAddrInt(type, bytewidth, text, [this, bytewidth](long long value) { return addr_upper(addr((int)value, bytewidth * 2), HexUppercase); });
		if (!retval.result)
			failmsg = retval.value;
		else
			text = retval.value;
	}
	else {
		failmsg.Format("Type '%s' is unrecognized.", (LPCSTR)type);
	}

	if (!failmsg.IsEmpty())
		AfxMessageBox(failmsg, MB_ICONERROR);
	else
		m_valueedit.SetWindowText(text); // set the default value

	return failmsg.IsEmpty();
}

void CDlgEditIniValue::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INI_EDIT_LABEL, m_keynameedit);
	DDX_Control(pDX, IDC_INI_EDIT_VALUE, m_valueedit);
	DDX_Control(pDX, IDC_INI_EDIT_GROUP, m_groupedit);
	DDX_Control(pDX, IDC_COMBO1, m_typecombo);
	DDX_Control(pDX, IDC_STATIC3, m_valuestatic);
}


BEGIN_MESSAGE_MAP(CDlgEditIniValue, CFFBaseDlg)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgEditIniValue::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CDlgEditIniValue message handlers


BOOL CDlgEditIniValue::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	//FUTURE - implement assigned values. for now, new Ini values are set to zero/default values.
	m_valuestatic.EnableWindow(FALSE);
	m_valueedit.EnableWindow(FALSE);
	m_valuestatic.ShowWindow(SW_HIDE);
	m_valueedit.ShowWindow(SW_HIDE);

	GetDlgItem(IDC_INI_LABEL)->SetWindowText(Label);
	m_keynameedit.SetWindowText(KeyName);
	m_valueedit.SetWindowText(Value);
	m_groupedit.SetWindowText(GroupName);

	m_keynameedit.EnableWindow((Disable & Disable::Key) == 0);
	m_groupedit.EnableWindow((Disable & Disable::Group) == 0);

	LoadTypes();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditIniValue::OnOK()
{
	CString thekeyname;
	m_keynameedit.GetWindowText(thekeyname);

	if (thekeyname.IsEmpty()) {
		AfxMessageBox("You must enter a name for the key.", MB_ICONERROR);
		return;
	}

	if (!ValidateEntry()) {
		return; // the function will emit a message upon validation failure
	}

	KeyName = thekeyname;
	m_valueedit.GetWindowText(Value);
	m_typecombo.GetWindowText(Type);
	m_groupedit.GetWindowText(GroupName);
	CFFBaseDlg::OnOK();
}

void CDlgEditIniValue::OnCbnSelchangeCombo1()
{
	auto newstyle = GetControlText(m_typecombo);
	if (newstyle != m_prevtype) {
		m_valueedit.Clear();
	}
	m_prevtype = newstyle;
}