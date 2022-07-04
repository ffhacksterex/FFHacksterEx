#pragma once

namespace Io {

	//REFACTOR - see FilePathRemover for full comments.
	//		Along those lines, I'd do the same thing here and rename
	//		this class to something like
	//		TempDirRef/TempFolderRef or AutoRemoveDir/AutoRemoveFolder.

	class DirPathRemover
	{
	public:
		DirPathRemover(CString dirpath);
		DirPathRemover(std::string dirpath);
		~DirPathRemover();

		void Revoke();

	private:
		CString m_dirpath;
	};

}
