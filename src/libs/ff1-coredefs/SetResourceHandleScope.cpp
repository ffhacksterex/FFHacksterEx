#include "stdafx.h"
#include "SetResourceHandleScope.h"

namespace Io
{

	SetResourceHandleScope::SetResourceHandleScope(HMODULE newmodule)
		: m_module(newmodule)
	{
		m_origmodule = AfxGetResourceHandle();
		AfxSetResourceHandle(m_module);
	}

	SetResourceHandleScope::~SetResourceHandleScope()
	{
		Restore();
	}

	void SetResourceHandleScope::Set(HMODULE newmodule)
	{
		m_module = newmodule;
		if (m_module != NULL)
			AfxSetResourceHandle(m_module);
		else
			Restore();
	}

	SetResourceHandleScope::operator HMODULE() const
	{
		return m_module;
	}

	void SetResourceHandleScope::Restore()
	{
		AfxSetResourceHandle(m_origmodule);
	}

}