#pragma once

#include <afx.h>
#include "collection_helpers.h"

namespace Ini
{

	// ### Typed read and write functions

	int ReadDec(CString inipath, CString section, int defvalue);
	int ReadDec(CString inipath, CString section);
	void WriteDec(CString inipath, CString section, int value);
	void AddDec(CString inipath, CString section, int value, CString type);

	int ReadAddr(CString inipath, CString section, int defvalue);
	int ReadAddr(CString inipath, CString section);
	void WriteAddr(CString inipath, CString section, int value);
	void AddAddr(CString inipath, CString section, int value, CString type);

	int ReadHex(CString inipath, CString section, int defvalue);
	int ReadHex(CString inipath, CString section);
	void WriteHex(CString inipath, CString section, int value);
	void AddHex(CString inipath, CString section, int value, CString type);

	int ReadRgb(CString inipath, CString section, int defvalue);
	int ReadRgb(CString inipath, CString section);
	void AddRgb(CString inipath, CString section, int value, CString type);


	// ### Ini read

	CString         ReadIni(CString ainifile, CString ainisection, CString ainikey, CString adefault);
	bytevector      ReadIni(CString ainifile, CString ainisection, CString ainikey, const bytevector & defvals);
	boolvector      ReadIni(CString ainifile, CString ainisection, CString ainikey, const boolvector & defvals);
	mfcstringvector ReadIni(CString ainifile, CString ainisection, CString ainikey, const mfcstringvector & defvals);
	stdstringvector ReadIni(CString ainifile, CString ainisection, CString ainikey, const stdstringvector & defvals);
	bool            ReadIniBool(CString ainifile, CString ainisection, CString ainikey, bool defval);

	bool            ReadIniBuffered(CString ainifile, CString ainisection, CString ainikey, CString & refstr, int length);


	// ### Ini writes

	void        WriteIni(CString ainifile, CString ainisection, CString ainikey, CString aoutvalue);
	void        WriteIni(CString ainifile, CString ainisection, CString ainikey, const bytevector & outvalues);
	void        WriteIni(CString ainifile, CString ainisection, CString ainikey, const boolvector & outvalues);
	void        WriteIni(CString ainifile, CString ainisection, CString ainikey, const mfcstringvector & outvalues);
	void        WriteIni(CString ainifile, CString ainisection, CString ainikey, const stdstringvector & outvalues);
	void        WriteIniBool(CString ainifile, CString ainisection, CString ainikey, bool outvalue);

	bool        WriteGroupedValue(CString inifile, CString section, CString value, CString group);

	bool        WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, CString aoutvalue);
	bool        WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, const bytevector & outvalues);
	bool        WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, const boolvector & outvalues);
	bool        WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, const mfcstringvector & outvalues);
	bool        WriteIniIfMissing(CString ainifile, CString ainisection, CString ainikey, const stdstringvector & outvalues);
	bool        WriteIniBoolIfMissing(CString ainifile, CString ainisection, CString ainikey, bool aoutvalue);

	// ### Utilities

	CString TrimIniValue(CString value);

	bool ReadIniRefonly(CString ainifile, CString ainisection);
	void WriteIniRefonly(CString ainifile, CString ainisection, bool value);
	bool ReadIniReserved(CString ainifile, CString ainisection);
	void WriteIniReserved(CString ainifile, CString ainisection, bool value);

	int ReadIniVersion(CString ainifile);
	bool WriteIniVersion(CString ainifile, int newversion);

	std::vector<CString> ReadIniSectionNames(CString ainifile);
	std::vector<CString> ReadIniKeyNames(CString ainifile, CString asection);
	std::vector<CString> ReadIniKeyValues(CString ainifile, CString asection);

	bool        HasIniSection(CString inifile, CString section);
	bool        HasIni(CString ainifile, CString ainisection, CString ainikey);

	bool        EnsureIni(CString inifile, CString section, CString key, CString newvalue);
	bool        EnsureIni(CString inifile, CString section, CString key, CString newvalue, CString group);

	int         CopyIniSections(CString sourceini, CString destini);
	int         CopyIniSections(CString sourceini, CString destini, mfcstringvector sections);
	bool        CopyIniSection(CString sourceini, CString destini, CString section);

	int         MoveIniSections(CString sourceini, CString destini, mfcstringvector sections);
	bool        MoveIniSection(CString sourceini, CString destini, CString section);
	void        MoveValue(CString inipath, CString oldsection, CString key, CString newsection);

	void        RemoveSection(CString inifile, CString section);
	void        RemoveValue(CString inifile, CString section, CString key);
	bool        RenameSection(CString inifile, CString oldsectionname, CString newname);

	bool        RenameValue(CString inifile, CString section, CString oldname, CString newname);
	void        UpdateIfValue(CString inifile, CString section, CString key, CString expectedvalue, CString newvalue);

	void        DeleteSection(CString inifile, CString section);

	bool GenerateLabels(CString inifile, CString section, size_t count, CString prefix);
	bool CopyLabels(CString inifile, CString section, const CString * labels, size_t count, CString prefix = "");

} // end namespace Ini
