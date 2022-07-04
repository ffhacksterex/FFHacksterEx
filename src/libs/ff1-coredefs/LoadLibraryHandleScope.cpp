#include "stdafx.h"
#include "LoadLibraryHandleScope.h"

namespace Io {

	LoadLibraryHandleScope::LoadLibraryHandleScope(HMODULE mod)
		: m_hmod(mod)
	{
	}

	LoadLibraryHandleScope::LoadLibraryHandleScope(CString filepath)
		: m_hmod(LoadLibrary(filepath))
	{
	}

	LoadLibraryHandleScope::~LoadLibraryHandleScope()
	{
		if (m_hmod != NULL) FreeLibrary(m_hmod);
	}

	LoadLibraryHandleScope::operator HMODULE() const
	{
		return m_hmod;
	}

}