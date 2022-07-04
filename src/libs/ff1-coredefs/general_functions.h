#pragma once

#include <basetsd.h>
#include <afx.h>
#include <set>
#include <vector>
#include <functional>
#include "common_symbols.h"
#include "pair_result.h"
class CComboBox;


#define IF_NOHANDLE(ctl) if (!(HANDLE)(ctl)) (ctl)

//REFACTOR - replace TraceFail stuff with Log.Warn/ErrorReturn macros in logging_functions.h
#define TraceFail(message) TraceFailMsg(message, __FILE__, __LINE__, __FUNCTION__)
#define TraceFailCode(code,message) TraceFailCodeMsg(code, message, __FILE__, __LINE__, __FUNCTION__)

bool TraceFailMsg(LPCSTR message, CString file, int line, CString func);
bool TraceFailMsg(std::string message, CString file, int line, CString func);
int TraceFailCodeMsg(int returncode, LPCSTR message, CString file, int line, CString func);
int TraceFailCodeMsg(int returncode, std::string message, CString file, int line, CString func);

int StringToInt_HEX(CString);
int StringToInt(CString);

bool HasAnyFlag(int value, int mask);
bool HasAllFlags(int value, int mask);
