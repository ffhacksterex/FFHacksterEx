#pragma once

#define WMA_SHOWFLOATINGMAP ((WM_APP) + 3)

class CFFHacksterProject;

using dcommap64 = BYTE[64][64];

struct sFloatingMapStateLink {
	CButton* m_showrooms = nullptr;
	const int* cur_map = nullptr;
	const int* cur_tile = nullptr;
	const int* cur_tool = nullptr;
	const CPoint* ptLastClick = nullptr;
	const dcommap64* DecompressedMap = nullptr;
	const CRect* rcToolRect = nullptr;

	bool IsValid() const;
};

// CFloatingMapDlg dialog

class CFloatingMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFloatingMapDlg)

public:
	CFloatingMapDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFloatingMapDlg();

	void Init(CFFHacksterProject* project, sFloatingMapStateLink* state); // call after calling Create

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FLOATING_MAP };
#endif

protected:
	CFFHacksterProject* cart; //TODO - remove and replace with Project

	CFFHacksterProject* Project = nullptr;
	sFloatingMapStateLink* State = nullptr;
	CRect rcMap;
	CPoint ScrollOffset;
	BYTE mousedown = 0;

	void HandleClose();
	bool IsValid() const;

	// Controls and handlers
	CButton m_mapstatic;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};
