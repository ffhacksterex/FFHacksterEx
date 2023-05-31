//REFACTOR
#include "stdafx.h"
#include "asmdll_impl.h"
#include "general_functions.h"
#include "ini_functions.h"
#include "io_functions.h"
#include "logging_functions.h"
#include "path_functions.h"
#include "string_functions.h"
#include "FFHacksterProject.h"
#include "FFH2Project.h"
#include "LoadLibraryHandleScope.h"
#include "FFHacksterProject.h"
#include "FilePathRemover.h"
#include <filesystem>
#include <memory>

using ffh::str::tomfc;
using ffh::str::tostd;

using namespace Io;
//STDFILESYS - see path_functions.cpp for more info
#if _MSVC_LANG > 201402L
namespace fsys = std::filesystem;
#else
namespace fsys = std::experimental::filesystem;
#endif

namespace {
	CFFHacksterProject ffdummy;
	FFH2Project ff2dummy;
}

namespace asmdll_impl {

	namespace {

		//REFACTOR - remove in next versio pass if not needed
		//const std::regex rx_asmtype_ver{ "ver=(\\d+).(\\d+).(\\d+).(\\d+)" };

		char* allocate_buffer(size_t length)
		{
			ASSERT(length > 0);
			if (length == 0) THROWEXCEPTION(std::runtime_error, "can't alocate a zero-byte buffer");

			auto buffer = new char[length];
			memset(buffer, 0, length);
			return buffer;
		}

		mfcstringvector GetDirAsmTypes(CString asmdirpath)
		{
			auto files = Paths::GetFiles(asmdirpath, "*.asmtype", false);
			std::transform(begin(files), end(files), begin(files), [](auto & file) { return Paths::GetFileStem(file);  });
			return files;
		}

		pair_result<CString> VerifyAssemblyTypeMatch(mfcstringvector srctypes, CString dlltype, CString source)
		{
			// If an assembly type isn't defined in either the DLL or Source, then it's considered a match.
			// if both empty, OK
			// if src.count == 1 AND src[0] == dll, OK
			// else, PROBLEMS

			if (srctypes.empty() && dlltype.IsEmpty())
				return { true, "" };
			if (srctypes.size() == 1 && srctypes[0] == dlltype)
				return { true, "" };

			if (srctypes.empty())
				return { false, "No '" + source + "' source assembly type defined to match DLL type '" + dlltype + "'.\nBoth a source and DLL assembly type must be defined or both must be empty" };

			if (srctypes.size() > 1) {
				auto srcstring = Strings::join(srctypes, ",");
				CString msg;
				msg.Format("One '" + source + "' source assembly type was expected, can't handle multiple types (%s)", (LPCSTR)srcstring);
				return { false, msg };
			}

			if (dlltype.IsEmpty()) {
				auto srcstring = Strings::join(srctypes, ",");
				CString msg;
				msg.Format("No DLL assembly type defined to match source '" + source + "' type '%s'.\nBoth a source and DLL assembly type must be defined or both must be empty", (LPCSTR)srcstring);
				return { false, msg };
			}
			if (srctypes.size() == 1) {
				CString msg;
				msg.Format("Source '" + source + "' assembly type '%s' doesn't match DLL type '%s'", (LPCSTR)srctypes[0], (LPCSTR)dlltype);
				return { false, msg };
			}

			auto srcstring = Strings::join(srctypes, ",");
			CString msg;
			msg.Format("DLL assembly type '%s' is incompatible with source '" + source + "' assembly types '%s'", (LPCSTR)dlltype, (LPCSTR)srcstring);
			return { false, msg };
		}
	
