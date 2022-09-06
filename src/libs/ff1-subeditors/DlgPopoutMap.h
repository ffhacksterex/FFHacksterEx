#pragma once

#include <memory>
#include "DrawingToolButton.h"
class IMapEditor;

//DUPE - WMA_SHOWFLOATINGMAP
#define WMA_SHOWFLOATINGMAP ((WM_APP) + 3)		// WPARAM: 1 = show, 0 = hide

// CDlgPopoutMap dialog

class CDlgPopoutMap : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPopoutMap)

public:
	CDlgPopoutMap(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgPopoutMap();

	BOOL CreateModeless(IMapEditor* editor, CSize mapdims, CSize tiledims, CWnd* parent = nullptr);
	bool SetButtons(const std::vector<sMapDlgButton>& buttons);
	void UpdateControls();
	void InvalidateMap();

	void ScrollByPercentage(int nBar, int percent);
	void ScrollToPos(int nBar, int mappos, bool center);

	CPoint GetMapPos() const;
	CPoint GetScrollOffset() const;
	CSize GetScrollLimits() const;
	CSize GetTileDims() const;
	CSize GetMiniMapDims() const;
	//CPoint GetScrollPercentages() const; //REMOVE?
	CRect GetDisplayArea() const;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POPOUT_MAP };
#endif

protected:
	IMapEditor* Editor = nullptr;
	CSize Mapsize;
	CSize Tilesize;
	std::vector<std::shared_ptr<CDrawingToolButton>> m_buttons;

	// Internal implementation
	bool is_valid() const;
	bool set_buttons(const std::vector<sMapDlgButton>& buttons);
	void update_tool_index();
	CRect get_sizer_rect(bool client);
	CPoint get_scroll_pos() const;
	CRect get_display_area() const;
	CPoint fix_map_point(CPoint point);
	void invalidate_display_area();
	void handle_close();
	void handle_sizing(int clientx, int clienty);

	// Controls
	bool m_ddxdone = false;
	CStatic m_displaystatic;
	CStatic m_buttonanchor;
	CButton m_customizebutton;
	mutable CScrollBar m_hscroll;
	mutable CScrollBar m_vscroll;

	// Overrides and handlers
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClickedCustomtool();
	afx_msg void OnClickedButtonImportMap();
	afx_msg void OnClickedButtonExportMap();
	afx_msg LRESULT OnDrawToolBnClick(WPARAM wparam, LPARAM lparam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnExitSizeMove();
};
