// DlgCreateNewProject.cpp : implementation file
//

#include "stdafx.h"
#include "DlgCreateNewProject.h"
#include <asmdll_impl.h>
#include <general_functions.h>
#include <io_functions.h>
#include <path_functions.h>
#include <ui_helpers.h>
#include <ui_prompts.h>
#include <window_messages.h>

using namespace Ui;

// CDlgCreateNewProject dialog
#define BaseClass CDialogWithBackground


IMPLEMENT_DYNAMIC(CDlgCreateNewProject, BaseClass)

CDlgCreateNewProject::CDlgCreateNewProject(CWnd* pParent /*= nullptr */)
	: BaseClass(IDD_DLG_NEWASM, pParent)
	, LockDefinedInputs(false)
	, ShowBackgroundArt(false)
	, ForceNewLabels(false)
{
}

CDlgCreateNewProject::~CDlgCreateNewProject()
{
}

void CDlgCreateNewProject::HideAssemblySpecificControls()
{
	// Get the difference in hiehght between the asmdll static and the forcenewlabels check box
	auto * pwnd = m_asmdlledit.GetWindow(GW_HWNDNEXT);
	auto rcforce = GetControlRect(pwnd);
	auto rcasm = GetControlRect(&m_asmdllstatic);
	auto diff = rcforce.top - rcasm.top;

	// Now hide the asm controls and slide the ones starting with the asmstatic up
	m_asmdllstatic.ShowWindow(SW_HIDE);
	m_asmdllbrowsebutton.ShowWindow(SW_HIDE);
	m_asmdllusedefaultcheck.ShowWindow(SW_HIDE);
	m_asmdlledit.ShowWindow(SW_HIDE);

	while (pwnd != nullptr) {
		if (pwnd->GetDlgCtrlID() != IDHELPBOOK)
			MoveControlBy(pwnd, 0, -diff);
		pwnd = pwnd->GetWindow(GW_HWNDNEXT);
	}

	ShrinkWindow(this, 0, diff);
}

UINT CDlgCreateNewProject::GetHelpID() const
{
	// This dialog defaults to assembly mode, but for the sake of the Help dialog, return an
	// alternate value if this is a ROM mode.
	return m_projtype == ProjType::assembly ? CFFBaseDlg::GetHelpID() : IDD_DLG_NEWROM;
}

void CDlgCreateNewProject::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NEWASM_EDIT_PARENTFOLDER, m_parentfolderedit);
	DDX_Control(pDX, IDC_NEWASM_EDIT_PROJECTNAME, m_projectnameedit);
	DDX_Control(pDX, IDC_NEWASM_EDIT_ZIPARCHIVE, m_revertfileedit);
	DDX_Control(pDX, IDC_NEWASM_EDIT_PUBLISH, m_publishedit);
	DDX_Control(pDX, IDC_NEWASM_EDIT_DATFILE, m_datfileedit);
	DDX_Control(pDX, IDC_NEWASM_BUTTON_IMPORTDAT, m_datfilebutton);
	DDX_Control(pDX, IDC_NEWASM_STATIC_ZIPARCHIVE, m_revertfilestatic);
	DDX_Control(pDX, IDC_NEWASM_CHECK_IMPORTDAT, m_datfilecheck);
	DDX_Control(pDX, IDC_CHECK1, m_forcenewlabelscheck);
	DDX_Control(pDX, IDC_NEWASM_EDIT_ASMDLL, m_asmdlledit);
	DDX_Control(pDX, IDC_STATIC1, m_asmdllstatic);
	DDX_Control(pDX, IDC_BUTTON1, m_asmdllbrowsebutton);
	DDX_Control(pDX, IDC_CHECK2, m_asmdllusedefaultcheck);
	DDX_Control(pDX, IDC_BUTTON2, m_addlmodulebutton);
	DDX_Control(pDX, IDC_EDIT1, m_addlmodulesedit);
	DDX_Control(pDX, IDC_NEWASM_BUTTON_PUBLISH, m_publishbutton);
	DDX_Control(pDX, IDC_NEWASM_BUTTON_ZIPARCHIVE, m_revertbutton);
	DDX_Control(pDX, IDC_NEWASM_BUTTON_PARENTFOLDER, m_parentfolderbutton);
	DDX_Control(pDX, IDC_STATIC2, m_publishstatic);
	DDX_Control(pDX, IDC_STATIC3, m_addlmodulesstatic);
	DDX_Control(pDX, IDC_STATIC4, m_projectnamestatic);
	DDX_Control(pDX, IDC_STATIC5, m_parentfolderstatic);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}


