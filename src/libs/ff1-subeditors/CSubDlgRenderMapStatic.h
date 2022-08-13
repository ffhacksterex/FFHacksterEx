#pragma once

class CFFHacksterProject;

struct sRenderMapState
{
	CFFHacksterProject* project = nullptr;
	CWnd* owner = nullptr;
	const bool* showrooms = nullptr;
	const CPoint* ptLastClick = nullptr;
	const CRect* rcToolRect = nullptr;
	const int* cur_map = nullptr;
	const int* cur_tile = nullptr;
	const int* cur_tool = nullptr;
	const BYTE* DecompressedMap = nullptr;
	CSize mapdims;
	CSize tiledims;

	bool IsValid() const;
};


// CSubDlgRenderMapStatic

class CSubDlgRenderMapStatic : public CStatic
{
	DECLARE_DYNAMIC(CSubDlgRenderMapStatic)

public:
	CSubDlgRenderMapStatic();
	virtual ~CSubDlgRenderMapStatic();

	void SetRenderState(const sRenderMapState& state);
	const sRenderMapState& GetRenderState() const;

protected:
	sRenderMapState State;
	CSize m_scaletiledims = { 16,16 };
	CPoint m_scrollpos;
	CBitmap m_bmp;
	CDC m_bmpdc;
	BYTE mousedown = 0;

	// Internal Implementation
	virtual BOOL init();
	virtual bool is_valid() const;

	void handle_sizing();
	void handle_sizing(int cx, int cy);
	void handle_paint();

	// Controls
	CPen m_pen;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void PreSubclassWindow();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


