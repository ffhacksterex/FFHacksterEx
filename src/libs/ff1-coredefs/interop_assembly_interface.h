#ifndef INTEROP_ASSEMBLY_INTERFACE_H
#define INTEROP_ASSEMBLY_INTERFACE_H

// Declarations used in both the DLL and modules

#include <memory>

typedef char* (*ASMFF1ALLOCFUNC)(size_t bufferlength);

#define ASMFF1_GETASMTYPEFUNC_NAME "asmff1_getasmtype"
#define ASMFF1_LOADFUNC_NAME "asmff1_load"
#define ASMFF1_SAVEFUNC_NAME "asmff1_save"
//#define ASMFF1_ISCOMPATIBLE_NAME "asmff1_iscompatible"
//#define ASMFF1_COERCEASMTYPE_NAME "asmff1_coerceasmtype"

// *** Return codes
#define ASMFF1_SUCCESS 0
#define ASMFF1_ERR_NODATA 1
#define ASMFF1_ERR_NOALLOC 2
#define ASMFF1_ERR_UNSUPPORTED 3
#define ASMFF1_ERR_BADOP 4
#define ASMFF1_ERR_NOOUTPUTBUFFER 5
#define ASMFF1_ERR_FAIL 6
#define ASMFF1_ERR_AMBIGUOUSASMTYPE 7
#define ASMFF1_ERR_WRONGASMTYPE 8
#define ASMFF1_ERR_ASMTYPEMISMATCH 9
#define ASMFF1_ERR_VERSIONMISMATCH 10
#define ASMFF1_ERR_ASMTYPEANDVERSIONMISMATCH 11
#define ASMFF1_ERR_MALFORMEDINPUT 12
#define ASMFF1_ERR_MALFORMEDOUTPUT 13
#define ASMFF1_ERR_CENTGETASMTYPE 14
#define ASMFF1_ERR_CANTGETDLLVERSION 15
#define ASMFF1_ERR_NOVERSIONTOCOERCE 16

struct asmff1_dlldata {
	ASMFF1ALLOCFUNC allocfunc;
	const char* varmap;
	const char* inputtext;
	const char* asmfile;
	const char* dialoguefile;
	char* returnbuffer;        // for ops that return data, the DLL uses allocfunc to allocate this
	size_t returnlength;       // set to the length of returnbuffer if it's allocated by the op
	const char* errormessage;  // allocated by allocfunc to hold a returned error message
};


// *** Interface

// Returns -1 if returnbuffer is NULL, 0 if the type is returned, and the required length if bufferlength is too short.
typedef int (*ASMFF1_GETASMTYPEFUNC)(char* returnbuffer, size_t bufferlength);

// These return the ASMFF1 return codes above.
typedef int (*ASMFF1_LOADFUNC)(const char* operation, asmff1_dlldata * asmdata, char* updatebuffer, size_t updatelength);
typedef int (*ASMFF1_SAVEFUNC)(const char* operation, asmff1_dlldata * asmdata, const char* sentdata, size_t sentlength);

//typedef int (*ASMFF1_ISCOMPATIBLEFUNC)(int pathtype, const char* path);

//typedef int (*ASMFF1_COERCEASMTYPEFUNC)(const char* projectpath);

#endif // ifndef INTEROP_ASSEMBLY_INTERFACE_H
