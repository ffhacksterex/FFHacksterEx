#include "stdafx.h"
#include "FontStack.h"
#include "string_functions.h"

FontStack::FontStack()
{
}

FontStack::FontStack(CString aname, CString afacenames, int apoint10ths)
	: name(aname), facenames(Strings::split(afacenames, "|")), point10ths(apoint10ths)
{
}

FontStack::FontStack(CString aname, const mfcstringvector& afacenames, int apoint10ths)
	: name(aname), facenames(afacenames), point10ths(apoint10ths)
{
}

FontStack::~FontStack()
{
}