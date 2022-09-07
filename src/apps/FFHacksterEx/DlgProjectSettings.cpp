// DlgProjectSettings.cpp : implementation file
//

#include "stdafx.h"
#include "DlgProjectSettings.h"
#include <AppSettings.h>
#include <FFHacksterProject.h>
#include <ProjectLoader.h>
#include <asmdll_impl.h>
#include <general_functions.h>
#include <ingame_text_functions.h>
#include <interop_assembly_interface.h>
#include <ui_helpers.h>
#include <path_functions.h>
#include <window_messages.h>
#include <ui_helpers.h>
#include <ui_prompts.h>
#include <imaging_helpers.h>
#include "std_project_properties.h"

using namespace asmdll_impl;
using namespace Imaging;
using namespace Ingametext;
using namespace Ui;

// CDlgProjectSettings dialog

IMPLEMENT_DYNAMIC(CDlgProjectSettings, CDialogWithBackground)

CDlgProjectSettings::CDlgProjectSettings(CWnd* pParent /*= nullptr */)
	: CDialogWithBackground(IDD_PROJECTSETTINGS, pParent)
	, ShowBackgroundArt(false)
{
}

CDlgProjectSettings::~CDlgProjectSettings()
{
}

void CDlgProjectSettings::HideAssemblySpecificControls()
{
	// Get the difference in height between the asmdll static and the forcenewlabels check box
	auto * pwnd = m_asmdlledit.GetWindow(GW_HWNDNEXT);
	auto rcforce = GetControlRect(pwnd);
	auto rcasm = GetControlRect(&m_asmdllstatic);
	auto diff = rcforce.top - rcasm.top;

	// Now hide the asm controls and slide the ones starting with the asmstatic upward
	m_asmdllstatic.ShowWindow(SW_HIDE);
	m_asmdllbrowsebutton.ShowWindow(SW_HIDE);
	m_asmdlledit.ShowWindow(SW_HIDE);
	m_suppressasmwarningcheck.ShowWindow(SW_HIDE);
	m_coerceasmbutton.ShowWindow(SW_HIDE);

	while (pwnd != nullptr) {
		if (pwnd != &m_closebutton && pwnd != &m_helpbookbutton)
			MoveControlBy(pwnd, 0, -diff);
		pwnd = pwnd->GetWindow(GW_HWNDNEXT);
	}

	ShrinkWindow(this, 0, diff);
}

void CDlgProjectSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogWithBackground::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROJSET_STATIC_PROJTYPE, m_statProjtype);
	DDX_Control(pDX, IDC_PROJSET_EDIT_REVERTPATH, m_editRevertPath);
	DDX_Control(pDX, IDC_EDIT2, m_editPublishPath);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_PROJSET_BUTTON_REVERTPATH, m_setbutton);
	DDX_Control(pDX, IDC_PROJSET_BUTTON_PUBLISHPATH, m_publishbutton);
	DDX_Control(pDX, IDC_PROJSET_BUTTON_REVERTCLEAR, m_clearbutton);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDC_STATIC1, m_static1);
	DDX_Control(pDX, IDC_STATIC2, m_static2);
	DDX_Control(pDX, IDC_APPSETTINGS_EDIT_ADDLMODFOLDER, m_addlmodfolderedit);
	DDX_Control(pDX, IDC_APPSETTINGS_BTN_ADDLMODFOLDER, m_addlmodfolderbutton);
	DDX_Control(pDX, IDC_STATIC3, m_addlmodfolderstatic);
	DDX_Control(pDX, IDC_BUTTON1, m_asmdllbrowsebutton);
	DDX_Control(pDX, IDC_EDIT1, m_asmdlledit);
	DDX_Control(pDX, IDC_STATIC4, m_asmdllstatic);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
	DDX_Control(pDX, IDC_PROJSTGS_CHECK_SUPPRESSASMWARNING, m_suppressasmwarningcheck);
	DDX_Control(pDX, IDC_PROJSTGS_BUTTON_COERCEASM, m_coerceasmbutton);
}

BEGIN_MESSAGE_MAP(CDlgProjectSettings, CDialogWithBackground)
	ON_BN_CLICKED(IDC_PROJSET_BUTTON_REVERTPATH, &CDlgProjectSettings::OnBnClickedProjsetButtonRevertpath)
	ON_BN_CLICKED(IDC_PROJSET_BUTTON_PUBLISHPATH, &CDlgProjectSettings::OnBnClickedProjsetButtonPublishpath)
	ON_BN_CLICKED(IDC_PROJSET_BUTTON_REVERTCLEAR, &CDlgProjectSettings::OnBnClickedProjsetButtonRevertclear)
	ON_BN_CLICKED(IDC_APPSETTINGS_BTN_ADDLMODFOLDER, &CDlgProjectSettings::OnBnClickedAppsettingsBtnAddlmodfolder)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgProjectSettings::OnBnClickedProjectAssemblyDllPath)
	ON_BN_CLICKED(IDC_PROJSTGS_BUTTON_COERCEASM, &CDlgProjectSettings::OnBnClickedProjstgsButtonCoerceasm)
