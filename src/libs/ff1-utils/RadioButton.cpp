// RadioButton.cpp : implementation file
//
#include "stdafx.h"
#include "RadioButton.h"
#include "ODUtils.h"

// CRadioButton

IMPLEMENT_DYNAMIC(CRadioButton, CButton)

CRadioButton::CRadioButton()
{
}

CRadioButton::~CRadioButton()
{
}


BEGIN_MESSAGE_MAP(CRadioButton, CButton)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CRadioButton message handlers

int CRadioButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT(!ODUtils::HasAllStyles(GetStyle(), BS_OWNERDRAW));
	ASSERT(ODUtils::HasAtLeastOneOf(GetStyle(), { BS_CHECKBOX, BS_AUTOCHECKBOX, BS_3STATE, BS_AUTO3STATE }));
	return 0;
}

void CRadioButton::PreSubclassWindow()
{
	CButton::PreSubclassWindow();
	ASSERT(!ODUtils::HasAllStyles(GetStyle(), BS_OWNERDRAW));
	ASSERT(ODUtils::HasAtLeastOneOf(GetStyle(), { BS_CHECKBOX, BS_AUTOCHECKBOX, BS_3STATE, BS_AUTO3STATE }));
}