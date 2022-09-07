// DlgAppSettings.cpp : implementation file
//

#include "stdafx.h"
#include "DlgAppSettings.h"
#include "AppSettings.h"
#include <imaging_helpers.h>
#include <path_functions.h>
#include <ui_helpers.h>
#include <ui_prompts.h>
#include <window_messages.h>
#include <algorithm>
#include "DlgFolderPrefs.h"

using namespace Imaging;
using namespace Ui;

namespace {
	const CString LocalAppHelpPath("./Help");
}

// CDlgAppSettings dialog
#define BaseClass CDialogWithBackground


IMPLEMENT_DYNAMIC(CDlgAppSettings, BaseClass)

CDlgAppSettings::CDlgAppSettings(CWnd* pParent /*= nullptr */)
	: BaseClass(IDD_APPSETTINGS, pParent)
	, m_bUpdateMru(false)
{
}

CDlgAppSettings::~CDlgAppSettings()
{
}

bool CDlgAppSettings::MruNeedsUpdate() const
{
	return m_bUpdateMru;
}

void CDlgAppSettings::HideHelpOptions()
{
	auto phelp = GetDlgItem(IDC_APPSTGS_STATIC_HELPTYPE);
	auto prun = GetDlgItem(IDC_APPSTGS_STATIC_EXECTORUN);
	auto pcancel = GetDlgItem(IDCANCEL);

	//Hide everything between help to the run (excluding run itself)
	for (auto pwnd = phelp; pwnd != prun; pwnd = pwnd->GetNextWindow(GW_HWNDNEXT)) {
		pwnd->ShowWindow(SW_HIDE);
	}

	// Move eveyrthing from run to cancel up by the distance between help and run
	auto sidiff = Ui::GetDistance(phelp, prun);
	auto diff = sidiff.cy;
	for (auto pwnd = prun; pwnd != pcancel; pwnd = pwnd->GetNextWindow(GW_HWNDNEXT)) {
		Ui::MoveControlBy(pwnd, 0, diff);
	}
	Ui::MoveControlBy(pcancel, 0, diff);

	// Shrink the dialog accordingly, negate the diff value
	Ui::ShrinkWindow(this, 0, -diff);
}

