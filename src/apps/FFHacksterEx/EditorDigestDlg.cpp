// EditorDigestDlg.cpp : implementation file
//

#include "stdafx.h"
#include <FFBaseApp.h>
#include "EditorDigestDlg.h"
#include <imaging_helpers.h>
#include <io_functions.h>
#include <path_functions.h>
#include <ui_helpers.h>
#include <window_messages.h>

#include <Editors2.h>
#include <FFHacksterProject.h>
#include <LoadLibraryHandleScope.h>
#include <iterator>

using namespace Imaging;
using namespace Io;
using namespace Ui;


// CEditorDigestDlg dialog

IMPLEMENT_DYNAMIC(CEditorDigestDlg, CEditorWithBackground)

CEditorDigestDlg::CEditorDigestDlg(CWnd* pParent /*=NULL*/)
	: CEditorWithBackground(IDD, pParent)
{
}

CEditorDigestDlg::~CEditorDigestDlg()
{
}

void CEditorDigestDlg::DoDataExchange(CDataExchange* pDX)
{
	CEditorWithBackground::DoDataExchange(pDX);
	m_editorlist.Project = Project;
	m_editorlist.Links = &m_links;
	DDX_Control(pDX, IDC_LIST1, m_editorlist);
	DDX_Control(pDX, IDOK, m_okbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDCANCEL2, m_closebutton);
	DDX_Control(pDX, IDC_ED_BTN_UP, m_upbutton);
	DDX_Control(pDX, IDC_ED_BTN_DOWN, m_downbutton);
	DDX_Control(pDX, IDC_STATIC1, m_reorderstatic);
	DDX_Control(pDX, IDC_STATIC2, m_infostatic);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
	DDX_Control(pDX, IDC_BUTTON1, m_selbuiltinsbutton);
}


BEGIN_MESSAGE_MAP(CEditorDigestDlg, CEditorWithBackground)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CEditorDigestDlg::OnNmClickEditorList)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ED_BTN_UP, &CEditorDigestDlg::OnClickedEdBtnUp)
	ON_BN_CLICKED(IDC_ED_BTN_DOWN, &CEditorDigestDlg::OnClickedEdBtnDown)
	ON_BN_CLICKED(IDC_BUTTON1, &CEditorDigestDlg::OnClickedSelectBuiltin)
//	ON_WM_FONTCHANGE()
END_MESSAGE_MAP()


// CEditorDigestDlg message handlers

