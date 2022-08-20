// CDrawingToolButton.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "DrawingToolButton.h"
#include <ui_helpers.h>
#include <imaging_helpers.h>

// CDrawingToolButton

IMPLEMENT_DYNAMIC(CDrawingToolButton, CSimpleImageButton)

CDrawingToolButton::CDrawingToolButton(UINT idimage, int index, COLORREF selcolor)
	: CSimpleImageButton(idimage, selcolor)
	, m_index(index)
{
}

CDrawingToolButton::~CDrawingToolButton()
{
}

int CDrawingToolButton::GetToolIndex() const
{
	return m_index;
}

sToolButtonData CDrawingToolButton::GetSpec() const
{
	return sToolButtonData{ m_idimage, m_restype, m_index };
}

void CDrawingToolButton::SuppressClickHandler(bool suppress)
{
	m_suppressclick = suppress;
}


BEGIN_MESSAGE_MAP(CDrawingToolButton, CSimpleImageButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CDrawingToolButton::OnBnClicked)
END_MESSAGE_MAP()


// CDrawingToolButton message handlers

void CDrawingToolButton::OnBnClicked()
{
	if (!m_suppressclick) {
		auto checked = Ui::GetCheckValue(*this);
		if (checked) {
			auto parent = GetParent();
			parent->SendMessage(WMA_DRAWTOOLBNCLICK, 0, m_index);
		}
	}
}