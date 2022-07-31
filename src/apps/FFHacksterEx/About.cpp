// About.cpp : implementation file
//
#include "stdafx.h"
#include "FFHackster.h"
#include "About.h"
#include "ui_helpers.h"
#include "window_messages.h"
#include "imaging_helpers.h"
#include "resource.h"
#include "version-number.h"

using namespace Ui;
using namespace Imaging;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAbout dialog

#define BaseClass CDialogWithBackground

CAbout::CAbout(CWnd* pParent /*= nullptr */)
	: BaseClass(CAbout::IDD, pParent)
{
	m_hIcon = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT);
}

void CAbout::HandleLink(LPNMHDR phdr)
{
	//DEVNOTE - another example of how moving to Common Controls 6 causes problems
	//			because it's Unicode-only.
	//			It's time to either move the app to Unicode or write a new one
	//			using Unicode from scratch.
	auto plink = (PNMLINK)phdr;
	LITEM item = plink->item;
	CStringA url(item.szUrl);
	ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOW);
}

void CAbout::PaintClient(CDC & dc)
{
	//DEVNOTE - not sure why, but the icon will no longer display when set in the resource editor.
	//		I had to hide the static and draw it manually just to make it visible.
	auto stat = GetDlgItem(IDC_ICONSTATIC);
	if (stat != nullptr) {
		auto rect = GetControlRect(stat);
		DrawIconEx(dc.GetSafeHdc(), rect.left, rect.top, m_hIcon, 32, 32, 0, NULL, DI_NORMAL);
	}

}

void CAbout::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ABOUT_STATIC_RELEASE, m_releasestatic);
	DDX_Control(pDX, IDC_ABOUT_STATIC_PROGNAME, m_progtitlestatic);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDC_SYSLINK_DISCH, m_linkdisch);
	DDX_Control(pDX, IDC_SYSLINK_PAULYGON, m_linkpaulygon);
	DDX_Control(pDX, IDC_SYSLINK3, m_linkgamefaqs);
	DDX_Control(pDX, IDC_SYSLINK4, m_7ziplicense);
	DDX_Control(pDX, IDC_SYSLINK5, m_linkiconmonstr);
}


BEGIN_MESSAGE_MAP(CAbout, BaseClass)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_DISCH, &CAbout::OnNMClickSyslinkDisch)
	ON_NOTIFY(NM_RETURN, IDC_SYSLINK_DISCH, &CAbout::OnNMReturnSyslinkDisch)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK_PAULYGON, &CAbout::OnNMClickSyslinkDisch)
	ON_NOTIFY(NM_RETURN, IDC_SYSLINK_PAULYGON, &CAbout::OnNMReturnSyslinkDisch)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK3, &CAbout::OnNMClickSyslinkDisch)
	ON_NOTIFY(NM_RETURN, IDC_SYSLINK3, &CAbout::OnNMReturnSyslinkDisch)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK4, &CAbout::OnNMClickSyslinkDisch)
	ON_NOTIFY(NM_RETURN, IDC_SYSLINK4, &CAbout::OnNMReturnSyslinkDisch)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK5, &CAbout::OnNMClickSyslinkDisch)
	ON_NOTIFY(NM_RETURN, IDC_SYSLINK5, &CAbout::OnNMReturnSyslinkDisch)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAbout message handlers

BOOL CAbout::OnInitDialog()
{
	BaseClass::OnInitDialog();

	CString progname = LoadStringResource(IDS_PROGRAMCAPTION, DEF_PROGRAMCAPTION);
	CString subtext = GetStaticText(m_progtitlestatic);
	subtext.Replace("FFHacksterEx", progname);
#if defined(_WIN64)
	subtext.AppendFormat(" (x64 build)");
#elif defined(_WIN32)
	subtext.AppendFormat(" (x86 build)");
#else
	subtext.AppendFormat(" (unknown architecture)");
#endif
	SetStaticText(m_progtitlestatic, subtext);

	CString reltext = LoadStringResource(IDS_RELEASETEXT, "");
	CString version = FFHEX_PROD_VERSION_TEXT;
	version.Replace(",", ".");
	version.Format("%s (%s)", (LPCSTR)version, (LPCSTR)reltext);
	m_releasestatic.SetWindowText(version);

	auto font = GetFont();
	m_linkdisch.SetFont(font);

	m_banner.Set(this, COLOR_BLACK, RGB(255, 32, 64), "About");
	m_okbutton.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAbout::OnDestroy()
{
	if (m_hIcon != NULL) {
		DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}
	BaseClass::OnDestroy();
}

void CAbout::OnNMClickSyslinkDisch(NMHDR* pNMHDR, LRESULT* pResult)
{
	HandleLink(pNMHDR);
	*pResult = 0;
}

void CAbout::OnNMReturnSyslinkDisch(NMHDR* pNMHDR, LRESULT* pResult)
{
	HandleLink(pNMHDR);
	*pResult = 0;
}