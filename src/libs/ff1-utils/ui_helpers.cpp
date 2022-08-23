#include "stdafx.h"
#include "ui_helpers.h"
#include "path_functions.h"

namespace {
	const CRect MouseOverSelRect = { 0,0,1,1 };
	CImage MouseOverSelImg;
	CImage MouseOverHotImg;
	CFont SubBannerFont;
}

namespace Ui
{
	// ################################################################

	RedrawScope::RedrawScope(CWnd * pwnd) : m_pwnd(pwnd)
	{
		if (m_pwnd != nullptr) m_pwnd->SetRedraw(FALSE);
	}

	RedrawScope::~RedrawScope()
	{
		if (m_pwnd != nullptr) {
			m_pwnd->SetRedraw(TRUE);
			m_pwnd->Invalidate();
		}
	}



	// ################################################################

	ui_exception::ui_exception(const char * file, int line, const char * function, std::string message)
	{
		TRACEEXCEPTIONPOINT(*this, file, line, function);

		m_message = message;

		LogMsg << m_message << std::endl;
	}

	ui_exception::~ui_exception()
	{
	}

	const char * ui_exception::what() const
	{
		return m_message.c_str();
	}



	// ################################################################
	// METHOD IMPLEMENTATIONS

	const CRect & GetMouseOverImgRect()
	{
		return MouseOverSelRect;
	}

	CImage & GetMouseOverSelImg()
	{
		if (!MouseOverSelImg)
			if (MouseOverSelImg.Create(1, 1, 32)) MouseOverSelImg.SetPixel(0, 0, GetSysColor(COLOR_HIGHLIGHT));
		return MouseOverSelImg;
	}

	CImage & GetMouseOverHotImg()
	{
		if (!MouseOverHotImg)
			if (MouseOverHotImg.Create(1, 1, 32)) MouseOverHotImg.SetPixel(0, 0, GetSysColor(COLOR_HOTLIGHT));
		return MouseOverHotImg;
	}

	CFont & GetSubBannerFont()
	{
		if (!SubBannerFont.operator HFONT())
			SubBannerFont.CreatePointFont(160, "Verdana");
		return SubBannerFont;
	}

	CWnd* GetControlByName(CWnd* parent, const Rc::dialogmappings& dlgmap, CString ctlname)
	{
		auto iter = dlgmap.find(ctlname);
		auto id = iter != cend(dlgmap) ? iter->second : -1;
		auto pwnd = parent->GetDlgItem(id);
		return pwnd;
	}

	void SetCheckValue(CButton & check, bool set)
	{
		check.SetCheck(set ? BST_CHECKED : BST_UNCHECKED);
	}

	bool GetCheckValue(CButton & check)
	{
		return check.GetCheck() == BST_CHECKED;
	}

	void SetCheckFlags(int flags, std::vector<CButton*> vchecks)
	{
		for (size_t st = 0; st < vchecks.size(); ++st) {
			int bit = (int)(1 << st);
			bool set = (flags & bit) == bit;
			SetCheckValue(*vchecks[st], set);
		}
	}

	int GetCheckFlags(std::vector<CButton*> vchecks)
	{
		int flags = 0;
		for (size_t st = 0; st < vchecks.size(); ++st) {
			if (GetCheckValue(*vchecks[st]))
			{
				int bit = (int)(1 << st);
				flags |= bit;
			}
		}
		return flags;
	}

	void SetCheckValue(CStrikeCheck & check, bool set)
	{
		check.SetCheck(set ? BST_CHECKED : BST_UNCHECKED);
		check.Invalidate();
	}

	bool GetCheckValue(CStrikeCheck & check)
	{
		return check.GetCheck() == BST_CHECKED;
	}

	void SetEditValue(CEdit & edit, LPCSTR newvalue)
	{
		edit.SetWindowText(newvalue);
	}

	void SetEditValue(CEdit & edit, std::string newvalue)
	{
		edit.SetWindowText(newvalue.c_str());
	}

	CString GetEditValue(CEdit & edit)
	{
		CString value;
		edit.GetWindowText(value);
		return value;
	}

	void SetEditTextAndFocus(CEdit& wnd, CString text, bool selectall)
	{
		wnd.SetWindowText(text);
		wnd.SetFocus();
		if (selectall) wnd.SetSel(0, -1);
	}

	CString GetStaticText(CStatic & stat)
	{
		CString value;
		stat.GetWindowText(value);
		return value;
	}

	void SetStaticText(CStatic & stat, CString text)
	{
		stat.SetWindowText(text);
	}

