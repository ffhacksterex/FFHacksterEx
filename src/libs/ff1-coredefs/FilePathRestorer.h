#pragma once

#include <afx.h>

namespace Paths
{

	// Sets the string represented by livepath to a temp newly-created temp file, and restores the original
	// path when the FilePathRestorer object is destructed.
	class FilePathRestorer
	{
	public:
		FilePathRestorer(CString & livepath);
		~FilePathRestorer();

		bool CanSave();
		bool Save();

	private:
		CString & m_livepath;
		CString m_origpath;
		CString m_temppath;
	};

}
