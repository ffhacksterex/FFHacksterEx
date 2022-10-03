// DlgWelcome.cpp : implementation file
//
#include "stdafx.h"
#include "DlgWelcome.h"
#include <FFBaseApp.h>
#include <general_functions.h>
#include <imaging_helpers.h>
#include <path_functions.h>
#include <ui_helpers.h>
#include <ui_prompts.h>
#include <window_messages.h>
#include "About.h"
#include "DlgAppSettings.h"
#include "AppSettings.h"

using namespace Ui;
using namespace Imaging;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDlgWelcome dialog
#define BaseClass CDialogWithBackground

IMPLEMENT_DYNAMIC(CDlgWelcome, BaseClass)

CDlgWelcome::CDlgWelcome(CWnd* pParent /*= nullptr */)
	: BaseClass(IDD_WELCOME, pParent)
{
}

CDlgWelcome::~CDlgWelcome()
{
}

void CDlgWelcome::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROJECTS_BTN_OPENROM, m_openprojectbutton);
	DDX_Control(pDX, IDC_PROJECTS_BTN_EDITROM, m_editrombutton);
	DDX_Control(pDX, IDC_PROJECTS_BTN_NEWROM, m_newrombutton);
	DDX_Control(pDX, IDC_PROJECTS_BTN_NEWASM, m_newasmbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDC_STATIC3, m_editromstatic);
	DDX_Control(pDX, IDC_STATIC4, m_newromstatic);
	DDX_Control(pDX, IDC_STATIC5, m_newasmstatic);
	DDX_Control(pDX, IDC_STATIC6, m_openprojectstatic);
	DDX_Control(pDX, IDC_STATIC2, m_subbannerstatic);
	DDX_Control(pDX, IDC_PROJECTS_BTN_UNZIP, m_unzipbutton);
	DDX_Control(pDX, IDC_STATIC8, m_unzipstatic);
	DDX_Control(pDX, IDC_FFH_BTN_APPSETTINGS, m_appsettingsbutton);
	DDX_Control(pDX, IDABOUT, m_aboutbutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}

BEGIN_MESSAGE_MAP(CDlgWelcome, BaseClass)
	ON_BN_CLICKED(IDC_PROJECTS_BTN_NEWROM, &CDlgWelcome::OnBnClickedProjectsBtnNewrom)
	ON_BN_CLICKED(IDC_PROJECTS_BTN_NEWASM, &CDlgWelcome::OnBnClickedProjectsBtnNewasm)
	ON_BN_CLICKED(IDC_PROJECTS_BTN_OPENROM, &CDlgWelcome::OnBnClickedProjectsBtnOpen)
	ON_BN_CLICKED(IDC_PROJECTS_BTN_EDITROM, &CDlgWelcome::OnBnClickedProjectsBtnEditrom)
	ON_BN_CLICKED(IDC_PROJECTS_BTN_UNZIP, &CDlgWelcome::OnBnClickedProjectsBtnUnzip)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_FFH_BTN_APPSETTINGS, &CDlgWelcome::OnBnClickedFfhBtnAppsettings)
	ON_BN_CLICKED(IDABOUT, &CDlgWelcome::OnBnClickedAbout)
END_MESSAGE_MAP()


void CDlgWelcome::OnSelectAction()
{
	SaveMru();
	EndDialog(IDOK);
}

void CDlgWelcome::LoadMru()
{
	m_mrupaths = AppStgs->MruPaths;

	bool hasmenu = false;
	m_mrupopup.DestroyMenu();
	if (!m_mrupaths.empty()) {
		if (m_mrupopup.CreatePopupMenu()) {
			CRect rc;
			CPaintDC dc(this);
			int width = (int)(Ui::GetClientRect(this).Width() * 1.5f);
			for (size_t st = 0; st < m_mrupaths.size(); ++st) {
				auto mru = m_mrupaths[st];
				auto buffer = mru.GetBuffer();
				PathCompactPath(dc.GetSafeHdc(), buffer, width);
				mru.ReleaseBuffer();
				m_mrupopup.AppendMenu(MF_STRING, st + 1, mru); // these are 1-based entries
			}
			hasmenu = m_mrupopup.GetMenuItemCount() > 0;
		}
	}

	CString opentext;
	opentext.Format("OPEN PROJECT\nOpen a project of any supported type%s",
				hasmenu ? ", or right-click here for recent projects." : ".");
	m_openprojectstatic.SetWindowText(opentext);
}

void CDlgWelcome::SaveMru()
{
	AppStgs->MruPaths = m_mrupaths;
}

void CDlgWelcome::InvokeMruMenu()
{
	if (!m_mrupopup || (m_mrupopup.GetMenuItemCount() == 0)) return;

	CSize size;
	{
		CPaintDC dc(this);
		size.cy = dc.GetTextExtent("Ay").cy;
	}
	CRect rclink = Ui::GetWindowRect(&m_openprojectstatic);
	CPoint point = { rclink.left, rclink.bottom - size.cy};
	auto selop = m_mrupopup.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, point.x, point.y, this);
	if (selop > 0) {
		Action = "recent";
		ActionParam = m_mrupaths[selop - 1]; // the menu uses 1-based entries, subtract 1 to get the list index
		OnSelectAction();
	}
}

