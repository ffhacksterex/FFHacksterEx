#include "stdafx.h"
#include "path_functions.h"
#include "asmdll_impl.h"
#include "general_functions.h"
#include "io_functions.h"
#include "logging_functions.h"
#include "string_functions.h"
#include <afxole.h>
#include <afxdlgs.h>
#include <io.h>
#include <fstream>
#include <iterator>
#include <filesystem>

//STDFILESYS - several issues:
// 1) While microsoft maintains that VC++ Platform Toolset v141_xp (VC++ 2017) supports XP,
//		that toolset uses Vista+ APIs to implement the standard, cuasing runtime errors.
//		e.g. using the stream classes (<fstream>) generates an error because the path class
//			uses GetFinalPathNameByHandleW, a Vista+ API that's unavailable on XP.
// 2) Instead of using std::filesystem all over the place, examine the uses and create
//		a sensible set of functions to encapsulate those cases.
// 3) Unicode support? Not sure if I need to go as far as ICU since I'm not localizing,
//		but if someone else decides to it'll help that effort.
// 4) Clarify usage boundaries between CString and std::string.

//N.B. - the compiler recognizes _MSVC_LANG as the compiler specified in the Platform Toolset setting, but
//		the preprocessor seems to interpret it as the base compiler of the IDE.
//		That results in the code compiling correctly but intellisense highlighting the wrong code.
//		e.g. if compiling with v140_xp, the code below compiles and prints the following:
//				1>Building with _MSC_VER: 1900
//				1>Building with _MSVC_LANG: 201402L
//			However, Intellisense will enable the wrong branch of code and hovering will show
//			_MSC_VER and _MSVC_LANG with a more recent SDK's compiler version values.
#pragma message("Building with _MSC_VER: " ppstr(_MSC_VER))
#pragma message("Building with _MSVC_LANG: " ppstr(_MSVC_LANG))
#if _MSVC_LANG  <= 201402L
	namespace fsys = std::experimental::filesystem;
#else
	namespace fsys = std::filesystem;
#endif


namespace Paths
{

	bool FileExists(CString filepath)
	{
		auto thepath = fsys::path((LPCSTR)filepath);
		return fsys::exists(thepath) && IsFile(filepath);
	}

	bool FileCopy(CString source, CString dest)
	{
		if (!IsFile(source)) return false;

		BOOL copied = CopyFile(source, dest, FALSE);
		return copied == TRUE;
	}

	bool FileCopyTo(CString source, CString destfolder)
	{
		if (!IsFile(source)) return false;
		if (!IsDir(destfolder)) return false;

		std::error_code ec;
		fsys::copy((LPCSTR)source, (LPCSTR)destfolder, ec);
		bool haserror = (bool)ec;
		return !haserror;
	}

	bool FileCopyOrZeroByte(CString source, CString dest)
	{
		if (Paths::FileCopy(source, dest)) return true;

		std::ofstream ofs;
		ofs.open((LPCSTR)dest, std::ios::trunc);
		return ofs.is_open();
	}