BOOL CEditorDigestDlg::OnInitDialog()
{
	//if (Project == nullptr)
	//	return AbortInitDialog(this, "No project was specified.");

	CEditorWithBackground::OnInitDialog();
	if (InitFailed())
		return TRUE;
	if (Project == nullptr)
		return FailInitAndReturn(IDABORT, "No project was provided to the editor settings dialog.");

	m_editorlist.SetWindowPos(nullptr, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE);

	m_inplacecombo.Create(m_inplacecombo.IDD, &m_editorlist);

	m_editorlist.Links = &m_links;

	m_editorlist.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_editorlist.InsertColumn(COL_NAME, "Editor", LVCFMT_LEFT, 112);
	m_editorlist.InsertColumn(COL_STAT, "Status", LVCFMT_LEFT, 112);
	m_editorlist.InsertColumn(COL_DISP, "Displayname", LVCFMT_LEFT, 144);
	m_editorlist.InsertColumn(COL_LEV, "Level", LVCFMT_LEFT, 96);
	m_editorlist.InsertColumn(COL_SRC, "Source", LVCFMT_LEFT, -1);
	m_editorlist.InsertColumn(COL_FOLDER, "Source Path", LVCFMT_LEFT, -1);

	auto infos = Editors2::GetAllEditorInfos(Project->ProjectPath);
	auto links = Editors2::BuildEditorLinks(infos);

	Editors2::SelectCurrentEditorLinks(EditorInfos, links);
	m_links = links;

	// Add the live editors first
	Editors2::sEditorLinkVector vselected;
	for (const auto & ed : EditorInfos) {
		auto iter = std::find_if(cbegin(links), cend(links), [ed](const auto & li) {
			auto * pnode = li.GetSelectedNode();
			return pnode != nullptr && ed.name == pnode->name;
		});
		if (iter != cend(links)) {
			auto & link = *iter;
			auto linkindex = std::distance(cbegin(links), iter);
			auto index = m_editorlist.InsertItem(m_editorlist.GetItemCount(), link.name);
			m_editorlist.SetItemData(index, linkindex); // this is the index into m_links
			m_editorlist.SetItemText(index, COL_NAME, link.name);
			UpdateRow(index);
			vselected.push_back(link);
		}
	}

	// Then wdd the rest
	for (const auto & link : links) {
		if (std::find_if(cbegin(vselected), cend(vselected), [link](const Editors2::sEditorLink & li) { return link.name == li.name; }) == cend(vselected)) {
			auto iter = std::find(cbegin(links), cend(links), link);
			auto linkindex = std::distance(cbegin(links), iter);
			auto index = m_editorlist.InsertItem(m_editorlist.GetItemCount(), link.name);
			m_editorlist.SetItemData(index, linkindex); // this is the index into m_links
			m_editorlist.SetItemText(index, COL_NAME, link.name);
			UpdateRow(index);
		}
	}
	// END REFACTOR

	if (m_editorlist.GetItemCount() > 0) {
		// Select the first item, then autosize specific columns based on their content
		m_editorlist.SetSelectionMark(0);
		m_editorlist.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		UpdateColumnWidth(COL_DISP);
		UpdateColumnWidth(COL_SRC);
		UpdateColumnWidth(COL_FOLDER);
	}

	m_banner.Set(this, RGB(0, 0, 0), RGB(255, 0, 255), "Editors");

	CenterToParent(this);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditorDigestDlg::OnOK()
{
	Editors2::sEditorInfoVector invalideditors;

	// Write the editors in the order prescribed by the list control
	Editors2::sEditorInfoVector neweditors;
	for (auto index = 0; index < m_editorlist.GetItemCount(); ++index) {
		auto linkindex = m_editorlist.GetItemData(index);
		auto & link = m_links[linkindex];
		auto selnode = link.GetSelectedNode();
		if (selnode != nullptr && selnode->show)
			neweditors.push_back(*selnode);
	}

	if (!invalideditors.empty()) {
		CString msg;
		msg.Format("%u editors were invalid and will be removed from the editors list:", invalideditors.size());
		for (auto & ed : invalideditors)
			msg.AppendFormat("\n%s (%s)", (LPCSTR)ed.displayname, (LPCSTR)Paths::GetFileStem(ed.modulepath));
		AfxMessageBox(msg);
	}

	// Warn that editors are being removed
	auto removedstatuscount = m_links.size() - neweditors.size();
	if (removedstatuscount > 0) {
		CString msg;
		msg.Format("%u editor(s) will be removed the list.\nDo you wish to continue?", removedstatuscount);
		auto result = AfxMessageBox(msg, MB_OKCANCEL);
		if (result != IDOK) return;
	}

	EditorInfos = neweditors;

	CEditorWithBackground::OnOK();
}

void CEditorDigestDlg::InitFonts()
{
	CEditorWithBackground::InitFonts();
	auto actionfont = AfxGetFFBaseApp()->GetFonts().GetActionFont();
	if (actionfont != nullptr) {
		m_editorlist.SetFont(actionfont);
		m_reorderstatic.SetFont(actionfont);
		m_infostatic.SetFont(actionfont);
	}

	LOGFONT lf = { 0 };
	GetFont()->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	VERIFY(m_boldfont.CreateFontIndirect(&lf));
}

void CEditorDigestDlg::UpdateColumnWidth(int col)
{
	Ui::AutosizeColumn(m_editorlist, col, m_editorlist.GetBoldFont());
}

void CEditorDigestDlg::UpdateRow(int item)
{
	auto & link = GetLink(item);
	auto & node = link.nodes[link.selindex];

	CString status = Editors2::GetEditorLiveStatus(node.show);
	CString level = Editors2::GetLevelFromPath(*Project, node.modulepath);

	m_editorlist.SetItemText(item, COL_STAT, status);
	m_editorlist.SetItemText(item, COL_DISP, node.displayname);
	m_editorlist.SetItemText(item, COL_LEV, level);
	m_editorlist.SetItemText(item, COL_SRC, Paths::GetFileName(node.modulepath));
	m_editorlist.SetItemText(item, COL_FOLDER, Paths::GetDirectoryPath(node.modulepath));
}

void CEditorDigestDlg::Edit(int item, int subitem)
{
	auto linkindex = m_editorlist.GetItemData(item);
	const auto OnComboOKSource = [this,item,subitem,linkindex](int selindex, DWORD_PTR seldata) -> bool {
		UNREFERENCED_PARAMETER(seldata);
		m_links[linkindex].selindex = (size_t)selindex;
		RedrawScope redraw(&m_editorlist);
		UpdateRow(item);
		if (subitem == COL_SRC) {
			UpdateColumnWidth(subitem);
			UpdateColumnWidth(COL_DISP);
			UpdateColumnWidth(COL_FOLDER);
		}
		return true;
	};

	switch (subitem) {
	case COL_SRC:
		{
			auto & link = m_links[linkindex];
			if (link.nodes.size() < 2) break;

			PrepareInplaceCombo(m_editorlist, item, subitem, m_inplacecombo, OnComboOKSource);

			CClientDC dc(this);
			auto * oldfont = dc.SelectObject(&m_boldfont);
			int maxwidth = 16;

			//N.B. - using MapDialogRect to map dialog units so the combo drop list
			//		will have the correct width. Not doing this results in a short
			//		drop list that truncates the text. I'm also adding a pixel of
			//		height just to make the text a little easier to read.
			auto height = dc.GetTextExtent("Iy").cx + 1;
			CRect rc(0, 0, 1, height);
			MapDialogRect(&rc);
			m_inplacecombo.m_combo.SetItemHeight(0, rc.Height());

			for (auto & node : link.nodes) {
				CString status = Editors2::GetEditorLiveStatus(node.show);
				CString level = Editors2::GetLevelFromPath(*Project, node.modulepath);

				CString fmt;
				if (level == EDITORLEVEL_BUILTIN) {
					fmt.Format(" %s as '%s' (built-in) ", (LPCSTR)status, (LPCSTR)node.displayname);
				}
				else {
					fmt.Format(" %s as '%s' (%s - %s) ", (LPCSTR)status, (LPCSTR)node.displayname,
						(LPCSTR)level, (LPCSTR)Paths::GetFileName(node.modulepath));
				}

				m_inplacecombo.m_combo.AddString(fmt);

				auto testwidth = dc.GetTextExtent(fmt).cx;
				rc.SetRect(0, 0, testwidth, 1);
				MapDialogRect(&rc);
				auto newwidth = rc.Width();
				if (newwidth > maxwidth) maxwidth = newwidth;
			}
			m_inplacecombo.SetDropWidth(maxwidth);

			if (link.selindex < link.nodes.size()) m_inplacecombo.m_combo.SetCurSel((int)link.selindex);
			dc.SelectObject(oldfont);
		}
		break;
	}
}

Editors2::sEditorLink& CEditorDigestDlg::GetLink(int item)
{
	auto linkindex = m_editorlist.GetItemData(item);
	return m_links[linkindex];
}

void CEditorDigestDlg::OnNmClickEditorList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pnm = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pnm->iItem != -1) {
		Edit(pnm->iItem, pnm->iSubItem);
		*pResult = 0;
	}
}

