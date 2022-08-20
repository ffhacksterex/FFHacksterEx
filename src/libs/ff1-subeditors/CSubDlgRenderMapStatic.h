#pragma once

#include "sRenderMapState.h"

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

	// Internal Implementation
	virtual BOOL init();
	virtual bool is_valid() const;

	CPoint fix_map_point(CPoint point);
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
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


