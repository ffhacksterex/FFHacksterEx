#include "stdafx.h"
#include "io_functions.h"
#include "path_functions.h"
#include "vector_types.h"
#include "FilePathRemover.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

//STDFILESYS - see path_functions.cpp for more info
#if _MSVC_LANG  <= 201402L
namespace fsys = std::experimental::filesystem;
#else
namespace fsys = std::filesystem;
#endif

namespace Io 
{

	bool open_file(CString filename, std::ifstream & ifs, bool asbinary)
	{
		if (ifs.is_open()) ifs.close();
		auto flags = asbinary ? std::ios::binary : 0;
		ifs.open((LPCSTR)filename, flags);
		if (ifs.is_open()) return true;

		ErrorHere << "Unable to open file " << (LPCSTR)filename << std::endl;
		return false;
	}

	bool open_file(CString filename, std::ofstream & ofs, bool asbinary)
	{
		if (ofs.is_open()) ofs.close();
		auto flags = asbinary ? std::ios::binary : 0;
		ofs.open((LPCSTR)filename, flags);
		if (ofs.is_open()) return true;

		ErrorHere << "Unable to open file " << (LPCSTR)filename << std::endl;
		return false;
	}

	bool create_file(CString filename, std::ofstream & ofs, bool asbinary)
	{
		if (ofs.is_open()) ofs.close();
		auto flags = std::ios::trunc | (asbinary ? std::ios::binary : 0);
		ofs.open((LPCSTR)filename, flags);

		if (!ofs.is_open())
			THROWEXCEPTION(std::ios_base::failure, "Failed to create the file " + std::string((LPCSTR)filename));

		return true;
	}

	std::ostream & create_asmstream(std::ofstream & ofs, std::string asmfile, int flags)
	{
		UNREFERENCED_PARAMETER(flags);

		if (ofs.is_open()) ofs.close();
		if (!Io::create_file(asmfile.c_str(), ofs))
			THROWEXCEPTION(std::runtime_error, "Can't load ASM inline, can't open the assembly file " + asmfile + " to load as inline assembly");

		return ofs;
	}

	bool move_to_marker(std::istream & is, std::string marker, bool gotonextline)
	{
		std::string line;
		while (is && !is.eof()) {
			auto pos = is.tellg();
			std::getline(is, line) >> std::ws;
			size_t linepos = line.find(marker);
			if (linepos != std::string::npos) {
				if (!gotonextline) {
					is.clear();
					is.seekg((size_t)pos + linepos + marker.size());
				}
				break;
			}
		}
		return is.good();
	}

	void clear_eof(std::istream & is)
	{
		is.clear(is.rdstate() & ~std::ios::eofbit);
	}

	size_t discard_line(std::istream & is)
	{
		std::string line;
		std::getline(is, line);
		return 0;
	}

	std::string preview_next_line(std::istream & is)
	{
		// If already EOF, nothing to return
		if (is.eof()) return std::string();

		// If the next character is EOF, then pull it and let EOF happen normally
		if (is.peek() == (char)EOF) {
			is.get();
			return std::string();
		}

		// Else, record the current position, read, and clear EOF (in case reading set it) before restoring the position
		auto pos = is.tellg();
		std::string line;
		std::getline(is, line);
		clear_eof(is);
		is.seekg(pos);
		return line;
	}

	std::ostream & write_next_line(std::ostream & os, std::string line)
	{
		if (os.tellp() > 0) os << "\n";
		os << line;
		return os;
	}

	std::ostream & write_current_line(std::ostream& os, std::string line)
	{
		os << line;
		return os;
	}

	size_t copy_next_line(std::istream & is, std::ostream & os)
	{
		std::string line;
		std::getline(is, line);
		write_next_line(os, line);
		return 0;
	}

	void copy_to_end(std::istream & is, std::ostream & os)
	{
		while (is && !is.eof()) {
			std::string line;
			std::getline(is, line);
			write_next_line(os, line);
		}
	}

