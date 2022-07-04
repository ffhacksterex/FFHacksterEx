#include "stdafx.h"
#include "imaging_helpers.h"
#include <gdiplusgraphics.h>

namespace Imaging
{

	ULONG_PTR GDIPTOKEN = 0;

	bool InitGdiplus()
	{
		Gdiplus::GdiplusStartupInput startup;
		startup.GdiplusVersion = 1;
		startup.DebugEventCallback = nullptr;
		startup.SuppressBackgroundThread = FALSE;
		startup.SuppressExternalCodecs = FALSE;

		Gdiplus::GdiplusStartupOutput output;
		auto gdstatus = Gdiplus::GdiplusStartup(&GDIPTOKEN, &startup, &output);
		if (gdstatus != Gdiplus::Status::Ok)
		{
			ErrorHere << "Unable to initialize GDI+" << std::endl;
			return false;
		}

		return true;
	}

	void FreeGdiplus()
	{
		if (GDIPTOKEN != 0) {
			Gdiplus::GdiplusShutdown(GDIPTOKEN);
		}
	}

	Gdiplus::Bitmap * ImageLoad(CString imagepath)
	{
		CStringW wimagepath(imagepath);
		return Gdiplus::Bitmap::FromFile(wimagepath);
	}

	Gdiplus::Bitmap * ImageLoad(HINSTANCE hinst, UINT resid, CString restype)
	{
		//DEVNOTE - CImage, CPngImage, and the rest of the ATL support classes only seem to actually
		//		support bitmaps with their resource loading functions. Other image types either
		//		won't load or produce strange results.
		//		Copying the resource to a memory stream and loading from there seems to be the solution.
		Gdiplus::Bitmap * bmp = nullptr;
		auto rsrc = FindResource(hinst, MAKEINTRESOURCE(resid), restype);
		auto size = SizeofResource(hinst, rsrc);
		auto data = LockResource(LoadResource(hinst, rsrc));
		auto hglobal = GlobalAlloc(GMEM_MOVEABLE, size);
		auto ptr = GlobalLock(hglobal);
		if (ptr) {
			// SIGH ... now that we've loaded the resource, copy it to global memory, attach it to a strem,
			// and then load the Gdiplus from the stream. Maybe I should've just used FreeImage?
			CopyMemory(ptr, data, size);
			IStream * stream = nullptr;
			if (SUCCEEDED(CreateStreamOnHGlobal(ptr, FALSE, &stream))) {
				bmp = new Gdiplus::Bitmap(stream);
				stream->Release();
			}
			GlobalUnlock(ptr);
		}
		GlobalFree(ptr);
		return bmp;
	}

} // end namespace Imaging