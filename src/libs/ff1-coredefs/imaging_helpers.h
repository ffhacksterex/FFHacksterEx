#pragma once

#include <gdiplus.h>

namespace Imaging
{

	extern ULONG_PTR GDIPTOKEN;

	bool InitGdiplus();
	void FreeGdiplus();

	Gdiplus::Bitmap * ImageLoad(CString imagepath);
	Gdiplus::Bitmap * ImageLoad(HINSTANCE hinst, UINT resid, CString restype);

}