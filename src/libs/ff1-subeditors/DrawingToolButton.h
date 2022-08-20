#pragma once

#include <SimpleImageButton.h>

#define WMA_DRAWTOOLBNCLICK ((WM_APP) + 1)		// LPARAM = tool index

typedef struct sToolButtonData {
	UINT resid;
	CString restype;
	int param;
} sMapDlgButton;

// CDrawingToolButton

class CDrawingToolButton : public CSimpleImageButton
{
	DECLARE_DYNAMIC(CDrawingToolButton)

public:
	CDrawingToolButton(UINT idimage, int index, COLORREF selcolor = 0x80000000);
	virtual ~CDrawingToolButton();

	int GetToolIndex() const;
	sToolButtonData GetSpec() const;
	void SuppressClickHandler(bool suppress);

protected:
	bool m_suppressclick = false;
	int m_index = -1;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClicked();
};
