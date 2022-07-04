#include "stdafx.h"
#include "DirPathRemover.h"
#include "path_functions.h"

namespace Io {

	DirPathRemover::DirPathRemover(CString dirpath)
		: m_dirpath(dirpath)
	{
	}

	DirPathRemover::DirPathRemover(std::string dirpath)
		: m_dirpath(dirpath.c_str())
	{
	}

	DirPathRemover::~DirPathRemover()
	{
		if (Paths::DirExists(m_dirpath))
			Paths::DirDelete(m_dirpath);
	}

	void DirPathRemover::Revoke()
	{
		m_dirpath = "";
	}

}