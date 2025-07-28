#include "stdafx.h"
#include "string_functions.h"
#include <sstream>
#include <limits>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <regex>

#define TRUESTR "true"
#define FALSESTR "false"

namespace Strings
{
	namespace Convert
	{
		std::vector<unsigned char> ToBytvector(std::string text, char separator, bool removeEmptyElements)
		{
			std::string seps(1, separator);
			auto parts = Strings::splitrx(text, seps, removeEmptyElements);

			std::vector<unsigned char> result;
			result.reserve(parts.size());
			for (const auto& p : parts) result.push_back((unsigned char)strtoul(p.c_str(), nullptr, 16));
			return result;
		}
	}


	CString strip_newlines(const CString & str)
	{
		CString newstr(str);
		newstr.Replace("\r", "");
		newstr.Replace("\n", "");
		return newstr;
	}

	size_t count_char(const CString & str, const char chtofind)
	{
		size_t count = 0;
		int length = str.GetLength();
		for (int len = 0; len < length; ++len) {
			if (str[len] == chtofind) ++count;
		}
		return count;
	}

	std::string right_pad(std::string str, int minlength)
	{
		if (str.length() >= (size_t)minlength)
			return str;
		return str + std::string(minlength - str.length(), ' ');
	}

	CString join(const mfcstringvector & strings, CString separator)
	{
		CString str;
		for (size_t st = 0; st < strings.size(); ++st) {
			if (st > 0) str.Append(separator);
			str.Append(strings[st]);
		}
		return str;
	}

	std::string join(const stdstringvector & strings, std::string separator)
	{
		std::string str;
		for (size_t st = 0; st < strings.size(); ++st) {
			if (st > 0) str += separator;
			str += strings[st];
		}
		return str;
	}

	std::string join(const stdstringset & strings, std::string separator)
	{
		std::string str;
		for (auto st = cbegin(strings), begin = cbegin(strings), end = cend(strings); st != end; ++st) {
			if (st != begin) str += separator;
			str += *st;
		}
		return str;
	}

	CString join(const mfcstringset & strings, CString separator)
	{
		CString str;
		for (auto st = cbegin(strings), begin = cbegin(strings), end = cend(strings); st != end; ++st) {
			if (st != begin) str += separator;
			str += *st;
		}
		return str;
	}

	mfcstringvector split(CString srcstring, CString separator)
	{
		std::vector<CString> parts;
		if (!srcstring.IsEmpty()) {
			int index = srcstring.Find(separator);
			if (index == -1) {
				parts.push_back(srcstring);
			}
			else {
				int base = 0;
				while (index != -1) {
					parts.push_back(srcstring.Mid(base, index - base));
					base = index + separator.GetLength();
					index = srcstring.Find(separator, base);
				}
				parts.push_back(srcstring.Mid(base));
			}
		}
		return parts;
	}

	stdstringvector split(std::string stdsrcstring, std::string stdseparator)
	{
		CString srcstring = stdsrcstring.c_str();
		CString separator = stdseparator.c_str();
		return convert(split(srcstring, separator));
	}

	mfcstringvector splitrx(CString srcstring, CString separators, bool removeEmptyElements)
	{
		std::string stdsrcstring = (LPCSTR)srcstring;
		std::string stdseparators = (LPCSTR)separators;
		return convert(splitrx(stdsrcstring, stdseparators, removeEmptyElements));
	}

	stdstringvector splitrx(std::string srcstring, std::string rxseparators, bool removeEmptyElements)
	{
		if (rxseparators.empty())
			throw std::domain_error(__FUNCTION__ " separators cannot be empty.");

		// Build the RX string as a [abc...]
		std::string restring = "[" + rxseparators + "]";

		auto const re = std::regex{ restring };
		auto vec = std::vector<std::string>(
			std::sregex_token_iterator{ std::begin(srcstring), std::end(srcstring), re, -1 },
			std::sregex_token_iterator{}
		);

		if (removeEmptyElements) {
			vec.erase(
				std::remove_if(vec.begin(), vec.end(), [](const auto& s) { return s.empty(); }),
				vec.end());
		}
		return vec;
	}

	CString extract_term(CString str, CString prefix)
	{
		if (str.Find(prefix) != 0) return CString();
		return str.Mid(prefix.GetLength());
	}

	int to_int32(CString str)
	{
		return atol(str);
	}

	CString from(int value)
	{
		CString s;
		s.Format("%d", value);
		return s;
	}

	std::string to_lower(std::string str)
	{
		//REFACTOR - std::transform(cbegin(str), cend(str), begin(str), ::tolower);
		std::transform(cbegin(str), cend(str), begin(str), [](char c) { return (char)::tolower(c); });
		return str;
	}

	std::string to_upper(std::string str)
	{
		//std::transform(cbegin(str), cend(str), begin(str), ::toupper);
		std::transform(cbegin(str), cend(str), begin(str), [](char c) { return (char)::toupper(c); });
		return str;
	}


	namespace
	{

		//INTERNAL - flags to control string trimming
		enum class trimaction { left = 1, right = 2, both = left | right };

