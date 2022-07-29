#pragma once

#include <vector>
#include <ram_value_functions.h>
#include <std_assembly.h>
#include <InplaceCombo.h>
#include <InplaceEdit.h>
#include <EditorWithBackground.h>
#include <FilePathRestorer.h>
#include "afxwin.h"
class CFFHacksterProject;
class CEntriesLoader;
class GameSerializer;

// CSpriteDialogue2 dialog

class CSpriteDialogue2 : public CEditorWithBackground
{
	DECLARE_DYNAMIC(CSpriteDialogue2)

public:
	CSpriteDialogue2(CFFHacksterProject & project, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpriteDialogue2();

	CEntriesLoader * Enloader = nullptr;

protected:
	CFFHacksterProject & m_proj;
	Paths::FilePathRestorer m_dialoguepathrestorer;

	virtual void LoadRom();
	virtual void SaveRom();

	void LoadRomTalkData();
	void SaveRomTalkData();
	void LoadAsmTalkData(GameSerializer & ser);
	void SaveAsmTalkData(GameSerializer & ser);

	void EnsureValidSpriteHandlers();
	void EnaureSynchedHandlerTables();

	void LoadValues();
	void StoreValues();

	void FormatElementList();
	void SetupLayout();
	void LoadElementList();
	void SaveElementList();

	void LoadElement(int elementindex, int bankaddr, CString routinename);
	void SaveElement(int elementindex, int bankaddr, CString routinename);

	std::pair<CString, int> GetElementValue(int elementindex, int bankaddr, CString marker, CString inputvalue);
	void SetRomValue(int elementindex, int bankaddr, CString marker, CString inputvalue, CString newvalue);
	void EditElementValue(int elementindex, CString marker, CString value);
	void EditElementComment(int elementindex, CString comment);

	int ReadHandlerAddr(int index);
	void WriteHandlerAddr(int index, int data);
	BYTE Read8(int addedoffset);
	void Write8(int addedoffset, BYTE data);
	int Read16(int addedoffset);
	void Write16(int addedoffset, int data);
	BYTE ReadParam(int index);
	void WriteParam(int index, BYTE value);

	int TextToValue(CString marker, std::string inputvalue);
	std::string ValueToText(CString marker, int newvalue);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPRITE_DIALOGUE2 };
#endif
	int m_curindex;

	int TALKROUTINEPTRTABLE_OFFSET = -1;
	int TALKROUTINEPTRTABLE_BYTES = -1;
	int TALKROUTINEPTRTABLE_PTRADD = -1;
	int TALKROUTINEDATA_OFFSET = -1;
	int TALKROUTINEDATA_BYTES = -1;
	int BANK0A_OFFSET = -1;

	int m_itemsaddr = -1;
	int m_unsramaddr = -1;
	std::map<int,int> m_fanfarevalues;

	stdstringvector m_asmroutinenames;
	talknamevectormap m_asmroutinemap;
	mfcstrintmap m_routineaddrmap;

	CSubBanner m_banner;
	CInplaceCombo m_inplacecombo;
	CInplaceEdit m_inplaceedit;

	CBorderedListBox m_mainlist;
	CBorderedListCtrl m_elementlist;
	CComboBox m_comboRoutineType;
	CStatic m_unknownfuncaddrstatic;
	CClearButton m_settingsbutton = { IDPNG_GEARSM, "PNG" };
	CCloseButton m_closebutton;
	CHelpbookButton m_helpbookbutton;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnBnClickedSettings();
	afx_msg void OnSelchangeComboTalkroutinetype();
	afx_msg void OnSelchangeList();
	afx_msg void OnBnClickedSave();
	afx_msg void OnNMClickListTalkelements(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
