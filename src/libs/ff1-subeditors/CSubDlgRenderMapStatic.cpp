// CSubDlgRenderMapStatic.cpp : implementation file
// Do map drawing here.

#include "stdafx.h"
#include "resource_subeditors.h"
#include "CSubDlgRenderMapStatic.h"
#include <FFHacksterProject.h>
#include <ui_helpers.h>

// CSubDlgRenderMapStatic

IMPLEMENT_DYNAMIC(CSubDlgRenderMapStatic, CStatic)

CSubDlgRenderMapStatic::CSubDlgRenderMapStatic()
{
}

CSubDlgRenderMapStatic::~CSubDlgRenderMapStatic()
{
}

BEGIN_MESSAGE_MAP(CSubDlgRenderMapStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// Public implementation

void CSubDlgRenderMapStatic::SetRenderState(const sRenderMapState& state)
{
	State = state;
	ASSERT(is_valid());
	if (is_valid()) {
		handle_sizing();
	}
}

const sRenderMapState& CSubDlgRenderMapStatic::GetRenderState() const
{
	return State;
}

// Internal implementation

BOOL CSubDlgRenderMapStatic::init()
{
	CRect rc(0, 0, 16, 16);
	rc = Ui::GetClientRect(this);
	m_pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	return TRUE;
}

bool CSubDlgRenderMapStatic::is_valid() const
{
	return State.IsValid();
}

void CSubDlgRenderMapStatic::handle_sizing()
{
	auto client = Ui::GetClientRect(this);
	return handle_sizing(client.Width(), client.Height());
}

void CSubDlgRenderMapStatic::handle_sizing(int cx, int cy)
{
	// Change: draw inside this control and let the parent deal with scrolling.
	// ignore m_mapframe

	// Size the control to its actual size, but don't move it
	int neww = State.mapdims.cx * State.tiledims.cx;
	int newh = State.mapdims.cy * State.tiledims.cy;
	//m_mapframe.SetWindowPos(nullptr, -1, -1, neww, newh, SWP_NOZORDER | SWP_NOMOVE);

	CRect ctlrc = Ui::GetControlRect(this);
	CRect wrc{ 0,0, neww, newh };
	AdjustWindowRectEx(wrc, GetStyle(), FALSE, GetExStyle());
	//m_mapframe.SetWindowPos(nullptr, -1, -1, wrc.Width(), wrc.Height(), SWP_NOZORDER | SWP_NOMOVE);
	SetWindowPos(nullptr, -1, -1, wrc.Width(), wrc.Height(), SWP_NOZORDER | SWP_NOMOVE);

	if (m_bmpdc.GetSafeHdc()) {
		m_bmpdc.SelectObject((CBitmap*)nullptr);
		m_bmpdc.DeleteDC();
	}
	CPaintDC dc(this);
	m_bmpdc.CreateCompatibleDC(&dc);
	if (m_bmp.GetSafeHandle() != NULL) m_bmp.DeleteObject();
	VERIFY(m_bmp.CreateCompatibleBitmap(&dc, neww, newh));
	m_bmpdc.SelectObject(&m_bmp);
	InvalidateRect(nullptr);
}

void CSubDlgRenderMapStatic::handle_paint()
{
	CDC& dc = m_bmpdc;
	int coX = 0, coY = 0, tile = 0, coy = 0, cox = 0;
	int row = 0;
	CPoint pt;
	auto& room = *State.showrooms;
	auto& cur_map = *State.cur_map;
	auto& cur_tile = *State.cur_tile;
	auto& cur_tool = *State.cur_tool;
	auto& ptLastClick = *State.ptLastClick;
	auto& DecompressedMap = State.DecompressedMap;
	auto& rcToolRect = *State.rcToolRect;
	auto& cart = State.project;

	//Draw the map
	auto rcMap = Ui::GetClientRect(this);
	CPoint ScrollOffset{ 0,0 };

	int maxrow = State.mapdims.cy;
	int maxcol = State.mapdims.cx;
	for (coY = 0, pt.y = rcMap.top, coy = ScrollOffset.y; coY < maxrow; coY++, pt.y += 16, coy++, row = coy * State.mapdims.cx) {
		for (coX = 0, pt.x = rcMap.left, cox = ScrollOffset.x; coX < maxcol; coX++, pt.x += 16, cox++)
			cart->GetStandardTiles(cur_map, room).Draw(&dc, DecompressedMap[(row)+cox], pt, ILD_NORMAL);
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

	// Draw last click rect if option is active
	if (cart->ShowLastClick) {
		pt.x = ((ptLastClick.x - ScrollOffset.x) << 4) + rcMap.left;
		pt.y = ((ptLastClick.y - ScrollOffset.y) << 4) + rcMap.top;
		if (PtInRect(rcMap, pt)) {
			auto oldpen = dc.SelectObject(&m_pen);
			dc.MoveTo(pt); pt.x += 15;
			dc.LineTo(pt); pt.y += 15;
			dc.LineTo(pt); pt.x -= 15;
			dc.LineTo(pt); pt.y -= 15;
			dc.LineTo(pt);
			dc.SelectObject(oldpen);
		}
	}

	//Draw the sprites
	auto& Sprite_Coords = *State.Sprite_Coords;
	auto& Sprite_InRoom = *State.Sprite_InRoom;
	auto& Sprite_StandStill = *State.Sprite_StandStill;
	auto& Sprite_Value = *State.Sprite_Value;
	auto& m_sprites = *State.m_sprites;
	auto SPRITE_COUNT = State.SPRITE_COUNT;
	auto SPRITE_PICASSIGNMENT = State.SPRITE_PICASSIGNMENT;
	CRect rc(0, 0, 16, 16);
	for (coX = 0; coX < SPRITE_COUNT; coX++) {
		if (!Sprite_Value[coX]) continue;
		if (room != Sprite_InRoom[coX]) continue;
		pt.x = Sprite_Coords[coX].x - ScrollOffset.x;
		pt.y = Sprite_Coords[coX].y - ScrollOffset.y;
		if (!PtInRect(rc, pt)) continue;
		pt.x = (pt.x << 4) + rcMap.left;
		pt.y = (pt.y << 4) + rcMap.top;
		m_sprites.Draw(&dc, cart->ROM[SPRITE_PICASSIGNMENT + Sprite_Value[coX]], pt, ILD_TRANSPARENT);
	}

	CPaintDC mydc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CStatic::OnPaint() for painting messages
	mydc.BitBlt(rcMap.left, rcMap.top, rcMap.Width(), rcMap.Height(), &dc, 0, 0, SRCCOPY);
}


// CSubDlgRenderMapStatic message handlers

void CSubDlgRenderMapStatic::PreSubclassWindow()
{
	VERIFY(init());
	CStatic::PreSubclassWindow();
}

int CSubDlgRenderMapStatic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	VERIFY(init());
	return 0;
}

void CSubDlgRenderMapStatic::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	//				   // TODO: Add your message handler code here
	//				   // Do not call CStatic::OnPaint() for painting messages

	//auto rc = Ui::GetClientRect(this);
	//dc.FillSolidRect(rc, RGB(0, 255, 127));

	if (!is_valid()) {
		//CStatic::OnPaint(); //TODO - displays text if subclassed static is label instead of imageframe
		return;
	}
	handle_paint();
}

void CSubDlgRenderMapStatic::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
}
