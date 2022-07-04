#include "stdafx.h"
#include "ini_functions.h"
#include "general_functions.h"
#include "string_functions.h"
#include "type_support.h"
#include <functional>

using namespace Strings;
using namespace Types;

namespace Ini
{
	namespace
	{
		const size_t INI_LENGTH = 16384;


		std::string bad_type_string(CString type, CString expected)
		{
			CString cs;
			cs.Format("Read type '%s' instead of the expected '%s'", (LPCSTR)type, (LPCSTR)expected);
			return std::string((LPCSTR)cs);
		}

		void throw_no_ini_type_exception(CString functionname, CString section, CString type)
		{
			CString fmt;
			fmt.Format("Function '%s' cannot process key '%s' with type '%s'.", (LPCSTR)functionname, (LPCSTR)section, (LPCSTR)type);
			throw std::invalid_argument(fmt);
		}

		void throw_no_ini_default_exception(CString functionname, CString section, CString type)
		{
			CString fmt;
			fmt.Format("Function '%s' could not provide a default value for key '%s' with type '%s'.", (LPCSTR)functionname, (LPCSTR)section, (LPCSTR)type);
			throw std::invalid_argument(fmt);
		}

		int read_dec(CString inipath, CString section, int * defvalue)
		{
			auto alltypes = Types::GetDecConvertibleTypes();

			auto type = ReadIni(inipath, section, "type", CString());
			if (!has(alltypes, type))
				throw_no_ini_type_exception(__FUNCTION__, section, type);

			auto inival = ReadIni(inipath, section, "value", CString());
			if (inival.IsEmpty()) {
				if (defvalue != nullptr) return *defvalue;
				throw_no_ini_default_exception(__FUNCTION__, section, type);
			}

			return dec(inival);
		}

		int read_addr(CString inipath, CString section, int * defvalue)
		{
			auto alltypes = Types::GetAddrConvertibleTypes();

			auto type = ReadIni(inipath, section, "type", CString());
			if (!has(alltypes, type))
				throw_no_ini_type_exception(__FUNCTION__, section, type);

			auto inival = ReadIni(inipath, section, "value", CString());
			if (inival.IsEmpty()) {
				if (defvalue != nullptr) return *defvalue;
				throw_no_ini_default_exception(__FUNCTION__, section, type);
			}

			return addr(inival);
		}

		int read_hex(CString inipath, CString section, int * defvalue)
		{
			auto alltypes = Types::GetHexConvertibleTypes();

			auto type = ReadIni(inipath, section, "type", CString());
			if (!has(alltypes, type))
				throw_no_ini_type_exception(__FUNCTION__, section, type);

			auto inival = ReadIni(inipath, section, "value", CString());
			if (inival.IsEmpty()) {
				if (defvalue != nullptr) return *defvalue;
				throw_no_ini_default_exception(__FUNCTION__, section, type);
			}

			return hex(inival);
		}

		int read_rgb(CString inipath, CString section, int * defvalue)
		{
			auto type = ReadIni(inipath, section, "type", CString());
			if (type != "rgb")
				throw_no_ini_type_exception(__FUNCTION__, section, type);

			auto inival = ReadIni(inipath, section, "value", CString());
			if (inival.IsEmpty()) {
				if (defvalue != nullptr) return *defvalue;
				throw_no_ini_default_exception(__FUNCTION__, section, type);
			}

			return hex(inival);
		}

	} // end namespace unnamed (local helpers)



	// ### Typed read functions

	int ReadDec(CString inipath, CString section, int defvalue)
	{
		return read_dec(inipath, section, &defvalue);
	}

	int ReadDec(CString inipath, CString section)
	{
		return read_dec(inipath, section, nullptr);
	}

	void WriteDec(CString inipath, CString section, int value)
	{
		auto type = ReadIni(inipath, section, "type", CString());
		if (type.IsEmpty())
			throw_no_ini_type_exception(__FUNCTION__, section, type);

		auto text = dec(value);
		WriteIni(inipath, section, "value", text);
	}

