// Coords.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "Coords.h"
#include "ICoordMap.h"
#include "FFHacksterProject.h"
#include "general_functions.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "editor_label_functions.h"
#include "ui_helpers.h"
#include "NewLabel.h"

using namespace Ini;
using namespace Editorlabels;
using namespace Ui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCoords dialog


CCoords::CCoords(CWnd* pParent /*= nullptr */)
	: CFFBaseDlg(CCoords::IDD, pParent)
	, Context(-1)
{
}


void CCoords::DoDataExchange(CDataExchange* pDX)
{
	CFFBaseDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TELEPORTLIST, m_teleportlist);
	DDX_Control(pDX, IDC_MOUSECLICK, m_mouseclick);
	DDX_Control(pDX, IDC_FOLLOWUP_FORWARD, m_followup_forward);
	DDX_Control(pDX, IDC_FOLLOWUP_BACK, m_followup_back);
	DDX_Control(pDX, IDC_COORD_Y, m_coord_y);
	DDX_Control(pDX, IDC_COORD_X, m_coord_x);
	DDX_Control(pDX, IDC_COORD_L, m_coord_l);
}

bool CCoords::LoadRom()
{
	//N.B. - the caller of Boot() will catch these exceptions
	MAP_COUNT = ReadDec(cart->ValuesPath, "MAP_COUNT");
	NNTELEPORT_COUNT = ReadDec(cart->ValuesPath, "NNTELEPORT_COUNT");
	ONTELEPORT_COUNT = ReadDec(cart->ValuesPath, "ONTELEPORT_COUNT");
	NOTELEPORT_COUNT = ReadDec(cart->ValuesPath, "NOTELEPORT_COUNT");
	NNTELEPORT_OFFSET = ReadHex(cart->ValuesPath, "NNTELEPORT_OFFSET");
	ONTELEPORT_OFFSET = ReadHex(cart->ValuesPath, "ONTELEPORT_OFFSET");
	NOTELEPORT_OFFSET = ReadHex(cart->ValuesPath, "NOTELEPORT_OFFSET");
	return true;
}

bool CCoords::SaveRom()
{
	// for now, this isn't needed, since changes are immediately set to the ROM buffer.
	return true;
}


BEGIN_MESSAGE_MAP(CCoords, CFFBaseDlg)
	ON_CBN_SELCHANGE(IDC_TELEPORTLIST, OnSelchangeTeleportlist)
	ON_CBN_SELCHANGE(IDC_COORD_L, OnSelchangeCoordL)
	ON_EN_CHANGE(IDC_COORD_X, OnChangeCoordX)
	ON_EN_CHANGE(IDC_COORD_Y, OnChangeCoordY)
	ON_BN_CLICKED(IDC_MOUSECLICK, OnMouseclick)
	ON_BN_CLICKED(IDC_EDITLABEL, OnEditlabel)
	ON_BN_CLICKED(IDC_FOLLOWUP, OnFollowup)
	ON_BN_CLICKED(IDC_FOLLOWUP_BACK, OnFollowupBack)
	ON_BN_CLICKED(IDC_FOLLOWUP_FORWARD, OnFollowupForward)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoords message handlers

BOOL CCoords::OnInitDialog()
{
	CFFBaseDlg::OnInitDialog();
	
	//DEVNOTE - since this dialog is used modelessly, it relies on a call to Boot() after Create() returns. LoadRom is called there.

	return TRUE;
}

// The caller must be prepared to catch an exception (std::invalid_argument, std::regex_error)
void CCoords::Boot()
{
	LoadRom();

	LoadCombo(m_teleportlist, LoadONTeleportLabels(*cart) + LoadNNTeleportLabels(*cart) + LoadNOTeleportLabels(*cart));
	LoadCombo(m_coord_l, LoadMapLabels(*cart));

	cur = -1;
	m_teleportlist.SetCurSel(0);
	OnSelchangeTeleportlist();

	UpdateButtons();
}

void CCoords::OnSelchangeTeleportlist()
{
	cur = m_teleportlist.GetCurSel();
	LoadValues();
}

void CCoords::LoadValues()
{
	type = 2;
	if(cur < ONTELEPORT_COUNT) type = 0;
	else if(cur < (ONTELEPORT_COUNT + NNTELEPORT_COUNT)) type = 1;
	m_coord_l.ShowWindow(type < 2);
	switch(type){
	case 0: offset = ONTELEPORT_OFFSET + cur;
		count = ONTELEPORT_COUNT; break;
	case 1: offset = NNTELEPORT_OFFSET + cur - ONTELEPORT_COUNT;
		count = NNTELEPORT_COUNT; break;
	case 2: offset = NOTELEPORT_OFFSET + cur - ONTELEPORT_COUNT - NNTELEPORT_COUNT;
		count = NOTELEPORT_COUNT; break;
	}

	CString text;

	text.Format("%X",cart->ROM[offset]);
	m_coord_x.SetWindowText(text);

	text.Format("%X",cart->ROM[offset + count]);
	m_coord_y.SetWindowText(text);
	if(type < 2)
		m_coord_l.SetCurSel(cart->ROM[offset + (count << 1)]);
}

