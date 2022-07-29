// DlgPasteTargets.cpp : implementation file
//

#include "stdafx.h"
#include "DlgPasteTargets.h"
#include "afxdialogex.h"
#include "imaging_helpers.h"
#include "ui_helpers.h"

using namespace Imaging;
using namespace Ui;

// CDlgPasteTargets dialog

IMPLEMENT_DYNAMIC(CDlgPasteTargets, CFFBaseDlg)

CDlgPasteTargets::CDlgPasteTargets(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(IDD_PASTETARGETS, pParent)
{
}

CDlgPasteTargets::~CDlgPasteTargets()
{
}

void CDlgPasteTargets::SetTargets(const std::vector<PasteTarget>& targets)
{
	m_targets = targets;
}

bool CDlgPasteTargets::IsChecked(int index) const
{
	return index < (int)m_checked.size() ? m_checked[index] : false;
}

std::vector<CButton*> CDlgPasteTargets::GetChecks()
{
	return m_pastechecks;
}

void CDlgPasteTargets::DestroyChecks()
{
	for (auto * pwnd : m_pastechecks) {
		//m_tooltips.DelTool(pwnd); //DEVNOTE - not sure if this is needed
		delete pwnd;
	}
	m_pastechecks.clear();

	for (auto * pwnd : m_infostatics) {
		//m_tooltips.DelTool(pwnd); //DEVNOTE - not sure if this is needed
		delete pwnd;
	}
	m_infostatics.clear();
}

void CDlgPasteTargets::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_pastetarget1);
	DDX_Control(pDX, IDC_CHECK_ALL, m_checkall);
}


BEGIN_MESSAGE_MAP(CDlgPasteTargets, CFFBaseDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_ALL, &CDlgPasteTargets::OnBnClickedCheckAll)
END_MESSAGE_MAP()


// CDlgPasteTargets message handlers


BOOL CDlgPasteTargets::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();

	if (m_targets.empty())
		return AbortInitDialog(this, "At least one target name must be specified for targeted pastes.");

	const CString hovernote = "(hover for warning)";
	CPaintDC dc(this);
	int ctrlid = 5000;

	auto dcstate = dc.SaveDC();
	dc.SelectObject(GetFont());

	auto CreateCheck = [&,this](CRect & rect, const PasteTarget & target) {
		auto * check = new CButton;
		auto haswarning = !target.warning.IsEmpty();
		CString fulltext;
		fulltext.Format("%s%s%s%s", (LPCSTR)target.caption,
			(LPCSTR)(haswarning ? " (" : ""),
			(LPCSTR)(haswarning ? target.warning : ""),
			(LPCSTR)(haswarning ? ")" : ""));

		dc.DrawText(fulltext + " Wy", &rect, DT_CALCRECT);

		check->Create(fulltext, m_pastetarget1.GetStyle() | WS_VISIBLE, rect, this, ctrlid++);
		check->SetFont(GetFont());
		check->SetCheck(target.checked ? BST_CHECKED : BST_UNCHECKED);
		return check;
	};
	auto CreateStatic = [&, this](const CRect & thisrect) {
		auto * stat = new CStatic;
		CRect rcstat = thisrect;
		rcstat.OffsetRect(thisrect.Width(), 0);
		dc.DrawText(hovernote, &rcstat, DT_CALCRECT);
		stat->Create(hovernote, WS_VISIBLE | SS_NOTIFY | SS_CENTERIMAGE, rcstat, this, ctrlid++);
		stat->SetFont(GetFont());
		return stat;
	};

	m_checked.clear();
	DestroyChecks();
	VERIFY(m_tooltips.Create(this));

	bool haswarnings = false;
	CRect client = Ui::GetClientRect(this);
	int horzdistance = 0;
	CRect rect = GetControlRect(&m_pastetarget1);

	for (const auto & target : m_targets) {
		CRect thisrect = rect;
		auto * check = CreateCheck(thisrect, target);
		m_pastechecks.push_back(check);
		if (!target.warning.IsEmpty()) {
			haswarnings = true;
			m_tooltips.AddTool(check, target.warning);
		}
		rect.OffsetRect(0, rect.Height() + 8);

		if (thisrect.right > client.right)
			horzdistance = (thisrect.right - client.right) + 8;
	}

	if (haswarnings) {
		m_tooltips.Activate(TRUE);
		EnableToolTips(TRUE);
	}

	if (!Title.IsEmpty()) SetWindowText(Title);

	auto rccheck = GetControlRect(m_pastechecks.back());
	auto rcbase = GetControlRect(&m_pastetarget1);
	auto distance = rccheck.bottom - rcbase.bottom;
	MoveControlBy(&m_checkall, 0, distance);
	MoveControlBy(GetDlgItem(IDOK), 0, distance);
	MoveControlBy(GetDlgItem(IDCANCEL), 0, distance);
	ShrinkWindow(this, -horzdistance, -distance);

	auto * cancel = GetDlgItem(IDCANCEL);
	auto newclient = Ui::GetClientRect(this);
	auto canceldist = (newclient.right - Ui::GetControlRect(cancel).right) - 8;
	
	const auto MoveOkCancel = [this, canceldist, newclient](CWnd * ctl) {
		auto rcctl = Ui::GetControlRect(ctl);
		Ui::MoveControlBy(ctl, canceldist, 0);
	};
	MoveOkCancel(GetDlgItem(IDOK));
	MoveOkCancel(GetDlgItem(IDCANCEL));

	dc.RestoreDC(dcstate);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPasteTargets::OnOK()
{
	auto checks = GetChecks();
	m_checked.resize(checks.size(), false);
	for (size_t st = 0; st < checks.size(); ++st) {
		m_checked[st] = (checks[st]->GetCheck() == BST_CHECKED);
	}

	CFFBaseDlg::OnOK();
}

void CDlgPasteTargets::OnDestroy()
{
	DestroyChecks();
	CFFBaseDlg::OnDestroy();
}

BOOL CDlgPasteTargets::PreTranslateMessage(MSG* pMsg)
{
	if (IsWindow(m_tooltips.GetSafeHwnd()))
		m_tooltips.RelayEvent(pMsg);
	return CFFBaseDlg::PreTranslateMessage(pMsg);
}

void CDlgPasteTargets::OnBnClickedCheckAll()
{
	auto checked = Ui::GetCheckValue(m_checkall);
	auto checks = GetChecks();
	for (auto& c : checks) Ui::SetCheckValue(*c, checked);
}
