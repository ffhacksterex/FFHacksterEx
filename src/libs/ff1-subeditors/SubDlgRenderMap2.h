#pragma once

class IMapEditor;

// CSubDlgRenderMap2 dialog

class CSubDlgRenderMap2 : public CDialogEx
{
	DECLARE_DYNAMIC(CSubDlgRenderMap2)

public:
	CSubDlgRenderMap2(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSubDlgRenderMap2();

	BOOL CreateOverControl(CWnd* pParent, CWnd* pOver);
	void SetSource(IMapEditor* owner, CSize tiledims);
	void InvalidateFrame();

	//void SetRenderState(const sRenderMapState& state);
	//const sRenderMapState& GetRenderState() const;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUBDLG_RENDERMAP2 };
#endif

protected:
	// Internal implementation
	BOOL init();
	int handle_scroll(UINT nBar, UINT nSBCode, UINT nPos);
	void handle_sizing();
	void handle_sizing(int clientx, int clienty);

	// Overrides and message handlers
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
