// FFHacksterDlg.cpp : implementation file

#include "stdafx.h"
#include "FFHackster.h"
#include "FFHacksterDlg.h"
#include "resource.h"
#include <algorithm>
#include <functional>
#include <map>

#include "FilePathRestorer.h"
#include <AppSettings.h>

#include <path_functions.h>
#include <general_functions.h>
#include <imaging_helpers.h>
#include <io_functions.h>
#include <LoadLibraryHandleScope.h>
#include <ram_value_functions.h>
#include <string_functions.h>
#include <ui_helpers.h>
#include <ui_prompts.h>
#include <window_messages.h>

#include <DlgEditScalars.h>
#include <DlgEditStrings.h>
#include <Minimap.h>
#include <IProgress.h>
#include <Loading.h>
#include <WaitingDlg.h>
#include <ProjectLoader.h>

#include "About.h"
#include "DlgAppSettings.h"
#include "DlgProjectSettings.h"
#include "DlgImport.h"
#include "EditorDigestDlg.h"

#include <Armor.h>
#include <Attack.h>
#include <Battle.h>
#include <Classes.h>
#include <Enemy.h>
#include <Magic.h>
#include <Shop.h>
#include <SpriteDialogue2.h>
#include <StartingItems.h>
#include <Text.h>
#include <Weapon.h>
#include <Maps.h>
#include <OverworldMap.h>
#include <PartySetup.h>

#include <BattleEditorSettingsDlg.h>
#include <ClassesEditorSettingsDlg.h>
#include <EnemyEditorSettingsDlg.h>
#include <MagicEditorSettingsDlg.h>
#include <SpriteDialogueSettingsDlg.h>
#include <TextEditorSettingsDlg.h>


using namespace Imaging;
using namespace Io;
using namespace Strings;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFFHacksterDlg dialog

#define BaseClass CDialogWithBackground

CFFHacksterDlg::CFFHacksterDlg(CWnd* pParent /*= nullptr */)
	: BaseClass(CFFHacksterDlg::IDD, pParent)
	, m_bRequestReload(false)
	, m_loader(m_proj)
{
}

bool CFFHacksterDlg::StartEditor()
{
	if (InitialEditorName.IsEmpty()) {
		ErrorHere << "Can't start an editor because no editor name was specified." << std::endl;
		return false;
	}

	auto infos = m_serializer.ReadEditorInfo(InitialEditorName);

	if (!infos.empty()) {
		Editors2::CEditor editor = CreateEditor(infos.front());

		Editors2::EditErrCode ec = Editors2::EditErrCode::Failure;
		CString message;
		std::tie(ec, message) = Editors2::ExtractResponse(editor.Edit(m_proj.ProjectPath, AllocBytes));

		bool success = Editors2::IsResponseSuccess(ec);
		if (success) AfxMessageBox("The editor " + InitialEditorName + " failed.\n" + message);
		return success;
	}

	AfxMessageBox("Didn't find an editor named '" + InitialEditorName + "'.", MB_ICONERROR);
	return false;
}

void CFFHacksterDlg::ReadAppSettings()
{
	AppStgs->Read();
}

void CFFHacksterDlg::WriteAppSettings()
{
	AppStgs->Write();
}

void CFFHacksterDlg::WriteMru()
{
	AppStgs->WriteMru();
}

bool CFFHacksterDlg::LoadEditorList(Editors2::CEditorVector & editors)
{
	bool loaded = false;
	try {
		// Initialize the editors when they're loaded.
		auto failures = InitializeExternalEditors(Editors, m_proj.ProjectPath);
		if (!failures.empty()) {
			AfxMessageBox("The following failures occurred while initializing external editors:\n-" + join(failures, "\n-")
				+ "\nThe external editors will have to be selected menually.");
		}
		//REFACTOR - don't fail here, let the internal editors load (even if they're incorrect).
		// Consider invoking the editor dialog at this point.
		//else {
			ClearDynaButtons();
			CreateDynaButtons(editors);
			loaded = true;
		//}
	}
	catch (std::exception & ex) {
		ErrorHere << "An error occurred while attempting to load the editors: "<< ex.what() << std::endl;
	}
	catch (...) {
		ErrorHere << "An unknown exception prevented the editors from loading." << std::endl;
	}
	return loaded;
}

void CFFHacksterDlg::ClearDynaButtons()
{
	for (auto btn : m_dynabuttons) {
		if (IsWindow(m_tooltips.GetSafeHwnd())) m_tooltips.DelTool(btn);
		delete btn;
	}
	m_dynabuttons.clear();
}

