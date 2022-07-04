#pragma once

#include <iosfwd>
#include <vector>

namespace Io {
	
	bool open_file(CString filename, std::ifstream & ifs, bool asbinary = false);
	bool open_file(CString filename, std::ofstream & ofs, bool asbinary = false);
	bool create_file(CString filename, std::ofstream & ofs, bool asbinary = false);
	std::ostream & create_asmstream(std::ofstream & ofs, std::string asmfile, int flags = std::ios::trunc);
	bool move_to_marker(std::istream & is, std::string marker, bool gotonextline = true); // marker must include the ending colon

	void clear_eof(std::istream & is);

	size_t discard_line(std::istream & is);
	std::string preview_next_line(std::istream & is);
	std::ostream & write_next_line(std::ostream & os, std::string line);
	std::ostream & write_current_line(std::ostream& os, std::string line);

	size_t copy_next_line(std::istream & is, std::ostream & os);
	void copy_to_end(std::istream & is, std::ostream & os);

	void load_binary(CString filepath, std::vector<unsigned char> & vec, bool allowzerolength = false);
	void save_binary(CString filepath, const std::vector<unsigned char> & vec, bool allowzerolength = false);

	HMODULE LoadModule(CString modulepath);
	HMODULE LoadCurrentModule();
	CString GetCurrentModulePath();
	CString GetModulePath(HMODULE module);

	bool RemoveAttributes(CString pathname, int attributes);
	bool MakeWritable(CString pathname);

	bool Unzip(CString archivepath, CString containerfolder);
	bool Zip(CString archivepath, CString sourcefolder, bool overwrite = false);
	CString GetZipListing(CString archivepath, bool technical = true);
	size_t ZipHasFileExtension(CString archivepath, std::vector<CString> & dotextensions);

	CString UnzipFile(CString archivepath, CString destfolder, CString filetoextract);
	CString UnzipFileToTemp(CString archivepath, CString filetoextract);
	CString UnzipFileToString(CString archivepath, CString filetoextract);

	CString ReadTextFile(CString filepath);

	DWORD ExecuteFile(CString workingdir, CString relfilename, CString cmdline, int timeout);

	bool OpenStream(std::ifstream & ifs, std::string filename);
	bool OpenStream(std::ofstream & ofs, int flags, std::string filename);

	bool CommitTempfile(std::string tempfile, std::string livefile);

} // end namespace Io