// CDialogWithBackground.cpp : implementation file
//
#include "stdafx.h"
#include "DialogWithBackground.h"
#include "afxdialogex.h"
#include "window_messages.h"
#include <AppSettings.h>

// CDialogWithBackground dialog

IMPLEMENT_DYNAMIC(CDialogWithBackground, CFFBaseDlg)

CDialogWithBackground::CDialogWithBackground(UINT IDD, CWnd* pParent /*=nullptr*/)
	: CFFBaseDlg(IDD, pParent)
{
}

CDialogWithBackground::CDialogWithBackground(UINT IDD, UINT nResid, CWnd * pParent)
	: CFFBaseDlg(IDD, pParent)
{
	SetBackgroundResImage(nResid, true);
}

CDialogWithBackground::~CDialogWithBackground()
{
}

void CDialogWithBackground::PaintBanner(CDC & dc)
{
	m_banner.Render(dc, 8, 8);
}

void CDialogWithBackground::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogWithBackground, CFFBaseDlg)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDialogWithBackground message handlers


BOOL CDialogWithBackground::OnInitDialog()
{
	__super::OnInitDialog();

	if (AppStgs == nullptr)
		return FailInitAndReturn(IDABORT, GetRuntimeClass()->m_lpszClassName + CString(" cannot launch without access to app settings."));
	//{
	//	EndDialog(IDABORT);
	//	m_initfailed = true;
	//	AfxMessageBox(GetRuntimeClass()->m_lpszClassName + CString(" cannot launch without access to app settings."));
	//	return TRUE;
	//}

	EnableImage(AppStgs->ShowBackgroundArt);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogWithBackground::OnPaint()
{
	CPaintDC dc(this);
	PaintBackground(dc);
	PaintBanner(dc);
	PaintClient(dc);
}

//REFACTOR - move into BackgroundMixin as a member
//		The mixin isn't a CWnd class, so we'll need to call th ebase class OnCtlColor here.
//		passing that to BackgroundMixin to call pOwner->CFFBaseDlg::OnCtlColor might be doing too much there.

//HBRUSH CDialogWithBackground::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

HBRUSH CDialogWithBackground::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
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