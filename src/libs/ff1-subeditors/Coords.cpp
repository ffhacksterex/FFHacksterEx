// Coords.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "Coords.h"
#include "ICoordMap.h"
#include "FFHacksterProject.h"
#include <FFH2Project.h>
#include "general_functions.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "editor_label_functions.h"
#include "ui_helpers.h"
#include <ValueDataAccessor.h>
#include <vda_std_collections.h>
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
	ffh::acc::ValueDataAccessor d(*Proj2);
	MAP_COUNT = d.get<int>("MAP_COUNT");
	NNTELEPORT_COUNT = d.get<int>("NNTELEPORT_COUNT");
	ONTELEPORT_COUNT = d.get<int>("ONTELEPORT_COUNT");
	NOTELEPORT_COUNT = d.get<int>("NOTELEPORT_COUNT");
	NNTELEPORT_OFFSET = d.get<int>("NNTELEPORT_OFFSET");
	ONTELEPORT_OFFSET = d.get<int>("ONTELEPORT_OFFSET");
	NOTELEPORT_OFFSET = d.get<int>("NOTELEPORT_OFFSET");
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

	FFH_THROW_NULL_PROJECT(Proj2, "Coords Subeditor");

	return TRUE;
}

// The caller must be prepared to catch an exception (std::invalid_argument, std::regex_error)
void CCoords::Boot()
{
	LoadRom();

	LoadCombo(m_teleportlist, Labels2::LoadONTeleportLabels(*Proj2) + Labels2::LoadNNTeleportLabels(*Proj2) + Labels2::LoadNOTeleportLabels(*Proj2));
	LoadCombo(m_coord_l, Labels2::LoadMapLabels(*Proj2));

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

	text.Format("%X",Proj2->ROM[offset]);
	m_coord_x.SetWindowText(text);

	text.Format("%X",Proj2->ROM[offset + count]);
	m_coord_y.SetWindowText(text);
	if(type < 2)
		m_coord_l.SetCurSel(Proj2->ROM[offset + (count << 1)]);
}

void CCoords::OnSelchangeCoordL()
{
	Proj2->ROM[offset + (count << 1)] = (BYTE)m_coord_l.GetCurSel();
}

void CCoords::OnChangeCoordX()
{
	CString text;
	m_coord_x.GetWindowText(text);
	Proj2->ROM[offset] = (BYTE)StringToInt_HEX(text);
}

void CCoords::OnChangeCoordY()
{
	CString text;
	m_coord_y.GetWindowText(text);
	Proj2->ROM[offset + count] = (BYTE)StringToInt_HEX(text);
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
	if (type > 0) arid -= ONTELEPORT_COUNT;
	if (type > 1) arid -= NNTELEPORT_COUNT;

	//TODO - remove WrapDataNodeReadFunc2 once CFFHacksterProject is gone (overload resolution can't resolve the two functions)
	#define WrapDataNodeReadFunc2(f) [](FFH2Project& proj, int index, bool showindex) { return (f)(proj, index, showindex); }
	auto getfunclist = std::vector<DataNodeReadFunc2>{ WrapDataNodeReadFunc2(Labels2::LoadONTeleportLabel),
		WrapDataNodeReadFunc2(Labels2::LoadNNTeleportLabel),
		WrapDataNodeReadFunc2(Labels2::LoadNOTeleportLabel) };
	CString label = getfunclist[type](*Proj2, arid, false);

	// The teleport listbox in this dialog has ON, NN, and NO targets, indexed with cur.
	// The INI stores the each type within its index range, using arid.
	// So use a lambda that ignores the index and uses arid to call our write handler.
	IniWriteFunc writefunclist[3] = { WriteONTeleportLabel, WriteNNTeleportLabel, WriteNOTeleportLabel };
	auto writehandler = writefunclist[type];
	auto changefunc = [writehandler,arid](CFFHacksterProject & proj, int, CString newvalue) {
		// Ignore the list/combo index passed in and use the type-specific arid instead.
		return writehandler(proj, arid, newvalue);
	};
	//ChangeLabel(*Proj2, -1, label, changefunc, cur, nullptr, &m_teleportlist); //TODO - translate ChangeLabel

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
	BYTE& cu = Proj2->curFollowup;
	BYTE& ma = Proj2->maxFollowup;
	m_followup_forward.EnableWindow(!(cu == (ma & 0x7F)));
	m_followup_back.EnableWindow(!(
		(cu == ((ma + 1) & 0x7F)) ||
		(!cu && !(ma & 0x80))));
}

void CCoords::OnFollowup()
{
	BYTE* Flw = Proj2->TeleportFollowup[Proj2->curFollowup];
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
	if(Proj2->curFollowup == (Proj2->maxFollowup & 0x7F)){
		if(Proj2->maxFollowup == 0xFF) Proj2->maxFollowup = 0x80;
		else Proj2->maxFollowup += 1;}
	Proj2->curFollowup = (Proj2->curFollowup + 1) & 0x7F;

	Flw = Proj2->TeleportFollowup[Proj2->curFollowup];

	//place the followup spot in the current followup
	Flw[1] = Proj2->ROM[offset];
	Flw[2] = Proj2->ROM[offset + count];
	if(type == 2) Flw[0] = 0xFF;
	else Flw[0] = Proj2->ROM[offset + (count << 1)];
	TeleportHere();
}

void CCoords::OnFollowupBack()
{
	if(!Proj2->curFollowup) Proj2->curFollowup = 0x7F;
	else Proj2->curFollowup -= 1;
	TeleportHere();
}

void CCoords::OnFollowupForward()
{
	Proj2->curFollowup = (Proj2->curFollowup + 1) & 0x7F;
	TeleportHere();
}

void CCoords::TeleportHere()
{
	UpdateButtons();
	BYTE _l = Proj2->TeleportFollowup[Proj2->curFollowup][0];
	BYTE _x = Proj2->TeleportFollowup[Proj2->curFollowup][1];
	BYTE _y = Proj2->TeleportFollowup[Proj2->curFollowup][2];

	if (IsOV)
		OVparent->TeleportHere(_l, _x, _y);
	else
		STparent->TeleportHere(_l, _x, _y);
}