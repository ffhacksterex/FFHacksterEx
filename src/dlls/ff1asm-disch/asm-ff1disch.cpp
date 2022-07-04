// asm-ff1disch.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "asm-ff1disch.h"
#include <asmdll_impl.h>
#include "data_functions.h"
#include "dialogue_functions.h"
#include <ini_functions.h>
#include <io_functions.h>
#include <path_functions.h>
#include <string_functions.h>
#include <vector_types.h>
#include <window_messages.h>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object
CWinApp theApp;

using namespace Io;
using namespace disch_functions;
using namespace disch_functions::dialogue;
using namespace std_assembly;
using namespace std_assembly::shared;
using namespace Strings;


namespace {

	const CString MY_ASMTYPE = "disch-annotated";
	const CString MY_ASMTYPE_FILENAME = MY_ASMTYPE + ".asmtype";

	char* format_message(ASMFF1ALLOCFUNC allocfunc, std::string msg)
	{
		auto length = msg.length();
		char * buf = allocfunc(msg.length() + 1);
		memset(buf, 0, sizeof(buf[0]));
		strncpy(buf, msg.c_str(), length);
		return buf;
	}

	int do_return_code(asmff1_dlldata * data, int code, const char* funcname, std::string message)
	{
		if (!message.empty()) {
			std::string fn(funcname);
			data->errormessage = format_message(data->allocfunc, "In " + fn + ", " + message);
		}
		return code;
	}

	int return_data(asmff1_dlldata * data, std::string returndata)
	{
		data->returnbuffer = format_message(data->allocfunc, returndata);
		data->returnlength = returndata.length();
		return ASMFF1_SUCCESS;
	}

} // end namespace unnamed



// ################################################################
// EXPORTED FUNCTION IMPLEMENTATIONS

extern "C" {

	using namespace disch_functions;

	#define return_code(data,code,msg) do_return_code(data, code, __FUNCTION__, msg)

	ASMFF1DISCH_API int asmff1_getasmtype(char* returnbuffer, size_t bufferlength)
	{
		static const std::string asmtype = (LPCSTR)MY_ASMTYPE; //"disch-annotated";

		if (returnbuffer == nullptr)
			return -1;

		if (bufferlength <= asmtype.length())
			return (int)asmtype.length() + 1;

		// If it got here, then the buffer is sized to include the null terminator.
		strncpy(returnbuffer, asmtype.c_str(), asmtype.length());
		returnbuffer[asmtype.length()] = 0;
		return 0;
	}

	ASMFF1DISCH_API int asmff1_load(const char * operation, asmff1_dlldata * asmdata, char * updatebuffer, size_t updatelength)
	{
		if (asmdata == nullptr) return ASMFF1_ERR_NODATA;
		if (asmdata->allocfunc == nullptr) return ASMFF1_ERR_NOALLOC;
		std::string op = operation;
		try {
			if (op == "asmtorom") {
				// The return value to this function is not used: errors are thrown as exceptions.
				return disch_functions::data::read_assembly_data(asmdata->asmfile, asmdata->varmap, asmdata->inputtext, reinterpret_cast<unsigned char*>(updatebuffer), updatelength);
			}
			else if (op == "readdialogue") {
				auto returndata = disch_functions::dialogue::read_dialogue_data(asmdata->asmfile, asmdata->dialoguefile, asmdata->inputtext, asmdata->varmap);
				return return_data(asmdata, returndata);
			}
			return return_code(asmdata, ASMFF1_ERR_BADOP, "can't process unsupported operation '" + op + "'.");
		}
		catch (std::exception & ex) {
			return return_code(asmdata, ASMFF1_ERR_FAIL, "an exception occured while attempting to load assembly data, " + std::string(ex.what()) + ".");
		}
		catch (...) {
			return return_code(asmdata, ASMFF1_ERR_FAIL, "an unexpected exception occured while attempting to load assembly data.");
		}
		return return_code(asmdata, ASMFF1_ERR_UNSUPPORTED, "the operation " + op + " is either unsupported or not implemented.");
	}

	ASMFF1DISCH_API int asmff1_save(const char * operation, asmff1_dlldata * asmdata, const char * sentdata, size_t sentlength)
	{
		if (asmdata == nullptr) return ASMFF1_ERR_NODATA;
		if (asmdata->allocfunc == nullptr) return ASMFF1_ERR_NOALLOC;
		std::string op = operation;
		try {
			if (op == "romtoasm") {
				return disch_functions::data::write_assembly_data(asmdata->asmfile, asmdata->varmap, asmdata->inputtext, reinterpret_cast<const unsigned char*>(sentdata), sentlength);
			}
			else if (op == "writedialogue") {
				disch_functions::dialogue::write_dialogue_data(asmdata->asmfile, asmdata->dialoguefile, asmdata->varmap, asmdata->inputtext);
				return ASMFF1_SUCCESS;
			}
			return return_code(asmdata, ASMFF1_ERR_BADOP, "can't process unsupported operation '" + op + "'.");
		}
		catch (std::exception & ex) {
			return return_code(asmdata, ASMFF1_ERR_FAIL, "an exception occured while attempting to load assembly data, " + std::string(ex.what()) + ".");
		}
		catch (...) {
			return return_code(asmdata, ASMFF1_ERR_FAIL, "an unexpected exception occured while attempting to load assembly data.");
		}
		return return_code(asmdata, ASMFF1_ERR_UNSUPPORTED, "the operation " + op + " is either unsupported or not implemented.");
	}

} // end extern "C"