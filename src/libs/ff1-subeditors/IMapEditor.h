#pragma once

#include <afxwin.h>

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

	virtual void HandleMapImport() = 0;
	virtual void HandleMapExport() = 0;
	virtual bool HandleCustomizeTool() = 0;
};