void CFFHacksterDlg::CreateDynaButtons(const Editors2::CEditorVector & editors)
{
	CWnd * frame = &m_dynabuttonframe;

	CRect rcframe;
	frame->GetWindowRect(&rcframe);
	ScreenToClient(&rcframe);

	// get the extent of the widest string and use it as a basis for all of them
	CPaintDC dc(this);
	//DEVNOTE - if using a different font from the dialog's, select it in and out for this block
	auto oldfont = dc.SelectObject(&m_actionbuttonfont);
	CSize fixedsize = { 0,0 };
	for (size_t i = 0; i < editors.size(); ++i) {
		const auto & editor = editors[i];
		auto size = dc.GetTextExtent(editor.displayname);
		if (size.cx > fixedsize.cx) fixedsize.cx = size.cx;
		if (size.cy > fixedsize.cy) fixedsize.cy = size.cy;
	}
	dc.SelectObject(oldfont);

	const CSize padding = { 4,4 };
	const CSize buttonpadding = { 16,16 };
	const CPoint base = rcframe.TopLeft();
	CPoint pos = base + padding;
	CWnd * prevz = frame;
	CFont * buttonfont = &m_actionbuttonfont;

	const UINT styles = WS_VISIBLE | WS_BORDER | WS_TABSTOP | BS_FLAT | BS_OWNERDRAW | BS_CENTER | BS_VCENTER;
	for (auto loop = 0u; loop < editors.size(); ++loop) {
		const auto & editor = editors[loop];

		//DEVNOTE - the IDs aren't guaranteed to be contiguous since removed editors get no button
		ASSERT(editor.live); // dead editors have unpredictable behavior

		if ((pos.x + fixedsize.cx + buttonpadding.cx) > rcframe.right) {
			pos.x = base.x + padding.cx;
			pos.y += fixedsize.cy + buttonpadding.cy + padding.cy;
		}

		CRect rcbutton = { pos.x,pos.y, pos.x + fixedsize.cx + buttonpadding.cx, pos.y + fixedsize.cy + buttonpadding.cy };

		auto index = (unsigned int)m_dynabuttons.size();
		auto newbutton = new CClearButton();
		newbutton->Create(editor.displayname, styles, rcbutton, this, IDC_DYNABUTTON_EDITORS + index);
		newbutton->SetFont(buttonfont);
		newbutton->SetWindowPos(prevz, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		newbutton->ModifyStyleEx(0, WS_EX_TRANSPARENT);
		SetWindowLongPtr(newbutton->GetSafeHwnd(), GWLP_USERDATA, loop); // use editors[loop] for this button

		pos.x += rcbutton.Width() + padding.cx;

		m_dynabuttons.push_back(newbutton);
		m_tooltips.AddTool(newbutton, editor.description);
		//m_tooltips.AddTool(newbutton, LPSTR_TEXTCALLBACK); //N.B. restore if dynamic tips are desired
		prevz = newbutton;
	}

	if (!m_dynabuttons.empty()) {
		// Calculate the offset needed to center the controls we just generated.
		CRect btnunion;
		for (auto btn : m_dynabuttons) {
			CRect rc;
			btn->GetWindowRect(&rc);
			ScreenToClient(&rc);
			btnunion.UnionRect(&btnunion, &rc);
		}

		CRect dlgclient;
		GetClientRect(&dlgclient);
		int offset = ((dlgclient.Width() - btnunion.Width()) / 2) - (base.x + padding.cx);

		// Now, center the windows by apply the offset (removing the base + padding used to start the position processing).
		for (auto btn : m_dynabuttons) {
			CRect rc;
			btn->GetWindowRect(&rc);
			ScreenToClient(&rc);
			rc.OffsetRect(offset, 0);
			btn->MoveWindow(&rc);
		}
	}
}

void CFFHacksterDlg::RepositionButtonsForROMProject()
{
	auto rccompile = GetControlRect(&m_compilebutton);
	auto rcrun = GetControlRect(&m_runbutton);
	int distance = rcrun.right - rccompile.right;
	MoveControlBy(&m_runbutton, -distance, 0);

	m_compilebutton.ShowWindow(SW_HIDE);

	int halfdist = distance / 2;
	auto buttons = std::vector<CButton*>{
		&m_importhacksterbutton, &m_revertbutton, &m_archivebutton, &m_clonebutton, &m_publishbutton, &m_runbutton
	};
	for (auto button : buttons) MoveControlBy(button, halfdist, 0);
}

void CFFHacksterDlg::EditBattles(CString action, CString params)
{
	try {
		if (action == "edit") {
			CBattle dlg;
			dlg.Project = &m_proj;
			dlg.DoModal();
		}
		else if (action == "rclick") {
			CBattleEditorSettingsDlg dlg(this);
			dlg.Project = &m_proj;
			dlg.DoModal();
		}
	}
	catch (std::exception& ex) {
		AfxMessageBox("An exception prevented the edit from completing:\n" + CString(ex.what()));
	}
	catch (...) {
		AfxMessageBox("An unknown exception prevented the operation from completing.");
	}
}

void CFFHacksterDlg::EditClasses(CString action, CString params)
{
	try {
		if (action == "edit") {
			CClasses dlg;
			dlg.Project = &m_proj;
			do {
				dlg.DoModal();
			} while (dlg.WantsToReload());
		}
		else if (action == "rclick") {
			CClassesEditorSettingsDlg dlg(this);
			dlg.m_proj = &m_proj;
			dlg.DoModal();
		}
	}
	catch (std::exception & ex) {
		AfxMessageBox("An exception prevented the edit from completing:\n" + CString(ex.what()));
	}
	catch (...) {
		AfxMessageBox("An unknown exception prevented the operation from completing.");
	}
}

void CFFHacksterDlg::EditEnemies(CString action, CString params)
{
	try {
		if (action == "edit") {
			CEnemy dlg;
			dlg.Project = &m_proj;
			dlg.DoModal();
		}
		else if (action == "rclick") {
			CEnemyEditorSettingsDlg dlg;
			dlg.Project = &m_proj;
			dlg.DoModal();
		}
	}
	catch (std::exception & ex) {
		AfxMessageBox("An exception prevented the edit from completing:\n" + CString(ex.what()));
	}
	catch (...) {
		AfxMessageBox("An unknown exception prevented the operation from completing.");
	}
}

void CFFHacksterDlg::EditMagic(CString action, CString params)
{
	try {
		if (action == "edit") {
			CMagic dlg(this);;
			dlg.Project = &m_proj;
			dlg.DoModal();
		}
		else if (action == "rclick") {
			CMagicEditorSettingsDlg dlg(this);
			dlg.m_proj = &m_proj;
			dlg.DoModal();
		}
	}
	catch (std::exception & ex) {
		AfxMessageBox("An exception prevented the edit from completing:\n" + CString(ex.what()));
	}
	catch (...) {
		AfxMessageBox("An unknown exception prevented the operation from completing.");
	}
}

void CFFHacksterDlg::EditText(CString action, CString params)
{
	try {
		if (action == "edit") {
			CText dlg(this);;
			dlg.Project = &m_proj;
			dlg.DoModal();
		}
		else if (action == "rclick") {
			CTextEditorSettingsDlg dlg(this);
			dlg.Project = &m_proj;
			dlg.DoModal();
		}
	}
	catch (std::exception & ex) {
		AfxMessageBox("An exception prevented the edit from completing:\n" + CString(ex.what()));
	}
	catch (...) {
		AfxMessageBox("An unknown exception prevented the operation from completing.");
	}
}

void CFFHacksterDlg::EditSpriteDialogue(CString action, CString params)
{
	try {
		if (action == "edit") {
			CSpriteDialogue2 dlg(m_proj, this);
			dlg.Project = &m_proj;
			dlg.Enloader = &m_loader;
			dlg.DoModal();
		}
		else if (action == "rclick") {
			CSpriteDialogueSettingsDlg dlg(this);
			dlg.m_proj = &m_proj;
			dlg.DoModal();
		}
	}
	catch (std::exception & ex) {
		AfxMessageBox("An exception prevented the edit from completing:\n" + CString(ex.what()));
	}
	catch (...) {
		AfxMessageBox("An unknown exception prevented the operation from completing.");
	}
}

void CFFHacksterDlg::PaintBanner(CDC & dc)
{
	CRect rc = GetControlRect(&m_cancelbutton);
	m_mainbanner.Render(dc, 16, 4, &rc);
}

void CFFHacksterDlg::PaintClient(CDC & dc)
{
	if (IsIconic()) {
		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, AfxGetFFBaseApp()->GetLargeIcon());
	}
	else {
		auto dcstate = dc.SaveDC();

		CRect rcclose = GetControlRect(&m_cancelbutton);
		CRect rcstill;
		m_mainbanner.Calc(dc, 16, 8, rcstill);

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

		// Draw the Editing project text right-aligned to the close button
		//REFACTOR - Make the Editing project text a right-aligned custom colored static control instead
		CString text;
		text.Format("Editing %s project", m_proj.IsRom() ? "ROM" : "Assembly");
		int rightedge = rcclose.right - (rcclose.Width() / 4);
		CRect rctext = { rightedge, rcstill.bottom - 4, rightedge, rcstill.bottom - 4 };
		dc.SelectObject(&m_bannerfont);
		dc.DrawText(text, &rctext, DT_CALCRECT | DT_RIGHT);
		rctext.OffsetRect(-rctext.Width(), -rctext.Height());
		dc.DrawText(text, &rctext, DT_NOCLIP | DT_RIGHT);

		//REFACTOR: make this a multiline, word-breaking custom static control
		// Draw the project path
		// To accomodate longer paths, instead of drawing text with NOCLIP, set the rectangle to be
		// centered at close to the window's width, and 3 times the height of the DC's font.
		// Drawing with the WORDBREAK and EDITCONTROL styles should wrap the path name as needed.
		int height = dc.GetTextExtent("Ay").cy;
		CRect rcclient = Ui::GetClientRect(this);
		CRect rctype = { rcstill.left, rcstill.bottom + 12, rcstill.left, rcstill.bottom + 12 };
		rctype.right = rcclient.right - rctype.left;
		rctype.bottom = rctype.top + (height * 3);
		dc.SetTextColor(m_mainbanner.End);
		dc.SelectObject(&m_bannerfont);
		dc.DrawText(m_proj.ProjectPath, &rctype, DT_LEFT | DT_WORDBREAK | DT_EDITCONTROL);

		dc.RestoreDC(dcstate);
	}
}

