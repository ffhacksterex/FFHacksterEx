// CCloseButton.cpp : implementation file
//

#include "stdafx.h"
#include "CloseButton.h"
#include <imaging_helpers.h>

// CCloseButton

IMPLEMENT_DYNAMIC(CCloseButton, CClearButton)

CCloseButton::CCloseButton()
{
}

CCloseButton::~CCloseButton()
{
}


BEGIN_MESSAGE_MAP(CCloseButton, CClearButton)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CCloseButton message handlers


void CCloseButton::PreSubclassWindow()
{
	SetButtonImage(AfxGetResourceHandle(), IDPNG_CLOSESM, "PNG");

	CClearButton::PreSubclassWindow();
}


int CCloseButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CClearButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetButtonImage(AfxGetResourceHandle(), IDPNG_CLOSESM, "PNG");

	return 0;
}