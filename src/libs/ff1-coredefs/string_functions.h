#pragma once

#include <afx.h>
#include <string>
#include <set>
#include "collection_helpers.h"
#include <functional>

namespace Strings
{
	CString strip_newlines(const CString & str);
	size_t count_char(const CString & str, const char chtofind);
	std::string right_pad(std::string str, int minlength);

	CString join(const mfcstringvector & strings, CString separator);
	std::string join(const stdstringvector & strings, std::string separator);
	mfcstringvector split(CString srcstring, CString separator);
	stdstringvector split(std::string srcstring, std::string separator);

	std::string join(const stdstringset & strings, std::string separator);
	CString join(const mfcstringset & strings, CString separator);

	template <typename T>
	CString join(const std::vector<T> & list, const char* separator, std::function<CString(const T& obj)> getstrfunc);
	template <typename T>
	std::string join(const std::vector<T> & list, const char* separator, std::function<std::string(const T& obj)> getstrfunc);

	CString extract_term(CString str, CString prefix);

	int to_int32(CString str);
	CString from(int value);

	std::string to_lower(std::string str);
	std::string to_upper(std::string str);

	std::string trim(std::string str);
	std::string trimright(std::string str);
	std::string trimleft(std::string str);

	bool isemptyorwhitespace(CString str);
	bool isemptyorwhitespace(std::string str);

	mfcstringvector      doublenullterm_to_list(const char* sz2buffer);
	int                  doublenullterm_to_kvmap(const char* pipesepkvbuffer, mfcstrstrmap & kvpairs);

	mfcstringvector      delimited_to_list(const char* szbuffer, char delim);

	std::wstring widen(std::string text);

	CString MakeVarName(CString text);

#pragma warning(push)
#pragma warning(disable: 4521) // const and non const copy stors warning is informational

	class quoted
	{
	public:
		quoted(std::string & str) : m_str(str) {}
		quoted(const std::string & str) : m_conststr(str), m_str(m_conststr) {}
		quoted(quoted & rhs) : m_str(rhs.m_str) {}
		quoted(const quoted & rhs) : m_conststr(m_str), m_str(rhs.m_str) {}
		operator std::string &() { return m_str; }
		operator const std::string&() const { return m_str; }
	private:
		std::string & m_str;
		std::string m_conststr;
	};

	std::ostream & operator << (std::ostream & os, const quoted & q);
	std::istream & operator >> (std::istream & is, quoted & q);
	std::istream & operator >> (std::istream & is, quoted q);

	class braced
	{
	public:
		braced(std::string & str) : m_str(str) {}
		braced(const std::string & str) : m_conststr(str), m_str(m_conststr) {}
		operator std::string &() { return m_str; }
		operator const std::string&() const { return m_str; }
	private:
		std::string & m_str;
		std::string m_conststr;
	};

	std::ostream & operator << (std::ostream & os, const braced & b);
	std::istream & operator >> (std::istream & is, braced & b);

	class backquoted
	{
	public:
		backquoted(std::string & str) : m_str(str) {}
		backquoted(const std::string & str) : m_conststr(str), m_str(m_conststr) {}
		backquoted(backquoted & rhs) : m_str(rhs.m_str) {}
		operator std::string &() { return m_str; }
		operator const std::string&() const { return m_str; }
	private:
		std::string & m_str;
		std::string m_conststr;
	};

	std::ostream & operator << (std::ostream & os, const backquoted & b);
	std::istream & operator >> (std::istream & is, backquoted & b);
	std::istream & operator >> (std::istream & is, backquoted b);

#pragma warning(pop)
}


namespace Strings // template implementations
{
	template <typename T>
	CString join(const std::vector<T> & list, const char* separator, std::function<CString(const T& obj)> getstrfunc)
	{
		CString cs;
		for (size_t st = 0; st < list.size(); ++st) {
			if (st > 0) cs.Append(separator);
			cs.Append(getstrfunc(list[st]));
		}
		return cs;
	}

	template <typename T>
	std::string join(const std::vector<T> & list, const char* separator, std::function<std::string(const T& obj)> getstrfunc)
	{
		CString cs;
		for (size_t st = 0; st < list.size(); ++st) {
			if (st > 0) cs.Append(separator);
			cs.Append(getstrfunc(list[st]));
		}
		return std::string((LPCSTR)cs);
	}
}
