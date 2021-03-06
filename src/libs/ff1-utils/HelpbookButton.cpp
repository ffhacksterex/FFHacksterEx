// CHelpbookButton.cpp : implementation file
//

#include "stdafx.h"
#include "HelpbookButton.h"
#include <imaging_helpers.h>

// CHelpbookButton

IMPLEMENT_DYNAMIC(CHelpbookButton, CClearButton)

CHelpbookButton::CHelpbookButton()
{
}

CHelpbookButton::~CHelpbookButton()
{
}


BEGIN_MESSAGE_MAP(CHelpbookButton, CClearButton)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CHelpbookButton message handlers


int CHelpbookButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CClearButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetButtonImage(AfxGetResourceHandle(), IDPNG_HELPBOOKSM, "PNG");

	return 0;
}


void CHelpbookButton::PreSubclassWindow()
{
	SetButtonImage(AfxGetResourceHandle(), IDPNG_HELPBOOKSM, "PNG");

	CClearButton::PreSubclassWindow();
}