		pair_result<CString> local_coerceasmtype(CString projectpath, CString asmdllpath)
		{
			// No need to search for files, we can build the filename we want.
			auto dir = Paths::Combine({ Paths::GetDirectoryPath(projectpath), "asm" });
			auto dlltype = GetAsmDllType(asmdllpath);
			auto dllver = GetAsmDllVersion(asmdllpath);
			if (!dlltype) return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_CENTGETASMTYPE) };
			if (!dllver) return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_CANTGETDLLVERSION) };
			if (dllver.value.IsEmpty()) return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_NOVERSIONTOCOERCE) };

			auto MY_ASMTYPE = dlltype.value;
			auto MY_ASMTYPE_FILENAME = MY_ASMTYPE + ".asmtype";
			auto vettedpath = Paths::Combine({ dir, MY_ASMTYPE_FILENAME });
			CString vertag = ReadAsmVersion(vettedpath, "0");
			auto backuppath = Paths::Combine({ dir, MY_ASMTYPE + ".v" + vertag + ".asmtype" });
			if (Paths::FileExists(vettedpath)) {
				// found it, rename it to a backup copy based on version (which could be empty)
				VERIFY(Paths::FileRename(vettedpath, backuppath) == true);
			}
			else if (!Paths::FileExists(backuppath)) {
				// doesn't exist, create a dummy backup with v0
				asmdll_impl::WriteAsmVersion(backuppath, "");
			}

			// Write/Truncate the new version to the file MY_ASMTYPE_FILENAME			
			asmdll_impl::WriteAsmVersion(vettedpath, dllver.value);
			return { true, "Succeeded." };
		}

		pair_result<CString> is_dll_type_compatible(CString asmtypefile, CString dlltype, CString dllversion)
		{
			CString sourcetype = Paths::FileExists(asmtypefile) ? Paths::GetFileStem(asmtypefile) : "";
			CString sourceversion = asmdll_impl::ReadAsmVersion(asmtypefile, "");

			auto typeok = (sourcetype == dlltype);
			auto verok = (sourceversion == dllversion);
			if (typeok && verok) return { true, "" };

			if (!typeok && !verok) return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_ASMTYPEANDVERSIONMISMATCH) };
			if (!typeok) return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_ASMTYPEMISMATCH) };
			return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_VERSIONMISMATCH) };
		}

		pair_result<CString> local_iscompatible(int pathtype, CString sourcepath, CString asmdllpath)
		{
			auto dlltype = GetAsmDllType(asmdllpath);
			auto dllver = GetAsmDllVersion(asmdllpath);
			if (!dlltype || dlltype.value.IsEmpty()) return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_CENTGETASMTYPE) };
			if (!dllver) return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_CANTGETDLLVERSION) };
			if (dllver.value.IsEmpty()) return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_NOVERSIONTOCOERCE) };

			auto MY_ASMTYPE = dlltype.value;
			auto MY_ASMTYPE_FILENAME = MY_ASMTYPE + ".asmtype";

			switch (pathtype) {
			case ASMDLL_COMPAT_DIR:
			{
				// open the file in the assembly source folder
				CString asmtypefile = Paths::Combine({ sourcepath, MY_ASMTYPE_FILENAME });
				return is_dll_type_compatible(asmtypefile, dlltype, dllver);
			}
			break;
			case ASMDLL_COMPAT_ZIP:
			{
				// get the file from the assembly zip archive
				auto asmtypefile = Io::UnzipFileToTemp(sourcepath, MY_ASMTYPE_FILENAME);
				Io::FilePathRemover fpremove(asmtypefile);
				auto result = is_dll_type_compatible(asmtypefile, dlltype, dllver);
				Paths::FileDelete(asmtypefile);
				return result;
			}
			break;
			}
			return { false, GetAsmDllReturnCodeText(ASMFF1_ERR_BADOP) };
		}

	} // end namespace unnamed



	bool is_ff1_asmdll(const char* asmdllpath)
	{
		LoadLibraryHandleScope mod(asmdllpath);
		if (mod == NULL) return false;

		auto getasmtypefunc = (ASMFF1_GETASMTYPEFUNC)GetProcAddress(mod, ASMFF1_GETASMTYPEFUNC_NAME);
		auto loadfunc = (ASMFF1_LOADFUNC)GetProcAddress(mod, ASMFF1_LOADFUNC_NAME);
		auto savefunc = (ASMFF1_SAVEFUNC)GetProcAddress(mod, ASMFF1_SAVEFUNC_NAME);
		return getasmtypefunc != nullptr && loadfunc != nullptr && savefunc != nullptr;
	}

	pair_result<CString> IsZipDllCompatible(CString zippath, CString asmdllpath)
	{
		return local_iscompatible(ASMDLL_COMPAT_ZIP, zippath, asmdllpath);
	}

	pair_result<CString> IsDirDllCompatible(CString dirpath, CString asmdllpath)
	{
		return local_iscompatible(ASMDLL_COMPAT_DIR, dirpath, asmdllpath);
	}

	pair_result<CString> CoerceAsmToDllCompatibility(CString projectpath, CString asmdllpath)
	{
		return local_coerceasmtype(projectpath, asmdllpath);
	}

	pair_result<CString> IsProjectDllCompatible(FFH2Project& proj)
	{
		if (!proj.IsAsm()) return { false, "This is not an Assembly project." };

		auto dir = Paths::Combine({ tomfc(proj.ProjectFolder), "asm" });
		return IsDirDllCompatible(dir, tomfc(proj.info.asmdll));
	}

	pair_result<CString> IsProjectDllCompatible(CFFHacksterProject& proj)
	{
		if (!proj.IsAsm()) return { false, "This is not an Assembly project." };

		auto dir = Paths::Combine({ proj.ProjectFolder, "asm" });
		return IsDirDllCompatible(dir, proj.AsmDLLPath);
	}

	pair_result<CString> CoerceAsmToDllCompatibility(FFH2Project& proj)
	{
		return CoerceAsmToDllCompatibility(tomfc(proj.ProjectPath), tomfc(proj.info.asmdll));
	}

	pair_result<CString> CoerceAsmToDllCompatibility(CFFHacksterProject& proj)
	{
		return CoerceAsmToDllCompatibility(proj.ProjectPath, proj.AsmDLLPath);
	}

	pair_result<CString> GetAsmDllType(CString asmdllpath)
	{
		LoadLibraryHandleScope scope(asmdllpath);
		auto func = (ASMFF1_GETASMTYPEFUNC)GetProcAddress(scope, ASMFF1_GETASMTYPEFUNC_NAME);
		if (func == nullptr)
			return { false, "Can't retrieve DLL's asmtype - can't find the entrypoint for extension DLL '" + asmdllpath + "'" };

		//TODO - report on the actual return code of func (0 is success)
		TCHAR asmtype[64] = { 0 };
		if (func(asmtype, 64) != 0)
			return { false, "Unable to retrieve asmtype to DLL return buffer." };

		return { true, asmtype };
	}

	pair_result<CString> GetAsmDllType(FFH2Project& proj)
	{
		if (!proj.IsAsm()) return { false, "Can't get asmtype from a non-assembly project. " };

		return GetAsmDllType(tomfc(proj.info.asmdll));
	}

	pair_result<CString> GetAsmDllType(CFFHacksterProject& proj)
	{
		if (!proj.IsAsm()) return { false, "Can't get asmtype from a non-assembly project. " };

		return GetAsmDllType(proj.AsmDLLPath);
	}

	CString GetAsmDllReturnCodeText(int code)
	{
		switch (code) {
			case ASMFF1_SUCCESS: return "Succeeded.";
			case ASMFF1_ERR_NODATA: return "No data available.";
			case ASMFF1_ERR_NOALLOC: return "Can't return a response without an allocator.";
			case ASMFF1_ERR_UNSUPPORTED: return "Unsuported operation.";
			case ASMFF1_ERR_BADOP: return "Invalid operation.";
			case ASMFF1_ERR_NOOUTPUTBUFFER: return "Output empty.";
			case ASMFF1_ERR_FAIL: return "Generic/unspecified failure.";
			case ASMFF1_ERR_AMBIGUOUSASMTYPE: return "Ambiguous asmtypes in the assembly source (should be only one).";
			case ASMFF1_ERR_WRONGASMTYPE: return "Assembly source asmtype doesn't match the DLL's asmtype.";
			case ASMFF1_ERR_ASMTYPEMISMATCH: return "Asmtypes of the asssembly source and DLL don't match.";
			case ASMFF1_ERR_VERSIONMISMATCH: return "Versions of the assembly source and DLL don't match.";
			case ASMFF1_ERR_ASMTYPEANDVERSIONMISMATCH: return "Neither the asmtypes nor versions of the assembly source and DLL match.";
			case ASMFF1_ERR_MALFORMEDINPUT: return "Successful call, but the input didn't match the format the DLL function expected.";
			case ASMFF1_ERR_MALFORMEDOUTPUT: return "Successful call, but the called DLL function returned malformed data.";
			case ASMFF1_ERR_CENTGETASMTYPE: return "";
			case ASMFF1_ERR_CANTGETDLLVERSION: return "";
			case ASMFF1_ERR_NOVERSIONTOCOERCE: return "";
		}
		CString err;
		err.Format("Unknown error code %d.", err);
		return err;
	}

	pair_result<CString> GetAsmDllVersion(CString asmdllpath)
	{
		auto size = GetFileVersionInfoSize(asmdllpath, nullptr);
		if (size > 0) {
			auto ubuf = std::unique_ptr<unsigned char[]>(new unsigned char[size]);
			auto buf = ubuf.get();
			if (GetFileVersionInfo(asmdllpath, NULL, size, buf)) {
				LPVOID data = nullptr;
				UINT datalen = 0;
				if (VerQueryValue(buf, "\\", &data, &datalen)) {
					if (datalen > 0) {
						auto pinfo = (VS_FIXEDFILEINFO*)data;
						CString ver;
						ver.Format("%d.%d.%d.%d", HIWORD(pinfo->dwFileVersionMS), LOWORD(pinfo->dwFileVersionMS),
							HIWORD(pinfo->dwFileVersionLS), LOWORD(pinfo->dwFileVersionLS));
						return { true, ver };
					}
				}
			}
		}
		CString err;
		err.Format("Failed to retrieve version info from file '%s'.", asmdllpath);
		return { false, err };
	}

	CString ReadAsmVersion(CString asmtypepath, CString defvalue)
	{
		return Ini::ReadIni(asmtypepath, "ASMTYPE", "ver", defvalue);
	}

	void WriteAsmVersion(CString asmtypepath, CString version)
	{
		Ini::WriteIni(asmtypepath, "ASMTYPE", "ver", version);
	}



	// ################################################################

	AsmDllModule::AsmDllModule(HMODULE module, FFH2Project& proj, bool showerrors, CWnd* pwnd)
		: m_hwnd(NULL)
		, m_showerrors(showerrors)
		, m_proj(ffdummy)
		, m_prj2(proj)
	{
		if (pwnd == nullptr) pwnd = AfxGetMainWnd();
		if (pwnd != nullptr) m_hwnd = pwnd->GetSafeHwnd();

		m_modulepath = GetModulePath(module);

		m_getasmtypefunc = (ASMFF1_GETASMTYPEFUNC)GetProcAddress(module, ASMFF1_GETASMTYPEFUNC_NAME);
		m_loadfunc = (ASMFF1_LOADFUNC)GetProcAddress(module, ASMFF1_LOADFUNC_NAME);
		m_savefunc = (ASMFF1_SAVEFUNC)GetProcAddress(module, ASMFF1_SAVEFUNC_NAME);

		CString msg;
		if (m_getasmtypefunc == nullptr) msg.AppendFormat(" " ASMFF1_GETASMTYPEFUNC_NAME);
		if (m_loadfunc == nullptr) msg.AppendFormat(" " ASMFF1_LOADFUNC_NAME);
		if (m_savefunc == nullptr) msg.AppendFormat(" " ASMFF1_SAVEFUNC_NAME);
		if (!msg.IsEmpty()) {
			msg.Insert(0, "asmdll module is missing the following functions:");
			throw asmdll_exception(EXCEPTIONPOINT, m_modulepath, msg);
		}
	}

	AsmDllModule::AsmDllModule(HMODULE module, CFFHacksterProject & proj, bool showerrors, CWnd * pwnd)
		: m_hwnd(NULL)
		, m_showerrors(showerrors)
		, m_proj(proj)
		, m_prj2(ff2dummy)
	{
		if (pwnd == nullptr) pwnd = AfxGetMainWnd();
		if (pwnd != nullptr) m_hwnd = pwnd->GetSafeHwnd();

		m_modulepath = GetModulePath(module);

		m_getasmtypefunc = (ASMFF1_GETASMTYPEFUNC)GetProcAddress(module, ASMFF1_GETASMTYPEFUNC_NAME);
		m_loadfunc = (ASMFF1_LOADFUNC)GetProcAddress(module, ASMFF1_LOADFUNC_NAME);
		m_savefunc = (ASMFF1_SAVEFUNC)GetProcAddress(module, ASMFF1_SAVEFUNC_NAME);

		CString msg;
		if (m_getasmtypefunc == nullptr) msg.AppendFormat(" " ASMFF1_GETASMTYPEFUNC_NAME);
		if (m_loadfunc == nullptr) msg.AppendFormat(" " ASMFF1_LOADFUNC_NAME);
		if (m_savefunc == nullptr) msg.AppendFormat(" " ASMFF1_SAVEFUNC_NAME);
		if (!msg.IsEmpty()) {
			msg.Insert(0, "asmdll module is missing the following functions:");
			throw asmdll_exception(EXCEPTIONPOINT, m_modulepath, msg);
		}
	}

	AsmDllModule::~AsmDllModule()
	{
	}

	bool AsmDllModule::GetRomDataFromAsm(const char * asmfilename, const stdstrintmap & varmap, const asmsourcemappingvector & entries, bytevector & rom)
	{
		std::ostringstream ostr;
		std::string strvarmap = ToString(ostr, varmap);
		std::string strsrcmap = ToString(ostr, entries);

		asmff1_dlldata data = { 0 };
		data.allocfunc = allocate_buffer;
		data.asmfile = asmfilename;
		data.dialoguefile = m_proj.DialoguePath;
		data.varmap = strvarmap.c_str();
		data.inputtext = strsrcmap.c_str();

		char * operation = "asmtorom";
		char* rombuffer = reinterpret_cast<char*>(address(rom));
		auto retcode = m_loadfunc(operation, &data, rombuffer, rom.size());
		auto returndata = std::unique_ptr<char[]>(data.returnbuffer);
		auto errmessage = std::unique_ptr<const char[]>(data.errormessage);

		LogHere;
		EmitResult(retcode, operation, errmessage.get());
		return retcode == ASMFF1_SUCCESS;
	}

	bool AsmDllModule::SetAsmFromRomData(const char * asmfilename, const stdstrintmap & varmap, const asmsourcemappingvector & entries, const bytevector & rom)
	{
		std::string strvarmap = ToString(varmap);
		std::string strsrcmap = ToString(entries);

		asmff1_dlldata data = { 0 };
		data.allocfunc = allocate_buffer;
		data.asmfile = asmfilename;
		data.dialoguefile = m_proj.DialoguePath;
		data.varmap = strvarmap.c_str();
		data.inputtext = strsrcmap.c_str();

		char * operation = "romtoasm";
		const char* rombuffer = reinterpret_cast<const char*>(address(rom));
		auto retcode = m_savefunc(operation, &data, rombuffer, rom.size());
		auto returndata = std::unique_ptr<char[]>(data.returnbuffer);
		auto errmessage = std::unique_ptr<const char[]>(data.errormessage);

		LogHere;
		EmitResult(retcode, operation, errmessage.get());
		return retcode == ASMFF1_SUCCESS;
	}

	bool AsmDllModule::ReadDialogue(const char* asmfilename, const stdstrintmap & varmap, std::string mappinglabel, stdstringvector & routinenames, talknamevectormap & routinemappings)
	{
		std::string strvarmap = ToString(varmap);

		asmff1_dlldata data = { 0 };
		data.allocfunc = allocate_buffer;
		data.asmfile = asmfilename;
		data.dialoguefile = m_proj.DialoguePath;
		data.varmap = strvarmap.c_str();
		data.inputtext = mappinglabel.c_str();

		char * operation = "readdialogue";
		auto retcode = m_loadfunc(operation, &data, nullptr, 0);
		auto returndata = std::unique_ptr<char[]>(data.returnbuffer);
		auto errmessage = std::unique_ptr<const char[]>(data.errormessage);

		if (retcode == ASMFF1_SUCCESS && returndata == nullptr)
			retcode = ASMFF1_ERR_NOOUTPUTBUFFER;

		if (retcode == ASMFF1_SUCCESS) {
			std::istringstream istr(returndata.get()); // this block throws if the text can't be converted
			istr >> routinenames;
			if (istr.get() != '\n') {
				retcode = ASMFF1_ERR_MALFORMEDINPUT;
			}
			else {
				istr >> routinemappings;
			}
		}

		EmitResult(retcode, operation, errmessage.get());
		return retcode == ASMFF1_SUCCESS;
	}

	bool AsmDllModule::WriteDialogue(const char* asmfilename, const stdstrintmap & varmap, std::string mappinglabel, const stdstringvector & routinenames, const talknamevectormap & routinemappings)
	{
		std::string strvarmap = ToString(varmap);
		std::ostringstream ostr;
		ostr << mappinglabel << std::endl;
		ostr << routinenames << std::endl;
		ostr << routinemappings;
		std::string inputtext = ostr.str();

		asmff1_dlldata data = { 0 };
		data.allocfunc = allocate_buffer;
		data.asmfile = asmfilename;
		data.dialoguefile = m_proj.DialoguePath;
		data.varmap = strvarmap.c_str();
		data.inputtext = inputtext.c_str();

		char * operation = "writedialogue";
		auto retcode = m_savefunc(operation, &data, nullptr, 0);
		auto returndata = std::unique_ptr<char[]>(data.returnbuffer);
		auto errmessage = std::unique_ptr<const char[]>(data.errormessage);

		LogHere;
		EmitResult(retcode, operation, errmessage.get());
		return retcode == ASMFF1_SUCCESS;	
	}

	void AsmDllModule::EmitResult(int retcode, CString operation, CString message)
	{
		if (retcode != ASMFF1_SUCCESS) {
			CString msg;
			msg.Format("Operation '%s' failed with return code %d: %s", operation, retcode, asmdll_impl::GetAsmDllReturnCodeText(retcode));
			if (!message.IsEmpty()) msg.AppendFormat(" %s", message);
			ErrorMsg << (LPCSTR)msg << std::endl;
			//FUTURE - stdout/stderr handles aren't being inherited from the EXE, for now call OutputDebugString manually
			OutputDebugString(msg); OutputDebugString("\n");
			if (m_showerrors) AfxMessageBox(msg);
		}
	}
}




// ################################################################

asmdll_exception::asmdll_exception(const char * file, int line, const char * function, const char * asmdll, const char * message)
{
	TRACEEXCEPTIONPOINT(*this, file, line, function);

	m_message = "ASMDLL '" + std::string(asmdll) + "' threw an exception, " + std::string(message);

	LogMsg << m_message << std::endl;
}

asmdll_exception::~asmdll_exception()
{
}

const char * asmdll_exception::what() const
{
	return m_message.c_str();;
}