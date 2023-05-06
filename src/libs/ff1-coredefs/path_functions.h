#pragma once

#include <afx.h>
#include <vector>
#include <functional>
#include <utility>
#include <iosfwd>
//#include "pair_result.h"

class CWnd;

//REFACTOR - split the actual FileIO functions below into their own namespace
//		path handling and IO are related but distinct
namespace Paths
{
	bool FileExists(CString filepath);
	bool FileExists(std::string filepath);
	bool FileCopy(CString source, CString dest);
	bool FileCopyTo(CString source, CString destfolder);
	bool FileCopyOrZeroByte(CString source, CString dest);
	bool FileMoveToFolder(CString source, CString destfolder);
	bool FileMove(CString source, CString dest);
	bool FileRename(CString source, CString dest);
	bool FileDelete(CString filepath, bool forceDeleteReadonly = false);
	int FileSize(CString filepath);

	bool DirExists(CString dirpath);
	bool DirEmpty(CString dirpath);
	bool DirCopy(CString source, CString dest);
	bool DirShallowCopy(CString source, CString dest);
	bool DirUpdate(CString source, CString dest);
	bool DirShallowUpdate(CString source, CString dest);
	bool DirMove(CString source, CString dest);
	bool DirDelete(CString dirpath);
	void DirDeleteContents(CString dirpath);
	bool DirCreate(CString dirpath);      // creates a new directory or empties an existing one
	bool DirEnsurePath(CString dirpath);  // ensure that the directory exists, creatign if necessary

	CString Combine(std::initializer_list<CString> parts);
	CString QuoteUrl(CString url);

	CString GetCanonicalPath(CString pathname);
	CString EnsureCanonicalPath(CString pathname); 

	enum GetFilesScope { NotRecursive = 0, Recursive };
	using GetFileScopeIncludeProc = std::function<bool(CString entry)>;
	std::vector<CString> GetFiles(CString folder, GetFilesScope scope, GetFileScopeIncludeProc includeproc = [](CString filepath) {return true;});

	std::vector<CString> GetFiles(CString folder, CString filefilters, bool recurse);

	bool IsFile(CString pathname);
	bool IsFile(std::string pathname);
	bool IsDir(CString pathname);

	CString GetModuleFilePath();
	CString GetCWD();
	CString GetProgramName();
	CString GetProgramFolder();
	CString GetProgramExePath();
	CString GetDirectoryPath(CString fullpath);
	std::string GetDirectoryPath(std::string fullpath);
	CString GetParentDirectory(CString fullpath);
	CString GetFileName(CString fullpath);
	CString GetFileStem(CString fullpath);
	CString GetFileExtension(CString fullpath);
	CString MakePath(CString folder, CString filename);
	CString MakeRelativeTo(CString rootpath, CString foldertochange);

	CString ReplacePath(CString filepath, CString newpath);
	CString ReplaceFileTitle(CString origpath, CString newtitle);
	CString ReplaceExtension(CString origpath, CString newext);
	CString ReplaceFileName(CString filepath, CString filename);

	CString AddTrailingSlash(CString fullpath);
	std::string AddTrailingSlash(std::string fullpath);

	CString RemoveExtension(CString filepath);
	CString RemoveTrailingSlash(CString fullpath);
	std::string RemoveTrailingSlash(std::string fullpath);
	CString & RemoveTrailingSlashInplace(CString & fullpath);
	std::string & RemoveTrailingSlashInplace(std::string & fullpath);

	CString Rebase(CString base, CString origpath);
	CString RebaseToAppPath(CString origpath);
	CString GetAncestorFolder(CString fullpath, unsigned int levelsup);

	CString GetTempFolder();
	CString BuildTempFilePath(CString ext = "tmp");
	CString CopyToTempFile(CString srcfile, CString ext = "tmp");

	CString GetDateFormattedFileTitle();

	CString MutexNameFromPath(CString path);
}

namespace Urls {
	CString Combine(std::initializer_list<CString> parts);
	CString FilePathToUrl(CString filepath);

	bool UrlIsAccessible(CString url);
}

using PathCombineFunc = CString(*)(std::initializer_list<CString> parts);