	void load_binary(CString filepath, std::vector<unsigned char> & vec, bool allowzerolength)
	{
		bool loaded = false;

		vec.clear();

		CString & path = filepath;
		std::ifstream ifs;
		ifs.open((LPCSTR)path, std::ios::binary | std::ios::ate);
		if (!ifs.is_open())
			THROWEXCEPTION(std::ios_base::failure, "Can't open the file " + std::string(path));

		auto length = (size_t)ifs.tellg();
		if (length == 0 && allowzerolength)
			THROWEXCEPTION(std::runtime_error, "Rejecting read of aero-length assembly binary file " + std::string((LPCSTR)filepath));

		vec.resize(length);
		ifs.seekg(0);
		ifs.read((char*)address(vec), length);
		loaded = vec.size() == length;
	}

	void save_binary(CString filepath, const std::vector<unsigned char> & vec, bool allowzerolength)
	{
		if (vec.empty() && allowzerolength)
			THROWEXCEPTION(std::runtime_error, "Rejecting write of aero-length assembly binary file " + std::string((LPCSTR)filepath));

		CString temppath(filepath + ".tmp"); //N.B.- chaep temp filename
		FilePathRemover remover(temppath);
		{
			std::ofstream ofs;
			ofs.open((LPCSTR)temppath, std::ios::binary | std::ios::trunc);
			if (!ofs.is_open())
				throw std::runtime_error("Can't set up write to temp file - " + std::string(temppath));

			ofs.write((char*)address(vec), vec.size());
		}
		if (!Paths::FileMove(temppath, filepath))
			THROWEXCEPTION(std::runtime_error, "Failed to save working temp file to assembly binary " + std::string((LPCSTR)filepath));
	}

	HMODULE LoadModule(CString modulepath)
	{
		HMODULE mod = NULL;
		GetModuleHandleEx(0, modulepath, &mod);
		return mod;
	}

