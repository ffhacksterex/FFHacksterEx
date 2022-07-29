// DlgPickDestinationFile.cpp : implementation file
//

#include "stdafx.h"
#include "resource_utils.h"
#include "DlgPickDestinationFile.h"
#include "afxdialogex.h"
#include <path_functions.h>
#include "ui_helpers.h"
#include "ui_prompts.h"


// CDlgPickDestinationFile dialog

IMPLEMENT_DYNAMIC(CDlgPickDestinationFile, CDialogEx)

CDlgPickDestinationFile::CDlgPickDestinationFile(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PICK_DESTINATION_FILE, pParent)
{
}

CDlgPickDestinationFile::~CDlgPickDestinationFile()
{
}

void CDlgPickDestinationFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PARENTFOLDER, m_editParentFolder);
	DDX_Control(pDX, IDC_BROWSE_PARENTFOLDER, m_browsebutton);
	DDX_Control(pDX, IDC_STATIC_BLURB, m_blurb);
}


BEGIN_MESSAGE_MAP(CDlgPickDestinationFile, CDialogEx)
	ON_BN_CLICKED(IDC_BROWSE_PARENTFOLDER, &CDlgPickDestinationFile::OnBnClickedBrowseParentfolder)
END_MESSAGE_MAP()


// CDlgPickDestinationFile message handlers


BOOL CDlgPickDestinationFile::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_blurb.SetWindowText(Blurb);
	if (!Blurb.IsEmpty()) Ui::ResizeBlurb(this, &m_blurb, Blurb);
	if (!Title.IsEmpty()) SetWindowText(Title);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgPickDestinationFile::OnOK()
{
	CString file = Ui::GetControlText(m_editParentFolder).Trim();
	if (file.IsEmpty()) {
		AfxMessageBox("File path must not be empty or blanks.");
		m_editParentFolder.SetFocus();
		m_editParentFolder.SetSel(0, -1);
	}
	else {
		DestFile = file;
		CDialogEx::OnOK();
	}
}

void CDlgPickDestinationFile::OnBnClickedBrowseParentfolder()
{
	CString startfolder = StartInFolder;
	auto file = Ui::PromptToSaveProjectByFilter(this, DefaultExt, Filter);
	if (file) {
		m_editParentFolder.SetWindowText(file);
		m_editParentFolder.SetFocus();
	}
}
