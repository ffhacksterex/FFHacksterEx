// CInfoButton.cpp : implementation file
//

#include "stdafx.h"
#include "InfoButton.h"
#include <imaging_helpers.h>

// CInfoButton

IMPLEMENT_DYNAMIC(CInfoButton, CClearButton)

CInfoButton::CInfoButton()
{
}

CInfoButton::~CInfoButton()
{
}


BEGIN_MESSAGE_MAP(CInfoButton, CClearButton)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CInfoButton message handlers


void CInfoButton::PreSubclassWindow()
{
	SetButtonImage(AfxGetResourceHandle(), IDPNG_INFOSM, "PNG");

	CClearButton::PreSubclassWindow();
}


int CInfoButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CClearButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetButtonImage(AfxGetResourceHandle(), IDPNG_INFOSM, "PNG");

	return 0;
}