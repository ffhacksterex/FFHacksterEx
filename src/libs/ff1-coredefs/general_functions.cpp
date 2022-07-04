#include "stdafx.h"
#include "general_functions.h"
#include "collection_helpers.h"
#include "path_functions.h"
#include "string_functions.h"
#include "window_messages.h"
#include <iostream>
#include <fstream>
#include <Shlobj.h>

using namespace Strings;

// Declared extern in window_messages.h
UINT RWM_FFHACKSTEREX_SHOWWAITMESSAGE = RegisterWindowMessage(STR_RWM_FFHACKSTEREX_SHOWWAITMESSAGE);


int StringToInt_HEX(CString text)
{
	int ret = 0;
	int len = text.GetLength();
	for (int co = 0; co < len; co++) {
		ret <<= 4;
		switch (text.GetAt(co)) {
		case '0': break;
		case '1': ret += 1; break;
		case '2': ret += 2; break;
		case '3': ret += 3; break;
		case '4': ret += 4; break;
		case '5': ret += 5; break;
		case '6': ret += 6; break;
		case '7': ret += 7; break;
		case '8': ret += 8; break;
		case '9': ret += 9; break;
		case 'a': case 'A': ret += 0xA; break;
		case 'b': case 'B': ret += 0xB; break;
		case 'c': case 'C': ret += 0xC; break;
		case 'd': case 'D': ret += 0xD; break;
		case 'e': case 'E': ret += 0xE; break;
		case 'f': case 'F': ret += 0xF; break;
		default: ret >>= 4; break;
		}
	}
	return ret;
}

int StringToInt(CString text)
{
	int ret = 0;
	int len = text.GetLength();
	for (int co = 0; co < len; co++) {
		ret *= 10;
		switch (text.GetAt(co)) {
		case '0': break;
		case '1': ret += 1; break;
		case '2': ret += 2; break;
		case '3': ret += 3; break;
		case '4': ret += 4; break;
		case '5': ret += 5; break;
		case '6': ret += 6; break;
		case '7': ret += 7; break;
		case '8': ret += 8; break;
		case '9': ret += 9; break;
		default: ret /= 10; break;
		}
	}
	return ret;
}

bool TraceFailMsg(LPCSTR message, CString file, int line, CString func)
{
	LogMsg << LOGAT(file,line,func) << message << std::endl;
	return false;
}

bool TraceFailMsg(std::string message, CString file, int line, CString func)
{
	LogMsg << LOGAT(file, line, func) << message << std::endl;
	return false;
}

int TraceFailCodeMsg(int returncode, LPCSTR message, CString file, int line, CString func)
{
	LogMsg << LOGAT(file, line, func) << message << std::endl;
	return returncode;
}

int TraceFailCodeMsg(int returncode, std::string message, CString file, int line, CString func)
{
	LogMsg << LOGAT(file, line, func) << message << std::endl;
	return returncode;
}

bool HasAnyFlag(int value, int mask)
{
	return (value & mask) != 0;
}

bool HasAllFlags(int value, int mask)
{
	return (value & mask) == mask;
}