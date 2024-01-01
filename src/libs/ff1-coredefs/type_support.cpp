#include "stdafx.h"
#include "type_support.h"
#include "map_types.h"
#include "set_types.h"
#include "string_functions.h"
#include <sstream>
#include <regex>
#include <bitset>

using namespace Strings;

namespace Types
{
	const std::set<CString> types_sint{
		"int8", "int16", "int24", "int32"
	};
	const std::set<CString> types_uint{
		"uint8", "uint16", "uint24", "uint32"
	};
	const std::set<CString> types_shex{
		"hex8", "hex16", "hex24", "hex32",
	};
	const std::set<CString> types_uhex{
		"byte", "word", "bword", "dword",
	};
	const std::set<CString> types_addr{
		"addr", "opcode"
	};
	const std::set<CString> types_rgb{
		"rgb"
	};

	const std::map<CString, int> INT_TYPE_WIDTHMAP{
		{ "byte", 1 },{ "word", 2 },{ "bword", 3 },{ "dword", 4 },
		{ "hex8", 1 },{ "hex16", 2 },{ "hex24", 3 },{ "hex32", 4 },
		{ "opcode", 1 },{ "addr", 2 },
		{ "int8", 1 },{ "int16", 2 },{ "int24", 3 },{ "int32", 4 },
		{ "uint8", 1 },{ "uint16", 2 },{ "uint24", 3 },{ "uint32", 4 },
		{ "rgb", 3 }
	};


	mfcstringvector GetUserCreatableTypes()
	{
		mfcstringvector usercreatabletypes;
		for (auto kv : types_sint) usercreatabletypes.push_back(kv);
		for (auto kv : types_uint) usercreatabletypes.push_back(kv);
		for (auto kv : types_shex) usercreatabletypes.push_back(kv);
		for (auto kv : types_uhex) usercreatabletypes.push_back(kv);
		for (auto kv : types_addr) usercreatabletypes.push_back(kv);
		for (auto kv : types_rgb) usercreatabletypes.push_back(kv);
		usercreatabletypes.push_back("str");
		usercreatabletypes.push_back("bool");
		usercreatabletypes.push_back("str[]");
		usercreatabletypes.push_back("bool[]");
		usercreatabletypes.push_back("byte[]");
		return usercreatabletypes;
	}

	std::set<CString> GetDecConvertibleTypes()
	{
		return types_sint + types_uint;
	}

	std::set<CString> GetAddrConvertibleTypes()
	{
		return types_addr;
	}

	std::set<CString> GetHexConvertibleTypes()
	{
		return types_shex + types_uhex;
	}

	std::set<CString> GetRgbConvertibleTypes()
	{
		return types_rgb;
	}


	//DEVNOTE - to get total control over this display, type traits would have to be introduced.
	//	these would define range and output format and could be retrieved from a map (or INI) by type name.
	//	That will require the type system to be formalized.

	// Returns true if the type is athe name of a decimal, hex, addr, or rgb type.
	bool IsIntegralType(CString type)
	{
		return INT_TYPE_WIDTHMAP.find(type) != cend(INT_TYPE_WIDTHMAP);
	}

	int GetTypeByteWidth(CString type)
	{
		auto iter = INT_TYPE_WIDTHMAP.find(type);
		if (iter == cend(INT_TYPE_WIDTHMAP))
			throw std::invalid_argument(std::string((LPCSTR)type) + " doesn't have a defined byte width.");

		return iter->second;
	}

	pair_result<CString> ValidateAddrInt(CString type, int bytewidth, CString text, Int64ToStrConvFunc cvtfunc)
	{
		// $12AB
		CString newstring;
		CString failmsg;

		if (cvtfunc == nullptr) {
			failmsg = "Can't return value because no conversion function was specified.";
		}
		else if (bytewidth == -1) {
			failmsg.Format("Invalud byte width '%d' specified for type '%s'.", bytewidth, (LPCSTR)type);
		}
		else {
			std::set<CString> knowntypes = types_addr; //{ "addr", "opcode" };
			if (!has(knowntypes, type)) {
				failmsg.Format("Type %s is unknown, the edit will be cancelled.", (LPCSTR)type);
			}
			else {
				try {
					unsigned int byteshift = (1 << (bytewidth * 8));
					unsigned int max = byteshift - 1;
					unsigned int value = addr(text);
					if (value > max) failmsg.Format("Value can't be above the [$%X,$%X] range.", 0, max);

					if (failmsg.IsEmpty())
						newstring = cvtfunc(value);
				}
				catch (std::exception & ex) {
					failmsg.Format("An exception prevented the inplace update: %s", ex.what());
				}
				catch (...) {
					failmsg = "An unknown exception prevented the inplace edit.";
				}
			}
		}

		bool valid = failmsg.IsEmpty();
		return{ valid, valid ? newstring : failmsg };
	}