BOOL CDlgAppSettings::OnInitDialog()
{
	BaseClass::OnInitDialog();

	if (AppStgs == nullptr) {
		AfxMessageBox("App AppStgs editor can't be displayed:\nno app settings were specified to edit.");
		EndDialog(IDABORT);
		return TRUE;
	}

	SetCheckValue(m_checkPromptOnClose, AppStgs->PromptOnClose);
	SetCheckValue(m_purgelogcheckbox, AppStgs->PurgeLogFileOnStartup);
	SetCheckValue(m_savehexuppercheck, AppStgs->SaveHexAsUppercase);
	SetCheckValue(m_showbackgroundartcheck, AppStgs->ShowBackgroundArt);
	SetCheckValue(m_forcenewlabelscheck, AppStgs->ForceNewLabelsForNewProjects);
	SetCheckValue(m_forcenewlabelscheck, AppStgs->ForceNewLabelsForNewProjects);
	SetCheckValue(m_displaystrikeasnormalcheck, !AppStgs->DisplayStrikeChecksAsNormalCheckboxes); //N.B. - this condition is inverted from the UI
	m_strikecheckcolorstatic.SetColor(AppStgs->StrikeCheckedColor);
	SetCheckValue(m_alwayscompilecheck, AppStgs->CompileBeforeRun);
	SetCheckValue(m_usefiledialogtobrowsecheck, AppStgs->UseFileDialogToBrowseFolders);
	SetCheckValue(m_enforceasmcompatcheck, AppStgs->EnforceAssemblyDllCompatibility);
	SetCheckValue(m_warnasmcompatcheck, AppStgs->WarnAssemblyDllCompatibility);
	SetCheckValue(m_usefolderprefscheck, AppStgs->UseFolderPrefs);
	m_editRunExe.SetWindowText(AppStgs->RunExe);
	m_cmdparamsedit.SetWindowText(AppStgs->RunParams);

	m_folderprefs = *((sAppFolderPrefs*)this->AppStgs); // slicing ...

	if (AppStgs->EnableHelpChoice) {
		if (HelpSettings == nullptr) {
			AfxMessageBox("App Settings editor can't modify help settings:\nno settings were specified to edit.");
			m_localhelpbutton.EnableWindow(FALSE);
			m_editlocalhelp.EnableWindow(FALSE);
		}
		else {
			for (auto i = (int)HelpType::None; i < (int)HelpType::Last_; ++i) {
				auto index = m_helptypes.AddString(GetHelpTypeName((HelpType)i));
				m_helptypes.SetItemData(index, i);
			}
			m_helptypes.SetCurSel(AppStgs->HelpTypeId);
			OnCbnSelchangeComboHelpTypes();
			m_editlocalhelp.SetWindowText(AppStgs->HelpPath);
		}
	}
	else {
		HideHelpOptions();
	}

	OnBnClickedStrikeCheck();
	OnBnClickedEnforceAsmCompatibility();

	m_banner.Set(this, RGB(0, 0, 0), RGB(32, 255, 64), "App Settings");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAppSettings::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APPSET_CHECK_PROMPTONCLOSE, m_checkPromptOnClose);
	DDX_Control(pDX, IDC_APPSETTINGS_EDIT_RUNEXE, m_editRunExe);
	DDX_Control(pDX, IDC_APPSET_CHECK_PURGELOGONSTARTUP, m_purgelogcheckbox);
	DDX_Control(pDX, IDC_CHECK1, m_savehexuppercheck);
	DDX_Control(pDX, IDC_CHECK2, m_showbackgroundartcheck);
	DDX_Control(pDX, IDC_CHECK3, m_forcenewlabelscheck);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDC_BUTTON1, m_clearmrubutton);
	DDX_Control(pDX, IDC_BUTTON2, m_clearinvalidmrubutton);
	DDX_Control(pDX, IDC_APPSETTINGS_BTN_RUNEXE, m_exebrowsebutton);
	DDX_Control(pDX, IDC_APPSTGS_STATIC_EXECTORUN, m_exestatic);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_CHECK4, m_displaystrikeasnormalcheck);
	DDX_Control(pDX, IDC_STATIC2, m_strikecheckcolorstatic);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
	DDX_Control(pDX, IDC_CHECK5, m_alwayscompilecheck);
	DDX_Control(pDX, IDC_CHECK6, m_usefiledialogtobrowsecheck);
	DDX_Control(pDX, IDC_CHECK7, m_enforceasmcompatcheck);
	DDX_Control(pDX, IDC_CHECK8, m_warnasmcompatcheck);
	DDX_Control(pDX, IDC_CHECK_USEFOLDERPREFS, m_usefolderprefscheck);
	DDX_Control(pDX, IDC_BUTTON_LOCALHELP, m_localhelpbutton);
	DDX_Control(pDX, IDC_EDIT_LOCALHELP, m_editlocalhelp);
	DDX_Control(pDX, IDC_COMBO_HELP_TYPES, m_helptypes);
	DDX_Control(pDX, IDC_APPSETTINGS_EDIT_PARAMS, m_cmdparamsedit);
	DDX_Control(pDX, IDC_APPSETTINGS_BUTTON3_DIRECTORIES, m_foldersbutton);
}

