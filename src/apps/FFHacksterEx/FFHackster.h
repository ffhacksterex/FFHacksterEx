// FFHackster.h : main header file for the FFHACKSTER application
//

#if !defined(AFX_FFHACKSTER_H__5C96F70B_02AC_4DCA_9FB4_B5B9F1D7687A__INCLUDED_)
#define AFX_FFHACKSTER_H__5C96F70B_02AC_4DCA_9FB4_B5B9F1D7687A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <FFBaseApp.h>

/////////////////////////////////////////////////////////////////////////////
// CFFHacksterApp:
// See FFHackster.cpp for the implementation of this class
//

class CFFHacksterApp : public CFFBaseApp
{
public:
	CFFHacksterApp();

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	CString AppIni;
	CString GetIniitalAction();

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_FFHACKSTER_H__5C96F70B_02AC_4DCA_9FB4_B5B9F1D7687A__INCLUDED_)