	// Gets a handle to the module that calls this function.
	//N.B. - since this is in a static library, this will be compiled into the calling module (EXE or DLL).
	HMODULE LoadCurrentModule()
	{
		HMODULE mod = NULL;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)LoadCurrentModule, &mod);
		return mod;
	}

	CString GetCurrentModulePath()
	{
		CString cspath;
		auto module = LoadCurrentModule();
		if (module != NULL) {
			cspath = GetModulePath(module);
			FreeLibrary(module);
		}
		return cspath;
	}

	CString GetModulePath(HMODULE module)
	{
		char szfilename[_MAX_PATH] = { 0 };
		GetModuleFileName(module, szfilename, _countof(szfilename));
		return szfilename;
	}

	bool RemoveAttributes(CString pathname, int attributes)
	{
		auto attrs = GetFileAttributes(pathname);
		if (attrs == INVALID_FILE_ATTRIBUTES)
			return false;

		int failures = 0;
		if (attrs & attributes) {
			if (!SetFileAttributes(pathname, attrs & ~attributes))
				return false;

			// Recurse through subdirectories.
			if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
				if (!pathname.IsEmpty() && pathname[0] != '.') {
					for (auto & it : fsys::recursive_directory_iterator((LPCSTR)pathname)) {
						// The attribute GET can fail, so increment the failure count if that happens.
						auto strpath = it.path().u8string();
						auto fileattrs = GetFileAttributes(strpath.c_str());
						if (fileattrs == INVALID_FILE_ATTRIBUTES) {
							++failures;
						}
						else if (fileattrs & attributes) {
							// If the attribute SET fails, then increment the failure count.
							if (!SetFileAttributes(strpath.c_str(), fileattrs & ~attributes))
								++failures;
						}
					}
				}
			}
		}

		if (failures > 0)
			ErrorHere << "Failed to remove atttributes from " << failures << " file entries." << std::endl;
		return failures == 0;
	}

	bool MakeWritable(CString pathname)
	{
		return RemoveAttributes(pathname, FILE_ATTRIBUTE_READONLY);
	}



	namespace {
		bool Call7za(CString zipparams)
		{
			CString path7za;
			path7za.Format("\"%s\\7za.exe\"", (LPCSTR)Paths::GetProgramFolder());

			//FUTURE - investigate 7za's return codes, for now, they're unknown, so don't rely on 0 as success
			// also, if this deadlocks, might have to switch to a timeout pattern instead, failing on timeouts.
			// Better yet, see if there's a real interface (COM or otherwise) to use the module.
			auto progdir = Paths::GetProgramFolder();
			//ShellExecute(NULL, nullptr, "7za", zipparams, progdir, SW_SHOW);

			// Let's wait for the zip invocation to finish (it doesn't hang).
			SHELLEXECUTEINFO inf = { 0 };
			inf.cbSize = sizeof(inf);
			inf.lpFile = "7za";
			inf.lpParameters = zipparams;
			inf.lpDirectory = progdir;
			inf.nShow = SW_SHOW;
			inf.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
			if (!ShellExecuteEx(&inf))
				return false;

			if (inf.hProcess) {
				WaitForSingleObject(inf.hProcess, 20000);
				CloseHandle(inf.hProcess);
			}
			return true;
		}
	}

	//REFACTOR - Unzip directory overwrite is under review. (//NOUNZIPOVERWRITE)
	//bool Unzip(CString archivepath, CString containerfolder, bool overwrite)
	//		Removed directory overwrites from the function for the moment after some private feedback.
	//		While I don't mind this operation personally, I got feedback that overwrites allowed easy
	//		accidental destruction of project folders.
	//		For now, I'm taking out the directory overwrite, but I'm not ready to remove this code from active source yet.
	//		The overwrite-releated code below has been commented out.

	bool Unzip(CString archivepath, CString containerfolder)
	{
		if (Paths::DirExists(containerfolder)) {
			if (!Paths::DirEmpty(containerfolder)) {
				return ErrorReturnFalse("Can't unzip into a non-empty directory.");
				//REFACTOR - clean this up with a better message after deciding the overwrite issue.
				//ErrorHere << "Can't unzip into a non-empty directory." << std::endl;
				//return false;
			}

			//NOUNZIPOVERWRITE - under review, overwrite param removed for now
			//if (!overwrite) {
			//	ErrorHere << "Can't unzip to the existing folder if the overwrite flag is set to false." << nl
			//			<< "    folder: " << (LPCSTR)containerfolder << std::endl;
			//	return false;
			//}
			//else {
			//	auto delpath = Paths::BuildTempFilePath("dir");
			//	Paths::DirMove(containerfolder, delpath);
			//	VERIFY(Paths::DirDelete(delpath)); // this still compiles in Release builds
			//}
		}

		CString zipcmd;
		zipcmd.Format(R"(x "%s" -y -o"%s")", (LPCSTR)archivepath, (LPCSTR)containerfolder);
		Call7za(zipcmd);

		Sleep(500); //HACK - allows 7za a little more time to work.
		return Paths::DirExists(containerfolder);
		//REFACTOR - for now, if overwrite == true, a return value of true is ambiguous.
		//		if the folder already existed, then the extraction could fail without throwing an exception,
		//		but the folder and its contents would still exist... how to cleanly detect if everything changed?
		//		should unzip simply fail if the foler exists already?
		//		or attempt to delete an existing folder and fail if it can't?
		//		For now, as stated above, folder overwrites are disabled.
	}

	// Zips the contents at the root of sourcefolder (not including the sourcefolder).
	bool Zip(CString archivepath, CString sourcefolder, bool overwrite)
	{
		if (Paths::FileExists(archivepath)) {
			if (!overwrite) {
				ErrorHere << "Can't zip to the existing file if the overwrite flag is set to false." << nl
						<< "    folder: " << (LPCSTR)archivepath << std::endl;
				return false;
			}
			else {
				VERIFY(Paths::FileDelete(archivepath) == true);
			}
		}

		CString zipcmd;
		zipcmd.Format(R"(a "%s" "%s\*" -tzip -r -y)", (LPCSTR)archivepath, (LPCSTR)sourcefolder);
		Call7za(zipcmd);

		return Paths::FileExists(archivepath);
	}

	CString GetZipListing(CString archivepath, bool technical)
	{
		auto tempfilepath = Paths::BuildTempFilePath();
		Paths::FileDelete(tempfilepath);
		Io::FilePathRemover remover(tempfilepath);

		CString flag = technical ? "-slt" : "";
		CString systemcmd;
		systemcmd.Format(R"(7za l %s "%s" > "%s")", (LPCSTR)flag, (LPCSTR)archivepath, (LPCSTR)tempfilepath);
		system(systemcmd);

		return Io::ReadTextFile(tempfilepath);
	}

	size_t ZipHasFileExtension(CString archivepath, std::vector<CString> & dotextensions)
	{
		auto contents = Io::GetZipListing(archivepath);
		for (size_t st = 0; st < dotextensions.size(); ++st) {
			CString ext(dotextensions[st]);
			ext.MakeLower();
			if (contents.Find(ext) != -1) return st;
		}

		return std::string::npos;
	}

	CString UnzipFile(CString archivepath, CString destfolder,  CString filetoextract)
	{
		// The file will be extracted to the temp folder with its origina filename
		CString systemcmd;
		systemcmd.Format("7za e \"%s\" -o\"%s\" \"%s\"", (LPCSTR)archivepath, (LPCSTR)destfolder, (LPCSTR)filetoextract);
		system(systemcmd);

		CString tempfile = Paths::Combine({ destfolder, filetoextract });
		return Paths::FileExists(tempfile) ? tempfile : "";
	}

	CString UnzipFileToTemp(CString archivepath, CString filetoextract)
	{
		return UnzipFile(archivepath, Paths::GetTempFolder(), filetoextract);
	}

	//N.B. - this works for text files, not binary
	CString UnzipFileToString(CString archivepath, CString filetoextract)
	{
		CString tempfile = Paths::BuildTempFilePath();
		CString systemcmd;
		systemcmd.Format("7za e \"%s\" -so \"%s\" > \"%s\"", (LPCSTR)archivepath, (LPCSTR)filetoextract, (LPCSTR)tempfile);
		system(systemcmd);

		auto contents = Paths::FileExists(tempfile) ? Io::ReadTextFile(tempfile) : "";
		Paths::FileDelete(tempfile);
		return contents;
	}

	CString ReadTextFile(CString filepath)
	{
		std::ifstream ifs;
		ifs.open((LPCSTR)filepath);
		if (!ifs.is_open()) return {};

		std::string contents;
		std::getline(ifs, contents, (char)EOF);
		return CString(contents.c_str());
	}

	DWORD ExecuteFile(CString workingdir, CString relfilename, CString cmdline, int timeout)
	{
		DWORD dwValue = (DWORD)-1;
		CString filepath = workingdir + "\\" + relfilename;;
		SHELLEXECUTEINFO sei = { 0 };
		sei.cbSize = sizeof(sei);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.lpFile = filepath;
		sei.lpDirectory = workingdir;
		sei.lpParameters = cmdline;
		sei.nShow = SW_SHOW;
		if (ShellExecuteEx(&sei)) {
			WaitForSingleObject(sei.hProcess, timeout);
			GetExitCodeProcess(sei.hProcess, &dwValue);
			CloseHandle(sei.hProcess);
		}
		return dwValue;
	}

	bool OpenStream(std::ifstream & ifs, std::string filename)
	{
		if (ifs.is_open()) ifs.close();
		ifs.open(filename);
		if (!ifs.is_open()) ErrorHere << "cannot open input file " << filename << std::endl;
		return ifs.is_open();
	}

	bool OpenStream(std::ofstream & ofs, int flags, std::string filename)
	{
		if (ofs.is_open()) ofs.close();
		ofs.open(filename, flags);
		if (!ofs.is_open()) ErrorHere << "cannot open output file " << filename << std::endl;
		return ofs.is_open();
	}

	bool CommitTempfile(std::string tempfile, std::string livefile)
	{
		auto * szsource = tempfile.c_str();
		auto * szdest = livefile.c_str();
		if (!Paths::FileExists(szsource)) return false;

		bool copied = Paths::FileCopy(szsource, szdest);

		if (Paths::FileExists(szsource))
			Paths::FileDelete(szsource);
		return copied;
	}

} // end namespace Io