void CDlgAppSettings::OnOK()
{
	CString runexe;
	m_editRunExe.GetWindowText(runexe);
	if (!runexe.IsEmpty() && !Paths::FileExists(runexe)) {
		AfxMessageBox("If the run EXE is specified, then the file must exist.", MB_ICONERROR);
		m_editRunExe.SetSel(0, -1);
		m_editRunExe.SetFocus();
		return;
	}

	HelpType helptype = HelpType::Invalid;
	CString helppath;
	if (AppStgs->EnableHelpChoice) {
		if (HelpSettings != nullptr) {
			helptype = (HelpType)Ui::GetSelectedItemData(m_helptypes, (DWORD_PTR)HelpType::Invalid);
			helppath = Ui::GetControlText(m_editlocalhelp);
			if (helptype == HelpType::Invalid) {
				helptype = HelpType::None;
				helppath = "";
			}
			if (!HelpSettings->Init(helptype, helppath)) {
				auto result = AfxMessageBox("Unable to set new help settings; continue saving anyway?",
					MB_OKCANCEL | MB_ICONWARNING);
				if (result != IDOK) return;
			}
		}
	}

	AppStgs->PromptOnClose = GetCheckValue(m_checkPromptOnClose);
	AppStgs->PurgeLogFileOnStartup = GetCheckValue(m_purgelogcheckbox);
	AppStgs->SaveHexAsUppercase = GetCheckValue(m_savehexuppercheck);
	AppStgs->ShowBackgroundArt = GetCheckValue(m_showbackgroundartcheck);
	AppStgs->ForceNewLabelsForNewProjects = GetCheckValue(m_forcenewlabelscheck);
	AppStgs->DisplayStrikeChecksAsNormalCheckboxes = !GetCheckValue(m_displaystrikeasnormalcheck); //N.B. - this condition is inverted from the UI
	AppStgs->StrikeCheckedColor = m_strikecheckcolorstatic.GetColor();
	AppStgs->CompileBeforeRun = GetCheckValue(m_alwayscompilecheck);
	AppStgs->UseFileDialogToBrowseFolders = GetCheckValue(m_usefiledialogtobrowsecheck);
	AppStgs->EnforceAssemblyDllCompatibility = GetCheckValue(m_enforceasmcompatcheck);
	AppStgs->WarnAssemblyDllCompatibility = GetCheckValue(m_warnasmcompatcheck);
	AppStgs->UseFolderPrefs = GetCheckValue(m_usefolderprefscheck);
	AppStgs->RunExe = runexe;
	AppStgs->RunParams = Ui::GetControlText(m_cmdparamsedit);

	(sAppFolderPrefs&)*this->AppStgs = m_folderprefs; // slicing the other way ...

	if (AppStgs->EnableHelpChoice) {
		if (HelpSettings != nullptr) {
			AppStgs->HelpTypeId = helptype;
			AppStgs->HelpPath = helppath;
		}
	}

	BaseClass::OnOK();
}


BEGIN_MESSAGE_MAP(CDlgAppSettings, BaseClass)
	ON_BN_CLICKED(IDC_APPSETTINGS_BTN_RUNEXE, &CDlgAppSettings::OnBnClickedAppsettingsBtnRunexe)
	ON_BN_CLICKED(IDC_BUTTON_LOCALHELP, &CDlgAppSettings::OnBnClickedButtonLocalhelp)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgAppSettings::OnBnClickedClearMru)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgAppSettings::OnBnClickedClearInvalidMRU)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgAppSettings::OnBnClickedShowBackgroundArt)
	ON_STN_CLICKED(IDC_STATIC2, &CDlgAppSettings::OnBnClickedColorStrikeCheck)
	ON_BN_CLICKED(IDC_CHECK4, &CDlgAppSettings::OnBnClickedStrikeCheck)
	ON_BN_CLICKED(IDC_CHECK7, &CDlgAppSettings::OnBnClickedEnforceAsmCompatibility)
	ON_CBN_SELCHANGE(IDC_COMBO_HELP_TYPES, &CDlgAppSettings::OnCbnSelchangeComboHelpTypes)
	ON_BN_CLICKED(IDC_APPSETTINGS_BUTTON3_DIRECTORIES, &CDlgAppSettings::OnBnClickedFolderPrefs)
END_MESSAGE_MAP()


// CDlgAppSettings message handlers

void CDlgAppSettings::OnBnClickedAppsettingsBtnRunexe()
{
	CString oldfile = Ui::GetControlText(m_editRunExe);
	CString deffile = Paths::FileExists(oldfile) ? oldfile : "";
	CString exepath = PromptForEXE(this, deffile);
	if (Paths::FileExists(exepath)) {
		m_editRunExe.SetWindowText(exepath);
		m_editRunExe.SetFocus();
	}
	else if (!exepath.IsEmpty()) {
		AfxMessageBox("Please select an existing file.");
	}
}