	void AddDec(CString inipath, CString section, int value, CString type)
	{
		auto types = Types::GetDecConvertibleTypes();
		if (types.find(type) == cend(types))
			THROWEXCEPTION(std::invalid_argument, std::string((LPCSTR)type) + " is not a valid dec type, can't add value " + std::string((LPCSTR)section));

		auto text = dec(value);
		WriteIni(inipath, section, "value", text);
		WriteIni(inipath, section, "type", type);
	}


	int ReadAddr(CString inipath, CString section, int defvalue)
	{
		return read_addr(inipath, section, &defvalue);
	}

	int ReadAddr(CString inipath, CString section)
	{
		return read_addr(inipath, section, nullptr);
	}

	void WriteAddr(CString inipath, CString section, int value)
	{
		auto type = ReadIni(inipath, section, "type", CString());
		if (type.IsEmpty())
			throw_no_ini_type_exception(__FUNCTION__, section, type);

		auto text = addr(value);
		WriteIni(inipath, section, "value", text);
	}

	void AddAddr(CString inipath, CString section, int value, CString type)
	{
		auto types = Types::GetAddrConvertibleTypes();
		if (types.find(type) == cend(types))
			THROWEXCEPTION(std::invalid_argument, std::string((LPCSTR)type) + " is not a valid addr type, can't add value " + std::string((LPCSTR)section));

		auto text = addr(value);
		WriteIni(inipath, section, "value", text);
		WriteIni(inipath, section, "type", type);
	}


	int ReadHex(CString inipath, CString section, int defvalue)
	{
		return read_hex(inipath, section, &defvalue);
	}

	int ReadHex(CString inipath, CString section)
	{
		return read_hex(inipath, section, nullptr);
	}

	void WriteHex(CString inipath, CString section, int value)
	{
		auto type = ReadIni(inipath, section, "type", CString());
		if (type.IsEmpty())
			throw_no_ini_type_exception(__FUNCTION__, section, type);

		auto text = hex(value);
		WriteIni(inipath, section, "value", text);
	}

	void AddHex(CString inipath, CString section, int value, CString type)
	{
		auto types = Types::GetHexConvertibleTypes();
		if (types.find(type) == cend(types))
			THROWEXCEPTION(std::invalid_argument, std::string((LPCSTR)type) + " is not a valid hex type, can't add value " + std::string((LPCSTR)section));

		auto text = addr(value);
		WriteIni(inipath, section, "value", text);
		WriteIni(inipath, section, "type", type);
	}


	int ReadRgb(CString inipath, CString section, int defvalue)
	{
		return read_rgb(inipath, section, &defvalue);
	}

	int ReadRgb(CString inipath, CString section)
	{
		return read_rgb(inipath, section, nullptr);
	}

	void AddRgb(CString inipath, CString section, int value, CString type)
	{
		auto types = Types::GetRgbConvertibleTypes();
		if (types.find(type) == cend(types))
			THROWEXCEPTION(std::invalid_argument, std::string((LPCSTR)type) + " is not a valid rgb type, can't add value " + std::string((LPCSTR)section));

		auto text = addr(value);
		WriteIni(inipath, section, "value", text);
		WriteIni(inipath, section, "type", type);
	}




	// ### Ini reads

	template <int BUFLENGTH>
	CString ReadIniLarge(CString ainifile, CString ainisection, CString ainikey, CString adefault)
	{
		char buffer[BUFLENGTH + 1] = { 0 };
		GetPrivateProfileString(ainisection, ainikey, adefault, buffer, BUFLENGTH, ainifile);
		return CString(buffer);
	}

	CString ReadIni(CString ainifile, CString ainisection, CString ainikey, CString adefault)
	{
		static const size_t length = 16384;
		char buffer[length + 1] = { 0 };
		GetPrivateProfileString(ainisection, ainikey, adefault, buffer, length, ainifile);
		return CString(buffer);
	}