void CFFHacksterDlg::InitFonts()
{
	auto actionfont = AfxGetFFBaseApp()->GetFonts().GetActionFont();
	if (actionfont != nullptr) {
		LOGFONT lf = { 0 };
		actionfont->GetLogFont(&lf);
		m_actionbuttonfont.DeleteObject();
		m_actionbuttonfont.CreateFontIndirectA(&lf);
		m_aboutbutton.SetFont(actionfont);
		m_minbutton.SetFont(actionfont);
		m_cancelbutton.SetFont(actionfont);
		m_hoverdescstatic.SetFont(actionfont);
	}

	auto subfont = AfxGetFFBaseApp()->GetFonts().GetSubbannerFont();
	if (subfont != nullptr) {
		LOGFONT lf = { 0 };
		subfont->GetLogFont(&lf);
		m_bannerfont.DeleteObject();
		m_bannerfont.CreateFontIndirect(&lf);
	}
}

void CFFHacksterDlg::InitTooltips()
{
	if (!m_tooltips.Create(this)) {
		TRACE("Unable to create tooltips for the application.\r\n");
	}
	else {
		m_tooltips.AddTool(&m_importhacksterbutton, "Import information from other projects and FFHackster.dat files.");
		m_tooltips.AddTool(&m_revertbutton, "Revert to a stored clean copy.");
		m_tooltips.AddTool(&m_archivebutton, "Archive this project to a compressed file.");
		m_tooltips.AddTool(&m_clonebutton, "Clone this project to a new folder and project name.");
		m_tooltips.AddTool(&m_publishbutton, "Copy the compiled working image to the publishing destination.");
		m_tooltips.AddTool(&m_compilebutton, "Compile the source into a runnable working image.");
		m_tooltips.AddTool(&m_runbutton, "Run the working image in the selected external program.");
		m_tooltips.AddTool(&m_labelsbutton, "Edit the labels used throughout the editor.");
		m_tooltips.AddTool(&m_valuesbutton, "Edit the values (counts, addresses, etc.) used in various editors.");
		m_tooltips.AddTool(&m_rambutton, "Edit the values used to initialize RAM when the game starts.");
		m_tooltips.AddTool(&m_editorsbutton, "Set the order for editors used in this project.");
		m_tooltips.AddTool(&m_projsettingsbutton, "Edit Project-specific settings.");
		m_tooltips.AddTool(&m_appsettingsbutton, "Edit Application-wide settings.");
		m_tooltips.AddTool(&m_aboutbutton, "Display information about this version of the program.");
	}
}

void CFFHacksterDlg::InitStdButtonImages()
{
	if (m_proj.IsRom()) {
		RepositionButtonsForROMProject();
	}
}

void CFFHacksterDlg::UpdateSharedDisplayProperties()
{
	//REFACTOR - can this function be moved into ff1-utils CFFBaseApp or CFFBaseDlg? It's also used by GoldItems
	CStrikeCheck::RenderAsCheckbox = AppStgs->DisplayStrikeChecksAsNormalCheckboxes == true;
	CStrikeCheck::SharedCheckedColor = AppStgs->StrikeCheckedColor;
}

void CFFHacksterDlg::ReloadProject()
{
	m_bRequestReload = true;
	OnCancel(); // invokes OnCancel to close and (ironically) save changes
}

void CFFHacksterDlg::AbortProject()
{
	EndDialog(IDABORT);
}

