#pragma once

namespace Io {

	//N.B. - this class' dtor  always calls FreeLibrary to release the module handle.
	//       don't mix calls to LoadLibrary/FreeLibrary with AfxLoadLibrary/AfxFreeLibrary!
	//       See MSDN documentation for more details.
	class LoadLibraryHandleScope
	{
	public:
		LoadLibraryHandleScope(HMODULE mod);
		LoadLibraryHandleScope(CString filepath);
		~LoadLibraryHandleScope();

		operator HMODULE() const;

	private:
		HMODULE m_hmod;
	};

}