	pair_result<CString> ValidateDecInt(CString type, int bytewidth, CString text, Int64ToStrConvFunc cvtfunc)
	{
		CString newstring;
		CString failmsg;

		std::set<CString> signedtypes = types_sint; //{ "int8", "int16", "int24", "int32" };
		std::set<CString> unsignedtypes = types_uint; //{ "uint8", "uint16", "uint24", "uint32" };
		if (!has(signedtypes, type) && !has(unsignedtypes, type)) {
			failmsg.Format("Type '%s' is unknown, the edit will be cancelled.", (LPCSTR)type);
		}
		else if (bytewidth == -1) {
			failmsg.Format("Invalud byte width '%d' specified for type '%s'.", bytewidth, (LPCSTR)type);
		}
		else {
			try
			{
				bool issigned = has(signedtypes, type);
				long long byteshift = ((long long)1 << ((bytewidth * 8) - 1));
				long long min = issigned ? -byteshift : 0;
				long long max = issigned ? byteshift - 1 : (byteshift - 1) + byteshift;
				long long value = dec(text);
				CString strsign = issigned ? "-" : "";

				if (value < min) failmsg.Format("Value can't be below the [%d, %d] range.", (int)min, (int)max);
				else if (value > max) failmsg.Format("Value can't be above the [%d, %d] range.", (int)min, (int)max);

				if (failmsg.IsEmpty())
					newstring = cvtfunc(value);
			}
			catch (std::exception & ex) {
				failmsg.Format("An exception prevented the inplace update: %s", ex.what());
			}
			catch (...) {
				failmsg = "An unknown exception prevented the inplace edit.";
			}
		}

		bool valid = failmsg.IsEmpty();
		return{ valid, valid ? newstring : failmsg };
	}

	pair_result<CString> ValidateHexInt(CString type, int bytewidth, CString text, Int64ToStrConvFunc cvtfunc)
	{
		// test the bytewidth, for the min and max values from the bytewidth specified (e.g. 1 =[-128, 127])
		//		[-(1 << (bytewidth*8 - 1), (1 << (bytewidth*8 - 1)) - 1]

		// if both tests pass, write the new value as "%0*x", bytewidth*2
		//	then "0x" + HexCase(newvalue)
		//	that'll produce values like 0x040010

		// 0x1234ABCD
		CString newstring;
		CString failmsg;

		std::set<CString> signedtypes = types_shex;
		std::set<CString> unsignedtypes = types_uhex + types_rgb;
		if (!has(signedtypes, type) && !has(unsignedtypes, type)) {
			failmsg.Format("Type '%s' is unknown, the edit will be cancelled.", (LPCSTR)type);
		}
		else if (bytewidth == -1) {
			failmsg.Format("Invalid byte width '%d' specified for type '%s'.", bytewidth, (LPCSTR)type);
		}
		else {
			try
			{
				bool issigned = has(signedtypes, type);
				long long byteshift = ((long long)1 << ((bytewidth * 8) - 1));
				long long min = issigned ? -byteshift : 0;
				long long max = issigned ? byteshift - 1 : (byteshift - 1) + byteshift;
				long long value = hex(text);
				CString strsign = issigned ? "-" : "";

				// Negative values in hex display as two's complement, e.g. for 16 bit values, -1 is 0xFFFF, -127 is 0xFF81
				// To display a negative min value, mask the min value with byteshift (or call abs) and display the sign manually.
				if (value < min)
					failmsg.Format("Value can't be below the [%s0x%X, 0x%X] range.", (LPCSTR)strsign, (int)(min & byteshift), (int)max);
				else if (value > max)
					failmsg.Format("Value can't be above the [%s0x%X, 0x%X] range.", (LPCSTR)strsign, (int)(min & byteshift), (int)max);

				if (failmsg.IsEmpty())
					newstring = cvtfunc(value);
			}
			catch (std::exception & ex) {
				failmsg.Format("An exception prevented the inplace update: %s", ex.what());
			}
			catch (...) {
				failmsg = "An unknown exception prevented the inplace edit.";
			}
		}

		bool valid = failmsg.IsEmpty();
		return{ valid, valid ? newstring : failmsg };
	}

