// CEditorWithBackground.cpp : implementation file
//
#include "stdafx.h"
#include "EditorWithBackground.h"
#include "afxdialogex.h"
#include <FFHacksterProject.h>
#include <AppSettings.h>


// CEditorWithBackground dialog
#define BaseClass CSaveableDialog

IMPLEMENT_DYNAMIC(CEditorWithBackground, BaseClass)

CEditorWithBackground::CEditorWithBackground(UINT nID, CWnd* pParent /*=nullptr*/)
	: BaseClass(nID, pParent)
{
}

CEditorWithBackground::CEditorWithBackground(UINT nID, UINT uImageID, CWnd * pParent)
	: BaseClass(nID, pParent)
{
	// If it doesn't load as a PNG, try loading as a BITMAP.
	SetBackgroundResImage(uImageID, true);
}

CEditorWithBackground::~CEditorWithBackground()
{
}

void CEditorWithBackground::PaintBanner(CDC & dc)
{
	m_banner.Render(dc, 8, 8);
}

void CEditorWithBackground::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditorWithBackground, BaseClass)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(DM_GETDEFID, OnGetDefId)
END_MESSAGE_MAP()


void CEditorWithBackground::SetODDefButtonID(UINT id)
{
	m_oddefid = id;
}

// CEditorWithBackground message handlers


BOOL CEditorWithBackground::OnInitDialog()
{
	__super::OnInitDialog();

	if (Project == nullptr) {
		EndDialog(IDABORT);
		m_initfailed = true;
		AfxMessageBox(GetRuntimeClass()->m_lpszClassName + CString(" cannot launch without access to the project."));
		return TRUE;
	}
	if (Project->AppSettings == nullptr) {
		m_initfailed = true;
		AfxMessageBox(GetRuntimeClass()->m_lpszClassName + CString(" cannot launch without access to app settings."));
		EndDialog(IDABORT);
		return TRUE;
	}

	EnableImage(Project->AppSettings->ShowBackgroundArt);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditorWithBackground::OnPaint()
{
	CPaintDC dc(this);
	PaintBackground(dc);
	PaintBanner(dc);
	PaintClient(dc);
}

//REFACTOR - move into BackgroundMixin as a member
//		The mixin isn't a CWnd class, so we'll need to call th ebase class OnCtlColor here.
//		passing that to BackgroundMixin to call pOwner->CFFBaseDlg::OnCtlColor might be doing too much there.

//HBRUSH CEditorWithBackground::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
//	if (!IsWindow(pWnd->GetSafeHwnd()))
//		return hbr;
//
//	{
//		TCHAR classname[MAX_PATH];
//		if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0) {
//			CString cs;
//			cs.Format(_T("Unknown control class '%s' cannot handle WM_CTLCOLOR\n"), classname);
//			OutputDebugString(cs);
//			return hbr;
//		}
//		if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
//			return hbr;
//		if (_tcsicmp(classname, _T("LISTBOX")) == 0)
//			return hbr;
//		if (_tcsicmp(classname, _T("EDIT")) == 0 && (pWnd->GetExStyle() & WS_EX_TRANSPARENT) == 0)
//			return hbr;
//	}
//
//	auto backbrush = GetBackgroundBrush();
//	if (backbrush == NULL)
//		return hbr;
//
//	CRect rccli;
//	pWnd->GetWindowRect(&rccli);
//	if (IsWindow(pWnd->GetParent()->GetSafeHwnd()))
//		pWnd->GetParent()->ScreenToClient(&rccli);
//	pDC->SetBrushOrg(-rccli.left, -rccli.top);
//	pDC->SetBkMode(TRANSPARENT);
//	return backbrush;
//}

HBRUSH CEditorWithBackground::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	auto hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	if (!IsWindow(pWnd->GetSafeHwnd()))
		return hbr;

	//REFACTOR - Instead of the base class brush, we'll use a white brush
	hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
	{
		TCHAR classname[MAX_PATH];
		if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0) {
			CString cs;
			cs.Format(_T("Unknown control class '%s' cannot handle WM_CTLCOLOR\n"), classname);
			OutputDebugString(cs);
			return hbr;
		}
		if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
			return hbr;
		if (_tcsicmp(classname, _T("LISTBOX")) == 0)
			return hbr;
		if (_tcsicmp(classname, _T("EDIT")) == 0 && (pWnd->GetExStyle() & WS_EX_TRANSPARENT) == 0)
			return hbr;
	}

	auto backbrush = iif(GetBackgroundBrush(), hbr);
	CRect rccli;
	pWnd->GetWindowRect(&rccli);
	if (IsWindow(pWnd->GetParent()->GetSafeHwnd()))
		pWnd->GetParent()->ScreenToClient(&rccli);
	pDC->SetBrushOrg(-rccli.left, -rccli.top);
	pDC->SetBkMode(TRANSPARENT);
	return backbrush;
}

LRESULT CEditorWithBackground::OnGetDefId(WPARAM wparam, LPARAM lparam)
{
	LRESULT id = m_oddefid != 0 ? m_oddefid : Default();
	return id;
}