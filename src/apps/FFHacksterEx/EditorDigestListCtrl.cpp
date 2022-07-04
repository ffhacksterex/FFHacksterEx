// EditorDigestListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EditorDigestListCtrl.h"
#include <path_functions.h>
#include <ui_helpers.h>
#include <window_messages.h>
#include <FFHacksterProject.h>

using namespace Ui;


// CEditorDigestListCtrl

IMPLEMENT_DYNAMIC(CEditorDigestListCtrl, CListCtrl)

CEditorDigestListCtrl::CEditorDigestListCtrl()
{
}

CEditorDigestListCtrl::~CEditorDigestListCtrl()
{
	m_font.DeleteObject();
	m_boldfont.DeleteObject();
	if (Paths::FileExists(m_tempimagepath))
		Paths::FileDelete(m_tempimagepath);
}

CFont * CEditorDigestListCtrl::GetBoldFont()
{
	return &m_boldfont;;
}

Editors2::sEditorLink & CEditorDigestListCtrl::GetLink(int item)
{
	auto linkindex = GetItemData(item);
	return (*Links)[linkindex];
}


BEGIN_MESSAGE_MAP(CEditorDigestListCtrl, CListCtrl)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CEditorDigestListCtrl::OnLvnItemchanged)
END_MESSAGE_MAP()


// CEditorDigestListCtrl message handlers

LRESULT CEditorDigestListCtrl::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	LRESULT result = 0;

	CRect rc;
	GetWindowRect(&rc);

	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);

	// Create the font we'll use for drawing
	LOGFONT lf = { 0 };
	GetObject((HFONT)wParam, sizeof(lf), &lf);
	m_font.DeleteObject();
	m_font.CreateFontIndirect(&lf);

	// Create the bold font
	lf.lfWeight = FW_BOLD;
	m_boldfont.DeleteObject();
	m_boldfont.CreateFontIndirect(&lf);

	return result;
}

LRESULT CEditorDigestListCtrl::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return (LRESULT)m_font.GetSafeHandle();
}

void CEditorDigestListCtrl::MeasureItem(LPMEASUREITEMSTRUCT pmi)
{
	if ((HFONT)m_font == NULL) return;

	LOGFONT lf;
	m_font.GetLogFont(&lf);

	if (lf.lfHeight < 0)
		pmi->itemHeight = -lf.lfHeight;
	else
		pmi->itemHeight = lf.lfHeight;

	pmi->itemHeight += 4;
}

bool CEditorDigestListCtrl::IsTransparent() const
{
	auto exstyle = GetExtendedStyle();
	bool transparent = (exstyle & WS_EX_TRANSPARENT) != 0;
	return transparent;
}

void CEditorDigestListCtrl::DoDrawItem(CDC & dc, LPDRAWITEMSTRUCT pds)
{
	CRect rcrow = pds->rcItem;
	++rcrow.bottom;

	auto index = pds->itemID;
	auto & list = *this;
	auto & link = GetLink(index);
	auto & ed = *link.GetSelectedNode();

	bool focused = (pds->itemState & ODS_FOCUS) == ODS_FOCUS;

	dc.FillSolidRect(&rcrow, GetSysColor(COLOR_WINDOW));

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

	CRect rcitem;
	CString level = Editors2::GetLevelFromPath(*Project, ed.modulepath);
	auto headercount = list.GetHeaderCtrl()->GetItemCount();

	for (int subindex = 0; subindex < headercount; ++subindex) {
		list.GetSubItemRect(index, subindex, LVIR_BOUNDS, rcitem);

		dc.SelectObject(&m_font);
		if (subindex == COL_SRC && link.nodes.size() > 1)
			dc.SelectObject(&m_boldfont);

		CRect rctext = { rcitem.left + 1, rcitem.top, rcitem.right - 2, rcitem.bottom };

		static const COLORREF SHADOWCOLOR = RGB(64, 64, 64);
		static const COLORREF SPECIALCOLOR = RGB(0, 0, 201);
		static const COLORREF WARNCOLOR = RGB(153, 0, 0);
		static const COLORREF CUSTOMCOLOR = RGB(128, 204, 176);

		CString text;
		bool shadowed = (level != EDITORLEVEL_BUILTIN);
		auto fore = dc.GetTextColor();

		if (level == EDITORLEVEL_MISSING || level == EDITORLEVEL_REMOVED)
			fore = WARNCOLOR;
		else if (level == EDITORLEVEL_CUSTOM)
			fore = CUSTOMCOLOR;
		else if (level != EDITORLEVEL_BUILTIN)
			fore = SPECIALCOLOR;

		switch (subindex) {
		case COL_NAME:
			text = ed.name;
			break;
		case COL_STAT:
			text = Editors2::GetEditorLiveStatus(ed.show);
			break;
		case COL_DISP:
			text = ed.displayname;
			break;
		case COL_LEV:
			text = Editors2::GetLevelFromPath(*Project, ed.modulepath);
			break;
		case COL_SRC:
			text = Paths::GetFileName(ed.modulepath);
			break;
		case COL_FOLDER:
			text = Paths::GetDirectoryPath(ed.modulepath);
			break;
		default:
			text = "<error>";
			break;
		}
		if (shadowed) {
			rctext.OffsetRect(1, 1);
			dc.SetTextColor(SHADOWCOLOR);
			dc.DrawText(text, &rctext, 0);
			rctext.OffsetRect(-1, -1);
		}
		dc.SetTextColor(fore);
		dc.DrawText(text, &rctext, 0);
	}

	if (list.GetSelectionMark() == (int)index) {
		// Show the focus if the control is focused, the item is focused, and NOFOCUSRECT isn't specified.
		auto showfocus = focused && ((pds->itemState & ODS_NOFOCUSRECT) == 0);
		auto controlfocused = GetFocus() == this;
		DrawSelectionRect(*this, &dc, index, rcrow, showfocus && controlfocused);
	}
}

void CEditorDigestListCtrl::DrawItem(LPDRAWITEMSTRUCT pds)
{
	CDC maindc;
	maindc.Attach(pds->hDC);
	auto dcstate = maindc.SaveDC();

	DoDrawItem(maindc, pds);

	maindc.RestoreDC(dcstate);
	maindc.Detach();
}

void CEditorDigestListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();
	AddThinBorder(this);
}

BOOL CEditorDigestListCtrl::OnEraseBkgnd(CDC* pDC)
{
	auto client = Ui::GetClientRect(this);
	pDC->FillSolidRect(&client, GetSysColor(COLOR_WINDOW));
	return TRUE;
}

void CEditorDigestListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	auto pnm = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	SetSelectionMark(pnm->iItem);
	*pResult = 0;
}