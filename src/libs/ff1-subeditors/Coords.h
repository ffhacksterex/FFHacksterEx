#if !defined(AFX_COORDS_H__2DA94D31_DBD6_49E1_A2CA_ED716736EE4D__INCLUDED_)
#define AFX_COORDS_H__2DA94D31_DBD6_49E1_A2CA_ED716736EE4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Coords.h : header file
//

#include "resource_subeditors.h"
#include <FFBaseDlg.h>
class CFFHacksterProject;
class ICoordMap;

////////////////////////////////////////////////////////////////////////////
// CCoords dialog

class CCoords : public CFFBaseDlg
{
// Construction
public:
	CCoords(CWnd* pParent = nullptr);   // standard constructor

	CFFHacksterProject * cart = nullptr;
	int Context;
	int cur;
	bool IsOV;

	BYTE type;
	ICoordMap* OVparent;
	ICoordMap* STparent;

	void InputCoords(CPoint);
	void LoadValues();
	void Boot();
	void UpdateButtons();
	int offset;
	int count;

	void TeleportHere();
	afx_msg void OnSelchangeCoordL();
	afx_msg void OnSelchangeTeleportlist();

// Dialog Data
	enum { IDD = IDD_COORDS };
	CComboBox	m_teleportlist;
	CButton	m_mouseclick;
	CButton	m_followup_forward;
	CButton	m_followup_back;
	CEdit	m_coord_y;
	CEdit	m_coord_x;
	CComboBox	m_coord_l;

	int MAP_COUNT = -1;
	int NNTELEPORT_COUNT = -1;
	int ONTELEPORT_COUNT = -1;
	int NOTELEPORT_COUNT = -1;
	int NNTELEPORT_OFFSET = -1;
	int ONTELEPORT_OFFSET = -1;
	int NOTELEPORT_OFFSET = -1;

protected:
	bool LoadRom();
	bool SaveRom();

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnChangeCoordX();
	afx_msg void OnChangeCoordY();
	afx_msg void OnMouseclick();
	afx_msg void OnEditlabel();
	afx_msg void OnFollowup();
	afx_msg void OnFollowupBack();
	afx_msg void OnFollowupForward();
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_COORDS_H__2DA94D31_DBD6_49E1_A2CA_ED716736EE4D__INCLUDED_)
