#pragma once

#include "pair_result.h"
#include "vector_types.h"
#include <functional>

using Int64ToStrConvFunc = std::function<CString(long long value)>;

namespace Types
{
	mfcstringvector GetUserCreatableTypes();

	std::set<CString> GetDecConvertibleTypes();
	std::set<CString> GetAddrConvertibleTypes();
	std::set<CString> GetHexConvertibleTypes();
	std::set<CString> GetRgbConvertibleTypes();

	bool IsIntegralType(CString type);
	int GetTypeByteWidth(CString type);

	pair_result<CString> ValidateAddrInt(CString type, int bytewidth, CString text, Int64ToStrConvFunc cvtfunc);
	pair_result<CString> ValidateDecInt(CString type, int bytewidth, CString text, Int64ToStrConvFunc cvtfunc);
	pair_result<CString> ValidateHexInt(CString type, int bytewidth, CString text, Int64ToStrConvFunc cvtfunc);


	// Notional type functions

	const std::string BYTETAG{ "byte[]" };
	const std::string BOOLTAG{ "bool[]" };
	const std::string STRTAG{ "str[]" };

	int to_int(CString text);

	// Wrappers used when passing the function as an argument to other functions
	CString from_dec(int value);
	CString from_addr(int value);
	CString from_hex(int value);
	int strtohex(CString text); // converts from both 0x12AB and 12AB formats

	int dec(CString text);
	CString dec(int value);
	bool is_dec(CString text);

	int hex(CString text);
	CString hex(int value); //N.B. - defaults to 2 digits, by default, translates to lowercase hex digits
	CString hex(int value, size_t digits); //N.B. - by default, translates to lowercase hex digits
	bool is_hex(CString text);
	CString hex_upper(CString text, bool forceupper);

	int addr(CString text);
	CString addr(int value); //N.B. - defaults to 2 digits, by default, translates to lowercase hex digits
	CString addr(int value, size_t digits); //N.B. - by default, translates to lowercase hex digits
	bool is_addr(CString text);
	CString addr_upper(CString text, bool forceupper);

	int bin(CString text);
	CString bin(int value); // defaults to 8 digits
	CString bin(int value, size_t digits);
	bool is_bin(CString text);

	//DEVNOTE - this is boole instead of boolean because rpcndr.h typedefs unsigned char as boolean
	//          defining __RPCNDR_H__ in stdafx would disable it project-wide, but it causes some problems.
	bool boole(CString text);
	CString boole(bool value);
	bool is_boole(CString text);
	CString boole_upper(CString text, bool forceupper); //N.B. - by default, translates to lowercase boolean word

	CString bytevec(const bytevector & bytes);
	bytevector bytevec(CString text);
	bool is_bytevec(CString text);

	CString boolvec(const boolvector & bools);
	boolvector boolvec(CString text);
	bool is_boolvec(CString text);

	CString stdstrvec(const stdstringvector & strs);
	stdstringvector stdstrvec(CString text);

	CString mfcstrvec(const mfcstringvector & strs);
	mfcstringvector mfcstrvec(CString text);

	bool is_strvec(CString text);
	CString get_arraytype(CString text);
}
