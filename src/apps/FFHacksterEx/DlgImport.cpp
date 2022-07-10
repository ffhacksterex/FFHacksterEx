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
	DDX_Control(pDX, IDC_BUTTON2, m_projectbrowsebtn);
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
	m_projectbrowsebtn.EnableWindow(checked);
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

	// Import the DAT file first if one was specified.
	CString errmsg;
	bool datchecked = m_datcheck.GetCheck() == BST_CHECKED;
	if (datchecked) {
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

	// Process project options if any were selected AND no errors occurred yet.
	if (errmsg.IsEmpty() &&
		(m_valuescheck.GetCheck() == BST_CHECKED ||
		m_labelscheck.GetCheck() == BST_CHECKED ||
		m_dialoguecheck.GetCheck() == BST_CHECKED))
	{
		auto projfile = GetControlText(m_projectedit);
		if (!Paths::FileExists(projfile)) {
			if (datchecked && errmsg.IsEmpty()) errmsg = "DAT import succeeded.\nHowever, ";
			errmsg.Append("Project import failed because the specified "
				"Project file could not be located.");
		}
		else if (projfile == Project->ProjectPath) {
			errmsg.Append("Please select a project file that's different from the current project.");
			m_projectbrowsebtn.SetFocus();
		}
		else {
			std::vector<std::tuple<CButton*, CString, CString>> projectsteps{
				{ &m_valuescheck, FFHFILE_ValuesPath, Project->ValuesPath},
				{ &m_labelscheck, FFHFILE_StringsPath, Project->StringsPath},
				{ &m_dialoguecheck, FFHFILE_DialoguePath, Project->DialoguePath}
			};
			for (const auto& step : projectsteps) {
				if (!errmsg.IsEmpty())
					break; // bail on the first error

				auto& checkbox = *std::get<0>(step);
				if (checkbox.GetCheck() == BST_CHECKED) {
					const auto& stepname = std::get<1>(step);
					const auto& destini = std::get<2>(step);
					auto srcini = CFFHacksterProject::GetIniFilePath(projfile, stepname);
					if (Paths::FileExists(srcini)) {
						if (!Paths::FileCopy(srcini, destini)) {
							errmsg.AppendFormat("Couldn't import the '%s' file tied to the the project '%s'.\n",
								(LPCSTR)stepname, (LPCSTR)projfile);
						}
					}
					else {
						errmsg.AppendFormat("Couldn't find the '%s' file tied to the the project '%s'.\n",
							(LPCSTR)stepname, (LPCSTR)projfile);
					}
				}
			}
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
		m_datedit.SetFocus();
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
		m_projectedit.SetFocus();
	}
	else if (!result.value.IsEmpty()) {
		AfxMessageBox(result.value, MB_ICONERROR);
	}
}