	pair_result<CString> ValidateArray(CString type, CString text)
	{
		//DEVNOTE - if the type system is rewritten, then the text.IsEmpty check below might need revision.
		if (type.IsEmpty()) return { false, "Empty type names ('') are not supported; a type must be declared." };
		if (text.IsEmpty()) return { false, "The '" + type + "' array value cannot be an empty string." };
		if (text[text.GetLength() - 1] != '}') return { false, "The '" + type + "' value is missing its closing brace." };

		try {
			std::string strtype = (LPCSTR)type;
			if (type == "str[]") {
				if (text.Find(type + "{") != 0)
					throw std::runtime_error("The '" + strtype + "' value is malformed.");

				return { true, text };
			}
			else if (type == "bool[]") {
				// if any terms aren't boolean, throw an exception
				//TODO - type system is in desperate need of a rewrite, but
				//		I can't tackle that for now.
				if (text.Find(type + "{") != 0)
					throw std::runtime_error("The '" + strtype + "' value is malformed.");

				auto mstrvec = as_mfcstrvec(text);
				for (auto i = 0; i < mstrvec.size(); ++i ) {
					const CString& str = mstrvec[i];
					if (str.CompareNoCase("true") != 0 && str.CompareNoCase("false") != 0)
						throw std::runtime_error("Can't convert index " + std::to_string(i) + " '"
							+ std::string((LPCSTR)str) + "' into a '" + strtype + "'.");
				}
				return { true, text };
			}
			else if (type == "byte[]") {
				if (text.Find(type + "{") != 0)
					throw std::runtime_error("The '" + strtype + "' value is malformed.");

				auto mstrvec = as_mfcstrvec(text);
				for (auto i = 0; i < mstrvec.size(); ++i) {
					const CString& str = mstrvec[i];
					if (str.CompareNoCase("true") != 0 && str.CompareNoCase("false") != 0)
						throw std::runtime_error("Can't convert index " + std::to_string(i) + " '"
							+ std::string((LPCSTR)str) + "' into a '" + strtype + "'.");
				}
				return { true, text };
			}
			else {
				return { false, "Array type '" + type + "' cannot be validated." };
			}
		}
		catch (std::exception& ex) {
			return { false, "Array type '" + type + "' failed validation.\n" + CString(ex.what()) };
		}
	}



	// ################################################################
	// Notional type system implementation

	namespace {
		const std::regex rx_dec{ "(\\-)?(\\d+)" };
		const std::regex rx_hex{ "(\\-)?(0x[0-9A-Fa-f]+)" };
		const std::regex rx_addr{ "(\\-)?\\$([0-9A-Fa-f]+)" }; // the $ isn't paert of the match to avoid read problems

		const std::regex rx_bin{ "(\\-)?\\%([0-1]+)" };       // the % isn't paert of the match to avoid read problems
		const std::regex rx_bool{ "(true|false)", std::regex_constants::icase };

		const std::regex rx_boolvec{ "bool\\[\\]\\{.*\\}" };
		const std::regex rx_bytevec{ "byte\\[\\]\\{.*\\}" };
		const std::regex rx_strvec{ "str\\[\\]\\{.*\\}" };
	}


	namespace
	{

		// Calls regex_match on tetx with a specified regex,
		// and throws an exception if there's no match or a problem.
		//N.B. - The text param must be a reference since the returned std::smatch will refer to it.
		//       Passing text by value means that it will go out of scope when the function returns, breaking std::smatch.
		std::smatch find_match(const std::string & text, std::string formattype, std::regex rx)
		{
			std::smatch m;

			if (!std::regex_match(text, m, rx))
				THROWEXCEPTION(std::invalid_argument, "failed to find a '" + formattype + "' match for text: " + text);
			if (m.position(0) != 0)
				THROWEXCEPTION(std::invalid_argument, formattype + " value must be at the front of the string '" + text + "'.");

			return m;
		}

	}


	int to_int(CString text)
	{
		if (is_dec(text))
			return dec(text);
		if (is_hex(text))
			return hex(text);
		if (is_addr(text))
			return addr(text);
		if (is_bin(text))
			return bin(text);

		THROWEXCEPTION(std::invalid_argument, "unknown integer type format for " + std::string((LPCSTR)text));
	}

