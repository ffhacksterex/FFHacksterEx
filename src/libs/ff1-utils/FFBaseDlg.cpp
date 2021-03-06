// CFFBaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FFBaseDlg.h"
#include "FFBaseApp.h"
#include "afxdialogex.h"
#include "ui_helpers.h"

// CFFBaseDlg dialog

IMPLEMENT_DYNAMIC(CFFBaseDlg, CDialogEx)

CFFBaseDlg::CFFBaseDlg(UINT nID, CWnd* pParent, UINT helpid)
	: CDialogEx(nID, pParent)
	, m_idd(nID)
	, m_helpid(helpid != 0 ? helpid : nID) // by default, the help ID is the dialog ID
{
}

CFFBaseDlg::~CFFBaseDlg()
{
}

bool CFFBaseDlg::InitFailed() const
{
	return m_initfailed;
}

UINT CFFBaseDlg::GetDlgId() const
{
	return m_idd;
}

BOOL CFFBaseDlg::EndAndReturn(int result, CString message)
{
	EndDialog(result);
	if (!message.IsEmpty()) AfxMessageBox(message);
	return TRUE;
}

BOOL CFFBaseDlg::FailInitAndReturn(int result, CString message)
{
	m_initfailed = true;
	return EndAndReturn(result, message);
}

// Set PreloadControlMappings to true before calling DoModal to opt into this function.
// Otherwise, it always returns nullptr.
CWnd* CFFBaseDlg::GetControlByName(CString controlname)
{
	auto iter = m_dlgmappings.find(controlname);
	auto id = iter != cend(m_dlgmappings) ? iter->second : -1;
	auto pwnd = GetDlgItem(id);
	return pwnd;
}

void CFFBaseDlg::InitFonts()
{
	auto normalfont = AfxGetFFBaseApp()->GetFonts().GetNormalFont();
	if (normalfont != nullptr) {
		SetFont(normalfont);
		this->SendMessageToDescendants(WM_SETFONT, (WPARAM)normalfont->operator HFONT(), 1);
	}
}

INT_PTR CFFBaseDlg::DoModal()
{
	try {
		// Opt into using RC data to locate controls.
		// This adds overrhead, but allows controls to be found by string name,
		// which can help when using scripts to manipulate the UI.
		if (PreloadControlMappings) {
			const auto& rcmap = AfxGetFFBaseApp()->GetIdDlgDataMap();
			m_dlgmappings = Rc::GetDialogControlMapping(rcmap, GetDlgId(), AfxGetResourceHandle());
		}
		return CDialogEx::DoModal();
	}
	catch (std::exception & ex) {
		CString msg;
		msg.Format(ex.what());
		AfxMessageBox(msg);
		EndDialog(IDABORT);
		return IDABORT;
	}
}

void CFFBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFFBaseDlg, CDialogEx)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDHELPBOOK, &CFFBaseDlg::OnBnClickedHelpbook)
	ON_BN_CLICKED(IDCANCEL2, &CFFBaseDlg::OnBnClickedCancel2)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


BOOL CFFBaseDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	auto ffapp = AfxGetFFBaseApp();
	if (ffapp != nullptr) {
		auto shift = GetKeyState(VK_SHIFT);
		auto contexthelp = (shift & 0x8000) != 0;
		auto ctrlid = contexthelp ? pHelpInfo->iCtrlId : (UINT)-1;
		ffapp->InvokeHelp(m_idd, ctrlid, this->GetHelpID());
	}
	return TRUE;
}

BOOL CFFBaseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	this->InitFonts();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

// This will always invoke the help of the container control
// (e.g. the dialog box instead of a focused control within the dialog).
void CFFBaseDlg::OnBnClickedHelpbook()
{
	auto ffapp = AfxGetFFBaseApp();
	if (ffapp != nullptr) ffapp->InvokeHelp(GetHelpID(), (UINT)-1, this->GetHelpID());
}

void CFFBaseDlg::OnBnClickedCancel2()
{
	OnCancel();
}

void CFFBaseDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDown(nFlags, point);
	Ui::HandleLbuttonDrag(this);
}