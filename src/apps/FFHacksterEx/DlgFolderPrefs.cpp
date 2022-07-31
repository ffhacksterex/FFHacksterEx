// DlgFolderPrefs.cpp : implementation file
//

#include "stdafx.h"
#include "DlgFolderPrefs.h"
#include "resource.h"
#include "afxdialogex.h"
#include <FFHPropertyGridFolderProperty.h>
#include <ui_helpers.h>

#define ADD_PROP(name) m_propgridfolders.AddProperty(new CFFHPropertyGridFolderProperty(#name,Prefs.Pref##name), FALSE)
#define SAVE_PROP(name,index) Prefs.Pref##name = m_propgridfolders.GetProperty((index))->GetValue()

// CDlgFolderPrefs dialog

IMPLEMENT_DYNAMIC(CDlgFolderPrefs, CDialogEx)

CDlgFolderPrefs::CDlgFolderPrefs(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FOLDER_PREFS, pParent)
{
}

CDlgFolderPrefs::~CDlgFolderPrefs()
{
}

void CDlgFolderPrefs::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPGRID_FOLDERS, m_propgridfolders);
}


BEGIN_MESSAGE_MAP(CDlgFolderPrefs, CDialogEx)
END_MESSAGE_MAP()


// CDlgFolderPrefs message handlers

BOOL CDlgFolderPrefs::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_propgridfolders.SetRedraw(FALSE);
	m_propgridfolders.EnableHeaderCtrl(TRUE, _T("Folder Type"), _T("Default Path"));

	auto client = Ui::GetClientRect(&m_propgridfolders);
	HDITEM col = { 0 };
	col.mask = HDI_WIDTH;
	col.cxy = client.Width() / 3;
	m_propgridfolders.GetHeaderCtrl().SetItem(0, &col);
	m_propgridfolders.AdjustLayout();

	ADD_PROP(AdditionalModulesFolder);
	ADD_PROP(ArchiveFolder);
	ADD_PROP(AsmDllFolder);
	ADD_PROP(CleanFolder);
	ADD_PROP(ImageImportExportFolder);
	ADD_PROP(MapImportExportFolder);
	ADD_PROP(ProjectParentFolder);
	ADD_PROP(PublishFolder);
	m_propgridfolders.SetRedraw(TRUE);
	m_propgridfolders.InvalidateRect(nullptr);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFolderPrefs::OnOK()
{
	int index = -1;
	SAVE_PROP(AdditionalModulesFolder, ++index);
	SAVE_PROP(ArchiveFolder, ++index);
	SAVE_PROP(AsmDllFolder, ++index);
	SAVE_PROP(CleanFolder, ++index);
	SAVE_PROP(ImageImportExportFolder, ++index);
	SAVE_PROP(MapImportExportFolder, ++index);
	SAVE_PROP(ProjectParentFolder, ++index);
	SAVE_PROP(PublishFolder, ++index);
	CDialogEx::OnOK();
}
