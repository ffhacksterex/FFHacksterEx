#pragma once

namespace Io {

	//REFACTOR - implement copy ctor/oper= and refcounting to allow these to
	//		be passed returned from factory functions and to exist in collections.
	//		I see use cases where these can be passed along with data and
	//		remove the related files once the lifetime expires.
	//		That would also allow factories to create these, and if we
	//		add a getting for the filepath that would eliminate the need
	//		to have to hold onto both as variables.
	//		In that case, I'd also rename this class to something like
	//		TempFileRef or AutoRemoveFile.

	class FilePathRemover
	{
	public:
		FilePathRemover(CString filepath);
		FilePathRemover(std::string filepath);
		~FilePathRemover();

		void Revoke();

	private:
		CString m_filepath;
	};

}
