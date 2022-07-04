#pragma once

#include <atlimage.h>

// A window that wants a background image can inherit from the class to reduce the amount
// of work needed to load and render the image.
//DEVNOTE - porting this to most other languages won't work the same way because other languages
//		use disallow multiple inheritance in favor of interfaces. This class behaves as a mixin,
//		which allows it to add functionality (through inheritance) without requiring every client
//		to implement an interface.

class CBackgroundMixin
{
public:
	CBackgroundMixin();
	CBackgroundMixin(UINT resid, bool enable = false);
	virtual ~CBackgroundMixin();

	void SetBackColor(COLORREF color);
	void ClearBackColor();
	bool HasBackColor();
	bool SetBackgroundResImage(UINT resid, bool enable);
	void EnableImage(bool enable);

	void DrawBackgroundColor(CDC & dc);
	void DrawBackgroundImage(CDC & dc);

	CBitmap* GetBackgroundImage();
	HBRUSH GetBackgroundBrush();

	void Release();

	virtual void PaintBackground(CDC & dc);
	virtual void PaintClient(CDC & dc);

	COLORREF BackgroundColor;
	CPoint DrawLocation;

protected:
	bool IsEnabled() const { return m_enabled; }

private:
	CBitmap * m_pimage = nullptr;
	CBrush * m_pbrush = nullptr;

	CBitmap m_liveimage;
	CBrush m_livebrush;
	CBitmap m_nullimage;
	CBrush m_nullbrush;

	bool m_enabled = false;
};