void CDlgAppSettings::OnBnClickedButtonLocalhelp()
{
	auto current = Paths::RebaseToAppPath(Ui::GetEditValue(m_editlocalhelp));
	auto initialdir = Paths::DirExists(current) ? current : "";

	// The way help is structured, use the folder containing the hele config.
	auto result = Ui::OpenFilePrompt(this, "*.helpconfig (FFHEx Help Configuration)|*.helpconfig||",
		"Select a .helpconfig to use its folder for Help", initialdir);
	if (result) {
		m_editlocalhelp.SetWindowText(Paths::GetDirectoryPath(result.value));
	}
}

void CDlgAppSettings::OnBnClickedClearMru()
{
	auto result = AfxMessageBox("This will immediately clear all MRU entries.\nProceed?", MB_YESNO | MB_ICONQUESTION);
	if (result == IDYES) {
		AppStgs->MruPaths.clear();
		m_bUpdateMru = true;
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
		m_editRunExe.SetFocus();
	}
}

void CDlgAppSettings::OnBnClickedClearInvalidMRU()
{
	auto result = AfxMessageBox("This will immediately remove all invalid MRU entries.\nProceed?", MB_YESNO | MB_ICONQUESTION);
	if (result == IDYES) {
		auto & paths = AppStgs->MruPaths;
		size_t origsize = paths.size();
		paths.erase(std::remove_if(begin(paths), end(paths), [](auto strpath) { return !Paths::FileExists(strpath); }), end(paths));
		size_t removecount = origsize - paths.size();
		if (removecount == 1) {
			AfxMessageBox("Removed one path.");
		}
		else if (removecount > 1) {
			CString msg; msg.Format("Removed %u of %u paths.", removecount, origsize);
			AfxMessageBox(msg);
		}
		m_bUpdateMru = true;
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
		m_editRunExe.SetFocus();
	}
}

void CDlgAppSettings::OnBnClickedShowBackgroundArt()
{
	EnableImage(GetCheckValue(m_showbackgroundartcheck));
}

void CDlgAppSettings::OnBnClickedColorStrikeCheck()
{
	DWORD color = m_strikecheckcolorstatic.GetColor();
	CColorDialog dlg(color, 0, this);
	if (dlg.DoModal() == IDOK) {
		DWORD newcolor = dlg.GetColor();
		m_strikecheckcolorstatic.SetColor(newcolor);
	}
}

void CDlgAppSettings::OnBnClickedStrikeCheck()
{
	bool usecolor = GetCheckValue(m_displaystrikeasnormalcheck);
	m_strikecheckcolorstatic.ShowWindow(usecolor ? SW_SHOW : SW_HIDE); //N.B. - state is inverted
}

void CDlgAppSettings::OnBnClickedEnforceAsmCompatibility()
{
	//DEVNOTE - if using background images, disabled text is hard to see, so for now toggle visibility instead.
	//m_warnasmcompatcheck.ShowWindow(!GetCheckValue(m_enforceasmcompatcheck) ? SW_SHOW : SW_HIDE);
	m_warnasmcompatcheck.EnableWindow(!GetCheckValue(m_enforceasmcompatcheck));
}

void CDlgAppSettings::OnCbnSelchangeComboHelpTypes()
{
	auto showedit = FALSE;
	auto showbtn = FALSE;
	auto type = (HelpType)m_helptypes.GetItemData(m_helptypes.GetCurSel());
	switch (type) {
		case HelpType::None:
		case HelpType::LocalApp:
			break;
		case HelpType::Folder:
			showedit = TRUE;
			showbtn = TRUE;
			break;
		case HelpType::Web:
			showedit = TRUE;
			break;
	}
	m_editlocalhelp.EnableWindow(showedit);
	m_localhelpbutton.EnableWindow(showbtn);
}

void CDlgAppSettings::OnBnClickedFolderPrefs()
{
	// In the settings file, we store *APP.
	// At runtime, the object contains the actual app folder path.
	CDlgFolderPrefs dlg(this);
	dlg.Prefs = m_folderprefs;
	dlg.Prefs.EncodePrefPaths(); // encode so the user sees *APP
	if (dlg.DoModal() == IDOK) {
		m_folderprefs = dlg.Prefs;
		m_folderprefs.DecodePrefPaths(); // so the object has the app folder, not *APP
	}
}
