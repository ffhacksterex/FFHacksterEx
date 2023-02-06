// DlgPromptProjectBackup.cpp : implementation file

#include "stdafx.h"
#include "DlgPromptProjectBackup.h"
#include "resource_utils.h"
#include "afxdialogex.h"
#include <AppSettings.h>
#include <path_functions.h>
#include <FFHacksterProject.h>
#include "ui_helpers.h"
#include "ui_prompts.h"
#include <chrono>
#include <sstream>
#include <iomanip>

// CDlgPromptProjectBackup dialog

IMPLEMENT_DYNAMIC(CDlgPromptProjectBackup, CDialogEx)

CDlgPromptProjectBackup::CDlgPromptProjectBackup(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROJECTUPGRADE, pParent)
{
}

CDlgPromptProjectBackup::~CDlgPromptProjectBackup()
{
}

void CDlgPromptProjectBackup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROJUPGRADE_STATTEMP, m_statictempfilename);
	DDX_Control(pDX, IDC_PROJUPGRADE_STATBROWSE, m_staticbrowsefilename);
	DDX_Control(pDX, IDC_PROJUPGRADE_EDITBROWSE, m_editbrowsefilename);
	DDX_Control(pDX, IDC_PROJUPGRADE_BTNBROWSE, m_buttonbrowsefilename);
	DDX_Control(pDX, IDC_PROJUPGRADE_RADNOUPGRADE, m_radionoupgrade);
}

BEGIN_MESSAGE_MAP(CDlgPromptProjectBackup, CDialogEx)
	ON_BN_CLICKED(IDC_PROJUPGRADE_RADNOUPGRADE, &CDlgPromptProjectBackup::OnBtnClickedProjUpgradeRadioNoUpgrade)
	ON_BN_CLICKED(IDC_PROJUPGRADE_RADUPGRADETEMP, &CDlgPromptProjectBackup::OnBnClickedProjupgradeRadupgradetemp)
	ON_BN_CLICKED(IDC_PROJUPGRADE_RADUPGRADEBROWSE, &CDlgPromptProjectBackup::OnBnClickedProjupgradeRadupgradebrowse)
	ON_BN_CLICKED(IDC_PROJUPGRADE_BTNBROWSE, &CDlgPromptProjectBackup::OnBnClickedProjupgradeBtnbrowse)
END_MESSAGE_MAP()


BOOL CDlgPromptProjectBackup::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (AppStgs == nullptr) {
		Ui::AbortFail(this, "App settings must be supplied to the project upgrade dialog.");
		return TRUE;
	}

	if (Paths::DirExists(StartInFolder)) {
		Ui::SetEditValue(m_editbrowsefilename, StartInFolder);
	}
	Ui::SetStaticText(m_statictempfilename, Paths::GetTempFolder());

	m_radionoupgrade.SetCheck(BST_CHECKED);
	OnBtnClickedProjUpgradeRadioNoUpgrade();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

namespace {
	std::string GetLocalTimeString(time_t t)
	{
		auto tm = *std::localtime(&t);
		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
		auto timestr = oss.str();
		return timestr;
	}
}

void CDlgPromptProjectBackup::OnOK()
{
	CString backfolder;
	switch (m_mode) {
	case BackupMode::None:
	{
		auto result = AfxMessageBox("Are you sure you want to proceed without a backup?",
			MB_YESNO | MB_DEFBUTTON2 | MB_ICONSTOP);
		if (result != IDYES) return;
		break;
	}
	case BackupMode::Temp:
		backfolder = Paths::GetTempFolder();
		break;
	case BackupMode::Browse:
	{
		backfolder = Ui::GetControlText(m_editbrowsefilename);
		if (backfolder.IsEmpty()) {
			AfxMessageBox("The selected backup folder cannopt be blank.");
			return;
		}
		if (!Paths::DirExists(backfolder))
		{
			AfxMessageBox("The selected backoup folder must exist.");
			return;
		}
		break;
	}
	default:
	{
		CString cs;
		cs.Format("Can't proceed with unexpected backup mode '%d'.", (int)m_mode);
		AfxMessageBox(cs);
		return;
	}
	}

	if (!backfolder.IsEmpty()) {
		std::string timestr = GetLocalTimeString(std::time(nullptr));
		auto folder = Paths::GetDirectoryPath(ProjectFilePath);
		auto stemext = Paths::GetFileName(ProjectFilePath);
		CString filename;
		filename.Format("%s.v%d@%s.ff1zip", stemext,
			CFFHacksterProject::ReadProjectVersion(ProjectFilePath), timestr.c_str());
		BackupFilePath = Paths::Combine({ backfolder, filename });
	}

	CDialogEx::OnOK();
}


// Implementation

void CDlgPromptProjectBackup::EnableRadioBrowseControls(bool enable)
{
	bool bEn = enable == true;
	m_buttonbrowsefilename.EnableWindow(bEn);
	m_editbrowsefilename.EnableWindow(bEn);
}


// CDlgPromptProjectBackup message handlers

void CDlgPromptProjectBackup::OnBtnClickedProjUpgradeRadioNoUpgrade()
{
	EnableRadioBrowseControls(false);
	m_mode = BackupMode::None;
}

void CDlgPromptProjectBackup::OnBnClickedProjupgradeRadupgradetemp()
{
	EnableRadioBrowseControls(false);
	m_mode = BackupMode::Temp;
}

void CDlgPromptProjectBackup::OnBnClickedProjupgradeRadupgradebrowse()
{
	EnableRadioBrowseControls(true);
	m_mode = BackupMode::Browse;
}

void CDlgPromptProjectBackup::OnBnClickedProjupgradeBtnbrowse()
{
	CString pathname;
	CString startfolder = FOLDERPREF_OR(AppStgs, PrefProjectParentFolder, Paths::GetDirectoryPath(pathname));
	auto folder = Ui::PromptForFolder(this, "Select parent folder", startfolder);
	if (folder) {
		if (Paths::DirExists(folder.value)) {
			m_editbrowsefilename.SetWindowText(folder.value);
			m_editbrowsefilename.SetFocus();
		}
		else {
			AfxMessageBox("Please select an existing folder.");
		}
	}
}
