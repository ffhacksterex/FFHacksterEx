#pragma once

#include <vector>
#include "collection_helpers.h"

class FontStack
{
public:
	FontStack();
	FontStack(CString aname, CString afacenames, int apoint10ths);
	FontStack(CString aname, const mfcstringvector & afacenames, int apoint10ths);
	~FontStack();

	CString name;
	std::vector<CString> facenames;
	int point10ths;
};