	bool FileMoveToFolder(CString source, CString destfolder)
	{
		if (!IsFile(source)) {
			ErrorHere << source << " is not a file path." << std::endl;
			return false;
		}
		if (!IsDir(destfolder)) {
			ErrorHere << destfolder << " is not a file path." << std::endl;
			return false;
		}

		auto destfile = Paths::Combine({ destfolder, Paths::GetFileName(source) });
		BOOL moved = MoveFileEx(source, destfile, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		return moved == TRUE;
	}

	bool FileMove(CString source, CString dest)
	{
		if (!IsFile(source)) {
			ErrorHere << source << " is not a file path." << std::endl;
			return false;
		}

		BOOL moved = MoveFileEx(source, dest, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		return moved == TRUE;
	}

	bool FileRename(CString source, CString dest)
	{
		if (!IsFile(source)) return false;

		std::error_code ec;
		fsys::rename((LPCSTR)source, (LPCSTR)dest, ec);
		bool haserror = (bool)ec;
		return !haserror;
	}

	bool FileDelete(CString filepath, bool forceDeleteReadonly)
	{
		if (!FileExists(filepath)) return true;

		if (!IsFile(filepath)) return false;

		if (forceDeleteReadonly)
			Io::MakeWritable(filepath);
		return DeleteFile(filepath) == TRUE;
	}

	int FileSize(CString filepath)
	{
		std::error_code ec;
		auto size = (size_t)fsys::file_size((LPCSTR)filepath, ec);
		if (size == (size_t)-1) {
			ErrorHere << "Unable to retrieve size of file " << (LPCSTR)filepath << std::endl;
			size = 0;
		}
		return (int)size;
	}

	bool DirExists(CString dirpath)
	{
		auto thepath = fsys::path((LPCSTR)dirpath);
		return fsys::exists(thepath) && IsDir(dirpath);
	}

	bool DirEmpty(CString dirpath)
	{
		return DirExists(dirpath) && fsys::is_empty((LPCSTR)dirpath);
	}

	bool DirCopy(CString source, CString dest)
	{
		if (!IsDir(source)) return false;

		std::error_code ec;
		fsys::copy((LPCSTR)source, (LPCSTR)dest, fsys::copy_options::recursive | fsys::copy_options::overwrite_existing, ec);
		bool haserror = (bool)ec;
		return !haserror;
	}

	bool DirShallowCopy(CString source, CString dest)
	{
		if (!IsDir(source)) return false;

		auto files = GetFiles(source, GetFilesScope::NotRecursive);
		size_t copycount = 0;
		for (auto file : files) {
			if (FileCopyTo(file, dest))
				++copycount;
		}

		return copycount == files.size();
	}

	bool DirUpdate(CString source, CString dest)
	{
		if (!IsDir(source)) return false;

		std::error_code ec;
		fsys::copy((LPCSTR)source, (LPCSTR)dest, fsys::copy_options::recursive | fsys::copy_options::update_existing, ec);
		bool haserror = (bool)ec;
		return !haserror;
	}

	bool DirShallowUpdate(CString source, CString dest)
	{
		if (!IsDir(source)) return false;

		auto files = GetFiles(source, GetFilesScope::NotRecursive);
		for (auto file : files) {
			auto destfile = Paths::ReplacePath(file, dest);
			std::error_code ec;
			if (!fsys::copy_file((LPCSTR)file, (LPCSTR)destfile, fsys::copy_options::update_existing, ec)) {
				if ((bool)ec)
					return TraceFail("Replace path files for " + std::string((LPCSTR)destfile) + ": " + ec.message());
			}
		}

		return true;
	}

	bool DirMove(CString source, CString dest)
	{
		if (!IsDir(source)) return false;

		//DEVNOTE - MoveFileEx moves folders and files across volumes, haven't yet confirmed if filesystem's rename() does.
		BOOL moved = MoveFileEx(source, dest, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		return moved == TRUE;
	}

	bool DirDelete(CString dirpath)
	{
		if (dirpath.IsEmpty()) return false;

		static const auto failval = static_cast<std::uintmax_t>(-1);

		std::error_code ec;
		auto retval = fsys::remove_all((LPCSTR)dirpath, ec);
		if (retval == failval)
			return false;

		return true;
	}

	void DirDeleteContents(CString dirpath)
	{
		fsys::directory_iterator iter((LPCSTR)dirpath);
		for (const auto & dir : iter) {
			//REFACTOR - if this check really isn't needed, remove this in the next pass
			//if (!fsys::is_directory(dir) || dir.path().u8string()[0] != '.')
				fsys::remove_all(dir.path());
		}
	}

	bool DirCreate(CString dirpath)
	{
		if (dirpath.IsEmpty()) return false;

		bool deleted = DirDelete(dirpath);
		//N.B.- timing is an issue here, the call might return before the OS deletes the folder,
		//      so wait briefly and try again.
		int attempts = 3;
		bool created = DirEnsurePath(dirpath);
		while (!created && attempts-- > 0) {
			Sleep(10); //HACK - replace with std::this_thread::sleep_for(10ms)? still a hack either way...
			created = DirEnsurePath(dirpath);
		}
		return deleted && created;
	}

	bool DirEnsurePath(CString dirpath)
	{
		std::error_code ec;
		if (!fsys::create_directories((LPCSTR)dirpath, ec))
			return false;

		return DirExists(dirpath);
	}

	CString Combine(std::initializer_list<CString> parts)
	{
		fsys::path fpath;
		for (auto part : parts)
			fpath.append((LPCSTR)part);
		return CString(fpath.u8string().c_str());
	}

	CString Paths::QuoteUrl(CString url)
	{
		CString quotedurl;
		auto quote = url.Find('"');
		if (quote == -1) {
			quotedurl.Format("\"%s\"", (LPCSTR)url);
		}
		else if (quote == 0) {
			//HACK - Hopefully it's properly dquoted, but that isn't enforced here.
			ASSERT(url[url.GetLength() - 1] == '"');
			quotedurl = url;
		}
		else {
			// Not sure wat the URL format is here, but don't alter it.
			//N.B. - Whoever passes the URL will have to alter it as needed.
			quotedurl = url;
		}
		return quotedurl;
	}

	// Returns the canonical version of pathname.
	CString GetCanonicalPath(CString pathname)
	{
		//fsys::path p((LPCSTR)pathname);
		//auto canon = fsys::weakly_canonical(p); //VISTA+
		//return CSting(canon.u8string().c_str());

		CString cs;
		auto buf = cs.GetBufferSetLength(_MAX_PATH);
		LPSTR filepath = nullptr;
		GetFullPathName(pathname, MAX_PATH, buf, &filepath);
		cs.ReleaseBuffer();
		return cs;
	}

	// Returns the canonical version of pathname, but throws if pathname doesn't exist.
	CString EnsureCanonicalPath(CString pathname)
	{
		//fsys::path p((LPCSTR)pathname);
		//auto canon = fsys::canonical(p); //VISTA+
		//return CString(canon.u8string().c_str());

		auto cs = GetCanonicalPath(pathname);
		if (!Paths::FileExists(cs))
			throw std::runtime_error(std::string("Canonical path does not exist for: ") + (LPCSTR)cs);
		return cs;
	}

	// Retrieve all files in a folder - optionally including subfolders and optionally including a
	// predicate to dictate what files are included.
	std::vector<CString> GetFiles(CString folder, GetFilesScope scope, GetFileScopeIncludeProc includeproc)
	{
		std::vector<CString> files;
		std::error_code ec;
		//DEVNOTE - directory_iterator and recursive_directory_iterator are unrelated classes (not derived
		//		from a common base class), so the loop implementation here is split.
		if (scope == GetFilesScope::Recursive) {
			fsys::recursive_directory_iterator iter((LPCSTR)folder, ec);
			for (const fsys::directory_entry & entry : iter) {
				if (!fsys::is_regular_file(entry.status())) continue;

				CString filepath = entry.path().u8string().c_str();
				if (!includeproc(filepath)) continue;

				files.push_back(filepath);
			}
		}
		else {
			fsys::directory_iterator iter((LPCSTR)folder, ec);
			for (const fsys::directory_entry & entry : iter) {
				if (!fsys::is_regular_file(entry.status())) continue;

				CString filepath = entry.path().u8string().c_str();
				if (!includeproc(filepath)) continue;

				files.push_back(filepath);
			}
		}
		return files;
	}



	//-- BEGIN
	namespace {

		void DoGetFiles(std::vector<CString> & files, CString folder, CString filters, bool recurse)
		{
			_finddata_t c_file = { 0 };
			intptr_t hFile;

			// Find the files matching the filter in this folder
			if (!filters.IsEmpty()) {
				auto thefilters = Strings::split(filters, ";");
				for (auto filter : thefilters) {
					intptr_t hspecific = _findfirst(Paths::Combine({ folder, filter }), &c_file);
					if (hspecific != -1L) {
						do {
							files.push_back(Paths::Combine({ folder,c_file.name }));
						} while (_findnext(hspecific, &c_file) == 0);
						_findclose(hspecific);
					}
				}
			}

			// Find all subfolders in the directory
			if ((hFile = _findfirst(folder + "\\*.*", &c_file)) == -1L)
				WarnHere << "No *.c files in current directory!" << std::endl;
			else
			{
				do {
					// If this is a folder, go into it if we're recursing
					if (c_file.attrib & _A_SUBDIR) {
						if (recurse && c_file.name[0] != '.') {
							auto combinedfile = Paths::Combine({ folder, c_file.name });
							DoGetFiles(files, combinedfile, filters, recurse);
						}
					}
					else if (filters.IsEmpty()) {
						files.push_back(c_file.name);
					}
				} while (_findnext(hFile, &c_file) == 0);
				_findclose(hFile);
			}
		}
	}

	std::vector<CString> GetFiles(CString folder, CString filefilters, bool recurse)
	{
		std::vector<CString> files;
		DoGetFiles(files, folder, filefilters, recurse);
		return files;
	}
	//-- END



	bool IsFile(CString pathname)
	{
		std::error_code ec;
		return fsys::is_regular_file((LPCSTR)pathname, ec);
	}

	bool IsDir(CString pathname)
	{
		std::error_code ec;
		return fsys::is_directory((LPCSTR)pathname, ec);
	}

	CString GetModuleFilePath()
	{
		char szpath[_MAX_PATH] = { 0 };
		GetModuleFileName(nullptr, szpath, _MAX_PATH);
		return CString(szpath);
	}

	CString GetCWD()
	{
		return CString(fsys::current_path().u8string().c_str());
	}

	CString GetProgramName()
	{
		char szpath[_MAX_PATH] = { 0 };
		GetModuleFileName(nullptr, szpath, _MAX_PATH);
		return GetFileStem(szpath);
	}

	CString GetProgramFolder()
	{
		char szpath[_MAX_PATH] = { 0 };
		GetModuleFileName(nullptr, szpath, _MAX_PATH);
		std::string appfolder = fsys::path(szpath).parent_path().u8string();
		return GetCanonicalPath(appfolder.c_str());
	}

	CString GetProgramExePath()
	{
		char szpath[_MAX_PATH] = { 0 };
		GetModuleFileName(nullptr, szpath, _MAX_PATH);
		return GetCanonicalPath(szpath);
	}

	// Gets the folder referenced by fullpath.
	// If fullpath is a file, the filename is removed and the folder is returned.
	// If fullpath doesn't end with a slash of either type, it's treated as a file path.
	// 	   // The trailing slash (of either type) is removed.
	// e.g. C:/some/folder/ -> C:/some/folder
	//      C:/some/folder/file.ext -> C:/some/folder
	//		C:\\some\\folder\\ -> C:\\some\\folder
	//		C:\\some\\folder\\file.ext -> C:\\some\\folder
	//		C:/some/folder -> C:/some
	//		C:\\some\\folder -> C:\\some
	//		*PRJ*/file.ext -> *PRJ*
	//		*PRJ*\\file.ext -> *PRJ*
	//		*PRJ* -> *PRJ*
	CString GetDirectoryPath(CString fullpath)
	{
		if (fullpath.IsEmpty()) return "";

		//DEVNOTE - if a folder path doesn't end with fwd or back slash,
		//	then path sees it as a file with no extension, not a directory.
		auto thepath = fsys::path((LPCTSTR)fullpath);
		if (fsys::is_directory(thepath)) {
			auto dir = CString(thepath.u8string().c_str());
			return RemoveTrailingSlashInplace(dir);
		}

		// If it's a path placeholder, special handling is required
		// because the leading star prevents the path class from recognizing
		// the string as a directory (which is intentional).
		if (fullpath[0] == '*') {
			auto last = fullpath.ReverseFind('/');
			last = (last != -1) ? last : fullpath.ReverseFind('\\');			
			auto dir = (last == -1) ? fullpath : fullpath.Mid(0, last);
			return dir;
		}

		auto str = RemoveTrailingSlash(thepath.remove_filename().u8string());
		return str.c_str();
	}

	// Gets the parent directory of the folder specified by fullpath, whether it's a file or folder.
	// If fullpath is a file, then the fileame is remove, then the parent folder is retrieved.
	// e.g. C:/some/folder -> C:/some
	//      C:/some/folder/file.ext -> C:/some
	CString GetParentDirectory(CString fullpath)
	{
		auto thepath = fsys::path((LPCTSTR)fullpath);
		if (!fsys::is_directory(thepath))
			thepath.remove_filename();
		auto parent = thepath.parent_path().u8string();
		return parent.c_str();
	}

	CString GetFileName(CString fullpath)
	{
		auto thepath = fsys::path((LPCTSTR)fullpath);
		return CString(thepath.filename().u8string().c_str());
	}

	CString GetFileStem(CString fullpath)
	{
		auto thepath = fsys::path((LPCTSTR)fullpath);
		return CString(thepath.stem().u8string().c_str());
	}

	// Returns the extension, including the leading dot.
	CString GetFileExtension(CString fullpath)
	{
		auto thepath = fsys::path((LPCTSTR)fullpath);
		return CString(thepath.extension().u8string().c_str());
	}

	CString MakePath(CString folder, CString filename)
	{
		auto thepath = fsys::path((LPCTSTR)GetDirectoryPath(folder));
		if (!fsys::is_directory(thepath))
			thepath.remove_filename();

		return CString(thepath.append((LPCTSTR)filename).u8string().c_str());
	}

	//TOO - should .\\ or ./ be prepended? for now, no.
	CString MakeRelativeTo(CString rootpath, CString foldertochange)
	{
		CString outfolder = foldertochange;
		CString normroot(rootpath);
		CString normftc(foldertochange);
		normroot.MakeLower();
		normftc.MakeLower();
		if (normftc.Find(normroot) == 0) {
			// The folder starts with the root, so remove the root from the folder
			outfolder = foldertochange.Mid(normroot.GetLength());
			if (outfolder.FindOneOf("\\/") == 0) {
				outfolder.Insert(0, ".");
			}
		}
		return outfolder;
	}

	CString ReplacePath(CString filepath, CString newpath)
	{
		fsys::path origpath = (LPCSTR)filepath;
		auto fname = origpath.filename();
		auto replacedpath = fsys::path((LPCSTR)newpath) / fname;
		auto strreplaced = replacedpath.u8string();
		return CString(strreplaced.c_str());
	}

	CString ReplaceExtension(CString filepath, CString newext)
	{
		fsys::path origpath = (LPCSTR)filepath;
		auto base = origpath.parent_path();
		auto stem = origpath.stem().u8string();

		// Ensure there's a dot between the filename that's there and the extension we're adding.
		// Then add the extension.
		if (newext.Find('.') != 0) newext.Insert(0, '.');
		CString retval(base.append(stem + (LPCSTR)newext).u8string().c_str());
		return retval;
	}

	CString ReplaceFileTitle(CString filepath, CString newtitle)
	{
		fsys::path origpath = (LPCSTR)filepath;
		auto base = origpath.parent_path();
		auto stem = origpath.stem().u8string();
		auto ext = origpath.extension().u8string();

		auto srcdotmark = stem.find('.');
		if (srcdotmark != std::string::npos) {
			auto dstdotmark = newtitle.Find('.');
			if (dstdotmark != -1) {
				// oldname.one.two.ext > newname.alpha.beta = newname.alpha.beta.ext
				// replace the stem
				stem = newtitle;
			}
			else {
				// oldname.one.two.ext > newname = newname.one.two.ext
				// only replace the first portion of the source
				stem = (LPCSTR)newtitle + stem.substr(srcdotmark);
			}
		}
		else {
			// just replace the stem with the new title
			stem = newtitle;
		}

		CString retval(base.append(stem + ext).u8string().c_str());
		return retval;
	}

	CString ReplaceFileName(CString filepath, CString filename)
	{
		fsys::path origpath = (LPCSTR)filepath;
		auto base = origpath.parent_path();
		CString retval = base.append((LPCSTR)filename).u8string().c_str();
		return retval;
	}

	CString RemoveExtension(CString filepath)
	{
		fsys::path origpath = (LPCSTR)filepath;
		auto base = origpath.parent_path();
		auto stem = origpath.stem().u8string();

		CString retval(base.append(stem).u8string().c_str());
		return retval;
	}

	CString RemoveTrailingSlash(CString fullpath)
	{
		if (fullpath.IsEmpty()) return fullpath;

		auto lastindex = fullpath.GetLength() - 1;
		auto lastch = fullpath[lastindex];
		auto prefsep = CStringA(fsys::path::preferred_separator, 1);
		if (lastch == prefsep[0] || lastch == '/')
			return fullpath.Mid(0, lastindex);

		return fullpath;
	}

	std::string RemoveTrailingSlash(std::string fullpath)
	{
		if (fullpath.empty()) return fullpath;

		auto lastindex = fullpath.length() - 1;
		auto lastch = fullpath[lastindex];
		auto prefsep = std::string(1, fsys::path::preferred_separator);
		if (lastch == prefsep[0] || lastch == '/')
			return fullpath.substr(0, lastindex);

		return fullpath;
	}

	CString& RemoveTrailingSlashInplace(CString& fullpath)
	{
		auto index = fullpath.GetLength() - 1;
		auto last = index > -1 ? (fullpath[index] == '/' || fullpath[index] == '\\') : false;
		if (last) fullpath.Delete(last, 1);
		return fullpath;
	}

	std::string& RemoveTrailingSlashInplace(std::string& fullpath)
	{
		auto index = fullpath.length() - 1;
		auto last = index > -1 ? (fullpath[index] == '/' || fullpath[index] == '\\') : false;
		if (last) fullpath.erase(last, 1);
		return fullpath;
	}
	
	// If origpath starts with a single dot (.\) or (./),
	// then rebase it with the path base.
	CString Rebase(CString base, CString origpath)
	{
		//TODO - update to C++ 17 to use filesystem::path::lexically_normal() here
		auto p = fsys::path((LPCSTR)origpath);
		auto root = p.root_name();
		auto parent = p.parent_path();
		if (root != L"." && parent != L".") return origpath;

		//REFACTOR - there's got to be a cleaner way to do this using the standard library
		auto stem = origpath.GetLength() > 1 ? origpath.Mid(1) : "";
		if (stem.GetLength() > 0 && (stem[0] == '/' || stem[0] == '\\'))
			stem = stem.Mid(1);
		auto temp = Paths::Combine({ base,  stem });
		auto newp = fsys::path((LPCSTR)temp);
		//auto final = fsys::weakly_canonical(newp); //VISTA+
		//return CString(final.u8string().c_str());

		auto newpstring = newp.u8string();
		auto final = GetCanonicalPath(newpstring.c_str());
		return final;
	}

	CString RebaseToAppPath(CString origpath)
	{
		return Rebase(Paths::GetProgramFolder(), origpath);
	}

	CString GetAncestorFolder(CString fullpath, unsigned int levelsup)
	{
		auto thepath = fsys::path((LPCTSTR)fullpath);
		auto root = thepath.root_directory();
		LogHere << "Original path is " << thepath.u8string() << std::endl;

		for (auto i = 0u; i < levelsup; ++i) {
			thepath.remove_filename(); // chops a folder name
			LogHere << "  -> ancestor is " << thepath.u8string() << std::endl;
			if (thepath == root) {
				ErrorHere << "Cannot access root folder as an ancestor of " << thepath.u8string() << std::endl;
				throw std::runtime_error("Cannot access root folder as an ancestor of " + thepath.u8string());
			}
		}
		return CString(thepath.u8string().c_str());
	}

	CString GetTempFolder()
	{
		auto temp = fsys::temp_directory_path().u8string();
		return CString(temp.c_str());
	}

	// Returns a pseudorandom temp file path in the temp directory.
	CString BuildTempFilePath(CString ext)
	{
		CString fnameext;
		fnameext.Format("%d.%s", rand(), (LPCSTR)ext);
		auto newpath = fsys::temp_directory_path().append((LPCSTR)fnameext);
		return CString(newpath.u8string().c_str());
	}

	CString CopyToTempFile(CString srcfile, CString ext)
	{
		CString temp = BuildTempFilePath(ext);
		FileDelete(temp, true); // delete read-only temp files as well
		bool copied = FileCopy(srcfile, temp);
		return copied ? temp : CString();
	}

	CString GetDateFormattedFileTitle()
	{
		COleDateTime now(COleDateTime::GetCurrentTime());
		return now.Format("%Y-%m-%d,%H%M%S");
	}

	CString MutexNameFromPath(CString path)
	{
		CString name = path;
		name.Replace('\\', '/'); // mutexes don't allow backslashes
		return name;
	}

} // end namespace Paths

namespace Urls
{

	CString Combine(std::initializer_list<CString> parts)
	{
		CString out;
		bool notfirst = false;
		for (auto part : parts) {
			out.Append(notfirst ? "/" : "");
			out.Append(part);
			notfirst = true;
		}
		return out;
	}

	CString FilePathToUrl(CString filepath)
	{
		if (filepath.Find("file:///") == 0) return filepath;

		// Prefix the protocol and change back slashes to forward.
		auto result = filepath;
		result.Replace("\\", "/");
		return "file:///" + result;
	}

	bool UrlIsAccessible(CString url)
	{
		//TODO - use CHttpFile or similar to send a HEAD request to see if the page exists?
		//		see these articles
		//		https://docs.microsoft.com/en-us/cpp/mfc/steps-in-a-typical-http-client-application?view=msvc-160
		//		https://docs.microsoft.com/en-us/cpp/mfc/reference/cinternetsession-class?view=msvc-160
		//		If a server doesn't handle HEAD, then what can I do at that point?
		//		And how do I tell, Http Status code response?
		//		For now, don't bother checking and just return true.
		return true;
	}

} // end namespace Urls