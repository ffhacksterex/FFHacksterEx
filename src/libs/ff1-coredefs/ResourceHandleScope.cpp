#include "stdafx.h"
#include "ResourceHandleScope.h"

namespace Io
{

	ResourceHandleScope::ResourceHandleScope(HMODULE newmodule)
		: m_module(newmodule)
	{
		m_origmodule = AfxGetResourceHandle();
		AfxSetResourceHandle(m_module);
	}

	ResourceHandleScope::~ResourceHandleScope()
	{
		Restore();
	}

	void ResourceHandleScope::Set(HMODULE newmodule)
	{
		m_module = newmodule;
		if (m_module != NULL)
			AfxSetResourceHandle(m_module);
		else
			Restore();
	}

	ResourceHandleScope::operator HMODULE() const
	{
		return m_module;
	}

	void ResourceHandleScope::Restore()
	{
		AfxSetResourceHandle(m_origmodule);
	}

}