	bytevector ReadIni(CString ainifile, CString ainisection, CString ainikey, const bytevector & defvals)
	{
		auto strvalue = ReadIniLarge<32768>(ainifile, ainisection, ainikey, CString());
		if (strvalue.IsEmpty()) return defvals;

		return bytevec(strvalue);
	}

	boolvector ReadIni(CString ainifile, CString ainisection, CString ainikey, const boolvector & defvals)
	{
		auto strvalue = ReadIniLarge<16384>(ainifile, ainisection, ainikey, CString());
		if (strvalue.IsEmpty()) return defvals;

		return boolvec(strvalue);
	}

	mfcstringvector ReadIni(CString ainifile, CString ainisection, CString ainikey, const mfcstringvector & defvals)
	{
		auto strvalue = ReadIniLarge<32768>(ainifile, ainisection, ainikey, CString());
		if (strvalue.IsEmpty()) return defvals;

		return mfcstrvec(strvalue);
	}

	stdstringvector ReadIni(CString ainifile, CString ainisection, CString ainikey, const stdstringvector & defvals)
	{
		auto strvalue = ReadIniLarge<32768>(ainifile, ainisection, ainikey, CString());
		if (strvalue.IsEmpty()) return defvals;

		return stdstrvec(strvalue);
	}

	bool ReadIniBool(CString ainifile, CString ainisection, CString ainikey, bool defval)
	{
		auto strvalue = ReadIni(ainifile, ainisection, ainikey, CString());
		if (strvalue.IsEmpty()) return defval;

		return boole(strvalue);
	}

	bool ReadIniBuffered(CString ainifile, CString ainisection, CString ainikey, CString & refstr, int length)
	{
		auto buffer = refstr.GetBufferSetLength(length);
		GetPrivateProfileString(ainisection, ainikey, nullptr, buffer, length, ainifile);
		refstr.ReleaseBuffer();
		return true;
	}



	// ### Ini Writes

	void WriteIni(CString ainifile, CString ainisection, CString ainikey, CString aoutvalue)
	{
		WritePrivateProfileString(ainisection, ainikey, aoutvalue, ainifile);
	}

	void WriteIni(CString ainifile, CString ainisection, CString ainikey, const bytevector & outvalues)
	{
		auto strvalue = bytevec(outvalues);
		WritePrivateProfileString(ainisection, ainikey, strvalue, ainifile);
	}

	void WriteIni(CString ainifile, CString ainisection, CString ainikey, const boolvector & outvalues)
	{
		auto strvalue = boolvec(outvalues);
		WritePrivateProfileString(ainisection, ainikey, strvalue, ainifile);
	}

	void WriteIni(CString ainifile, CString ainisection, CString ainikey, const mfcstringvector & outvalues)
	{
		auto strvalue = mfcstrvec(outvalues);
		WritePrivateProfileString(ainisection, ainikey, strvalue, ainifile);
	}

	void WriteIni(CString ainifile, CString ainisection, CString ainikey, const stdstringvector & outvalues)
	{
		auto strvalue = stdstrvec(outvalues);
		WritePrivateProfileString(ainisection, ainikey, strvalue, ainifile);
	}

	void WriteIniBool(CString ainifile, CString ainisection, CString ainikey, bool outvalue)
	{
		WriteIni(ainifile, ainisection, ainikey, boole(outvalue));
	}

	bool WriteGroupedValue(CString inifile, CString section, CString value, CString group)
	{
		WriteIni(inifile, section, "value", value);
		WriteIni(inifile, section, "group", group);
		return HasIni(inifile, section, "value");
	}