	int to_int(std::string text)
	{
		return to_int((CString)text.c_str());
	}

	CString from_dec(int value)
	{
		return dec(value);
	}

	CString from_addr(int value)
	{
		return addr(value);
	}

	CString from_hex(int value)
	{
		return hex(value);
	}

	int strtohex(CString text)
	{
		return strtoul(text, nullptr, 16);
	}

	int dec(CString text)
	{
		if (text.IsEmpty()) return 0;

		const std::string type = "dec";
		std::string str = trim((LPCSTR)text);

		auto matches = find_match(str, type, rx_dec);
		int sign = matches[1].matched ? -1 : 1;
		int value = (int)std::stoll(matches[2].str());
		return sign * value;
	}

	CString dec(int value)
	{
		CString text;
		text.Format("%d", value);
		return text;
	}

	bool is_dec(CString text)
	{
		if (text.IsEmpty()) return true;

		std::string stdstr = (LPCSTR)text;
		std::smatch m;
		bool found = std::regex_match(stdstr, m, rx_dec);
		return found;
	}

	int hex(CString text)
	{
		if (text.IsEmpty()) return 0;

		const std::string type = "hex";
		std::string str = trim((LPCSTR)text);

		auto matches = find_match(str, type, rx_hex);
		int sign = matches[1].matched ? -1 : 1;
		int value = (int)std::stoll(matches[2].str(), nullptr, 16);
		return sign * value;
	}

	CString hex(int value)
	{
		return hex(value, 2);
	}

	CString hex(int value, size_t digits)
	{
		CString text;
		CString sign = value < 0 ? "-" : "";
		text.Format("%s0x%0*x", (LPCSTR)sign, digits, value);
		return text;
	}

	bool is_hex(CString text)
	{
		if (text.IsEmpty()) return true;

		std::string str = trim((LPCSTR)text);
		auto iter = std::sregex_iterator(cbegin(str), cend(str), rx_hex);
		bool found = iter != std::sregex_iterator();
		return found;
	}

	CString hex_upper(CString text, bool forceupper)
	{
		if (forceupper) {
			text.MakeUpper();
			text.Replace("0X", "0x");  // don't make the 0x uppercase.
		}
		return text;
	}


	int addr(CString text)
	{
		if (text.IsEmpty()) return 0;

		const std::string type = "addr";
		std::string str = trim((LPCSTR)text);

		auto matches = find_match(str, type, rx_addr);
		int sign = matches[1].matched ? -1 : 1;
		int value = (int)std::stoll(matches[2].str(), nullptr, 16);
		return sign * value;
	}

	CString addr(int value)
	{
		return addr(value, 2);
	}

	CString addr(int value, size_t digits)
	{
		CString text;
		CString sign = value < 0 ? "-" : "";
		text.Format("%s$%0*x", (LPCSTR)sign, digits, value);
		return text;
	}

	bool is_addr(CString text)
	{
		if (text.IsEmpty()) return true;

		std::string str = trim((LPCSTR)text);
		auto iter = std::sregex_iterator(cbegin(str), cend(str), rx_addr);
		bool found = iter != std::sregex_iterator();
		return found;
	}

	CString addr_upper(CString text, bool forceupper)
	{
		if (forceupper) text.MakeUpper();
		return text;
	}


	int bin(CString text)
	{
		if (text.IsEmpty()) return 0;

		const std::string type = "bin";
		std::string str = trim((LPCSTR)text);

		auto matches = find_match(str, type, rx_bin);
		int sign = matches[1].matched ? -1 : 1;
		int value = (int)std::stoll(matches[2].str(), nullptr, 2);
		return sign * value;
	}

	CString bin(int value)
	{
		return bin(value, 8);
	}

	CString bin(int value, size_t digits)
	{
		std::string strnum;
		strnum.reserve(64); // 64-bit unsigned length
		size_t bit = 0;
		int absvalue = abs(value);
		while (absvalue >= (1i64 << bit)) {
			strnum.insert(strnum.begin(), (value & (1i64 << bit)) ? '1' : '0');
			++bit;
		}

		if (strnum.length() < digits) {
			std::string padding = std::string(digits - strnum.length(), '0');
			strnum.insert(strnum.begin(), begin(padding), end(padding));
		}
		strnum.insert(0, "%");
		if (value < 0) strnum.insert(0, "-");

		strnum.shrink_to_fit();
		return CString(strnum.c_str());
	}

