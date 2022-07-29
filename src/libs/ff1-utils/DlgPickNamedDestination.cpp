// DlgPickNamedDestination.cpp : implementation file
//

#include "stdafx.h"
#include "resource_utils.h"
#include "DlgPickNamedDestination.h"
#include "afxdialogex.h"
#include <path_functions.h>
#include "ui_helpers.h"
#include "ui_prompts.h"


// CDlgPickNamedDestination dialog

IMPLEMENT_DYNAMIC(CDlgPickNamedDestination, CDialogEx)

CDlgPickNamedDestination::CDlgPickNamedDestination(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PICK_NAMED_DESTINATION, pParent)
{
}

CDlgPickNamedDestination::~CDlgPickNamedDestination()
{
}

void CDlgPickNamedDestination::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PARENTFOLDER, m_editParentFolder);
	DDX_Control(pDX, IDC_BROWSE_PARENTFOLDER, m_browsebutton);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_STATIC_BLURB, m_blurb);
}

BEGIN_MESSAGE_MAP(CDlgPickNamedDestination, CDialogEx)
	ON_BN_CLICKED(IDC_BROWSE_PARENTFOLDER, &CDlgPickNamedDestination::OnBnClickedBrowseParentfolder)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgPickNamedDestination message handlers

namespace {
	void ResizeBlurb(CDialogEx* me, CWnd* label, CString text)
	{
		// Yeah, Windows API isn't so great for UI stuff...
		// the raw API does none of this automatically, and MFC
		// only handles dynamic layout for the simplest scenarios.
		// I've defintely got to get FFHEx off of MFC for sure.

		// Use the dialog's PaintDC to calculate a rect
		// for the text at the top of teh dialog with some
		// padding on the left, top, and right.
		constexpr int pad = 16;
		CRect client = Ui::GetClientRect(me);
		client.InflateRect(0, 0, -pad * 2, 0);
		CPaintDC dc(me);
		dc.DrawText(text, client, DT_CALCRECT);

		// Now move the label into the calculated rect.
		client.OffsetRect(pad, pad);
		label->MoveWindow(client);

		// Move all of the controls down except the blurb label.
		auto diff = (client.bottom + 4);
		auto pwnd = me->GetWindow(GW_CHILD);
		while (pwnd != nullptr) {
			if (pwnd != label) Ui::MoveControlBy(pwnd, 0, diff);
			pwnd = pwnd->GetNextWindow();
		}

		// Grow the dialog (by passing a negative shrink value).
		Ui::ShrinkWindow(me, 0, -diff);
	}
}

BOOL CDlgPickNamedDestination::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_blurb.SetWindowText(Blurb);
	if (!Blurb.IsEmpty()) ResizeBlurb(this, &m_blurb, Blurb);
	if (!Title.IsEmpty()) SetWindowText(Title);
	if (!SourceFolderName.IsEmpty()) m_editName.SetWindowText(SourceFolderName);
	if (PromptImmediately) SetTimer(1, 100, nullptr);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPickNamedDestination::OnOK()
{
	CString parentfolder = Ui::GetControlText(m_editParentFolder);
	CString foldername = Ui::GetControlText(m_editName).Trim();
	if (!Paths::DirExists(parentfolder)) {
		AfxMessageBox("Please select an existing folder as the parent folder.");
		m_editParentFolder.SetFocus();
		m_editParentFolder.SetSel(0, -1);
	}
	else if (foldername.IsEmpty()) {
		AfxMessageBox("Folder name must not be empty or blanks.");
		m_editName.SetFocus();
		m_editName.SetSel(0, -1);
	} else {
		DestParentFolder = parentfolder;
		DestFolderName = foldername;
		DestFolderPath = Paths::Combine({ DestParentFolder, DestFolderName });
		CDialogEx::OnOK();
	}
}

void CDlgPickNamedDestination::OnBnClickedBrowseParentfolder()
{
	CString startfolder = StartInFolder;
	auto folder = Ui::PromptForFolder(this, "Select parent folder", startfolder);
	if (folder) {
		if (Paths::DirExists(folder.value)) {
			m_editParentFolder.SetWindowText(folder.value);
			m_editParentFolder.SetFocus();
		}
		else {
			AfxMessageBox("Please select an existing folder.");
		}
	}
}

void CDlgPickNamedDestination::OnTimer(UINT_PTR nIDEvent)
{
	// This is a single use timer, so get rid of it immediately.
	CDialogEx::OnTimer(nIDEvent);
	KillTimer(nIDEvent);
	OnBnClickedBrowseParentfolder();
}
