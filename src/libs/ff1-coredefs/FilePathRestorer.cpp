#include "StdAfx.h"
#include "FilePathRestorer.h"
#include "path_functions.h"

namespace Paths
{

	FilePathRestorer::FilePathRestorer(CString & livepath)
		: m_livepath(livepath)
		, m_origpath(livepath)
	{
		auto temp = CopyToTempFile(livepath, "ffh");
		if (FileExists(temp))
			m_livepath = temp;
		else
			ErrorHere << "A temp file couldn't be created to back the changes, won't be able to save to " << (LPCSTR)livepath << std::endl;
	}

	FilePathRestorer::~FilePathRestorer()
	{
		if (m_livepath != m_origpath) {
			if (FileExists(m_livepath))
				FileDelete(m_livepath);
		}
		m_livepath = m_origpath;
	}

	bool FilePathRestorer::CanSave()
	{
		return m_livepath != m_origpath && FileExists(m_origpath);
	}

	bool FilePathRestorer::Save()
	{
		if (!CanSave()) {
			if (m_livepath != m_origpath)
				ErrorHere << "Can't save, a temp file wasn't created to back the changes." << std::endl;
			else
				ErrorHere << "Can't save, the original file doesn't exist." << std::endl;
			return false;
		}

		return FileCopy(m_livepath, m_origpath);
	}

}