BEGIN_MESSAGE_MAP(CDlgCreateNewProject, BaseClass)
	ON_BN_CLICKED(IDC_NEWASM_CHECK_IMPORTDAT, &CDlgCreateNewProject::OnBnClickedNewasmCheckImportdat)
	ON_BN_CLICKED(IDC_NEWASM_BUTTON_PARENTFOLDER, &CDlgCreateNewProject::OnBnClickedNewasmButtonParentfolder)
	ON_BN_CLICKED(IDC_NEWASM_BUTTON_ZIPARCHIVE, &CDlgCreateNewProject::OnBnClickedNewasmButtonRevert)
	ON_BN_CLICKED(IDC_NEWASM_BUTTON_PUBLISH, &CDlgCreateNewProject::OnBnClickedNewasmButtonPublish)
	ON_BN_CLICKED(IDC_NEWASM_BUTTON_IMPORTDAT, &CDlgCreateNewProject::OnBnClickedNewasmButtonImportdat)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgCreateNewProject::OnBnClickedAsmDllLookInAppFolder)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgCreateNewProject::OnBnClickedAdditionalModulesFolder)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgCreateNewProject::OnBnClickedButton1BrowseAsmDll)
END_MESSAGE_MAP()


// CDlgCreateNewProject message handlers

BOOL CDlgCreateNewProject::OnInitDialog()
{
	BaseClass::OnInitDialog();

	CString reverttext;
	CString caption;
	if (ProjectType == "rom" || ProjectType == "editrom") {
		SetWindowText("New ROM Project");
		reverttext = "Prototype ROM (also used to revert the ROM to an initial clean state)";
		caption = (ProjectType == "editrom") ? "Edit ROM" : "New ROM Project";
		ProjectType = "rom";
		m_projtype = rom;
		HideAssemblySpecificControls();
	}
	else if (ProjectType == "asm") {
		SetWindowText("New Assembly Project");
		reverttext = "Prototype assembly archive (also used to revert the source to an initial clean state)";
		caption = "New Assembly Project";
		m_projtype = assembly;
	}
	else {
		m_projtype = none;
		AfxMessageBox("Unrecognized project type '" + ProjectType + "' cannot be created.", MB_ICONERROR);
		EndDialog(IDABORT);
		return TRUE;
	}

	SetCheckValue(m_forcenewlabelscheck, ForceNewLabels);

	m_revertfilestatic.SetWindowText(reverttext);
	if (Paths::FileExists(RevertPath)) 
		m_revertfileedit.SetWindowText(RevertPath);

	m_asmdllusedefaultcheck.SetCheck(BST_CHECKED);
	OnBnClickedAsmDllLookInAppFolder();
	
	if (LockDefinedInputs) {
		if (!ParentFolder.IsEmpty()) {
			m_parentfolderstatic.EnableWindow(FALSE);
			m_parentfolderbutton.EnableWindow(FALSE);
			m_parentfolderedit.EnableWindow(FALSE);
			m_parentfolderedit.SetWindowText(ParentFolder);
		}
		if (!ProjectName.IsEmpty()) {
			m_projectnamestatic.EnableWindow(FALSE);
			m_projectnameedit.EnableWindow(FALSE);
			m_projectnameedit.SetWindowText(ProjectName);
		}
		if (!RevertPath.IsEmpty()) {
			m_revertfilestatic.EnableWindow(FALSE);
			m_revertbutton.EnableWindow(FALSE);
			m_revertfileedit.EnableWindow(FALSE);
			m_revertfileedit.SetWindowText(RevertPath);
		}
		if (!PublishPath.IsEmpty()) {
			m_publishstatic.EnableWindow(FALSE);
			m_publishbutton.EnableWindow(FALSE);
			m_publishedit.EnableWindow(FALSE);
			m_publishedit.SetWindowText(PublishPath);
		}
	}

	CString title;
	m_banner.Set(this, COLOR_BLACK, COLOR_FFBLUE, caption);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCreateNewProject::OnOK()
{
	const auto ShowMessage = [](CString message, CEdit & edit) {
		if (!message.IsEmpty()) AfxMessageBox(message, MB_ICONERROR);
		edit.SetFocus();
		edit.SetSel(0, -1);
	};
	const auto checkcontrol = [ShowMessage](CEdit & wnd, CString & value, CString message = "") {
		wnd.GetWindowText(value);
		if (!value.IsEmpty()) return true;
		ShowMessage(message, wnd);
		return false;
	};

	// Required
	if (!checkcontrol(m_parentfolderedit, ParentFolder, "A parent folder is required.")) return;
	if (!checkcontrol(m_projectnameedit, ProjectName, "A project must have a name.")) return;

	auto newprojfolder = Paths::Combine({ ParentFolder, ProjectName });
	if (Paths::DirExists(newprojfolder)) {
		ShowMessage("To avoid accidental overwrites,\nyou cannot create a project in an existing project folder\n'"
			+ newprojfolder + "'", m_projectnameedit);
		return;
	}

	if (!checkcontrol(m_revertfileedit, RevertPath, "A project must specify a prototype file.")) return;
	if (ProjectType == "asm") {
		CWnd* failcontrol = nullptr;
		CString asmtestpath;
		if (Ui::GetCheckValue(m_asmdllusedefaultcheck)) {
			CString asmdlldefault;
			asmdlldefault.LoadString(IDS_ASMDLL_DEFAULT);
			asmtestpath = Paths::Combine({ Paths::GetProgramFolder(), asmdlldefault });
			failcontrol = &m_asmdllusedefaultcheck;
		}
		else {
			asmtestpath = Ui::GetControlText(m_asmdlledit);
			failcontrol = &m_asmdlledit;
		}

		CString errormsg;
		if (!Paths::FileExists(asmtestpath))
			errormsg.Format("Cannot find the assembly DLL file %s", (LPCSTR)asmtestpath);
		else if (!asmdll_impl::is_ff1_asmdll(asmtestpath))
			errormsg.Format("The specified file\n(%s)\nis not recognized as an FF1 assembly DLL.", (LPCSTR)asmtestpath);
		else {
			auto result = asmdll_impl::IsZipDllCompatible(RevertPath, asmtestpath);
			if (!result) {
				if (AppStgs->EnforceAssemblyDllCompatibility)
					errormsg.Format("The specified assembly DLL and clean disassembly ZIP are not compatible:\n%s.", (LPCSTR)result.value);
				else if (AppStgs->WarnAssemblyDllCompatibility)
					AfxMessageBox("Warning:\n" + result.value + "\n\n"
						"You can suppress this warning either app-wide (App Settings)\nor per-project (Project Settings).");
			}
		}

		if (!errormsg.IsEmpty()) {
			AfxMessageBox(errormsg);
			failcontrol->SetFocus();
			return;
		}

		AsmDllPath = asmtestpath;
	}

	ForceNewLabels = Ui::GetCheckValue(m_forcenewlabelscheck);

	// Optional
	m_publishedit.GetWindowTextA(PublishPath);
	if (m_datfilecheck.GetCheck() == BST_CHECKED)
		m_datfileedit.GetWindowText(DATPath);
	else
		DATPath = "";

	// If specified, the folder has to exist
	CString addlmodfolder;
	m_addlmodulesedit.GetWindowText(addlmodfolder);
	if (!addlmodfolder.IsEmpty() && !Paths::DirExists(addlmodfolder)) {
		ShowMessage("If specified, the addtional modules folder must exist.", m_addlmodulesedit);
		return;
	}

	AdditionalModulesFolder = addlmodfolder;

	BaseClass::OnOK();
}

void CDlgCreateNewProject::OnBnClickedNewasmCheckImportdat()
{
	BOOL importing = m_datfilecheck.GetCheck() == BST_CHECKED;
	m_datfileedit.EnableWindow(importing);
	m_datfilebutton.EnableWindow(importing);
}

void CDlgCreateNewProject::OnBnClickedNewasmButtonParentfolder()
{
	//FUTURE - implement favorite or frequent ROM/ASM project parent folders
	auto result = Ui::PromptForFolder(this, "Select parent folder for the new project.", AppStgs->PrefProjectParentFolder);
	if (result)
		Ui::SetEditTextAndFocus(m_parentfolderedit, result.value);
}

void CDlgCreateNewProject::OnBnClickedNewasmButtonRevert()
{
	CString filter, title;
	switch (m_projtype) {
	case rom:
		filter = "FF1 ROM (*.nes)|*.nes|All files (*.*)|*.*||";
		title = "Select a clean FF1 file";
		break;
	case assembly:
		filter = "Zipped Disassembly (*.zip)|*.zip|All files (*.*)|*.*||";
		title = "Select a zipped FF1 Disassembly archive";
		break;
	default:
		AfxMessageBox("Project type " + ProjectType + " doesn't support reversion.", MB_ICONERROR);
		return;
	}

	auto result = OpenFilePrompt(this, filter, title, AppStgs->PrefCleanFolder);
	if (result)
		Ui::SetEditTextAndFocus(m_revertfileedit, result.value);
}

void CDlgCreateNewProject::OnBnClickedNewasmButtonPublish()
{
	auto curfile = Ui::GetControlText(m_publishedit);
	auto result = SaveFilePrompt(this, "FF1 ROM (*.nes)|*.nes||", "Select Destination ROM location",
		Paths::DirExists(Paths::GetDirectoryPath(curfile)) ? curfile : AppStgs->PrefPublishFolder);
	if (result)
		Ui::SetEditTextAndFocus(m_publishedit, result.value);
}

void CDlgCreateNewProject::OnBnClickedNewasmButtonImportdat()
{
	auto result = OpenFilePrompt(this, "FFHackster DAT files (FFHackster*.dat)|FFHackster*.dat||",
		"Import a FFHackster DAT file", AppStgs->PrefCleanFolder);
	if (result)
		Ui::SetEditTextAndFocus(m_datfileedit, result.value);
}

void CDlgCreateNewProject::OnBnClickedAsmDllLookInAppFolder()
{
	BOOL enabled = Ui::GetCheckValue(m_asmdllusedefaultcheck) == false;
	m_asmdllbrowsebutton.EnableWindow(enabled);
	m_asmdlledit.EnableWindow(enabled);
}

void CDlgCreateNewProject::OnBnClickedAdditionalModulesFolder()
{
	auto result = Ui::PromptForFolder(this, "Select additonal modules folder for the new project.",
		AppStgs->PrefAdditionalModulesFolder);
	if (result)
		Ui::SetEditTextAndFocus(m_addlmodulesedit, result.value);
}

void CDlgCreateNewProject::OnBnClickedButton1BrowseAsmDll()
{
	auto result = PromptForAsmDll(this, AppStgs->PrefAsmDllFolder);
	if (result)
		Ui::SetEditTextAndFocus(m_asmdlledit, result.value);
	else if (!result.value.IsEmpty())
		AfxMessageBox(result.value);
}