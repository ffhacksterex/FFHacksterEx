#ifndef ASM_FF1DISCH_H
#define ASM_FF1DISCH_H

#ifdef ASMFF1DISCH_EXPORTS
#define ASMFF1DISCH_API __declspec(dllexport)
#else
#define ASMFF1DISCH_API __declspec(dllimport)
#endif

#include <interop_assembly_interface.h>

// DLL (library-side) declarations

extern "C" {

	ASMFF1DISCH_API int asmff1_getasmtype(char* returnbuffer, size_t bufferlength);
	ASMFF1DISCH_API int asmff1_load(const char* operation, asmff1_dlldata * asmdata, char* updatebuffer, size_t updatelength);
	ASMFF1DISCH_API int asmff1_save(const char* operation, asmff1_dlldata * asmdata, const char* sentdata, size_t sentlength);

}

#endif // ifndef ASM_FF1DISCH_H
