#pragma once

// Preprocessor stringize
// e.g. Allows things like #pragma message("Building with compiler _MSVC_LANG version: " ppstr(_MSVC_LANG ))
//		which displays (for VC 2015) in the build log "Building with compiler _MSVC_LANG version: 1900"
#define ppstr2(s) #s
#define ppstr(s) ppstr2(s)

#define PRAGHERE __FILE__ "(" ppstr(__LINE__) ") - "

#include <iostream>

#define HERE __FILE__<< "(" << __LINE__ << "): " << __FUNCTION__ << " - "
#define LOGAT(file,line,func) file << "(" << line << "): " << func << " - "

#define LogMsg			std::cout
#define LogHere			std::cout << HERE
#define WarnMsg			std::cerr << "WARNING: "
#define WarnHere		std::cerr << HERE << "WARNING: "
#define ErrorMsg    	std::cerr << "ERROR! "
#define ErrorHere		std::cerr << HERE << "ERROR! "
#define FatalMsg    	std::cerr << "FATAL!!! "
#define FatalHere		std::cerr << HERE << "FATAL!!! "
constexpr auto nl = '\n';

// Use in a return statement or assignment, e.g.
//		auto result = LogReturn("Loading data...", LoadData(param1, param2));
//		return ErrorReturn("Unable to load some file". false);

#define LogReturn(msg,boolval) (LogHere << msg << std::endl, boolval)
#define WarnReturn(msg,boolval) (WarnHere << msg << std::endl, boolval)
#define ErrorReturn(msg,boolval) (ErrorHere << msg << std::endl, boolval)

#define WarnReturnFalse(msg) WarnReturn(msg,false)
#define ErrorReturnFalse(msg) ErrorReturn(msg,false)


//DEVNOTE - using the function implementation of THROWEXCEPTION produces C4715 'not all code paths return a value'
//          when used as the last statement in a function that returns a value.
//          I went with a curly-braced macro instead, but the function version is useful for breakpoints when testing,
//          so I commented out the definition instead of deleting it.
//template <typename T>
//void throw_exception(std::string file, int line, std::string function, std::string message)
//{
//	ErrorHere << file << "(" << std::to_string(line) << "): " << function << " - " << message << std::endl;
//	throw T(message);
//}
//#define THROWEXCEPTION(type,message) throw_exception<type>(__FILE__, __LINE__, __FUNCTION__, message)

#define THROWEXCEPTION(extype,message) { \
	ErrorHere << __FILE__ << "(" << __LINE__ << "): " << __FUNCTION__ << " - " << message << std::endl; \
	throw extype(message); }

// Essentially becomes 3 parameters: string, int , string.
#define EXCEPTIONPOINT __FILE__, __LINE__, __FUNCTION__

#define TRACEEXCEPTIONPOINT(ex,file,line,function) ErrorHere << "Caught " << typeid(ex).name() << " at\n" << file << "(" << line << "): " << function << " - "


//REFACTOR - va_list vs. Args...
//template <typename ...Args>
//CString build_execption_msg(CString file, int line, CString func, CString fmtmsg, Args... args) {
//	CString msg = "%s(%d): %s - " + fmtmsg;
//	CString cs;
//	//REFACTOR
//	//cs.Format("%s(%d): %s - " + fmtmsg, (LPCSTR)file, line, (LPCSTR)func, args...); // args alw
//	cs.Format((LPCSTR)msg, (LPCSTR)file, line, (LPCSTR)func, args...); // args alw
//	return cs;
//}

inline CString build_execption_msg(CString fmtmsg, ...) {
	va_list args;
	va_start(args, fmtmsg);
	CString cs;
	cs.FormatV(fmtmsg, args);
	va_end(args);
	return cs;
}

//#define RAISEEXCEPTION(extype,fmtmsg,...) \
//	(ErrorHere << (LPCSTR)(build_execption_msg(__FILE__, __LINE__, __FUNCTION__, fmtmsg, __VA_ARGS__)) << std::endl), throw extype((build_execption_msg(__FILE__, __LINE__, __FUNCTION__, fmtmsg, __VA_ARGS__)))


//REFACTOR - replace this macro with a variadic template function
//#define RAISEEXCEPTION(extype,fmtmsg,...) { \
//	auto msg = build_execption_msg(__FILE__, __LINE__, __FUNCTION__, fmtmsg, __VA_ARGS__); \
//	throw extype(msg); }


//REFACTOR - might not need build_execption_msg at all now
//auto msg = build_execption_msg(fmt, __VA_ARGS__); \

#define RAISEEXCEPTION(extype,fmtmsg,...) { \
	auto msg = build_execption_msg(fmtmsg, __VA_ARGS__); \
	ErrorHere << (LPCSTR)msg << std::endl; \
	throw extype(msg); }

//#define RAISEEXCEPTION(extype,fmtmsg,...) { \
//	CString fmt; fmt.Format("%s(%d): %s - %s", __FILE__, __LINE__, __FUNCTION__, (LPCSTR)fmtmsg); \
//	auto msg = build_execption_msg(fmt, __VA_ARGS__); \
//	throw extype(msg); }


// Operations reported in an exception, used in exception classes to semantically aid formatting
// (instead of using a boolean bReading param set to true/false).
enum class exceptop { initializing, reading, writing };

inline std::string get_exceptop_text(exceptop op)
{
	switch (op) {
	case exceptop::initializing: return "initializing";
	case exceptop::reading: return "reading";
	case exceptop::writing: return "writing";
	}
	return "";
}