END_MESSAGE_MAP()


BOOL CDlgProjectSettings::OnInitDialog()
{
	if (m_proj == nullptr) {
		EndDialog(IDABORT);
		return TRUE;
	}

	AppStgs = m_proj->AppSettings;

	CDialogWithBackground::OnInitDialog();

	Ui::CreateResizedFont(GetFont(), m_projfonttype, 2.0f);
	m_statProjtype.SetFont(&m_projfonttype);

	CString title;
	title.Format("Project Settings: %s", (LPCSTR)m_proj->ProjectName);
	SetWindowText(title);

	CString projtype(m_proj->ProjectTypeName);
	projtype.MakeUpper();
	projtype += " Project";

	//N.B. since project is being used live, a call to m_proj->LoadProjectSettings() doesn't happen here.
	//		If that starts causing problems, add it, but be sure to understand what that function does first.

	m_statProjtype.SetWindowText(projtype);
	m_editPublishPath.SetWindowText(m_proj->PublishRomPath);
	m_addlmodfolderedit.SetWindowText(m_proj->AdditionalModulesFolder);
	if (m_proj->IsAsm()) {
		m_asmdlledit.SetWindowText(m_proj->AsmDLLPath);
	}
	else {
		HideAssemblySpecificControls();
	}

	Ui::SetCheckValue(m_suppressasmwarningcheck, m_proj->SuppressWarnOnAssemblyMismatch);

	if (m_proj->IsAsm()) {
		auto iscompat = asmdll_impl::IsProjectDllCompatible(*m_proj);
		m_coerceasmbutton.EnableWindow(iscompat != true);
	}

	m_banner.Set(this, COLOR_BLACK, COLOR_FFBLUE, "Project Settings");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProjectSettings::OnOK()
{
	CString revertpath;
	m_editRevertPath.GetWindowText(revertpath);
	if (!revertpath.IsEmpty() && !Paths::FileExists(revertpath)) {
		AfxMessageBox("If the reversion file is specified, then it must exist.", MB_ICONERROR);
		m_editRevertPath.SetSel(0, -1);
		m_editRevertPath.SetFocus();
		return;
	}

	CString addlfolder;
	m_addlmodfolderedit.GetWindowText(addlfolder);
	if (!addlfolder.IsEmpty() && !Paths::DirExists(addlfolder)) {
		AfxMessageBox("If the additional modules folder is specified, then the folder must exist.", MB_ICONERROR);
		m_addlmodfolderedit.SetSel(0, -1);
		m_addlmodfolderedit.SetFocus();
		return;
	}

	CString publishpath, publishdir;
	m_editPublishPath.GetWindowText(publishpath);
	publishdir = Paths::GetDirectoryPath(publishpath);
	if (!publishdir.IsEmpty() && !Paths::DirExists(publishdir)) {
		AfxMessageBox("If the publish path folder is specified, then the folder must exist.", MB_ICONERROR);
		m_editPublishPath.SetSel(0, -1);
		m_editPublishPath.SetFocus();
		return;
	}

	CString asmdllpath;
	if (m_proj->IsAsm()) {
		m_asmdlledit.GetWindowText(asmdllpath);
		CString origasmdll = m_proj->AsmDLLPath;
		CString msg = "";
		if (!asmdllpath.IsEmpty() && !Paths::FileExists(asmdllpath))
			msg = "The assembly DLL must exist.";
		else if (!is_ff1_asmdll(asmdllpath))
			msg = "The specified DLL is not recognized as an FF1 assembly DLL.";

		if (!msg.IsEmpty()) {
			AfxMessageBox(msg, MB_ICONERROR);
			m_asmdlledit.SetSel(0, -1);
			m_asmdlledit.SetFocus();
			return;
		}

		if (!origasmdll.IsEmpty() && (origasmdll != asmdllpath)) {
			auto answer = AfxMessageBox("Are you sure you want to change the assembly DLL from\n" +
				origasmdll + "\nto\n" + asmdllpath + "?", MB_OKCANCEL | MB_ICONQUESTION);
			bool isvalid = false;
			if (answer == IDOK) {
				auto asmresult = ProjectLoader::IsProjectDllCompatible(*m_proj, asmdllpath);
				isvalid = asmresult;
				if (!asmresult.value.IsEmpty()) AfxMessageBox(asmresult.value);
			}
			if (!isvalid) {
				m_asmdlledit.SetSel(0, -1);
				m_asmdlledit.SetFocus();
				return; // close without another prompt
			}
		}
	}

	// All other prerequisites are done, can we copy the reversion file
	if (!revertpath.IsEmpty()) {
		if (!Paths::FileCopy(revertpath, m_proj->RevertGameDataPath)) {
			auto result = AfxMessageBox(
				"The reversion file could not be updated.\nContinue saving the other project settings?",
				MB_OKCANCEL | MB_ICONQUESTION);
			if (result != IDOK) return;
		}
	}

	m_proj->SuppressWarnOnAssemblyMismatch = Ui::GetCheckValue(m_suppressasmwarningcheck);
	m_proj->PublishRomPath = publishpath;
	m_proj->AdditionalModulesFolder = addlfolder;
	m_proj->AsmDLLPath = asmdllpath;
	m_proj->SaveSharedSettings();
	VERIFY(m_proj->SaveRefPaths());

	CDialogWithBackground::OnOK();
}

// CDlgProjectSettings message handlers

void CDlgProjectSettings::OnBnClickedProjsetButtonRevertpath()
{
	CString filter, title;
	if (m_proj->IsRom()) {
		filter = "FF1 ROM (*.nes)|*.nes|All files (*.*)|*.*||";
		title = "Select a clean FF1 file";
	}
	else if (m_proj->IsAsm()) {
		filter = "Zipped Disassembly (*.zip)|*.zip|All files (*.*)|*.*||";
		title = "Select a zipped FF1 Disassembly archive";
	}
	else {
		AfxMessageBox("Project type " + m_proj->ProjectTypeName + " doesn't support reversion.", MB_ICONERROR);
		return;
	}

	// The file must exist, so use OpenFilePrompt
	auto result = OpenFilePrompt(this, filter, title, FOLDERPREF(m_proj->AppSettings, PrefCleanFolder));
	if (result)
		Ui::SetEditTextAndFocus(m_editRevertPath, result.value);
}

void CDlgProjectSettings::OnBnClickedProjsetButtonRevertclear()
{
	m_editRevertPath.SetWindowText("");
}

void CDlgProjectSettings::OnBnClickedProjsetButtonPublishpath()
{
	auto prevfile = iif(Ui::GetControlText(m_editPublishPath), m_proj->PublishRomPath);
	if (!Paths::FileExists(prevfile) && m_proj->AppSettings->UseFolderPrefs)
		prevfile = m_proj->AppSettings->PrefPublishFolder;
	auto result = SaveFilePrompt(this, "FF1 ROM (*.nes)|*.nes||", "Select Destination ROM location", prevfile);
	if (result)
		Ui::SetEditTextAndFocus(m_editPublishPath, result.value);
}

void CDlgProjectSettings::OnBnClickedAppsettingsBtnAddlmodfolder()
{
	auto prevfolder = iif(Ui::GetControlText(m_addlmodfolderedit), m_proj->AdditionalModulesFolder);
	if (!Paths::DirExists(prevfolder) && m_proj->AppSettings->UseFolderPrefs)
		prevfolder = m_proj->AppSettings->PrefAdditionalModulesFolder;
	auto folder = PromptForFolder(this, "Select Additional Modules Folder", prevfolder);
	if (Paths::DirExists(folder))
		Ui::SetEditTextAndFocus(m_addlmodfolderedit, folder.value);
}

void CDlgProjectSettings::OnBnClickedProjectAssemblyDllPath()
{
	auto prevfile = iif(Ui::GetControlText(m_asmdlledit), m_proj->AsmDLLPath);
	if (!Paths::FileExists(prevfile) && m_proj->AppSettings->UseFolderPrefs)
		prevfile = m_proj->AppSettings->PrefAsmDllFolder;
	auto result = OpenFilePrompt(AfxGetMainWnd(), "FFHacksterEX Assembly parsing DLL (*.dll)|*.dll||",
		"Locate FFHacksterEX assembly DLL", prevfile);
	if (result) {
		bool isvalid = is_ff1_asmdll(result.value);
		if (isvalid)
			Ui::SetEditTextAndFocus(m_asmdlledit, result.value);
		else
			AfxMessageBox("The selected DLL is not supported.");
	}
}

void CDlgProjectSettings::OnBnClickedProjstgsButtonCoerceasm()
{
	auto result = AfxMessageBox(
		"Do want to try to coerce the assembly's asmtype to match the ASM DLL's?\n"
		"This does ***NOT*** change the assembly source, just the asmtype stored in it.\n"
		"If successful, it eliminates the compatibility warnings, but the source might still be incompatible. "
		"This feature is mostly used bringing in projects from older assembly versions.",
		MB_OKCANCEL | MB_ICONSTOP | MB_DEFBUTTON2);
	if (result == IDOK) {
		auto coerced = asmdll_impl::CoerceAsmToDllCompatibility(*m_proj);
		if (!coerced) {
			AfxMessageBox(coerced.value);
		} else {
			// Test it again to make sure it worked
			auto iscompat = asmdll_impl::IsProjectDllCompatible(*m_proj);
			m_coerceasmbutton.EnableWindow(iscompat != true);
			CString msg = iscompat ? "Succeeded." : "Coercion apparently failed despite returning no initial errors: " + iscompat.value;
			AfxMessageBox(msg);
		}
	}
}
