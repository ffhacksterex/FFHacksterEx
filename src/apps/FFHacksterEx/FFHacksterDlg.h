// FFHacksterDlg.h : header file
#pragma once

#include <ClearEdit.h>
#include <EntriesLoader.h>
#include <FFHacksterProject.h>
#include <MainBanner.h>
#include <Editors2.h>
#include <EditorSettingsSerializer.h>
#include <DialogWithBackground.h>
#include <afxlinkctrl.h>

/////////////////////////////////////////////////////////////////////////////
// CFFHacksterDlg dialog

class CFFHacksterDlg : public CDialogWithBackground
{
// Construction
public:
	CFFHacksterDlg(CWnd* pParent = nullptr);	// standard constructor

	bool WantsToReload() const;
	void GoToMapScreen(bool);

	CString InitialEditorName;
	CString ProjectFile;
	CString ExitAction;
	CEntriesLoader m_loader;
	CFFHacksterProject m_proj;
	Editors2::CEditorVector Editors;
	Editors2::EditorSettingsSerializer m_serializer;

protected:
	CMutex * m_pmutex = nullptr;      // Ensure only one instance edits the same project at a time
	CSingleLock * m_plock = nullptr;

// Dialog Data
	enum { IDD = IDD_FFHACKSTER_DIALOG };
	CClearStatic m_dynabuttonframe;
	CClearStatic m_hoverdescstatic;
	CClearButton m_importhacksterbutton = { IDPNG_IMPORT, "PNG" };
	CClearButton m_revertbutton = { IDPNG_REVERT, "PNG" };
	CClearButton m_clonebutton = { IDPNG_CLONE, "PNG" };
	CClearButton m_archivebutton = { IDPNG_ZIP, "PNG" };
	CClearButton m_compilebutton = { IDPNG_SAVEDOWN, "PNG" };
	CClearButton m_publishbutton = { IDPNG_PUBLISH, "PNG" };
	CClearButton m_runbutton = { IDPNG_GAMEPAD, "PNG" };
	CClearButton m_labelsbutton = { IDPNG_LABEL, "PNG" };
	CClearButton m_valuesbutton = { IDPNG_EDITVALUES, "PNG" };
	CClearButton m_rambutton = { IDPNG_RAMCHIP, "PNG" };
	CClearButton m_projsettingsbutton = { IDPNG_CONTROLPANEL, "PNG" };
	CClearButton m_editorsbutton = { IDPNG_EDITOR, "PNG" };
	CClearButton m_appsettingsbutton = { IDPNG_GEARSM, "PNG" };
	CHelpbookButton m_helpbookbutton;
	CClearButton m_aboutbutton = { IDPNG_HELPSM, "PNG" };
	CClearButton m_minbutton = { IDPNG_MINSM, "PNG" };
	CCloseButton m_cancelbutton;

	CMainBanner m_mainbanner;
	CToolTipCtrl m_tooltips;
	std::vector<CButton*> m_dynabuttons;

	CFont m_bannerfont;
	CFont m_actionbuttonfont;

	CString m_bannertext;
	bool m_bRequestReload;

	bool StartEditor();
	void ReadAppSettings();
	void WriteAppSettings();
	void WriteMru();

	void InitFonts();
	void InitTooltips();
	void InitStdButtonImages();
	void UpdateSharedDisplayProperties();

	void ReloadProject();
	void AbortProject();

	bool LoadEditorList(Editors2::CEditorVector & editors);

	Editors2::CEditor CreateEditor(const Editors2::sEditorInfo & info);
	Editors2::CEditorVector CreateEditors(const Editors2::sEditorInfoVector & infos);
	Editors2::EditorImplFunc GetBuiltinEditorFunc();

	static char* AllocBytes(int64_t length);

	void InvokeEditor(const Editors2::CEditor & editor);
	bool InvokeInternalEditor(CString editorcmd);
	bool InvokeInternalContextMenu(CString editcommand, CString paramstring);

	void ClearDynaButtons();
	void CreateDynaButtons(const Editors2::CEditorVector & editors);
	void RepositionButtonsForROMProject();

	void EditBattles(CString action, CString params = "");
	void EditClasses(CString action, CString params = "");
	void EditEnemies(CString action, CString params = "");
	void EditMagic(CString action, CString params = "");
	void EditSpriteDialogue(CString action, CString params = "");
	void EditText(CString action, CString params = "");

	virtual void PaintBanner(CDC & dc);
	virtual void PaintClient(CDC & dc);

	// ClassWizard generated virtual function overrides
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnFfttmShowDesc(WPARAM wparam, LPARAM lparam);
	afx_msg void OnAbout();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnNmRclickActionButton(UINT id, NMHDR * pNotify, LRESULT * result);
	afx_msg void OnBnClickedActionButton(UINT id);
	afx_msg void OnPartySetup();
	afx_msg void OnOverworldmap();
	afx_msg void OnText();
	afx_msg void OnArmor();
	afx_msg void OnWeapon();
	afx_msg void OnEnemy();
	afx_msg void OnAttack();
	afx_msg void OnMagic();
	afx_msg void OnShop();
	afx_msg void OnBattle();
	afx_msg void OnMaps();
	afx_msg void OnClasses();
	afx_msg void OnImportHacksterDAT();
	afx_msg void OnRevertProject();
	afx_msg void OnPublishProject();
	afx_msg void OnCompileProject();
	afx_msg void OnDialogue();
	afx_msg void OnStartingItems();
	afx_msg void OnRunClicked();
	afx_msg void OnDestroy();
	afx_msg void EditProjectLabels();
	afx_msg void EditProjectValues();
	afx_msg void EditProjectRamValues();
	afx_msg void EditAppSettings();
	afx_msg void EditProjectSettings();
	afx_msg void EditProjectEditorsList();
	afx_msg void OnCloneProject();
	afx_msg void OnArchiveProject();
	afx_msg void OnMinimizeButton();
	afx_msg void OnMove(int x, int y);
	//afx_msg BOOL OnTtnNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult); //DEVNOTE - REVISIT
	//afx_msg LRESULT OnFFHacksterExShowWaitMessage(WPARAM wParam, LPARAM lParam);
};
