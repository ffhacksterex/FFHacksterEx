// FloatingMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "FloatingMapDlg.h"
#include "afxdialogex.h"
#include <ui_helpers.h>
#include <FFHacksterProject.h>


bool sFloatingMapStateLink::IsValid() const
{
	return this->m_showrooms != nullptr;
}

// CFloatingMapDlg dialog

IMPLEMENT_DYNAMIC(CFloatingMapDlg, CDialogEx)

CFloatingMapDlg::CFloatingMapDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FLOATING_MAP, pParent)
{
}

CFloatingMapDlg::~CFloatingMapDlg()
{
}

void CFloatingMapDlg::Init(CFFHacksterProject* project, sFloatingMapStateLink* state)
{
	Project = project;
	State = state;
	auto rcpos = Ui::GetControlRect(&m_mapstatic);
	rcMap.SetRect(0, 0, 256, 256);
	rcMap.OffsetRect(rcpos.left + 8, rcpos.top + 16);

	cart = project; //TODO - remove during refactor
}

void CFloatingMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MAP, m_mapstatic);
}


BEGIN_MESSAGE_MAP(CFloatingMapDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


void CFloatingMapDlg::HandleClose()
{
	// Send the message to the owner (which defaults to the parent).
	CWnd* owner = GetOwner();
	CWnd* parent = GetParent();
	CWnd* wnd = owner != nullptr ? owner : parent;
	wnd->SendMessage(WMA_SHOWFLOATINGMAP, 0, (LPARAM)1);
}

bool CFloatingMapDlg::IsValid() const
{
	auto valid = (Project != nullptr && State != nullptr && State->IsValid());
	return valid;
}


// CFloatingMapDlg message handlers

void CFloatingMapDlg::OnCancel()
{
	HandleClose();
}

void CFloatingMapDlg::OnOK()
{
	HandleClose();
}

void CFloatingMapDlg::OnClose()
{
	HandleClose();
}

int CFloatingMapDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetIcon(NULL, TRUE);
	SetIcon(NULL, FALSE);
	return 0;
}

void CFloatingMapDlg::OnPaint()
{
	if (!IsValid()) {
		CDialogEx::OnPaint();
		return;
	}

	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting message

	//ELJNOTE - this is largely a straight port from the CMaps editor.
	//		the only change introduced here is the ability to
	//		resize the dialog and drawing area.
	//		This could be developed into a control that can be
	//		dropped onto any dialog.
	//		Not sure how long that'll take, but that feels like
	//		a better approach than coding it directly into the dialog.

	int coX = 0, coY = 0, tile = 0, coy = 0, cox = 0;
	CPoint pt;
	bool room = State->m_showrooms->GetCheck() == 1;
	auto& cur_map = *State->cur_map;
	auto& cur_tile = *State->cur_tile;
	auto& cur_tool = *State->cur_tool;
	auto& ptLastClick = *State->ptLastClick;
	auto& DecompressedMap = *State->DecompressedMap;
	auto& rcToolRect = *State->rcToolRect;

	for (coY = 0, pt.y = rcMap.top, coy = ScrollOffset.y; coY < 0x10; coY++, pt.y += 16, coy++) {
		for (coX = 0, pt.x = rcMap.left, cox = ScrollOffset.x; coX < 0x10; coX++, pt.x += 16, cox++)
			cart->GetStandardTiles(cur_map, room).Draw(&dc, DecompressedMap[coy][cox], pt, ILD_NORMAL);
	}
	CRect rcTemp = rcToolRect; rcTemp.NormalizeRect(); rcTemp.bottom += 1; rcTemp.right += 1;
	CPoint copt;
	if (mousedown == 1) {
		switch (cur_tool) {
		case 0: break;
		case 1: {			//fill
			coY = rcTemp.top - ScrollOffset.y; coX = rcTemp.left - ScrollOffset.x;
			copt.y = rcTemp.bottom - ScrollOffset.y; copt.x = rcTemp.right - ScrollOffset.x;
			tile = coX;
			for (; coY < copt.y; coY++) {
				for (coX = tile; coX < copt.x; coX++) {
					pt.x = rcMap.left + (coX << 4); pt.y = rcMap.top + (coY << 4);
					cart->GetStandardTiles(cur_map, room).Draw(&dc, cur_tile, pt, ILD_NORMAL);
				}
			}
		}break;
		default: {
			CBrush br; br.CreateSolidBrush(RGB(128, 64, 255));
			rcTemp.left = ((rcTemp.left - ScrollOffset.x) << 4) + rcMap.left;
			rcTemp.right = ((rcTemp.right - ScrollOffset.x) << 4) + rcMap.left;
			rcTemp.top = ((rcTemp.top - ScrollOffset.y) << 4) + rcMap.top;
			rcTemp.bottom = ((rcTemp.bottom - ScrollOffset.y) << 4) + rcMap.top;
			dc.FillRect(rcTemp, &br);
			br.DeleteObject();
		}break;
		}
	}
	if (cart->ShowLastClick) {
		pt.x = ((ptLastClick.x - ScrollOffset.x) << 4) + rcMap.left;
		pt.y = ((ptLastClick.y - ScrollOffset.y) << 4) + rcMap.top;
		if (PtInRect(rcMap, pt)) {
			dc.MoveTo(pt); pt.x += 15;
			dc.LineTo(pt); pt.y += 15;
			dc.LineTo(pt); pt.x -= 15;
			dc.LineTo(pt); pt.y -= 15;
			dc.LineTo(pt);
		}
	}
}