		//INTERNAL - trims a string inplace and returns a reference to the string passed in.
		std::string & triminplace(std::string & str, trimaction act)
		{
			if (str.empty()) return str;

			// trim the leading space
			size_t first = 0;
			if (act == trimaction::left || act == trimaction::both) {
				while (first < str.length() && isspace(str[first])) ++first;
			}
			size_t last = str.length() - 1;
			if (act == trimaction::right || act == trimaction::both) {
				while (last > 0 && isspace(str[last])) --last;
			}

			auto result = first > 0 || last < (str.length() - 1) ? str.substr(first, last - first + 1) : str;
			str = result;
			return str;
		}

	} // end namespace unnamed (trim)


	std::string trim(std::string str)
	{
		std::string newstr(str);
		return triminplace(newstr, trimaction::both);
	}

	std::string trimright(std::string str)
	{
		std::string newstr(str);
		return triminplace(newstr, trimaction::right);
	}

	std::string trimleft(std::string str)
	{
		std::string newstr(str);
		return triminplace(newstr, trimaction::left);
	}

	bool isemptyorwhitespace(CString str)
	{
		CString strtest(str);
		return strtest.Trim().IsEmpty();
	}

	bool isemptyorwhitespace(std::string str)
	{
		CString strtest(str.c_str());
		return strtest.Trim().IsEmpty();
	}

	mfcstringvector doublenullterm_to_list(const char * sz2buffer)
	{
		std::vector<CString> strings;
		const char* sztext = sz2buffer;
		while (*sztext) {
			CString str = sztext;
			strings.push_back(str);
			sztext += str.GetLength() + 1; // skip past the string terminator
		}
		return strings;
	}

	// Returns the number of duplicates removed.
	int doublenullterm_to_kvmap(const char * pipesepkvbuffer, mfcstrstrmap & kvpairs)
	{
		int dupes = 0;
		const char* sztext = pipesepkvbuffer;
		while (*sztext) {
			CString key = sztext;
			sztext += key.GetLength() + 1; // skip past the string terminator

			int pipe = key.Find('|');
			if (pipe == -1) continue;

			CString value = key.Mid(pipe + 1);
			key = key.Left(pipe);
			if (kvpairs.find(key) != kvpairs.end()) {
				++dupes;
				continue;
			}
			kvpairs[key] = value;
		}
		return dupes;
	}

	mfcstringvector delimited_to_list(const char * szbuffer, char delim)
	{
		std::vector<CString> values;
		if (szbuffer != nullptr) {
			std::istringstream istr(szbuffer);
			while (istr && !istr.eof()) {
				std::string value;
				std::getline(istr, value, delim);
				values.push_back(value.c_str());
			}
		}
		return values;
	}


	std::wstring widen(std::string text)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
		std::wstring ws = cvt.from_bytes(text);
		return ws;
	}

	// Return a string that is similar to a varname..
	// This functon does not remove leading digits.
	CString MakeVarName(CString text)
	{
		static const char removals[] = " `~!@#$%^&*()=+[{]}\\|;:'\",<.>/?";
		CString varname(text);
		for (auto ch : removals) varname.Remove(ch);
		return varname;
	}



	std::ostream & operator<<(std::ostream & os, const quoted & q)
	{
		os << '"' << (const std::string&)q << '"';
		return os;
	}

	std::istream & operator >> (std::istream & is, quoted & q)
	{
		if (is.peek() != '"')
			THROWEXCEPTION(std::runtime_error, "can't read quoted string with missing leading quote");

		is.ignore(1);
		std::getline(is, (std::string &)q, '"');
		return is;
	}

	std::istream & operator >> (std::istream & is, quoted q)
	{
		if (is.peek() != '"')
			THROWEXCEPTION(std::runtime_error, "can't read quoted string with missing leading quote");

		is.ignore(1);
		std::getline(is, (std::string &)q, '"');
		return is;
	}

	std::ostream & operator<<(std::ostream & os, const braced & b)
	{
		os << '{' << (const std::string&)b << '}';
		return os;
	}

	std::istream & operator >> (std::istream & is, braced & b)
	{
		std::string & s = b;
		s.clear();

		if (is.peek() != '{')
			THROWEXCEPTION(std::runtime_error, "can't read braced string with missing openbrace");

		is.ignore(1);
		char ch = (char)is.get();
		if (ch != '}') {
			size_t count = 1;
			while (is && !is.eof() && count > 0) {
				if (ch == '}') {
					--count;
					if (count > 0) s += ch;
				}
				else {
					s += ch;
					if (ch == '{') ++count;
				}
			}
			if (count > 0)
				THROWEXCEPTION(std::runtime_error, "can't read braced text missing " + std::to_string(count) + " endbraces");
		}
		return is;
	}

	std::ostream & operator<<(std::ostream & os, const backquoted & b)
	{
		os << '`' << (const std::string&)b << '`';
		return os;
	}

	std::istream & operator >> (std::istream & is, backquoted & b)
	{
		if (is.peek() != '`')
			THROWEXCEPTION(std::runtime_error, "can't read quoted string with missing leading quote");

		is.ignore(1);
		std::getline(is, (std::string &)b, '`');
		return is;
	}

	std::istream & operator >> (std::istream & is, backquoted b)
	{
		if (is.peek() != '`')
			THROWEXCEPTION(std::runtime_error, "can't read quoted string with missing leading quote");

		is.ignore(1);
		std::getline(is, (std::string &)b, '`');
		return is;
	}


} // end namespace Strings