	bool is_bin(CString text)
	{
		if (text.IsEmpty()) return true;

		std::string str = trim((LPCSTR)text);
		auto iter = std::sregex_iterator(cbegin(str), cend(str), rx_bin);
		bool found = iter != std::sregex_iterator();
		return found;
	}


	bool boole(CString text)
	{
		const std::string type = "boolean";
		std::string str = trim((LPCSTR)text);

		auto matches = find_match(str, type, rx_bool);
		auto strvalue = matches[1].str();
		CString cs = strvalue.c_str();
		bool result = cs.CompareNoCase("true") == 0;
		return result;
	}

	CString boole(bool value)
	{
		CString cs = value ? "true" : "false";
		return cs;
	}

	bool is_boole(CString text)
	{
		bool matched = (text.CompareNoCase("true") == 0) || (text.CompareNoCase("false") == 0);
		return matched;
	}

	CString boole_upper(CString text, bool forceupper)
	{
		if (forceupper) text.MakeUpper();
		return text;
	}


	CString bytevec(const bytevector & bytes)
	{
		static const int width = 2;

		std::ostringstream o;
		o.fill('0');
		o.setf(o.flags() | std::ios::uppercase);

		o << BYTETAG << "{ ";
		for (size_t st = 0; st < bytes.size(); ++st) {
			if (st > 0) o << ' ';
			o << "0x";
			o.width(width); // per docs, oper<< resets width to 0, so reset it here
			o << std::hex << (int)bytes[st];
		}
		o << " }";
		return CString(o.str().c_str());
	}

	bytevector bytevec(CString text)
	{
		if (text.IsEmpty()) return{};

		std::istringstream i((LPCSTR)text);
		std::string tag;
		std::getline(i, tag, '{') >> std::ws;
		if (tag != BYTETAG)
			THROWEXCEPTION(std::runtime_error, __FUNCTION__ " expected an array tag of " + BYTETAG + " ,but instead found " + tag + " (check whitespace as well)");

		bytevector bytes;
		while (i && !i.eof()) {
			if (i.peek() == '}') break;

			unsigned int b = 0;
			i >> std::hex >> b >> std::ws;
			bytes.push_back((BYTE)b);
		}
		return bytes;

	}

	bool is_bytevec(CString text)
	{
		std::string str = trim((LPCSTR)text);
		auto iter = std::sregex_iterator(cbegin(str), cend(str), rx_bytevec);
		bool found = iter != std::sregex_iterator();
		return found;
	}


	CString boolvec(const boolvector & bools)
	{
		std::ostringstream o;

		o << BOOLTAG << "{ ";
		for (size_t st = 0; st < bools.size(); ++st) {
			if (st > 0) o << ' ';
			o << std::boolalpha << bools[st];
		}
		o << " }";
		return CString(o.str().c_str());
	}

	//TODO - need to use ICU for text at some point.
	//		The C++ standard doesn't have good tools to handle text manipulation.
	boolvector boolvec(CString text)
	{
		if (text.IsEmpty()) return{};

		std::istringstream i((LPCSTR)text);
		std::string tag;
		std::getline(i, tag, '{') >> std::ws;
		if (tag != BOOLTAG)
			THROWEXCEPTION(std::runtime_error, __FUNCTION__ " expected an array tag of " + BOOLTAG + ", but instead found " + tag + " (check whitespace as well)");

		boolvector bools;
		while (i && !i.eof()) {
			if (i.peek() == '}') break;

			bool b = false;
			i >> std::boolalpha >> b >> std::ws;
			bools.push_back(b);
		}
		return bools;

	}

	bool is_boolvec(CString text)
	{
		std::string str = trim((LPCSTR)text);
		auto iter = std::sregex_iterator(cbegin(str), cend(str), rx_boolvec);
		bool found = iter != std::sregex_iterator();
		return found;
	}


	CString stdstrvec(const stdstringvector & strs)
	{
		std::ostringstream o;

		o << STRTAG << "{ ";
		for (size_t st = 0; st < strs.size(); ++st) {
			//FUTURE - dquote strings containing spaces? for now, no, throw if a string contains a space
			if (strs[st].find(' ') != std::string::npos)
				THROWEXCEPTION(std::runtime_error, "to_string(stdstr) doesn't yet support strings with embedded spaces");

			if (st > 0) o << ' ';
			o << strs[st];
		}
		o << " }";
		return CString(o.str().c_str());
	}