Editors2::CEditor CFFHacksterDlg::CreateEditor(const Editors2::sEditorInfo & info)
{
	//N.B. - if an invalid editor is created, it will throw an exception when invoked
	Editors2::CEditor editor(info);
	if (editor.modulepath == EDITORPATH_BUILTIN)
		editor.implfunc = GetBuiltinEditorFunc();
	return editor;
}

Editors2::CEditorVector CFFHacksterDlg::CreateEditors(const Editors2::sEditorInfoVector & infos)
{
	Editors2::CEditorVector editors;
	for (const auto & info : infos) editors.push_back(CreateEditor(info));
	return editors;
}

Editors2::EditorImplFunc CFFHacksterDlg::GetBuiltinEditorFunc()
{
	Editors2::EditorImplFunc func = [this](CString command, CString action, CString params) -> bool {
		if (command == "edit")
			return this->InvokeInternalEditor(action);
		if (command == "rclick")
			return this->InvokeInternalContextMenu(action, params); // add a new function if desired to handle context menus
		return false;
	};
	return func;
}

char * CFFHacksterDlg::AllocBytes(int64_t length)
{
	return new char[(size_t)length];
}

void CFFHacksterDlg::InvokeEditor(const Editors2::CEditor & editor)
{
	auto message = editor.Edit(m_proj.ProjectPath, AllocBytes);

	if (message == "reload")
		ReloadProject();
	else if (!message.IsEmpty())
		AfxMessageBox("No editor was found for '" + editor.displayname + "'.\n" + message, MB_ICONERROR);
}

bool CFFHacksterDlg::InvokeInternalEditor(CString editorcmd)
{
	#define DOINVOKE(cmd,func) if (editorcmd == cmd) return invokeEdit(&CFFHacksterDlg::func)

	auto invokeEdit = [this](void (CFFHacksterDlg::*func)()) { (this->*func)(); return true; };

	DOINVOKE(ARMOREDIT,    OnArmor);
	DOINVOKE(ATTACKSEDIT,  OnAttack);
	DOINVOKE(BATTLESEDIT,  OnBattle);
	DOINVOKE(CLASSESEDIT,  OnClasses);
	DOINVOKE(ENEMIESEDIT,  OnEnemy);
	DOINVOKE(SHOPSEDIT,    OnShop);
	DOINVOKE(MAGICEDIT,    OnMagic);
	DOINVOKE(WEAPONSEDIT,  OnWeapon);
	DOINVOKE(TEXTEDIT,     OnText);
	DOINVOKE(STARTITEMSEDIT, OnStartingItems);
	DOINVOKE(DIALOGUEEDIT, OnDialogue);
	DOINVOKE(STDMAPSEDIT,  OnMaps);
	DOINVOKE(OVERWORLDEDIT, OnOverworldmap);
	DOINVOKE(PARTYSETUPEDIT, OnPartySetup);

	return false;
}

bool CFFHacksterDlg::InvokeInternalContextMenu(CString editcommand, CString paramstring)
{
	#define DOACTION(cmd,func) if (cmd == editcommand) { func("rclick", paramstring); return true; }

	DOACTION(BATTLESEDIT, EditBattles);
	DOACTION(CLASSESEDIT, EditClasses);
	DOACTION(ENEMIESEDIT, EditEnemies);
	DOACTION(MAGICEDIT, EditMagic);
	DOACTION(TEXTEDIT, EditText);
	DOACTION(DIALOGUEEDIT, EditSpriteDialogue);

	return false;
}

void CFFHacksterDlg::DoDataExchange(CDataExchange* pDX)
{
	BaseClass::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FFH_BTN_IMPORTFFHDAT, m_importhacksterbutton);
	DDX_Control(pDX, IDC_FFH_BTN_REVERT, m_revertbutton);
	DDX_Control(pDX, IDC_FFH_BTN_PUBLISH, m_publishbutton);
	DDX_Control(pDX, IDC_FFH_BTN_COMPILE, m_compilebutton);
	DDX_Control(pDX, IDC_FFH_BTN_RUN, m_runbutton);
	DDX_Control(pDX, IDABOUT, m_aboutbutton);
	DDX_Control(pDX, IDC_FFH_BTN_LABELS, m_labelsbutton);
	DDX_Control(pDX, IDC_FFH_BTN_VALUES, m_valuesbutton);
	DDX_Control(pDX, IDC_FFH_BTN_RAM, m_rambutton);
	DDX_Control(pDX, IDC_FFH_BTN_APPSETTINGS, m_appsettingsbutton);
	DDX_Control(pDX, IDC_FFH_BTN_PROJSETTINGS, m_projsettingsbutton);
	DDX_Control(pDX, IDC_FFH_BTN_MIN, m_minbutton);
	DDX_Control(pDX, IDCANCEL, m_cancelbutton);
	DDX_Control(pDX, IDC_FFH_BTN_EDITORS, m_editorsbutton);
	DDX_Control(pDX, IDC_STATIC_EDITORDESC, m_hoverdescstatic);
	DDX_Control(pDX, IDC_FFH_BTN_ARCHIVEPROJECT, m_archivebutton);
	DDX_Control(pDX, IDC_BUTTON1, m_clonebutton);
	DDX_Control(pDX, IDC_FRAME_CONTENT, m_dynabuttonframe);
	DDX_Control(pDX, IDHELPBOOK, m_helpbookbutton);
}


