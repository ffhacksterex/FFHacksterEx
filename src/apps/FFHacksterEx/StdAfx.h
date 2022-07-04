// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D113D5C7_DA0E_4EC3_829C_072530854E6E__INCLUDED_)
#define AFX_STDAFX_H__D113D5C7_DA0E_4EC3_829C_072530854E6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//XPCOMPAT
//--
#include <winsdkver.h>
#define WINVER _WIN32_WINNT_WINXP
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <SDKDDKVer.h>
//--

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxbutton.h>
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcontrolbars.h>
#include <afxdialogex.h>

#include "shared_resids.h"
#include "resource_coredefs.h"
#include "resource_utils.h"
#include "resource_subeditors.h"
#include "resource_editors.h"
#include "resource.h"

#include "logging_functions.h"

#include <ff1-utils-controls.h>
#include <afxwin.h>

#define DEF_PROGRAMCAPTION "FFHacksterEx"

#endif