	bool WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, CString aoutvalue)
	{
		if (HasIni(ainifile, ainisection, ainikey)) return true;

		WriteIni(ainifile, ainisection, ainikey, aoutvalue);
		return ReadIni(ainifile, ainisection, ainikey, CString()) == aoutvalue;
	}

	bool WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, const bytevector & outvalues)
	{
		if (HasIni(ainifile, ainisection, ainikey)) return true;

		WriteIni(ainifile, ainisection, ainikey, outvalues);
		bytevector def;
		return ReadIni(ainifile, ainisection, ainikey, def) == outvalues;
	}

	bool WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, const boolvector & outvalues)
	{
		if (HasIni(ainifile, ainisection, ainikey)) return true;

		WriteIni(ainifile, ainisection, ainikey, outvalues);
		const boolvector def;

		//DEVNOTE - seems to be some issue between the globally-defined operator== for std::vector<T> and std::vector<bool>.
		// Try using std::equal_to as a workaround.
		const auto vec = ReadIni(ainifile, ainisection, ainikey, def);
		return std::equal_to<const boolvector>()(vec, def);
	}

	bool WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, const mfcstringvector & outvalues)
	{
		if (HasIni(ainifile, ainisection, ainikey)) return true;

		WriteIni(ainifile, ainisection, ainikey, outvalues);
		decltype(outvalues) def{};
		return ReadIni(ainifile, ainisection, ainikey, def) == outvalues;
	}

	bool WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, const stdstringvector & outvalues)
	{
		if (HasIni(ainifile, ainisection, ainikey)) return true;

		WriteIni(ainifile, ainisection, ainikey, outvalues);
		decltype(outvalues) def{};
		return ReadIni(ainifile, ainisection, ainikey, def) == outvalues;
	}

	bool WriteIniBoolIfMissing(CString ainifile, CString ainisection, CString ainikey, bool aoutvalue)
	{
		if (HasIni(ainifile, ainisection, ainikey)) return true;

		WriteIniBool(ainifile, ainisection, ainikey, aoutvalue);
		return ReadIniBool(ainifile, ainisection, ainikey, false) == aoutvalue;
	}



	// ### Ini utilities

	CString TrimIniValue(CString value)
	{
		auto comment = value.Find(';');
		if (comment != -1) {
			value.Delete(comment, value.GetLength());
			value.TrimRight();
		}
		return value;
	}

	bool ReadIniRefonly(CString ainifile, CString ainisection)
	{
		static const size_t length = 6;
		char buffer[length + 1] = { 0 };
		GetPrivateProfileString(ainisection, "refonly", "false", buffer, length, ainifile);
		return CString(buffer) == "true";
	}

	void WriteIniRefonly(CString ainifile, CString ainisection, bool value)
	{
		auto sval = value ? "true" : "false";
		WritePrivateProfileString(ainisection, "refonly", sval, ainifile);
	}

	bool ReadIniReserved(CString ainifile, CString ainisection)
	{
		static const size_t length = 6;
		char buffer[length + 1] = { 0 };
		GetPrivateProfileString(ainisection, "reserved", "false", buffer, length, ainifile);
		return CString(buffer) == "true";
	}

	void WriteIniReserved(CString ainifile, CString ainisection, bool value)
	{
		auto sval = value ? "true" : "false";
		WritePrivateProfileString(ainisection, "reserved", sval, ainifile);
	}

	int ReadIniVersion(CString ainifile)
	{
		int version = dec(ReadIni(ainifile, "VERSION", "value", "-1"));
		return version;
	}

	bool WriteIniVersion(CString ainifile, int newversion)
	{
		if (newversion == -1) throw std::invalid_argument(__FUNCTION__ " can't take -1 as a version number");

		WriteIni(ainifile, "VERSION", "type", "uint32");   // force the type to a large decimal int value
		WriteIni(ainifile, "VERSION", "group", "VERSION"); // force the group to VERSION
		WriteDec(ainifile, "VERSION", newversion);
		auto readversion = ReadDec(ainifile, "VERSION");
		return readversion == newversion;
	}

	std::vector<CString> ReadIniSectionNames(CString ainifile)
	{
		char sznames[16384] = { 0 };
		GetPrivateProfileSectionNames(sznames, _countof(sznames), ainifile);

		std::vector<CString> sections;
		const char* buffer = sznames;
		while (*buffer) {
			sections.push_back(buffer);
			buffer += strlen(buffer) + 1;
		}
		return sections;
	}

	std::vector<CString> ReadIniKeyNames(CString ainifile, CString asection)
	{
		char sznames[16384] = { 0 };
		GetPrivateProfileString(asection, nullptr, nullptr, sznames, _countof(sznames), ainifile);

		std::vector<CString> keys;
		const char* buffer = sznames;
		while (*buffer) {
			keys.push_back(buffer);
			buffer += strlen(buffer) + 1;
		}
		return keys;
	}

	std::vector<CString> ReadIniKeyValues(CString ainifile, CString asection)
	{
		std::vector<CString> names = ReadIniKeyNames(ainifile, asection);
		std::vector<CString> values;
		for (auto name : names) values.push_back(ReadIni(ainifile, asection, name, ""));
		return values;
	}

	bool HasIniSection(CString inifile, CString section)
	{
		static const size_t length = 16384;
		char buffer[length + 1] = { 0 };
		DWORD read = GetPrivateProfileSection(section, buffer, length, inifile);
		return (read > 0) && (*buffer != 0);
	}

	bool HasIni(CString ainifile, CString ainisection, CString ainikey)
	{
		static const size_t length = 16384;
		char buffer[length + 1] = { 0 };
		DWORD read = GetPrivateProfileString(ainisection, ainikey, nullptr, buffer, length, ainifile);
		return (read > 0) && (*buffer != 0);
	}

	bool EnsureIni(CString inifile, CString section, CString key, CString newvalue)
	{
		if (HasIni(inifile, section, key)) return true;

		WriteIni(inifile, section, key, newvalue);
		return HasIni(inifile, section, key);
	}

	bool EnsureIni(CString inifile, CString section, CString key, CString newvalue, CString group)
	{
		if (HasIni(inifile, section, key)) return true;

		return WriteGroupedValue(inifile, section, newvalue, group);
	}



	namespace // copy/move ini helpers
	{
		bool DoCopyIniSection(CString sourceini, CString destini, CString section, char* buffer, const int buflen)
		{
			buffer[0] = 0; // cheap 'clear'
			GetPrivateProfileSection(section, buffer, buflen, sourceini);
			bool result = (strlen(buffer) > 0 && WritePrivateProfileSection(section, buffer, destini));
			return result;
		}

		bool DoMoveIniSection(CString sourceini, CString destini, CString section, char* buffer, const int buflen)
		{
			auto result = DoCopyIniSection(sourceini, destini, section, buffer, buflen);
			if (result) {
				RemoveSection(sourceini, section);
				result = !HasIniSection(sourceini, section);
			}
			return result;
		}
	}

	int CopyIniSections(CString sourceini, CString destini)
	{
		return CopyIniSections(sourceini, destini, ReadIniSectionNames(sourceini));
	}

	int CopyIniSections(CString sourceini, CString destini, mfcstringvector sections)
	{
		int copied = 0;
		const int buflen = 65535;
		char buffer[buflen + 1] = { 0 };
		for (const auto & name : sections) {
			if (DoCopyIniSection(sourceini, destini, name, buffer, buflen))
				++copied;
		}
		return copied;
	}

	bool CopyIniSection(CString sourceini, CString destini, CString section)
	{
		const int buflen = 65535;
		char buffer[buflen + 1] = { 0 };
		return DoCopyIniSection(sourceini, destini, section, buffer, buflen);
	}

	int MoveIniSections(CString sourceini, CString destini, mfcstringvector sections)
	{
		mfcstringvector failedmove;

		int copied = 0;
		const int buflen = 65535;
		char buffer[buflen + 1] = { 0 };
		for (const auto & section : sections) {
			if (DoMoveIniSection(sourceini, destini, section, buffer, buflen))
				++copied;
			else
				failedmove.push_back(section);
		}

		if (!failedmove.empty()) {
			CString msg;
			msg.Format("The following sections were copied, not moved from the source:\n");
			for (auto str : failedmove) msg.Append(str + "\n");
			THROWEXCEPTION(std::runtime_error, (LPCSTR)msg);
		}

		return copied;
	}

	bool MoveIniSection(CString sourceini, CString destini, CString section)
	{
		const int buflen = 65535;
		char buffer[buflen + 1] = { 0 };
		return DoMoveIniSection(sourceini, destini, section, buffer, buflen);
	}

	void MoveValue(CString inipath, CString oldsection, CString key, CString newsection)
	{
		auto value = ReadIni(inipath, oldsection, key, "");
		RemoveValue(inipath, oldsection, key);
		WriteIni(inipath, newsection, key, value);
	}

	void RemoveSection(CString inifile, CString section)
	{
		WritePrivateProfileString(section, nullptr, nullptr, inifile);
	}

	void RemoveValue(CString inifile, CString section, CString key)
	{
		WritePrivateProfileString(section, key, nullptr, inifile);
	}

	bool RenameSection(CString inifile, CString oldsectionname, CString newname)
	{
		if (!HasIniSection(inifile, newname)) {
			// Fail if we can't copy the old section to the new
			const int buflen = 65535;
			char buffer[buflen + 1] = { 0 };
			GetPrivateProfileSection(oldsectionname, buffer, buflen, inifile);

			bool result = (strlen(buffer) > 0 && WritePrivateProfileSection(newname, buffer, inifile));
			if (!result) return result;

			// We wrote to the new name, remove the old section
			if (!HasIniSection(inifile, newname)) return false;
		}

		// At this point, the new section already existed or has been written, remove the old section.
		DeleteSection(inifile, oldsectionname);
		return true;
	}

	bool RenameValue(CString inifile, CString section, CString oldname, CString newname)
	{
		if (!HasIni(inifile, section, newname)) {
			// New value is missing; if both are missing, that's unexpected when this function is called, FAIL.
			if (!HasIni(inifile, section, oldname)) return false;

			auto oldvalue = ReadIni(inifile, section, oldname, "");
			WriteIni(inifile, section, newname, oldvalue);
		}

		// We should have the new value by now (either existing or written above), so remove the old one.
		RemoveValue(inifile, section, oldname);
		return true;
	}

	void UpdateIfValue(CString inifile, CString section, CString key, CString expectedvalue, CString newvalue)
	{
		auto patterntables = ReadIni(inifile, section, key, "");
		if (patterntables == expectedvalue)
			WriteIni(inifile, section, key, newvalue);
	}

	void DeleteSection(CString inifile, CString section)
	{
		WritePrivateProfileString(section, nullptr, nullptr, inifile);
	}



	bool GenerateLabels(CString inifile, CString section, size_t count, CString prefix)
	{
		ASSERT(!section.IsEmpty());
		ASSERT(count > 0);
		ASSERT(!prefix.IsEmpty());
		if (section.IsEmpty()) return false;
		if (count == 0) return false;
		if (prefix.IsEmpty()) return false;

		for (size_t co = 0; co < count; co++) {
			CString key, value;
			key.Format("%d", co);
			value.Format("%s: %02X", (LPCSTR)prefix, co);
			WriteIni(inifile, section, key, value);
		}
		return true;
	}

	bool CopyLabels(CString inifile, CString section, const CString * labels, size_t count, CString prefix)
	{
		ASSERT(!section.IsEmpty());
		ASSERT(labels != nullptr);
		ASSERT(count > 0);
		if (section.IsEmpty()) return false;
		if (labels == nullptr) return false;
		if (count == 0) return false;

		CString pre(prefix);
		if (!pre.IsEmpty()) pre += ": ";

		for (size_t co = 0; co < count; co++) {
			CString key, value;
			key.Format("%d", co);
			value = pre + labels[co];
			WriteIni(inifile, section, key, value);
		}
		return true;
	}

} // end namespace Ini