	void SetCheckFlags(int flags, std::vector<CStrikeCheck*> vchecks)
	{
		for (size_t st = 0; st < vchecks.size(); ++st) {
			int bit = (int)(1 << st);
			bool set = (flags & bit) == bit;
			SetCheckValue(*vchecks[st], set ? BST_CHECKED : BST_UNCHECKED);
			vchecks[st]->InvalidateRect(nullptr);
		}
	}

	int GetCheckFlags(std::vector<CStrikeCheck*> vchecks)
	{
		int flags = 0;
		for (size_t st = 0; st < vchecks.size(); ++st) {
			if (GetCheckValue(*vchecks[st]) == BST_CHECKED)
			{
				int bit = (int)(1 << st);
				flags |= bit;
			}
		}
		return flags;
	}

	void OffsetWindow(CWnd & wnd, int dx, int dy)
	{
		CRect rc(0, 0, 0, 0);
		wnd.GetWindowRect(&rc);
		rc.OffsetRect(dx, dy);

		CWnd* parent = wnd.GetParent();
		if (parent != nullptr)
			parent->ScreenToClient(&rc);

		wnd.MoveWindow(&rc);
	}

	void InflateWindowRect(CWnd & wnd, int cx, int cy)
	{
		CRect rc(0, 0, 0, 0);
		wnd.GetWindowRect(&rc);
		rc.right = rc.left + rc.Width() + cx;
		rc.bottom = rc.top + rc.Height() + cy;
		wnd.SetWindowPos(nullptr, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}

	void ShowWindow(CWnd & wnd, bool show)
	{
		wnd.ShowWindow(show ? SW_SHOW : SW_HIDE);
	}

	void EnableWindow(CWnd & wnd, bool enabled)
	{
		wnd.EnableWindow(enabled ? TRUE : FALSE);
	}

	CString GetControlText(CWnd & wnd)
	{
		CString str;
		wnd.GetWindowText(str);
		return str;
	}

	int GetControlInt(CWnd& wnd, int radix)
	{
		auto cs = GetControlText(wnd);
		switch (radix) {
		case 10:
		case 16:
			return strtoul(cs, nullptr, radix);
		}

		cs.Format("Getting a control int requires a radix of either 10 or 16, not %d.", radix);
		ErrorHere << (LPCSTR)cs << std::endl;
		throw std::runtime_error((LPCSTR)cs);
	}

	void SetControlInt(CWnd& wnd, int value, int radix)
	{
		CString cs;
		switch (radix) {
		case 16:
			cs.Format("%02X", value);
			break;
		case 10:
			cs.Format("%d", value);
			break;
		default:
			cs.Format("Setting a control int must use a radix of either 10 or 16, not %d.", radix);
			ErrorHere << (LPCSTR)cs << std::endl;
			throw std::runtime_error((LPCSTR)cs);
		}
		wnd.SetWindowText(cs);
	}

	void AddEntry(CComboBox & box, CString string, DWORD_PTR data)
	{
		int index = box.AddString(string);
		box.SetItemData(index, data);
	}

	void AddEntry(CListBox & box, CString string, DWORD_PTR data)
	{
		int index = box.AddString(string);
		box.SetItemData(index, data);
	}

	void InsertEntry(CComboBox & box, int index, CString string, DWORD_PTR data)
	{
		int addedindex = box.InsertString(index, string);
		box.SetItemData(addedindex, data);
	}

	void InsertEntry(CListBox & box, int index, CString string, DWORD_PTR data)
	{
		int addedindex = box.InsertString(index, string);
		box.SetItemData(addedindex, data);
	}

	int FindIndexByData(CComboBox & box, DWORD_PTR data)
	{
		for (int i = 0; i < box.GetCount(); ++i) {
			auto thisdata = box.GetItemData(i);
			if (thisdata == data)
				return i;
		}
		return CB_ERR;
	}

	int FindIndexByPrefix(CComboBox & box, int matchvalue)
	{
		for (int i = 0; i < box.GetCount(); ++i) {
			CString csvalue;
			box.GetLBText(i, csvalue);
			int value = strtoul(csvalue, nullptr, 16);
			if (value == matchvalue)
				return i;
		}
		return CB_ERR;
	}

	DWORD_PTR GetSelectedItemData(CComboBox & box, DWORD_PTR defvalue)
	{
		int cursel = box.GetCurSel();
		return cursel != CB_ERR ? box.GetItemData(cursel) : defvalue;
	}

	void RetrieveSelectedItemData(CComboBox & box, DWORD_PTR & value)
	{
		int cursel = box.GetCurSel();
		if (cursel != CB_ERR) value = box.GetItemData(cursel);
	}

	void SelectFirstItem(CComboBox & box)
	{
		if (box.GetCount() > 0)
			box.SetCurSel(0);
	}

	bool SelectItemByData(CComboBox & box, DWORD_PTR data)
	{
		int sel = FindIndexByData(box, data);
		if (sel == CB_ERR) return false;

		box.SetCurSel(sel);
		return true;
	}

	DWORD_PTR GetSelectedItemData(CListBox & box, DWORD_PTR defvalue)
	{
		int cursel = box.GetCurSel();
		return cursel != CB_ERR ? box.GetItemData(cursel) : defvalue;
	}

	int LoadCombo(CComboBox & combo, dataintnodevector dvec)
	{
		combo.ResetContent();
		for (const auto & dnode : dvec) {
			int index = combo.InsertString(-1, dnode.name);
			combo.SetItemData(index, dnode.value);
		}
		return combo.GetCount();
	}

	int LoadListBox(CListBox & list, dataintnodevector dvec)
	{
		list.ResetContent();
		for (const auto & dnode : dvec) {
			int index = list.InsertString(-1, dnode.name);
			list.SetItemData(index, dnode.value);
		}
		return list.GetCount();
	}

	int LoadCaptions(std::vector<CWnd*> ctls, dataintnodevector dvec)
	{
		int setcount = 0;
		for (size_t st = 0; st < ctls.size(); ++st) {
			if (st < dvec.size()) {
				ctls[st]->SetWindowText(dvec[st].name);
			}
			else {
				CString cs;
				cs.Format("Class %lu", st);
				ctls[st]->SetWindowText(cs);
			}
			++setcount;
		}
		return setcount;
	}

	BOOL SetTrackMouseEvent(CWnd * pwnd, UINT eventtype)
	{
		UNREFERENCED_PARAMETER(eventtype);

		TRACKMOUSEEVENT tmEvent;
		tmEvent.cbSize = sizeof(tmEvent);
		tmEvent.dwFlags = TME_LEAVE;
		tmEvent.hwndTrack = pwnd->m_hWnd;
		BOOL tracking = ::_TrackMouseEvent(&tmEvent);
		if (tracking) pwnd->Invalidate(0);
		return tracking;
	}

	void HandleLbuttonDrag(CWnd * pwnd)
	{
		ReleaseCapture();
		pwnd->SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}

	void ClientToParentRect(CWnd * pwnd, RECT * prc)
	{
		pwnd->GetClientRect(prc);
		pwnd->ClientToScreen(prc);
		pwnd->GetParent()->ScreenToClient(prc);
	}

	// Get the rectangle of the entire window in its parent's coordinates.
	void WindowToParentRect(CWnd * pwnd, RECT * prc)
	{
		pwnd->GetWindowRect(prc);
		auto parent = pwnd->GetParent();
		if (parent) parent->ScreenToClient(prc);
	}

	//REFACTOR - should these take a CWnd & instead of CWnd * as the first param?

	// Get the rectangle of the entire control in its parent's coordinates.
	//FUTURE - add a check to ensure that pcontrol isn't a top-level window, and throw an exception if it is.
	//       Currently, the difference between this and WindowToParentRect() is purely semantic.
	void GetControlRect(CWnd * pcontrol, RECT * prc)
	{
		pcontrol->GetWindowRect(prc);
		auto parent = pcontrol->GetParent();
		if (parent) parent->ScreenToClient(prc);
	}

	CRect GetControlRect(CWnd * pcontrol)
	{
		CRect rc(0,0,0,0);
		GetControlRect(pcontrol, &rc);
		return rc;
	}

	CRect GetClientRect(CWnd * pcontrol)
	{
		CRect rc(0, 0, 0, 0);
		pcontrol->GetClientRect(&rc);
		return rc;
	}

	CRect GetNonClientRect(CWnd& wnd)
	{
		CRect rc = Ui::GetWindowRect(&wnd);
		rc.OffsetRect(-rc.left, -rc.top);
		return rc;
	}

	CRect GetWindowRect(CWnd * pwnd)
	{
		CRect rc(0, 0, 0, 0);
		pwnd->GetWindowRect(&rc);
		return rc;
	}

	CPoint GetCursorPos()
	{
		CPoint pt{ 0,0 };
		::GetCursorPos(&pt);
		return pt;
	}

	CPoint GetControlCursorPos(CWnd* pwnd)
	{
		CPoint pt{ 0,0 };
		::GetCursorPos(&pt);
		pwnd->ScreenToClient(&pt);
		return pt;
	}

	// Returns - true if scroll is shown, false if hidden.
	// Params:
	// pwnd - window whose scroll bar will be set or hidden
	// nBar - SB_HORZ or SB_VERT
	// rcarea - rect of the area that might need to scroll
	// clientextent - the client width (if nBar == SB_HORZ) or height (SB_VERT)
	// tilespan - if tiled, the span of a single tile
	bool SetClientScroll(CWnd* pwnd, int nBar, const CRect& rcarea, int clientextent, int tilespan)
	{
		// Does the control's extent exceeds the host client area's extent?
		int ctlw = nBar == SB_HORZ ? rcarea.Width() : rcarea.Height();
		if (ctlw > clientextent) {
			// Yes, so show the bar and set it's range
			int bump = nBar == SB_HORZ ? GetSystemMetrics(SM_CXVSCROLL) : GetSystemMetrics(SM_CYHSCROLL);
			auto scmax = (ctlw - clientextent) + bump + tilespan;
			pwnd->EnableScrollBarCtrl(nBar, TRUE);
			SCROLLINFO si = { 0 };
			pwnd->GetScrollInfo(nBar, &si, SIF_ALL);
			si.nPage = 16;
			si.nMin = 0;
			si.nMax = scmax;
			si.nPos = (si.nPos > scmax) ? scmax : si.nPos;
			pwnd->SetScrollInfo(nBar, &si, SIF_ALL);
			return true;
		}
		else {
			pwnd->EnableScrollBarCtrl(nBar, FALSE);
			return false;
		}
	}

	int HandleClientScroll(CWnd* pwnd, UINT nBar, UINT nSBCode, UINT nPos)
	{
		int curpos = pwnd->GetScrollPos(nBar);
		int limit = pwnd->GetScrollLimit(nBar);
		switch (nSBCode)
		{
		case SB_LEFT: // == SB_TOP
			curpos = 0;
			break;
		case SB_RIGHT: // == SB_BOTTOM
			curpos = limit;
			break;
		case SB_ENDSCROLL:
			break;
		case SB_LINELEFT: // == SB_LINEUP
			if (curpos > 0) --curpos;
			break;
		case SB_LINERIGHT: // == SB_LINEDOWN
			if (curpos < limit) ++curpos;
			break;
		case SB_PAGELEFT: // == SB_PAGEUP
		{
			SCROLLINFO info;
			pwnd->GetScrollInfo(nBar, &info, SIF_ALL);
			if (curpos > 0) curpos = max(0, curpos - (int)info.nPage);
		}
		break;
		case SB_PAGERIGHT: // == SB_PAGEDOWN
		{
			SCROLLINFO info;
			pwnd->GetScrollInfo(nBar, &info, SIF_ALL);
			if (curpos < limit) curpos = min(limit, curpos + (int)info.nPage);
		}
		break;
		case SB_THUMBPOSITION:
			curpos = nPos;	// nPos is the absolute position at the end of the drag.
			break;
		case SB_THUMBTRACK:
			curpos = nPos;  // nPos is the specified position where the scroll box is currently.
			break;          // This occurs when dragging the scroll box with the mouse.
		}

		pwnd->SetScrollPos(nBar, curpos);
		return curpos;
	}

	bool SetContainedScroll(CScrollBar* bar, int nBar, const CRect& rcarea, int clientextent, int tilespan)
	{
		// Does the control's extent exceeds the host client area's extent?
		int ctlw = nBar == SB_HORZ ? rcarea.Width() : rcarea.Height();
		if (ctlw > clientextent) {
			// Yes, so show the bar and set it's range
			//int bump = nBar == SB_HORZ ? GetSystemMetrics(SM_CXVSCROLL) : GetSystemMetrics(SM_CYHSCROLL);
			int bump = 0;
			auto scmax = (ctlw - clientextent) + bump + tilespan;
			bar->ShowWindow(SW_SHOW);
			SCROLLINFO si = { 0 };
			bar->GetScrollInfo(&si, SIF_ALL);
			si.nPage = 16;
			si.nMin = 0;
			si.nMax = scmax;
			si.nPos = (si.nPos > scmax) ? scmax : si.nPos;
			bar->SetScrollInfo(&si, SIF_ALL);
			return true;
		}
		else {
			bar->ShowWindow(SW_HIDE);
			return false;
		}
	}

	int HandleContainedScroll(CScrollBar* bar, UINT nBar, UINT nSBCode, UINT nPos)
	{
		int curpos = bar->GetScrollPos();
		int limit = bar->GetScrollLimit();
		switch (nSBCode)
		{
		case SB_LEFT: // == SB_TOP
			curpos = 0;
			break;
		case SB_RIGHT: // == SB_BOTTOM
			curpos = limit;
			break;
		case SB_ENDSCROLL:
			break;
		case SB_LINELEFT: // == SB_LINEUP
			if (curpos > 0) --curpos;
			break;
		case SB_LINERIGHT: // == SB_LINEDOWN
			if (curpos < limit) ++curpos;
			break;
		case SB_PAGELEFT: // == SB_PAGEUP
		{
			SCROLLINFO info;
			bar->GetScrollInfo(&info, SIF_ALL);
			if (curpos > 0) curpos = max(0, curpos - (int)info.nPage);
		}
		break;
		case SB_PAGERIGHT: // == SB_PAGEDOWN
		{
			SCROLLINFO info;
			bar->GetScrollInfo(&info, SIF_ALL);
			if (curpos < limit) curpos = min(limit, curpos + (int)info.nPage);
		}
		break;
		case SB_THUMBPOSITION:
			curpos = nPos;	// nPos is the absolute position at the end of the drag.
			break;
		case SB_THUMBTRACK:
			curpos = nPos;  // nPos is the specified position where the scroll box is currently.
			break;          // This occurs when dragging the scroll box with the mouse.
		}

		bar->SetScrollPos(curpos);
		return curpos;
	}

	CRect GetSubitemRect(CListCtrl & list, int item, int subitem)
	{
		CRect rcitem(0, 0, 0, 0);
		VERIFY(list.GetSubItemRect(item, subitem, LVIR_BOUNDS, rcitem));
		list.ClientToScreen(&rcitem);
		return rcitem;
	}

	// Renumbers the specified column (subitem).
	// Adds base to each value in that column (e.g. pass 1 if the column is to be 1-based).
	void RenumberList(CListCtrl & list, int subitem, int base, int startat)
	{
		RedrawScope redraw(&list);
		CString fmt;
		for (int i = startat; i < list.GetItemCount(); ++i) {
			fmt.Format("%d", i + base);
			list.SetItemText(i, subitem, fmt);
		}
	}

	void AutosizeAllColumns(CListCtrl & list)
	{
		for (int i = 0; i < list.GetHeaderCtrl()->GetItemCount(); ++i)
			list.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
	}

	void AutosizeAllColumns(CListCtrl & list, CFont * overridingfont)
	{
		CClientDC dc(&list);
		auto oldfont = dc.GetCurrentFont();
		if (overridingfont != nullptr) dc.SelectObject(overridingfont);

		for (int i = 0; i < list.GetHeaderCtrl()->GetItemCount(); ++i)
			list.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

		dc.SelectObject(oldfont);
	}

	void AutosizeColumn(CListCtrl & list, int column, CFont * overridingfont)
	{
		CClientDC dc(&list);
		auto oldfont = dc.GetCurrentFont();
		if (overridingfont != nullptr) dc.SelectObject(overridingfont);

		list.SetColumnWidth(column, LVSCW_AUTOSIZE_USEHEADER);
		int dx = list.GetColumnWidth(column);
		for (int i = 0; i < list.GetItemCount(); ++i)
		{
			auto textsize = dc.GetTextExtent(list.GetItemText(i, column));
			if ((textsize.cx + 8) > dx)
				dx = textsize.cx + 8; //FUTURE - can GetSystemMetrics give an accurate item padding value here?
		}
		list.SetColumnWidth(column, dx);

		dc.SelectObject(oldfont);
	}

	void PrepareInplaceEdit(CListCtrl & list, int item, int subitem, CInplaceEdit & edit, std::function<InplaceEditOKFunc> func)
	{
		CRect client(0, 0, 0, 0);
		list.GetClientRect(&client);
		CRect rcitem(0, 0, 0, 0);
		VERIFY(list.GetSubItemRect(item, subitem, LVIR_BOUNDS, rcitem));
		if (rcitem.right > client.right) rcitem.right = client.right;

		list.ClientToScreen(&rcitem); // it's actually a dialog that uses screen coords

		edit.OKFunc = func;
		edit.ClearData();
		edit.SetPosition(rcitem);
		edit.ShowWindow(SW_SHOW);
	}

	void PrepareInplaceCheck(CListCtrl & list, int item, int subitem, CInplaceCheck & check, std::function<InplaceCheckOKFunc> func)
	{
		CRect client(0, 0, 0, 0);
		list.GetClientRect(&client);
		CRect rcitem(0, 0, 0, 0);
		VERIFY(list.GetSubItemRect(item, subitem, LVIR_BOUNDS, rcitem));
		if (rcitem.right > client.right) rcitem.right = client.right;

		list.ClientToScreen(&rcitem); // it's actually a dialog that uses screen coords

		check.OKFunc = func;
		check.ClearData();
		check.SetPosition(rcitem);
		check.ShowWindow(SW_SHOW);
	}

	void PrepareInplaceCombo(CListCtrl & list, int item, int subitem, CInplaceCombo & combo, std::function<InplaceComboOKFunc> func)
	{
		combo.m_combo.SetFont(list.GetFont());

		CRect client(0, 0, 0, 0);
		list.GetClientRect(&client);
		CRect rcitem(0, 0, 0, 0);
		VERIFY(list.GetSubItemRect(item, subitem, LVIR_BOUNDS, rcitem));
		if (rcitem.right > client.right) rcitem.right = client.right;

		list.ClientToScreen(&rcitem); // it's actually a dialog that uses screen coords

		combo.OKFunc = func;
		combo.ClearData();
		combo.SetPosition(rcitem);
		combo.ShowWindow(SW_SHOW);
	}

	void UpdateBackground(CWnd * wnd, CDC * dc, CRect rect, CBrush* optionalbrush)
	{
		auto oldpen = dc->GetCurrentPen();
		auto oldbr = dc->GetCurrentBrush();

		if (optionalbrush != nullptr && optionalbrush->operator HBRUSH() != NULL)
		{
			auto rcwnd = GetControlRect(wnd);
			dc->SelectStockObject(NULL_PEN);
			dc->SelectObject(optionalbrush);
			dc->Rectangle(&rect);
		}
		else {
			dc->FillSolidRect(&rect, GetSysColor(COLOR_WINDOW));
		}

		dc->SelectObject(oldpen);
		dc->SelectObject(oldbr);
	}

	void DrawSelectionRect(CListCtrl & list, CDC * dc, int item, CRect rect, bool focused)
	{
		auto dcstate = dc->SaveDC();
		bool selected = (list.GetSelectionMark() == item);
		CImage* pimg = nullptr;
		if (selected) pimg = &GetMouseOverSelImg();
		if (pimg && *pimg) {
			BYTE alpha = focused ? 0x70 : 0x40;
			pimg->AlphaBlend(dc->GetSafeHdc(), rect, GetMouseOverImgRect(), alpha);
		}

		if (focused) {
			CRect focus(rect);
			focus.InflateRect(-1, -1);
			dc->DrawFocusRect(&focus);
		}
		dc->RestoreDC(dcstate);
	}

	void CenterToParent(CWnd * pwnd)
	{
		if (pwnd == nullptr) return;

		CWnd* parent = pwnd->GetParent();
		if (parent == nullptr) 
			parent = pwnd->GetDesktopWindow();
		CRect rcparent = GetWindowRect(parent);
		CRect rcchild = GetWindowRect(pwnd);
		CPoint org = { (rcparent.Width() - rcchild.Width()) / 2, (rcparent.Height() - rcchild.Height()) / 2 };
		CRect rcnew = { 0, 0, rcchild.Width(), rcchild.Height() };
		rcnew.OffsetRect(rcparent.left + org.x, rcparent.top + org.y);
		pwnd->MoveWindow(&rcnew);
	}

	// Put a control into the rectangle of a sibling, then hide the sibling.
	BOOL SeatControl(CWnd* pcontrol, CWnd* pplaceholder)
	{
		ASSERT(pcontrol != nullptr);
		ASSERT(pplaceholder != nullptr);
		if (pcontrol == nullptr || pplaceholder == nullptr) return FALSE;

		ASSERT(pcontrol->GetParent() != nullptr);
		auto myparent = pcontrol->GetParent();
		auto seatparent = pplaceholder->GetParent();
		if (myparent != seatparent) return FALSE;
		if (myparent == nullptr) return FALSE;

		auto rc = Ui::GetControlRect(pplaceholder);
		pcontrol->SetWindowPos(pplaceholder, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
		pplaceholder->ShowWindow(SW_HIDE);
		return TRUE;
	}

	CSize GetDistance(CWnd * pwnd, CWnd * pother)
	{
		CRect rcwnd, rcother;
		pwnd->GetWindowRect(&rcwnd);
		pother->GetWindowRect(&rcother);
		return rcwnd.TopLeft() - rcother.TopLeft();
	}

	void MoveControlBy(CWnd * pwnd, CSize delta)
	{
		MoveControlBy(pwnd, delta.cx, delta.cy);
	}

	void MoveControlBy(CWnd * pwnd, int cx, int cy)
	{
		auto rc = GetControlRect(pwnd);
		pwnd->SetWindowPos(nullptr, rc.left + cx, rc.top + cy, -1, -1, SWP_NOZORDER | SWP_NOSIZE);
	}

	void ShrinkWindow(CWnd * pwnd, int cx, int cy)
	{
		auto rc = GetControlRect(pwnd);
		pwnd->SetWindowPos(nullptr, -1, -1, rc.Width() - cx, rc.Height() - cy, SWP_NOZORDER | SWP_NOMOVE);
	}

	void MoveWindowBy(CWnd* pwnd, int cx, int cy)
	{
		auto rc = GetWindowRect(pwnd);
		pwnd->SetWindowPos(nullptr, rc.left + cx, rc.top + cy, -1, -1, SWP_NOZORDER | SWP_NOSIZE);
	}

	void MoveWindowTo(CWnd* pwnd, int x, int y)
	{
		pwnd->SetWindowPos(nullptr, x, y, -1, -1, SWP_NOZORDER | SWP_NOSIZE);
	}

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

	int AlignToTopLeft(CWnd * pwnd, std::initializer_list<CWnd*> windows)
	{
		if (pwnd == nullptr || !IsWindow(pwnd->GetSafeHwnd())) return -1;

		int aligncount = 0;
		auto parent = pwnd->GetParent();

		auto rect = GetControlRect(pwnd);

		for (auto pctl : windows) {
			if (pctl != nullptr && pctl->GetParent() == parent) {
				if (pctl->SetWindowPos(nullptr, rect.left, rect.top, -1, -1, SWP_NOZORDER | SWP_NOSIZE))
					++aligncount;
			}
		}
		return aligncount;
	}

	int ConvertStyleToAlignment(int style)
	{
		int align = 0;
		if ((style & (BS_CENTER)) == BS_CENTER)
			align |= DT_CENTER;
		else if (style & BS_LEFT)
			align |= DT_LEFT;
		else if (style & BS_RIGHT)
			align |= DT_RIGHT;

		if ((style & BS_MULTILINE) == 0 && ((style & BS_VCENTER) == BS_VCENTER))
			align |= DT_VCENTER | DT_SINGLELINE;
		else if (style & BS_TOP)
			align |= DT_TOP;
		else if (style & BS_BOTTOM)
			align |= DT_BOTTOM;
		return align;
	}

	void SizeToText(CButton & btn)
	{
		CString text;
		btn.GetWindowText(text);
		if (text.IsEmpty()) return;

		CRect rc;
		WindowToParentRect(&btn, &rc);
		CPaintDC dc(&btn);

		CRect rctext = rc;
		int align = ConvertStyleToAlignment(btn.GetStyle());
		dc.DrawText(text, &rctext, align | DT_CALCRECT);
		++rctext.right;
		++rctext.bottom;

		btn.MoveWindow(&rctext);
	}

	void SetImage(CWnd * wnd, CString imagepath)
	{
		auto btn = (CButton*)wnd;
		if (btn == nullptr) return;

		CImage img;
		if (FAILED(img.Load(imagepath)))
			return;

		btn->ModifyStyle(0, BS_BITMAP);
		btn->SetBitmap(img.Detach());
	}

	bool LoadImageResource(CImage & img, HINSTANCE hinst, CString type, UINT image)
	{
		img.Destroy();

		auto rsrc = FindResource(hinst, MAKEINTRESOURCE(image), type);
		auto size = SizeofResource(hinst, rsrc);
		auto data = LockResource(LoadResource(hinst, rsrc));
		auto hglobal = GlobalAlloc(GMEM_MOVEABLE, size);
		auto ptr = GlobalLock(hglobal);
		if (ptr) {
			CopyMemory(ptr, data, size);
			IStream * stream = nullptr;
			if (SUCCEEDED(CreateStreamOnHGlobal(ptr, FALSE, &stream))) {
				img.Load(stream);
			}
			GlobalUnlock(ptr);
		}
		GlobalFree(ptr);
		return img != nullptr;
	}

	HBITMAP LoadImageHandle(HINSTANCE hinst, CString type, UINT image)
	{
		CImage img;
		if (LoadImageResource(img, hinst, type, image))
			return img.Detach();

		return nullptr;
	}

	bool LoadPNG(CBitmap & bmp, HINSTANCE hinst, UINT image)
	{
		bmp.DeleteObject();

		CImage img;
		if (!LoadImageResource(img, hinst, "PNG", image))
			return false;

		bmp.Attach(img.Detach());
		return true;
	}

	// Another Windows API quirk. If WS_BORDER isnt specfied in the resource, adding the style won't make it visible.
	// The window has to be told to redraw the frame.
	void AddThinBorder(CWnd * wnd)
	{
		wnd->ModifyStyle(0, WS_BORDER);
		wnd->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
	}

	void InvalidateThinBorder(CWnd & wnd)
	{
		CRect rcborder;
		WindowToParentRect(&wnd, &rcborder);
		rcborder.InflateRect(2, 2);
		wnd.GetParent()->InvalidateRect(&rcborder);
	}

	void InvalidateChild(CWnd * pwnd)
	{
		if (pwnd == nullptr) return;

		pwnd->Invalidate();
		auto parent = pwnd->GetParent();
		if (parent != nullptr) {
			auto rc = GetControlRect(pwnd);
			parent->InvalidateRect(rc);
		}
	}

	bool IsKeyDown(int vkey)
	{
		return (GetAsyncKeyState(vkey) & 0x8000) != 0;
	}

	void NotifyParent(CWnd * control, UINT uNotifyMessage)
	{
		NMHDR hdr;
		hdr.code = uNotifyMessage;
		hdr.hwndFrom = control->GetSafeHwnd();
		hdr.idFrom = control->GetDlgCtrlID();
		control->GetParent()->SendMessage(WM_NOTIFY, (WPARAM)hdr.idFrom, (LPARAM)&hdr);
	}

	void CommandParent(CWnd * control, UINT uNotifyMessage)
	{
		auto idFrom = control->GetDlgCtrlID();
		auto hwnd = control->GetSafeHwnd();
		control->GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(idFrom, uNotifyMessage), (LPARAM)hwnd);
	}

	CString LoadStringResource(UINT id, CString defaulttext)
	{
		CString str;
		str.LoadString(id);
		if (str.IsEmpty()) str = defaulttext;
		return str;
	}

	CString GetFontFace(CFont* font)
	{
		if (font != nullptr) {
			LOGFONT lf = { 0 };
			font->GetLogFont(&lf);
			return lf.lfFaceName;
		}
		return CString();
	}

	bool CreateResizedFont(CFont* origfont, CFont& newfont, float sizefactor)
	{
		LOGFONT lf = { 0 };
		origfont->GetLogFont(&lf);

		lf.lfHeight = (int)(sizefactor * (float)lf.lfHeight);
		newfont.DeleteObject();
		auto result = newfont.CreateFontIndirect(&lf);
		return result == TRUE;
	}

	BOOL MakePointFont(CString name, int pointsize, CFont & font)
	{
		font.DeleteObject();
		return font.CreatePointFont(pointsize * 10, name);
	}

	BOOL GetResizedFont(CFont * origfont, int pointsize, CFont & newfont)
	{
		if (origfont == nullptr) return FALSE;

		LOGFONT lf = { 0 };
		origfont->GetLogFont(&lf);
		lf.lfHeight = 10 * pointsize; // this is a point size, in tenths of a point

		newfont.DeleteObject();
		return newfont.CreatePointFontIndirect(&lf);
	}

	int PointsToHeight(int points)
	{
		auto dc = GetDC(NULL);
		auto height = -MulDiv(points, GetDeviceCaps(dc, LOGPIXELSY), 72);
		ReleaseDC(NULL, dc);
		return height;
	}

	int PointsToHeight(int points, HDC dc)
	{
		return -MulDiv(points, GetDeviceCaps(dc, LOGPIXELSY), 72);
	}

	int HeightToPoints(int height)
	{
		auto dc = GetDC(NULL);
		auto points = MulDiv(-height, 72, GetDeviceCaps(dc, LOGPIXELSY));
		ReleaseDC(NULL, dc);
		return points;
	}

	int HeightToPoints(int height, HDC dc)
	{
		return MulDiv(-height, 72, GetDeviceCaps(dc, LOGPIXELSY));
	}

	HMODULE GetExternalModule(CString editorpath)
	{
		if (!Paths::FileExists(editorpath))
			return NULL;

		auto module = LoadLibrary(editorpath);
		return module;
	}

	BOOL AbortInitDialog(CDialog * dlg, CString message)
	{
		AfxMessageBox(message, MB_ICONERROR);
		dlg->EndDialog(IDABORT);
		return TRUE;
	}

	bool AbortFail(CDialog * dlg, CString message)
	{
		AfxMessageBox(message, MB_ICONERROR);
		dlg->EndDialog(IDABORT);
		return false;
	}

	int Done(int code, CString message)
	{
		AfxMessageBox(message);
		return code;
	}

	bool Fail(CString message)
	{
		AfxMessageBox(message, MB_ICONERROR);
		return false;
	}

	LRESULT SendNotificationToParent(CWnd * wnd, UINT notifyid)
	{
		if (wnd == nullptr || wnd->GetParent() == nullptr)
			return 0;
		return wnd->GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(wnd->GetDlgCtrlID(), notifyid), (LPARAM)wnd->GetSafeHwnd());
	}

	LRESULT SendBnClickedToParent(CWnd * wnd)
	{
		return SendNotificationToParent(wnd, BN_CLICKED);
	}

	LRESULT SendLbnSelchangeToParent(CWnd * wnd)
	{
		return SendNotificationToParent(wnd, LBN_SELCHANGE);
	}

} // end namespace Ui