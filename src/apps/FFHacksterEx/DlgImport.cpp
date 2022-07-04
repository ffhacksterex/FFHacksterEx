// DlgImport.cpp : implementation file
//

#include "stdafx.h"
#include "DlgImport.h"
#include "afxdialogex.h"
#include "FFHacksterProject.h"
#include "general_functions.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "ui_helpers.h"
#include "ui_prompts.h"

#include <ProjectLoader.h>

using namespace Ini;
using namespace Ui;

// CDlgImport dialog

IMPLEMENT_DYNAMIC(CDlgImport, CDialogEx)

CDlgImport::CDlgImport(CWnd* pParent /*= nullptr */)
	: CDialogEx(IDD_IMPORT, pParent)
{
}

CDlgImport::~CDlgImport()
{
}

BOOL CDlgImport::IsProjectChecked()
{
	auto checked = Ui::GetCheckValue(m_valuescheck)
		|| Ui::GetCheckValue(m_labelscheck)
		|| Ui::GetCheckValue(m_dialoguecheck);
	return checked ? TRUE : FALSE;
}

void CDlgImport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_datcheck);
	DDX_Control(pDX, IDC_BUTTON1, m_datbutton);
	DDX_Control(pDX, IDC_EDIT1, m_datedit);
	DDX_Control(pDX, IDC_CHECK2, m_valuescheck);
	DDX_Control(pDX, IDC_BUTTON2, m_projectcheck);
	DDX_Control(pDX, IDC_EDIT2, m_projectedit);
	DDX_Control(pDX, IDC_CHECK3, m_dialoguecheck);
	DDX_Control(pDX, IDC_CHECK4, m_labelscheck);
}


BEGIN_MESSAGE_MAP(CDlgImport, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgImport::OnBnClickedCheckDat)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgImport::OnBnClickedButtonDat)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgImport::OnBnClickedButtonProject)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgImport::OnBnClickedProjectCheckBoxes)
	ON_BN_CLICKED(IDC_CHECK3, &CDlgImport::OnBnClickedProjectCheckBoxes)
	ON_BN_CLICKED(IDC_CHECK4, &CDlgImport::OnBnClickedProjectCheckBoxes)
END_MESSAGE_MAP()


// CDlgImport message handlers

void CDlgImport::OnBnClickedCheckDat()
{
	BOOL checked = m_datcheck.GetCheck() == BST_CHECKED;
	m_datbutton.EnableWindow(checked);
	m_datedit.EnableWindow(checked);
}

void CDlgImport::OnBnClickedProjectCheckBoxes()
{
	auto checked = IsProjectChecked();
	m_projectcheck.EnableWindow(checked);
	m_projectedit.EnableWindow(checked);
}

BOOL CDlgImport::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (Project == nullptr)
		return AbortInitDialog(this, "A project must be specified before anything can be imported into it.");

	OnBnClickedCheckDat();
	OnBnClickedProjectCheckBoxes();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImport::OnOK()
{
	CWaitCursor wait;
	CString errmsg;
	if (m_datcheck.GetCheck() == BST_CHECKED) {
		auto datfile = GetControlText(m_datedit);
		if (Paths::FileExists(datfile)) {
			Project->ImportHacksterDAT(datfile);
			if (!SaveProject(*Project)) {
				errmsg.AppendFormat("The DAT file was imported, but it could not be saved: the changes will be lost.\n", (LPCSTR)datfile);
			}
		}
		else {
			errmsg.AppendFormat("Can't find the DAT file '%s'.\n", (LPCSTR)datfile);
		}
	}

	// If a previous step failed, don't allow any step to actually process an action.
	// However, do allow other steps to check for an error and append a message if needed.
	auto projfile = GetControlText(m_projectedit);
	if (!Paths::FileExists(projfile)) {
		AfxMessageBox("Can't find the specified project file for label/value/dialogue import.", MB_ICONERROR);
		return;
	}

	//REFACTOR - this can be tightened into a loop.
	if (m_valuescheck.GetCheck() == BST_CHECKED) {
		auto inifile = CFFHacksterProject::GetIniFilePath(projfile, FFHFILE_ValuesPath);
		if (Paths::FileExists(inifile)) {
			if (errmsg.IsEmpty()) {
				if (!Paths::FileCopy(inifile, Project->ValuesPath)) {
					errmsg.AppendFormat("Couldn't import the values file tied to the the project '%s'.\n", (LPCSTR)projfile);
				}
			}
		}
		else {
			errmsg.AppendFormat("Couldn't find the values file tied to the the project '%s'.\n", (LPCSTR)projfile);
		}
	}

	if (m_labelscheck.GetCheck() == BST_CHECKED) {
		auto inifile = CFFHacksterProject::GetIniFilePath(projfile, FFHFILE_StringsPath);
		if (Paths::FileExists(inifile)) {
			if (errmsg.IsEmpty()) {
				if (!Paths::FileCopy(inifile, Project->StringsPath)) {
					errmsg.AppendFormat("Couldn't import the labels file tied to the the project '%s'.\n", (LPCSTR)projfile);
				}
			}
		}
		else {
			errmsg.AppendFormat("Couldn't find the labels file tied to the the project '%s'.\n", (LPCSTR)projfile);
		}
	}

	if (m_dialoguecheck.GetCheck() == BST_CHECKED) {
		auto inifile = CFFHacksterProject::GetIniFilePath(projfile, FFHFILE_DialoguePath);
		if (Paths::FileExists(inifile)) {
			if (errmsg.IsEmpty()) {
				if (!Paths::FileCopy(inifile, Project->DialoguePath)) {
					errmsg.AppendFormat("Couldn't import the dialogue file tied to the the project '%s'.\n", (LPCSTR)projfile);
				}
			}
		}
		else {
			errmsg.AppendFormat("Couldn't find the dialogue file tied to the the project '%s'.\n", (LPCSTR)projfile);
		}
	}

	if (!errmsg.IsEmpty()) {
		AfxMessageBox(errmsg, MB_ICONERROR);
		return;
	}

	CDialogEx::OnOK();
}

void CDlgImport::OnBnClickedButtonDat()
{
	auto result = OpenFilePrompt(this, "FFHackster DAT files (FFHackster*.dat)|FFHackster*.dat|All files (*.*)|*.*||",
		"Open an FFHackster Data file");
	if (result) {
		m_datedit.SetWindowText(result.value);
	}
	else if (!result.value.IsEmpty()) {
		AfxMessageBox(result.value, MB_ICONERROR);
	}
}

void CDlgImport::OnBnClickedButtonProject()
{
	auto result = BrowseForProject(this, "Open Project containing Values file");
	if (result) {
		m_projectedit.SetWindowText(result.value);
	}
	else if (!result.value.IsEmpty()) {
		AfxMessageBox(result.value, MB_ICONERROR);
	}
}