void CCoords::OnSelchangeCoordL()
{
	cart->ROM[offset + (count << 1)] = (BYTE)m_coord_l.GetCurSel();
}

void CCoords::OnChangeCoordX()
{
	CString text;
	m_coord_x.GetWindowText(text);
	cart->ROM[offset] = (BYTE)StringToInt_HEX(text);
}

void CCoords::OnChangeCoordY()
{
	CString text;
	m_coord_y.GetWindowText(text);
	cart->ROM[offset + count] = (BYTE)StringToInt_HEX(text);
}

void CCoords::OnMouseclick()
{
	if ((type < 2) && IsOV) {
		m_mouseclick.SetCheck(0);
		AfxMessageBox("Can't set Standard Map destination coords by clicking on the Overworld Map.", MB_ICONERROR);
	}
	else if ((type == 2) && !IsOV) {
		m_mouseclick.SetCheck(0);
		AfxMessageBox("Can't set Overworld Map destination coords by clicking on a Standard Map.", MB_ICONERROR);
	}
}

void CCoords::OnEditlabel()
{
	int arid = cur;
	if(type > 0) arid -= ONTELEPORT_COUNT;
	if(type > 1) arid -= NNTELEPORT_COUNT;

	auto getfunclist = std::vector<DataNodeReadFunc>{ LoadONTeleportLabel, LoadNNTeleportLabel, LoadNOTeleportLabel };
	CString label = getfunclist[type](*cart, arid, false);

	// The teleport listbox in this dialog has ON, NN, and NO targets, indexed with cur.
	// The INI stores the each type within its index range, using arid.
	// So use a lambda that ignores the index and uses arid to call our write handler.
	IniWriteFunc writefunclist[3] = { WriteONTeleportLabel, WriteNNTeleportLabel, WriteNOTeleportLabel };
	auto writehandler = writefunclist[type];
	auto changefunc = [writehandler,arid](CFFHacksterProject & proj, int, CString newvalue) {
		// Ignore the list/combo index passed in and use the type-specific arid instead.
		return writehandler(proj, arid, newvalue);
	};
	ChangeLabel(*cart, -1, label, changefunc, cur, nullptr, &m_teleportlist);

	if (!type && IsOV)
		OVparent->UpdateTeleportLabel(arid, type);
	else if (type && !IsOV)
		STparent->UpdateTeleportLabel(arid, type);
}

void CCoords::InputCoords(CPoint pt)
{
	CString text;
	text.Format("%X",pt.x);
	m_coord_x.SetWindowText(text); OnChangeCoordX();
	text.Format("%X",pt.y);
	m_coord_y.SetWindowText(text); OnChangeCoordY();
	m_mouseclick.SetCheck(0);
}

void CCoords::OnCancel()
{
	if (IsOV)
		OVparent->Cancel(Context);
	else
		STparent->Cancel(Context);
}

void CCoords::UpdateButtons()
{
	BYTE& cu = cart->curFollowup;
	BYTE& ma = cart->maxFollowup;
	m_followup_forward.EnableWindow(!(cu == (ma & 0x7F)));
	m_followup_back.EnableWindow(!(
		(cu == ((ma + 1) & 0x7F)) ||
		(!cu && !(ma & 0x80))));
}

void CCoords::OnFollowup()
{
	BYTE* Flw = cart->TeleportFollowup[cart->curFollowup];
	//place the last click in current Followup
	if (IsOV) {
		Flw[0] = 0xFF;
		Flw[1] = (BYTE)OVparent->GetLastClick().x;
		Flw[2] = (BYTE)OVparent->GetLastClick().y;
	}
	else {
		Flw[0] = (BYTE)STparent->GetCurMap();
		Flw[1] = (BYTE)STparent->GetLastClick().x;
		Flw[2] = (BYTE)STparent->GetLastClick().y;
	}

	//increment curFollowup (and maxFollowup if needed)
	if(cart->curFollowup == (cart->maxFollowup & 0x7F)){
		if(cart->maxFollowup == 0xFF) cart->maxFollowup = 0x80;
		else cart->maxFollowup += 1;}
	cart->curFollowup = (cart->curFollowup + 1) & 0x7F;

	Flw = cart->TeleportFollowup[cart->curFollowup];

	//place the followup spot in the current followup
	Flw[1] = cart->ROM[offset];
	Flw[2] = cart->ROM[offset + count];
	if(type == 2) Flw[0] = 0xFF;
	else Flw[0] = cart->ROM[offset + (count << 1)];
	TeleportHere();
}

void CCoords::OnFollowupBack()
{
	if(!cart->curFollowup) cart->curFollowup = 0x7F;
	else cart->curFollowup -= 1;
	TeleportHere();
}

void CCoords::OnFollowupForward()
{
	cart->curFollowup = (cart->curFollowup + 1) & 0x7F;
	TeleportHere();
}

void CCoords::TeleportHere()
{
	UpdateButtons();
	BYTE _l = cart->TeleportFollowup[cart->curFollowup][0];
	BYTE _x = cart->TeleportFollowup[cart->curFollowup][1];
	BYTE _y = cart->TeleportFollowup[cart->curFollowup][2];

	if (IsOV)
		OVparent->TeleportHere(_l, _x, _y);
	else
		STparent->TeleportHere(_l, _x, _y);
}