	stdstringvector stdstrvec(CString text)
	{
		if (text.IsEmpty()) return{};

		std::istringstream i((LPCSTR)text);
		std::string tag;
		std::getline(i, tag, '{') >> std::ws;
		if (tag != STRTAG)
			THROWEXCEPTION(std::runtime_error, __FUNCTION__ " expected an array tag of " + STRTAG + " , but instead found " + tag);

		stdstringvector strs;
		while (i && !i.eof()) {
			if (i.peek() == '}') break;

			std::string s;
			//FUTURE - include the ability to read dquoted strings? for now, no, but if so, we must throw on unmatched dquotes.
			i >> s >> std::ws;
			strs.push_back(s);
		}
		return strs;
	}

	CString mfcstrvec(const mfcstringvector & strs)
	{
		std::ostringstream o;

		o << STRTAG << "{ ";
		for (size_t st = 0; st < strs.size(); ++st) {
			//FUTURE - dquote strings containing spaces? for now, no, throw if a string contains a space
			if (strs[st].Find(' ') != -1)
				THROWEXCEPTION(std::runtime_error, "to_string(mfcstr) doesn't yet support strings with embedded spaces");

			if (st > 0) o << ' ';
			o << strs[st];
		}
		o << " }";
		return CString(o.str().c_str());
	}

	mfcstringvector mfcstrvec(CString text)
	{
		if (text.IsEmpty()) return{};

		std::istringstream i((LPCSTR)text);
		std::string tag;
		std::getline(i, tag, '{') >> std::ws;
		if (tag != STRTAG)
			THROWEXCEPTION(std::runtime_error, __FUNCTION__ " expected an array tag of " + STRTAG + " , but instead found " + tag);

		mfcstringvector strs;
		while (i && !i.eof()) {
			if (i.peek() == '}') break;

			std::string s;
			//FUTURE - include the ability to read dquoted strings? for now, no, but if so, we must throw on unmatched dquotes.
			i >> s >> std::ws;
			strs.push_back(s.c_str());
		}
		return strs;
	}

	bool is_strvec(CString text)
	{
		std::string str = trim((LPCSTR)text);
		auto iter = std::sregex_iterator(cbegin(str), cend(str), rx_strvec);
		bool found = iter != std::sregex_iterator();
		return found;
	}

	CString get_arraytype(CString text)
	{
		if (text.Find((STRTAG + "{").c_str()) == 0)
			return STRTAG.c_str();

		if (text.Find((BOOLTAG + "{").c_str()) == 0)
			return BOOLTAG.c_str();

		if (text.Find((BYTETAG + "{").c_str()) == 0)
			return BYTETAG.c_str();

		return CString();
	}

	namespace // unnamed
	{
		// Remove the array type declaration if it's present
		// and return the ready-to-convert altered string.
		CString prep_as_strvec(CString text)
		{
			CString buf = text;
			auto tag = get_arraytype(buf);
			if (!tag.IsEmpty()) {
				buf.Delete(0, tag.GetLength());
				if (buf.Find('{') == 0) buf.Delete(0, 1);
				if (buf.ReverseFind('}') == (buf.GetLength() - 1)) buf.Delete(buf.GetLength() - 1, 1);
			}
			return buf;
		}
	}

	// Convert a string to stdstringvector even if it isn't a str[]{}
	stdstringvector as_stdstrvec(CString text)
	{
		CString buf = prep_as_strvec(text);
		std::istringstream is((LPCSTR)buf);
		std::istream_iterator<std::string> iend;
		stdstringvector vec;
		for (std::istream_iterator<std::string> iter(is); iter != iend; ++iter) {
			vec.push_back(*iter);
		}
		return vec;
	}

	// Convert a string to mfcstringvector even if it isn't a str[]{}
	mfcstringvector as_mfcstrvec(CString text)
	{
		CString buf = prep_as_strvec(text);
		std::istringstream is((LPCSTR)buf);
		std::istream_iterator<std::string> iend;
		mfcstringvector vec;
		for (std::istream_iterator<std::string> iter(is); iter != iend; ++iter) {
			vec.push_back(iter->c_str());
		}
		return vec;
	}

} // end namespace Types