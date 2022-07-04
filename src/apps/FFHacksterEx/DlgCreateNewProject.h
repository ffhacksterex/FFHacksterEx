#pragma once

#include "resource.h"
#include <DialogWithBackground.h>

// CDlgCreateNewProject dialog

class CDlgCreateNewProject : public CDialogWithBackground
{
	DECLARE_DYNAMIC(CDlgCreateNewProject)

public:
	CDlgCreateNewProject(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgCreateNewProject();

	CString ProjectType;
	bool ShowBackgroundArt;
	bool LockDefinedInputs;

	CString ParentFolder;
	CString ProjectName;
	CString RevertPath;
	CString AsmDllPath;              // assembly projects only
	CString PublishPath;             // optional
	CString DATPath;                 // optional
	CString AdditionalModulesFolder; // optional
	bool ForceNewLabels;

protected:
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_NEWASM };
#endif
	enum ProjType { none = 0, rom, assembly };
	ProjType m_projtype = none;

	void HideAssemblySpecificControls();

	CClearStatic m_parentfolderstatic;
	CClearButton m_parentfolderbutton;
	CClearEdit m_parentfolderedit;
	CClearStatic m_projectnamestatic;
	CClearEdit m_projectnameedit;
	CClearStatic m_revertfilestatic;
	CClearButton m_revertbutton;
	CClearEdit m_revertfileedit;
	CClearStatic m_asmdllstatic;
	CClearButton m_asmdllusedefaultcheck;
	CClearButton m_asmdllbrowsebutton;
	CClearEdit m_asmdlledit;
	CClearButton m_forcenewlabelscheck;
	CClearButton m_datfilecheck;
	CClearButton m_datfilebutton;
	CClearEdit m_datfileedit;
	CClearStatic m_addlmodulesstatic;
	CClearButton m_addlmodulebutton;
	CClearEdit m_addlmodulesedit;
	CClearStatic m_publishstatic;
	CClearButton m_publishbutton;
	CClearEdit m_publishedit;
	CClearButton m_okbutton;
	CClearButton m_cancelbutton;

	virtual UINT GetHelpID() const;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedNewasmCheckImportdat();
	afx_msg void OnBnClickedNewasmButtonParentfolder();
	afx_msg void OnBnClickedNewasmButtonRevert();
	afx_msg void OnBnClickedNewasmButtonPublish();
	afx_msg void OnBnClickedNewasmButtonImportdat();
	afx_msg void OnBnClickedAsmDllLookInAppFolder();
	afx_msg void OnBnClickedAdditionalModulesFolder();
	afx_msg void OnBnClickedButton1BrowseAsmDll();
	CHelpbookButton m_helpbookbutton;
};
