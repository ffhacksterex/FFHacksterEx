// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxcmn.h>
#include <afxcontrolbars.h>
#include <afxdlgs.h>
#include <afxdialogex.h>

#include "shared_resids.h"
#include "resource_coredefs.h"
#include "resource_utils.h"

#include "logging_functions.h"

#include "ClearStatic.h"
#include "ClearButton.h"
#include "StrikeCheck.h"
#include <afxcontrolbars.h>