void CDlgWelcome::PaintClient(CDC& dc)
{
	if (IsIconic()) {
		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, AfxGetFFBaseApp()->GetLargeIcon());
	}
	else {
		BaseClass::PaintClient(dc);
	}
}

BOOL CDlgWelcome::OnInitDialog()
{
	BaseClass::OnInitDialog();

	SetIcon(AfxGetFFBaseApp()->GetLargeIcon(), TRUE);   // Load the Icon  :)
	SetIcon(AfxGetFFBaseApp()->GetSmallIcon(), FALSE);

	if (m_bannerfont.GetSafeHandle() == nullptr) {
		// Make the banner font times larger than the dialog's standard font
		LOGFONT lfbanner = { 0 };
		GetFont()->GetLogFont(&lfbanner);
		lfbanner.lfHeight = (lfbanner.lfHeight * 8) / 3;
		lfbanner.lfUnderline = TRUE;
		lfbanner.lfWeight = FW_BOLD;
		strcpy(lfbanner.lfFaceName, "Segoe UI"); // if it doesn't exist, it might not look to good, but hey...
		m_bannerfont.CreateFontIndirect(&lfbanner);
	}

	if (m_actionbuttonfont.GetSafeHandle() == nullptr) m_actionbuttonfont.CreatePointFont(100, "Verdana");

	LoadMru();

	Action = "";
	ActionParam = "";

	CString progname = LoadStringResource(IDS_PROGRAMCAPTION, DEF_PROGRAMCAPTION);
	SetWindowText(progname);
	m_banner.Set(this, COLOR_BLACK, COLOR_FFRED, progname);

	CString subtext = GetStaticText(m_subbannerstatic);
	subtext.Replace("FFHackster", progname);
	SetStaticText(m_subbannerstatic, subtext);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


// CDlgWelcome message handlers

void CDlgWelcome::OnBnClickedProjectsBtnEditrom()
{
	auto result = Ui::OpenFilePromptExt(this, "NES ROM files (*.nes)|*.nes||",
		".nes", "Select ROM to edit",
		FOLDERPREF(AppStgs, PrefProjectParentFolder));
	if (!result)
		return;

	Action = "immediaterom";
	ActionParam = result.value;
	OnSelectAction();
}

void CDlgWelcome::OnBnClickedProjectsBtnNewrom()
{
	Action = "newrom";
	OnSelectAction();
}

void CDlgWelcome::OnBnClickedProjectsBtnNewasm()
{
	Action = "newasm";
	OnSelectAction();
}

void CDlgWelcome::OnBnClickedProjectsBtnOpen()
{
	Action = "open";
	OnSelectAction();
}

void CDlgWelcome::OnBnClickedProjectsBtnUnzip()
{
	Action = "unzip";
	OnSelectAction();
}

void CDlgWelcome::OnOK()
{
	// React with the focused button if the user presses ENTER (which generates OnOK).
	// EndDialog(IDOK) does not invoke this handler.
	//DEVNOTE - changes to the dialog have adversely affecting the command routing for the enter key, sigh...
	//		this is faster than diving into MFC to fix the underlying issue.
	auto focuswnd = GetFocus();
	switch (focuswnd->GetDlgCtrlID()) {
		case IDC_PROJECTS_BTN_NEWROM: OnBnClickedProjectsBtnNewrom(); break;
		case IDC_PROJECTS_BTN_NEWASM: OnBnClickedProjectsBtnNewasm(); break;
		case IDC_PROJECTS_BTN_OPENROM: OnBnClickedProjectsBtnOpen(); break;
		case IDC_PROJECTS_BTN_EDITROM: OnBnClickedProjectsBtnEditrom(); break;
		case IDC_PROJECTS_BTN_UNZIP: OnBnClickedProjectsBtnUnzip(); break;
		case IDC_FFH_BTN_APPSETTINGS: OnBnClickedFfhBtnAppsettings(); break;
		case IDABOUT: OnBnClickedAbout(); break;
		case IDCANCEL: OnCancel(); break;
	}
}

void CDlgWelcome::OnContextMenu(CWnd* pWnd, CPoint /*point*/)
{
	if (pWnd == &m_openprojectstatic || pWnd == &m_openprojectbutton)
		InvokeMruMenu();
}

void CDlgWelcome::OnBnClickedFfhBtnAppsettings()
{
	CDlgAppSettings dlg(this);
	dlg.AppStgs = AppStgs;
	dlg.HelpSettings = &AfxGetFFBaseApp()->GetHelpSettings();
	AppStgs->Read();
	if (dlg.DoModal() == IDOK) {
		AppStgs->Write();
		LoadMru();
		EnableImage(AppStgs->ShowBackgroundArt);
		Invalidate();
	}
	else if (dlg.MruNeedsUpdate()) {
		LoadMru();
		Invalidate();
	}
}

void CDlgWelcome::OnBnClickedAbout()
{
	CAbout dlg(this);
	dlg.AppStgs = AppStgs;
	dlg.DoModal();
}