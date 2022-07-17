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

namespace {
	pair_result<CString> ValidateBoolEntry(CString text)
	{
		if (text.IsEmpty())
			text = "false";
		if (text.CompareNoCase("true") == 0) text = "true";
		else if (text.CompareNoCase("false") == 0) text = "false";
		else throw std::runtime_error("Can't convert value '" + std::string((LPCSTR)text) + "' to boolean.");

		return { true, text };
	}

	pair_result<CString> ValidateIntEntry(CString type, CString text)
	{
		if (text.IsEmpty())
			text = "0";
		int bytewidth = GetTypeByteWidth(type);
		auto retval = ValidateDecInt(type, bytewidth, text, [](long long value) { return dec((int)value); });
		return retval;
	}

	pair_result<CString> ValidateHexEntry(CString type, bool isupper, CString text)
	{
		if (text.IsEmpty())
			text = "0x0";
		int bytewidth = GetTypeByteWidth(type);
		auto retval = Types::ValidateHexInt(type, bytewidth, text, [isupper, bytewidth](long long value) { return hex_upper(hex((int)value, bytewidth * 2), isupper); });
		return retval;
	}

	pair_result<CString> ValidateAddrEntry(CString type, bool isupper, CString text)
	{
		if (text.IsEmpty())
			text = "$0";
		int bytewidth = GetTypeByteWidth(type);
		auto retval = Types::ValidateAddrInt(type, bytewidth, text, [isupper, bytewidth](long long value) { return addr_upper(addr((int)value, bytewidth * 2), isupper); });
		return retval;
	}

	pair_result<CString> ValidateRgbEntry(CString type, bool isupper, CString text)
	{
		if (text.IsEmpty()) text = "0xFF00FF";
		return ValidateHexEntry(type, isupper, text);
	}

	pair_result<CString> ValidateArrayEntry(CString type, CString text)
	{
		if (text.IsEmpty()) text = type + "{ }";
		return Types::ValidateArray(type, text);
	}
	

	using prcstr = pair_result<CString>;
	using editortypemappingfunc = std::function<pair_result<CString>(CString type, bool isupper, CString text)>;
	using editortypemap = std::map<CString, editortypemappingfunc>;

	const editortypemap l_editortypemap{
		{ "bool", [](CString type, bool, CString text) -> prcstr { return ValidateBoolEntry(text); } },
		{ "str", [](CString type, bool, CString text) -> prcstr { return {true,text}; } },
		{ "rgb", [](CString type, bool isupper, CString text) -> prcstr { return ValidateRgbEntry(type, isupper, text); } },

		{ "int8", [](CString type, bool, CString text) -> prcstr { return ValidateIntEntry(type, text); } },
		{ "int16", [](CString type, bool, CString text) -> prcstr { return ValidateIntEntry(type, text); } },
		{ "int24", [](CString type, bool, CString text) -> prcstr { return ValidateIntEntry(type, text); } },
		{ "int32", [](CString type, bool, CString text) -> prcstr { return ValidateIntEntry(type, text); } },

		{ "uint8", [](CString type, bool, CString text) -> prcstr { return ValidateIntEntry(type, text); } },
		{ "uint16", [](CString type, bool, CString text) -> prcstr { return ValidateIntEntry(type, text); } },
		{ "uint24", [](CString type, bool, CString text) -> prcstr { return ValidateIntEntry(type, text); } },
		{ "uint32", [](CString type, bool, CString text) -> prcstr { return ValidateIntEntry(type, text); } },

		{ "hex8", [](CString type, bool isupper, CString text) -> prcstr { return ValidateHexEntry(type, isupper, text); } },
		{ "hex16", [](CString type, bool isupper, CString text) -> prcstr { return ValidateHexEntry(type, isupper, text); } },
		{ "hex24", [](CString type, bool isupper, CString text) -> prcstr { return ValidateHexEntry(type, isupper, text); } },
		{ "hex32", [](CString type, bool isupper, CString text) -> prcstr { return ValidateHexEntry(type, isupper, text); } },

		{ "byte", [](CString type, bool isupper, CString text) -> prcstr { return ValidateHexEntry(type, isupper, text); } },
		{ "word", [](CString type, bool isupper, CString text) -> prcstr { return ValidateHexEntry(type, isupper, text); } },
		{ "bword", [](CString type, bool isupper, CString text) -> prcstr { return ValidateHexEntry(type, isupper, text); } },
		{ "dword", [](CString type, bool isupper, CString text) -> prcstr { return ValidateHexEntry(type, isupper, text); } },

		{ "addr", [](CString type, bool isupper, CString text) -> prcstr { return ValidateAddrEntry(type, isupper, text); } },
		{ "opcode", [](CString type, bool isupper, CString text) -> prcstr { return ValidateAddrEntry(type, isupper, text); } },

		{ "str[]", [](CString type, bool, CString text) -> prcstr { return ValidateArrayEntry(type, text); }},
		{ "bool[]", [](CString type, bool, CString text) -> prcstr { return ValidateArrayEntry(type, text); }},
		{ "byte[]", [](CString type, bool, CString text) -> prcstr { return ValidateArrayEntry(type, text); }}
	};
}

bool CDlgEditIniValue::ValidateEntry()
{
	auto text = GetControlText(m_valueedit);
	auto type = GetControlText(m_typecombo);
	auto iter = l_editortypemap.find(type);
	auto result = (iter != cend(l_editortypemap))
		? iter->second(type, HexUppercase, text)
		: pair_result<CString>{false, "Type '" + type + "' not mapped and cannot be validated by the editor."};
	if (!result) {
		CString failmsg;
		failmsg.Format("Type '%s' is unrecognized.\n%s", (LPCSTR)type, (LPCSTR)result.value);
		AfxMessageBox(failmsg, MB_ICONERROR);
	} else {
		m_valueedit.SetWindowText(result.value);
	}
	return result;
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