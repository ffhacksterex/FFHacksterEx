#pragma once

#include <iosfwd>
#include <deque>
#include <map>
#include <vector>
#include <regex>

namespace RomAsm
{
	class CondValues
	{
	public:
		std::string asmtrue;
		std::string asmfalse;
		std::string romtrue;
		std::string romfalse;

		std::string ToString();
		bool FromString(std::string text); // unescaped pipe-separated text, e.g. asmtrue|asmfalse|romtrue|romfalse
		void Import(std::string text);     //throws on error
		void Clear();
	};

	namespace Traverse
	{
		void SeekToInstruction(std::istream& is, std::string nappingname, std::string label, std::string sublabel, int instoffset);
		void SeekToInstruction(std::istream& is, std::string nappingname, std::string label, int instoffset);
		void SeekToInstruction(std::istream& is, int instoffset, std::string contextname);
	}

	namespace Modify
	{
		bool SyncDestToCurrent(std::istream& source, std::ostream& dest, std::streampos sourcestart);
		bool SyncDestToEnd(std::istream& source, std::ostream& dest);

		std::string SaveToBinFile(std::string binpath, int offset, int count, const std::vector<unsigned char>& rom);
	}

	namespace Query
	{
		bool IsWhitespaceLine(std::string line); //TODO - REVIEW
		bool IsBlankOrWhitespaceLine(std::string line);
		bool IsComment(const std::string& line);
		bool IsApparentDataRow(const std::string& line);
	}

	namespace Options
	{
		bool On(const std::map<std::string, std::string>& options, std::string name);
	}

	namespace Parse
	{
		int RomToInt(const std::vector<unsigned char>& rom, int offset, int recwidth, std::string line);
		std::string IntToAsm(int value, std::string format);

		int DataDirectiveToByteWidth(std::string directive);
		int ExtractDataRowRecWidth(std::string& line);

		std::string ExtractLineContent(std::string entry, bool trimWhitespace);
	}

	namespace Dialogue
	{
		int GetByteCountByElementType(std::string type);
	}

	//TODO - add to Query namespace
	//--
	bool IsStdOrLocalLabel(const std::string& line); //TODO - move into Query
	//--

	// GENERAL PURPOSE ROUTINES
	// These are also used to implemnt some of the routines above.

	//TODO - add to Mappings namespace
	//--
	CString ReadIniText(CString mapini, CString valini, CString section, CString key, CString defaultValue = "");
	int ReadIniInt(CString mapini, CString valini, CString section, CString key, CString defaultValue = "");
	std::string ReadSource(CString mapini, CString valini, CString section, CString key);
	//--

	std::deque<std::string> BuildLabelQueue(std::string label, std::string sublabel);

	//TODO - add to Traverse namespace
	//--
	bool LabelSearch(std::istream& is, std::string label);
	bool InstructionSearch(std::istream& is, int count);
	//--

	std::string ExtractIncbinPath(std::string mappingName, std::string thepath);
	std::string LoadFromBinFile(std::string binpath, int offset, int count, std::vector<unsigned char>& rom);

	//TODO - add to Query namespace
	//--
	bool LineMatchesRegex(const std::string line, const std::regex& rx);
	bool LineContainsRegex(const std::string line, const std::regex& rx);
	//--
}