BEGIN_MESSAGE_MAP(CFFHacksterDlg, BaseClass)
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(FFTTM_SHOWDESC, &CFFHacksterDlg::OnFfttmShowDesc)
	ON_BN_CLICKED(IDC_FFH_BTN_IMPORTFFHDAT, &CFFHacksterDlg::OnImportHacksterDAT)
	ON_BN_CLICKED(IDC_FFH_BTN_REVERT, &CFFHacksterDlg::OnRevertProject)
	ON_BN_CLICKED(IDC_FFH_BTN_PUBLISH, &CFFHacksterDlg::OnPublishProject)
	ON_BN_CLICKED(IDC_FFH_BTN_COMPILE, &CFFHacksterDlg::OnCompileProject)
	ON_BN_CLICKED(IDC_FFH_BTN_RUN, &CFFHacksterDlg::OnRunClicked)
	ON_BN_CLICKED(IDC_FFH_BTN_LABELS, &CFFHacksterDlg::EditProjectLabels)
	ON_BN_CLICKED(IDC_FFH_BTN_VALUES, &CFFHacksterDlg::EditProjectValues)
	ON_BN_CLICKED(IDC_FFH_BTN_RAM, &CFFHacksterDlg::EditProjectRamValues)
	ON_BN_CLICKED(IDC_FFH_BTN_APPSETTINGS, &CFFHacksterDlg::EditAppSettings)
	ON_BN_CLICKED(IDC_FFH_BTN_PROJSETTINGS, &CFFHacksterDlg::EditProjectSettings)
	ON_BN_CLICKED(IDABOUT, &CFFHacksterDlg::OnAbout)
	ON_BN_CLICKED(IDC_FFH_BTN_EDITORS, &CFFHacksterDlg::EditProjectEditorsList)
	ON_BN_CLICKED(IDC_FFH_BTN_ARCHIVEPROJECT, &CFFHacksterDlg::OnArchiveProject)
	ON_BN_CLICKED(IDC_FFH_BTN_MIN, &CFFHacksterDlg::OnMinimizeButton)
	ON_BN_CLICKED(IDC_BUTTON1, &CFFHacksterDlg::OnCloneProject)
	ON_NOTIFY_RANGE(NM_RCLICK, IDC_DYNABUTTON_EDITORS, IDC_DYNABUTTON_EDITORS + 100, &CFFHacksterDlg::OnNmRclickActionButton)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_DYNABUTTON_EDITORS, IDC_DYNABUTTON_EDITORS + 100, &CFFHacksterDlg::OnBnClickedActionButton)
	ON_WM_MOVE()
	//ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &CFFHacksterDlg::OnTtnNeedText) //DEVNOTE - restore if dynamic tips are desired
	//ON_REGISTERED_MESSAGE(RWM_FFHACKSTEREX_SHOWWAITMESSAGE, &CFFHacksterDlg::OnFFHacksterExShowWaitMessage) //DEVNOTE - not implemented
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFFHacksterDlg message handlers

