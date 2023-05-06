// EXE (caller-side) implementation
// These functions and objects are used in conjunction with the assembly DLL interface.
#pragma once

#include <exception>
#include "interop_assembly_interface.h"
#include "map_types.h"
#include "vector_types.h"
#include "std_assembly.h"
#include "pair_result.h"
class CFFHacksterProject;
class FFH2Project;

#define ASMDLL_COMPAT_DIR 0
#define ASMDLL_COMPAT_ZIP 1

namespace asmdll_impl {

	// Tests the DLL to see if the expected exported functions are present.
	bool is_ff1_asmdll(const char* asmdllpath);

	pair_result<CString> IsZipDllCompatible(CString zippath, CString asmdllpath);
	pair_result<CString> IsDirDllCompatible(CString dirpath, CString asmdllpath);

	pair_result<CString> IsProjectDllCompatible(FFH2Project& proj);
	pair_result<CString> IsProjectDllCompatible(CFFHacksterProject& proj);
	pair_result<CString> CoerceAsmToDllCompatibility(FFH2Project& proj);
	pair_result<CString> CoerceAsmToDllCompatibility(CFFHacksterProject& proj);

	pair_result<CString> GetAsmDllType(CString asmdllpath);
	pair_result<CString> GetAsmDllType(FFH2Project& proj);
	pair_result<CString> GetAsmDllType(CFFHacksterProject& proj);
	pair_result<CString> GetAsmDllVersion(CString asmdllpath);

	CString GetAsmDllReturnCodeText(int code);

	CString ReadAsmVersion(CString asmtypepath, CString defvalue);
	void WriteAsmVersion(CString asmtypepath, CString version);

	class AsmDllModule
	{
	public:
		AsmDllModule(HMODULE module, FFH2Project & proj, bool showerrors = false, CWnd* pwnd = nullptr);
		AsmDllModule(HMODULE module, CFFHacksterProject & proj, bool showerrors = false, CWnd * pwnd = nullptr);
		~AsmDllModule();

		bool GetRomDataFromAsm(const char* asmfilename, const stdstrintmap & varmap, const asmsourcemappingvector & entries, bytevector & rom);
		bool SetAsmFromRomData(const char* asmfilename, const stdstrintmap & varmap, const asmsourcemappingvector & entries, const bytevector & rom);

		bool ReadDialogue(const char* asmfilename, const stdstrintmap & varmap, std::string mappinglabel, stdstringvector & routinenames , talknamevectormap & routinemappings);
		bool WriteDialogue(const char* asmfilename, const stdstrintmap & varmap, std::string mappinglabel, const stdstringvector & routinenames, const talknamevectormap & routinemappings);

	private:
		void EmitResult(int retcode, CString operation, CString message = "");

		FFH2Project & m_prj2;
		CFFHacksterProject & m_proj;
		HWND m_hwnd;
		CString m_modulepath;
		ASMFF1_GETASMTYPEFUNC m_getasmtypefunc;
		ASMFF1_LOADFUNC m_loadfunc;
		ASMFF1_SAVEFUNC m_savefunc;
		bool m_showerrors;
	};

}


class asmdll_exception : public std::exception
{
public:
	asmdll_exception(const char* file, int line, const char* function, const char* asmdll, const char* message);
	virtual ~asmdll_exception();

	virtual const char* what() const;

protected:
	std::string m_message;
};
