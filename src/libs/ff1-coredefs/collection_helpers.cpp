#include "stdafx.h"
#include "collection_helpers.h"

bytevector convert(const boolvector & bools)
{
	bytevector bytes(bools.size());
	for (size_t st = 0; st < bools.size(); ++st) bytes[st] = bools[st] ? 1 : 0;
	return bytes;
}

boolvector convert(const bytevector & bytes)
{
	boolvector bools(bytes.size());
	for (size_t st = 0; st < bytes.size(); ++st) bools[st] = bytes[st] == 1;
	return bools;
}

mfcstringvector & append(mfcstringvector & vec, const stdstringvector & insvec)
{
	for (auto i : insvec) vec.push_back(i.c_str());
	return vec;
}
