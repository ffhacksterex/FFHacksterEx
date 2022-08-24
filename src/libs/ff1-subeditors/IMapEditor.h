#pragma once

#include <afxwin.h>
struct sRenderMapState;

class IMapEditor
{
public:
	IMapEditor() {}
	virtual ~IMapEditor() {}

	virtual void HandleLButtonDown(UINT nFlags, CPoint point) = 0;
	virtual void HandleLButtonUp(UINT nFlags, CPoint point) = 0;
	virtual void HandleLButtonDblClk(UINT nFlags, CPoint point) = 0;
	virtual void HandleRButtonDown(UINT nFlags, CPoint pt) = 0;
	virtual void HandleRButtonUp(UINT nFlags, CPoint point) = 0;
	virtual void HandleRButtonDblClk(UINT nFlags, CPoint point) = 0;
	virtual void HandleMouseMove(UINT nFlags, CPoint point) = 0;
	virtual void HandleAfterScroll(CPoint scrolloffset, CRect displayarea) {} //TODO - make pure
	virtual void HandleMapImport() = 0;
	virtual void HandleMapExport() = 0;
	virtual bool HandleCustomizeTool() = 0;
	virtual void RenderMapEx(CDC& dc, CRect displayarea, CPoint scrolloff, CSize tiledims) {} //TODO - make pure
	virtual int GetCurrentToolIndex() const { return -1; } //TODO - make pure
	virtual void SetMouseDown(int imousedown) {} //TODO - make pure
	virtual int GetMouseDown() const { return 0; } //TODO - make pure
};
