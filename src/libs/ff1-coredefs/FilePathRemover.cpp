#include "stdafx.h"
#include "FilePathRemover.h"
#include "path_functions.h"

namespace Io
{

	FilePathRemover::FilePathRemover(CString filepath)
		: m_filepath(filepath)
	{
	}

	FilePathRemover::FilePathRemover(std::string filepath)
		: m_filepath(filepath.c_str())
	{
	}

	FilePathRemover::~FilePathRemover()
	{
		if (Paths::FileExists(m_filepath))
			Paths::FileDelete(m_filepath);
	}

	void FilePathRemover::Revoke()
	{
		m_filepath = "";
	}

}