BOOL CFFHacksterDlg::OnInitDialog()
{
	ReadAppSettings();

	BaseClass::OnInitDialog();
	if (m_initfailed)
		return TRUE;

	ShowWindow(SW_HIDE);
	Ui::CenterToParent(this);

	m_bRequestReload = false;

	if (ExitAction.IsEmpty())
		ExitAction = "prompt";

	if (!Paths::FileExists(ProjectFile)) {
		EndDialog(IDCANCEL);
		return FALSE;
	}

	m_pmutex = new CMutex(TRUE, Paths::MutexNameFromPath(ProjectFile));
	m_plock = new CSingleLock(m_pmutex);
	if (!m_plock->Lock(2000)) {
		AfxMessageBox("Ensure that another instance of the application doesn't already have this file open.");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	SetIcon(AfxGetFFBaseApp()->GetLargeIcon(), TRUE);   // Load the Icon  :)
	SetIcon(AfxGetFFBaseApp()->GetSmallIcon(), FALSE);

	CWaitCursor wait;
	CLoading progress;
	progress.Init(0, 6, 1, this); // initial steps, substeps mught add more.

	// Load the project
	try {
		wait.Restore();
		progress.StepAndProgressText("Finishing project pre-load setup...");
		progress.SetWindowText("Loading project...");

		m_proj.AppSettings = AppStgs;

		auto loadresult = LoadProject(m_proj, ProjectFile, &progress);
		if (!loadresult) {
			AfxMessageBox(loadresult.value, MB_ICONERROR);
			EndDialog(IDABORT);
			return FALSE;
		}

		m_loader.Init();
	}
	catch (std::exception & ex) {
		CString msg;
		msg.Format("An unexpected exception while loading the project.\n%s\n\nThe operation cannot continue.", ex.what());
		AfxMessageBox(msg, MB_ICONERROR);
		EndDialog(IDABORT);
		return FALSE;
	}
	catch (...) {
		AfxMessageBox("An unexpected exception while loading the project.\nThe operation cannot continue.", MB_ICONERROR);
		EndDialog(IDABORT);
		return FALSE;
	}

	// Load editing resources, controls, and associated data
	wait.Restore();

	//REFACTOR - return IDOK if an editor is successfully invoked, not IDCANCEL
	//if (!InitialEditorName.IsEmpty() && StartEditor()) {
	//	EndDialog(IDCANCEL);
	if (!InitialEditorName.IsEmpty()) {
		progress.StepAndProgressText("Done");
		progress.DestroyWindow();

		auto retcode = StartEditor() ? IDOK : IDCANCEL;
		EndDialog(retcode);
	} else {
		SetRedraw(FALSE);
		progress.SetWindowText("Loading controls...");

		progress.StepAndProgressText("Loading editors...");
		InitFonts();
		InitTooltips();

		m_serializer.ProjectFolder = m_proj.ProjectFolder;
		m_serializer.EditorSettingsPath = m_proj.EditorSettingsPath;
		m_serializer.ProjectAdditionModulesFolder = m_proj.AdditionalModulesFolder;

		Editors = CreateEditors(m_serializer.ReadAllEditorInfos());

		if (!LoadEditorList(Editors)) {
			AbortInitDialog(this, "Encountered an unrecoverable error while loading the editors list.");
			return TRUE;
		}

		progress.StepAndProgressText("Setting up UI...");
		InitStdButtonImages();

		progress.StepAndProgressText("Finishing project post-load setup...");

		if (m_proj.IsRom()) {
			CWnd * btncompile = GetDlgItem(IDC_FFH_BTN_COMPILE);
			if (btncompile != nullptr) {
				btncompile->ShowWindow(SW_HIDE);
			}
		}
		m_hoverdescstatic.SetWindowText("");

		m_mainbanner.SetParent(this);
		m_mainbanner.Text = LoadStringResource(IDS_PROGRAMCAPTION, DEF_PROGRAMCAPTION);
		m_mainbanner.Subtext = LoadStringResource(IDS_PROGRAMSUBTITLE, "");
		m_mainbanner.Start = ::GetSysColor(COLOR_WINDOWTEXT);
		m_mainbanner.End = COLOR_FFRED;
		SetWindowText(m_mainbanner.Text);

		m_tooltips.Activate(TRUE);
		EnableToolTips(TRUE);

		UpdateSharedDisplayProperties();

		SetRedraw(TRUE);
		Invalidate();
		Ui::CenterToParent(this);

		progress.StepAndProgressText("Updating MRU...");
		AppStgs->UpdateMru(ProjectFile);
		AppStgs->WriteMru();

		progress.StepAndProgressText("Done");
		progress.DestroyWindow();
	}

	ShowWindow(AfxGetApp()->m_nCmdShow);
	Invalidate();

	return TRUE;
}

void CFFHacksterDlg::OnCancel()
{
	if (AppStgs->PromptOnClose && !m_bRequestReload) {
		auto result = AfxMessageBox("Are you sure you want to close the project?", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1);
		if (result == IDNO) return;
	}

	SaveProject(m_proj);

	BaseClass::OnCancel();
}

void CFFHacksterDlg::OnAbout()
{
	CAbout dlg;
	dlg.AppStgs = AppStgs;
	dlg.DoModal();
}

// The system calls this to obtain the cursor to display while the user drags the minimized window.
HCURSOR CFFHacksterDlg::OnQueryDragIcon()
{
	return (HCURSOR)AfxGetFFBaseApp()->GetLargeIcon();
}

bool CFFHacksterDlg::WantsToReload() const
{
	return m_bRequestReload;
}

void CFFHacksterDlg::GoToMapScreen(bool OV)
{
	bool teleport = 0;
	COverworldMap dlg;
	dlg.Project = &m_proj;
	CMaps ndlg;
	ndlg.Project = &m_proj;
	ndlg.Enloader = &m_loader;

	if (OV) goto LBL_overworld;
	else goto LBL_standard;

LBL_overworld:
	dlg.BootToTeleportFollowup = teleport;
	if (dlg.DoModal() == IDOK) {
		m_proj.SaveEditorSettings();
		teleport = dlg.BootToTeleportFollowup;
		if (teleport) goto LBL_standard;
	}
	goto LBL_end;

LBL_standard:
	ndlg.BootToTeleportFollowup = teleport;
	if (ndlg.DoModal() == IDOK) {
		m_proj.SaveEditorSettings();
		teleport = ndlg.BootToTeleportFollowup;
		if (teleport) goto LBL_overworld;
	}
	goto LBL_end;

LBL_end:
	return;
}

void CFFHacksterDlg::OnArmor() 
{
	CArmor dlg;
	dlg.Project = &m_proj;
	dlg.DoModal();
}

void CFFHacksterDlg::OnAttack()
{
	CAttack dlg;
	dlg.Project = &m_proj;
	dlg.DoModal();
}

void CFFHacksterDlg::OnBattle()
{
	CBattle dlg;
	dlg.Project = &m_proj;
	dlg.DoModal();
}

void CFFHacksterDlg::OnClasses()
{
	EditClasses("edit");
}

void CFFHacksterDlg::OnEnemy()
{
	EditEnemies("edit");
}

void CFFHacksterDlg::OnMagic()
{
	EditMagic("edit");
}

void CFFHacksterDlg::OnShop()
{
	CShop dlg;
	dlg.Project = &m_proj;
	dlg.Enloader = &m_loader;
	dlg.DoModal();
}

void CFFHacksterDlg::OnText()
{
	CText dlg;
	dlg.Project = &m_proj;
	if (dlg.DoModal() == IDOK) {
		m_proj.SaveEditorSettings();
	}
}

void CFFHacksterDlg::OnWeapon() 
{
	CWeapon dlg;
	dlg.Project = &m_proj;
	dlg.DoModal();
}

void CFFHacksterDlg::OnDialogue()
{
	EditSpriteDialogue("edit");
}

void CFFHacksterDlg::OnStartingItems()
{
	CStartingItems dlg;
	dlg.Project = &m_proj;
	dlg.DoModal();
}

void CFFHacksterDlg::OnMaps()
{
	GoToMapScreen(0);
}

void CFFHacksterDlg::OnOverworldmap()
{
	GoToMapScreen(1);
}

void CFFHacksterDlg::OnPartySetup()
{
	CPartySetup dlg;
	dlg.Project = &m_proj;
	dlg.DoModal();
}

void CFFHacksterDlg::OnImportHacksterDAT()
{
	CDlgImport dlg(this);
	dlg.Project = &m_proj;
	dlg.DoModal();
}

//FUTURE - expand revert into undo? or perhaps a multi-step revert? not enough time for this now, unfortunately.
void CFFHacksterDlg::OnRevertProject()
{
	if (m_proj.CanRevert()) {
		auto modalresult = AfxMessageBox("Do you really want to revert the game data to its initial state?\n"
			"You will lose all changes you've made to the game's data.\nThe project's other files won't be affected.",
			MB_OKCANCEL | MB_ICONQUESTION);
		if (modalresult == IDOK) {
			auto result = m_proj.Revert();
			if (!result) AfxMessageBox("The attempt to revert the game's data failed:\n" + result.value, MB_ICONERROR);
		}
	}
	else {
		AfxMessageBox("This project does not have a reversion file and cannot revert to its initial state.", MB_ICONERROR);
	}
}

void CFFHacksterDlg::OnPublishProject()
{
	CString msg;
	if (m_proj.Publish()) {
		msg.Format("Successfully published working ROM to\n'%s'.", (LPCSTR)m_proj.PublishRomPath);
		AfxMessageBox(msg);
	}
	else {
		msg.Format("Unable to publish working ROM\n'%s'\nto\n'%s'.", (LPCSTR)m_proj.WorkRomPath, (LPCSTR)m_proj.PublishRomPath);
		AfxMessageBox(msg, MB_ICONERROR);
	}
}

void CFFHacksterDlg::OnCompileProject()
{
	auto result = m_proj.Compile();
	if (result)
		AfxMessageBox("Compilation successful.");
	else if (!result.value.IsEmpty())
		AfxMessageBox("Compilation failed:\n" + result.value + ".", MB_ICONERROR);
}

void CFFHacksterDlg::OnRunClicked()
{
	if (AppStgs->CompileBeforeRun && m_proj.IsAsm()) {
		auto result = m_proj.Compile();
		if (!result) {
			AfxMessageBox("Cannot run the project: " + result.value, MB_ICONERROR);
			return;
		}
	}

	if (!Paths::FileExists(m_proj.WorkRomPath)) {
		//FUTURE - add A ROM_SIZE check here? if it fails, allow the user the option to still run?
		AfxMessageBox("Project working ROM not found - did you compile the project first?", MB_ICONSTOP);
		return;
	}

	if (!Paths::FileExists(AppStgs->RunExe)) {
		AfxMessageBox("The specified executable can't be found.\r\nPlease ensure that a valid file path is specified in the Application Settings.", MB_ICONERROR);
		return;
	}

	auto dir = Paths::GetDirectoryPath(AppStgs->RunExe);
	auto params = AppStgs->RunParams;
	if (params.IsEmpty()) params = "%1";
	params.Replace("%1", '"' + m_proj.WorkRomPath + '"');
	ShellExecute(nullptr, "open", AppStgs->RunExe, params, dir, SW_SHOW);
}

void CFFHacksterDlg::OnNmRclickActionButton(UINT id, NMHDR * pNotify, LRESULT * result)
{
	UNREFERENCED_PARAMETER(pNotify);

	*result = 0;
	auto idslot = id - IDC_DYNABUTTON_EDITORS;
	if (idslot < m_dynabuttons.size()) {
		size_t index = GetWindowLongPtr(m_dynabuttons[idslot]->GetSafeHwnd(), GWLP_USERDATA);
		ASSERT(index < Editors.size());
		if (index < Editors.size()) {
			const auto & edref = Editors[index];
			
			CPoint cursor;
			GetCursorPos(&cursor);
			edref.Rclick(m_proj.ProjectPath, AllocBytes, GetSafeHwnd(), cursor.x, cursor.y);
			*result = 1;
		}
	}
}

void CFFHacksterDlg::OnBnClickedActionButton(UINT id)
{
	auto idslot = id - IDC_DYNABUTTON_EDITORS;
	if (idslot < m_dynabuttons.size()) {
		size_t index = GetWindowLongPtr(m_dynabuttons[idslot]->GetSafeHwnd(), GWLP_USERDATA);
		ASSERT(index < Editors.size());
		if (index < Editors.size()) {
			const auto & edref = Editors[index];
			InvokeEditor(edref);
		}
	}
}

void CFFHacksterDlg::OnDestroy()
{
	if (m_plock != nullptr) m_plock->Unlock();
	delete m_plock, m_plock = nullptr;
	delete m_pmutex, m_pmutex = nullptr;
	ClearDynaButtons();
	m_actionbuttonfont.DeleteObject();
	m_bannerfont.DeleteObject();
	BaseClass::OnDestroy();
}

//BOOL CFFHacksterDlg::OnTtnNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult) //DEVNOTE - restore if dynamic tips are desired
//{
//	UNREFERENCED_PARAMETER(id);
//	UNREFERENCED_PARAMETER(pResult);
//	auto ptt = (NMTTDISPINFO*)pNMHDR;
//	
//	BOOL bResult = FALSE;
//	auto htool = (HWND)ptt->hdr.idFrom;
//	int ctrlid = ::GetDlgCtrlID(htool);
//
//	if (ctrlid >= IDC_DYNABUTTON_EDITORS && ctrlid < (IDC_DYNABUTTON_EDITORS + 100)) {
//		auto index = GetWindowLongPtr(htool, GWLP_USERDATA);
//		ptt->lpszText = (LPSTR)(LPCSTR)Editors[index].desc;
//		bResult = TRUE;
//	}
//
//	return bResult;
//}

BOOL CFFHacksterDlg::PreTranslateMessage(MSG* pMsg)
{
	if (IsWindow(m_tooltips.GetSafeHwnd()))
		m_tooltips.RelayEvent(pMsg);
	return BaseClass::PreTranslateMessage(pMsg);
}

LRESULT CFFHacksterDlg::OnFfttmShowDesc(WPARAM wparam, LPARAM lparam)
{
	UNREFERENCED_PARAMETER(wparam);
	CClearButton * pbtn = DYNAMIC_DOWNCAST(CClearButton, (CWnd*)lparam);
	if (pbtn != nullptr) {
		CString text;
		m_tooltips.GetText(text, pbtn);

		// If this butteon is linked to an editor, then it might have a discrete description.
		// Don't use the index, since some buttons might be hidden
		// If defined, this description might not always match the tooltip.
		if (has(m_dynabuttons, (CButton*)pbtn)) {
			size_t index = (size_t)GetWindowLongPtr(pbtn->GetSafeHwnd(), GWLP_USERDATA);
			if (index < Editors.size()) {
				CString dynatext = Editors[index].description.Trim();
				if (!dynatext.IsEmpty()) text = dynatext;
			}
		}

		m_hoverdescstatic.SetWindowText(text);
	}
	else {
		m_hoverdescstatic.SetWindowText("");
	}
	CRect rect;
	ClientToParentRect(&m_hoverdescstatic, &rect);
	InvalidateRect(&rect);
	return (LRESULT)0;
}

void CFFHacksterDlg::EditProjectLabels()
{
	Paths::FilePathRestorer fprestore(m_proj.StringsPath);
	ASSERT(fprestore.CanSave());

	CDlgEditStrings dlg(this, IDD_EDIT_LABELS);
	dlg.m_proj = &m_proj;
	dlg.SaveAction = [&fprestore]() { return fprestore.Save(); };
	dlg.DoModal();
}

void CFFHacksterDlg::EditProjectValues()
{
	Paths::FilePathRestorer fprestore(m_proj.ValuesPath);
	ASSERT(fprestore.CanSave());

	CDlgEditScalars dlg(this, IDD_EDIT_VALUES);
	dlg.m_proj = &m_proj;
	dlg.HexUppercase = AppStgs->SaveHexAsUppercase;
	dlg.SaveAction = [&fprestore,this]() { m_proj.UpdateVarsAndConstants(); return fprestore.Save(); };
	dlg.GroupNames = { "Ram Values" };
	dlg.Exclude = true; // will include all groups EXCEPT RAM Values
	dlg.DoModal();
}

void CFFHacksterDlg::EditProjectRamValues()
{
	Paths::FilePathRestorer fprestore(m_proj.ValuesPath);
	ASSERT(fprestore.CanSave());

	CDlgEditScalars dlg(this, IDD_EDIT_RAM);
	dlg.m_proj = &m_proj;
	dlg.HexUppercase = AppStgs->SaveHexAsUppercase;
	dlg.SaveAction = [&fprestore, this]() { m_proj.UpdateVarsAndConstants(); return fprestore.Save(); };
	dlg.GroupNames = { "Ram Values" }; // will include ONLY RAM Values
	dlg.EditLabels = true;
	dlg.DoModal();
}

void CFFHacksterDlg::EditAppSettings()
{
	CDlgAppSettings dlg(this);
	dlg.AppStgs = AppStgs;
	dlg.HelpSettings = &AfxGetFFBaseApp()->GetHelpSettings();
	if (dlg.DoModal() == IDOK) {
		WriteAppSettings();
		ReadAppSettings();
		EnableImage(AppStgs->ShowBackgroundArt);
		Invalidate();
	}
	else if (dlg.MruNeedsUpdate()) {
		WriteMru();
		ReadAppSettings();
	}

	UpdateSharedDisplayProperties();
}

void CFFHacksterDlg::EditProjectSettings()
{
	CDlgProjectSettings dlg(this);
	dlg.m_proj = &m_proj;
	dlg.ShowBackgroundArt = AppStgs->ShowBackgroundArt;
	if (dlg.DoModal() == IDOK) {
		m_serializer.ProjectAdditionModulesFolder = m_proj.AdditionalModulesFolder;
	}
}

void CFFHacksterDlg::EditProjectEditorsList()
{
	//REMOVE
	//CEditorDigestDlg dlged(this);
	//dlged.Project = &m_proj;
	//dlged.EditorInfos = EditorsToInfos(Editors);
	////dlged.AppFolder = Paths::GetDirectoryPath(AppIni);
	//if (dlged.DoModal() == IDOK) {
	//	SetRedraw(FALSE);
	//	try {
	//		Editors = CreateEditors(dlged.EditorInfos);
	//		m_serializer.WriteAllEditorInfos(EditorsToInfos(Editors));
	//		LoadEditorList(Editors);
	//	}
	//	catch (...) {
	//		AfxMessageBox("An error occurred while recreating the editor buttons.", MB_ICONERROR);
	//	}
	//	SetRedraw(TRUE);
	//	Invalidate();
	//}

	//N.B. - if an exception is thrown here, catching it renders the CDialog unresponsive AND leaks memory
	//		when the object is destroyed. Not sure why the derived-class destructors aren't being called
	//		yet, but that's the apparent reason. Exceptions in these diallogs will currently terminate
	//		the CFFHacksterDialog and return to the main program's action processing loop - which goes
	//		back to the Welcome screen.
	CEditorDigestDlg dlged(this);
	dlged.Project = &m_proj;
	dlged.EditorInfos = EditorsToInfos(Editors);
	dlged.AppFolder = Paths::GetProgramFolder(); //Paths::GetDirectoryPath(AppIni);
	if (dlged.DoModal() == IDOK) {
		try {
			Ui::RedrawScope redraw(this);
			Editors = CreateEditors(dlged.EditorInfos);
			m_serializer.WriteAllEditorInfos(EditorsToInfos(Editors));
			LoadEditorList(Editors);
		}
		catch (...) {
			AfxMessageBox("An error occurred while recreating the editor buttons.", MB_ICONERROR);
		}
	}
}

void CFFHacksterDlg::OnCloneProject()
{
	using namespace Paths;
	auto newprojectfile = PromptToSaveProject(this, m_proj.IsRom(), "Clone Project As");
	if (!newprojectfile.IsEmpty()) {
		auto folder = Paths::GetDirectoryPath(newprojectfile);
		if (FileExists(newprojectfile)) {
			AfxMessageBox("Please select a new project: cloning is not allowed to overwrite existing projects.", MB_ICONSTOP);
		}
		else if (IsDir(folder) && !DirEmpty(folder)) {
			AfxMessageBox("Please select an empty folder: cloning is not allowed to overwrite existing folders.", MB_ICONSTOP);
		}
		else {
			if (m_proj.Clone(newprojectfile)) {
				AfxMessageBox("Cloned the project to " + Paths::GetDirectoryPath(newprojectfile) + ".");
			}
			else {
				AfxMessageBox("Failed to clone the project to " + Paths::GetDirectoryPath(newprojectfile) + ".", MB_ICONERROR);
			}
		}
	}
}

void CFFHacksterDlg::OnArchiveProject()
{
	if (!m_proj.IsAsm() && !m_proj.IsRom()) {
		AfxMessageBox("The project type does not support archiving.", MB_ICONERROR);
		return;
	}

	if (!SaveProject(m_proj)) {
		auto result = AfxMessageBox("Failed to save the project.\nDo you want to archive it now anyway?", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION);
		if (result != IDOK) return;
	}

	CString filter = "FF1 ROM zipped Projects (*.ff1zip)|*.ff1zip||";
	CString defext = "ff1zip";
	CString deffile;
	deffile.Format("%s.ff1zip", (LPCSTR)Paths::GetFileStem(m_proj.ProjectPath));
	auto savefile = Ui::PromptToSaveProjectByFilter(this, defext, filter, deffile);
	if (!savefile.IsEmpty()) {
		Io::Zip(savefile, m_proj.ProjectFolder, true);
	}
}

void CFFHacksterDlg::OnMinimizeButton()
{
	//DEVNOTE - using flat borders disables the default minimize/maximize animation.
	//		To implement it manually, handle the WM_SHOWWINDOW message and either use
	//		AnimateWindow (easy) or some custom implementation (non-trivial).
	ShowWindow(SW_MINIMIZE);
}

//LRESULT CFFHacksterDlg::OnFFHacksterExShowWaitMessage(WPARAM wParam, LPARAM lParam) //DEVNOTE - not implemented
//{
//	this->SendMessageToDescendants(RWM_FFHACKSTEREX_SHOWWAITMESSAGE, wParam, lParam, TRUE);
//	return 0;
//}

void CFFHacksterDlg::OnMove(int x, int y)
{
	__super::OnMove(x, y);
}