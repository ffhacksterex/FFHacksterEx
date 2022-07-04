#pragma once

#include <afxwin.h>

#ifndef VERSIONHELPERAPI
#define VERSIONHELPERAPI inline bool
#endif

namespace {
	//DEVNOTE - I didn't have the Windows 8.1 SDK, so instead of downloading it I followed the implementation of VersionHelpers.h.
	//		I probably should have just downloaded the SDK ... maybe in the next revision I'll do so, remove this file,
	//		and add the necessary include where needed.
	//		Alas, MFC (in dlgfile.cpp) still calls the deprecated GetVersionEx(), wrapping it in pragma warning directives to
	//		suppress warning 4996 (deprecated function use). It's not a functional problem (yet) since it behaves correctly
	//		for Windows versions up to 8.0 and we're checking against 6.0 (Vista), but it is deprecated...

	VERSIONHELPERAPI IsWindowsVistaOrGreater()
	{
		//DEVNOTE - If building with XP, you might have to manually define _WIN32_WINNT_VISTA as 0x0600
		//		if the available SDK you use to build this doesn't support the new symbols
		//		(or just hardcode it since the Vista version values won't ever change).
		WORD wMajorVersion = HIBYTE(_WIN32_WINNT_VISTA);
		WORD wMinorVersion = LOBYTE(_WIN32_WINNT_VISTA);

		OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
		DWORDLONG const dwlConditionMask =
			VerSetConditionMask(
				VerSetConditionMask(
					VerSetConditionMask(
						0, VER_MAJORVERSION, VER_GREATER_EQUAL),
					VER_MINORVERSION, VER_GREATER_EQUAL),
				VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

		osvi.dwMajorVersion = wMajorVersion;
		osvi.dwMinorVersion = wMinorVersion;
		osvi.wServicePackMajor = 0;

		return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
	}
}