void CEditorDigestDlg::OnPaint()
{
	CPaintDC dc(this);
	m_banner.Render(dc, 8, 8);
}

void CEditorDigestDlg::OnClickedEdBtnUp()
{
	int oldsel = m_editorlist.GetSelectionMark();
	if (oldsel == -1) return;

	int newsel = __max(0, (oldsel - 1));
	if (oldsel != newsel) {
		auto olddata = m_editorlist.GetItemData(oldsel);
		auto newdata = m_editorlist.GetItemData(newsel);
		m_editorlist.SetItemData(oldsel, newdata);
		m_editorlist.SetItemData(newsel, olddata);
		m_editorlist.SetSelectionMark(newsel);
		UpdateRow(oldsel);
		UpdateRow(newsel);
		//m_editorlist.Invalidate(); //REFACTOR - only invalidate the rows that switched
		RECT rc = { 0 };
		m_editorlist.GetItemRect(oldsel, &rc, LVIR_BOUNDS);
		m_editorlist.InvalidateRect(&rc);
		m_editorlist.GetItemRect(newsel, &rc, LVIR_BOUNDS);
		m_editorlist.InvalidateRect(&rc);
	}
}

void CEditorDigestDlg::OnClickedEdBtnDown()
{
	int oldsel = m_editorlist.GetSelectionMark();
	if (oldsel == -1) return;

	int newsel = __min(m_editorlist.GetItemCount() - 1, (oldsel + 1));
	if (oldsel != newsel) {
		auto olddata = m_editorlist.GetItemData(oldsel);
		auto newdata = m_editorlist.GetItemData(newsel);
		m_editorlist.SetItemData(oldsel, newdata);
		m_editorlist.SetItemData(newsel, olddata);
		m_editorlist.SetSelectionMark(newsel);
		UpdateRow(oldsel);
		UpdateRow(newsel);
		//m_editorlist.Invalidate(); //REFACTOR - only invalidate the rows that switched
		RECT rc = { 0 };
		m_editorlist.GetItemRect(oldsel, &rc, LVIR_BOUNDS);
		m_editorlist.InvalidateRect(&rc);
		m_editorlist.GetItemRect(newsel, &rc, LVIR_BOUNDS);
		m_editorlist.InvalidateRect(&rc);
	}
}

void CEditorDigestDlg::OnClickedSelectBuiltin()
{
	static const auto IsBuiltin = [](const Editors2::sEditorInfo & node) { return node.modulepath == EDITORPATH_BUILTIN; };
	static const auto IsRemoved = [](const Editors2::sEditorInfo & node) { return !node.show; };
	static const std::function<bool(const Editors2::sEditorInfo & node)> preds[] = { IsBuiltin, IsRemoved };

	m_editorlist.SetRedraw(FALSE);

	for (auto i = 0; i < m_editorlist.GetItemCount(); ++i) {
		auto index = m_editorlist.GetItemData(i);
		auto & link = m_links[index];
		for (auto & pred : preds) {
			auto iter = std::find_if(cbegin(link.nodes), cend(link.nodes), pred);
			if (iter != cend(link.nodes)) {
				link.selindex = std::distance(cbegin(link.nodes), iter);
				UpdateRow(i);
				break;
			}
		}
	}
	m_editorlist.SetRedraw(TRUE);
}