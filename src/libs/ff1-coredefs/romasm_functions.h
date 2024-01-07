#pragma once

#include <iosfwd>
#include <deque>
#include <map>
#include <vector>
#include <regex>

namespace RomAsm
{
	namespace Traverse
	{
		void SeekToInstruction(std::istream& is, std::string nappingname, std::string label, std::string sublabel, int instoffset);
		void SeekToInstruction(std::istream& is, std::string nappingname, std::string label, int instoffset);
	}

	namespace Modify
	{
		bool SyncDestToCurrent(std::istream& source, std::ostream& dest, std::streampos sourcestart);
		bool SyncDestToEnd(std::istream& source, std::ostream& dest);

		std::string SaveToBinFile(std::string binpath, int offset, int count, const std::vector<unsigned char>& rom);
	}

	namespace Query
	{
		bool IsWhitespaceLine(std::string line);
		bool IsComment(const std::string& line);
	}

	namespace Options
	{
		bool On(const std::map<std::string, std::string>& options, std::string name);
	}

	namespace Parse
	{
		int RomToInt(const std::vector<unsigned char>& rom, int offset, int recwidth, std::string line);
		std::string IntToAsm(int value, std::string format);
	}

	bool IsStdOrLocalLabel(const std::string& line); //TODO - move into Query

	// GENERAL PURPOSE ROUTINES
	// These are also used to implemnt some of the routines above.

	CString ReadIniText(CString mapini, CString valini, CString section, CString key, CString defaultValue = "");
	int ReadIniInt(CString mapini, CString valini, CString section, CString key, CString defaultValue = "");
	std::string ReadSource(CString mapini, CString valini, CString section, CString key);

	std::deque<std::string> BuildLabelQueue(std::string label, std::string sublabel);
	bool LabelSearch(std::istream& is, std::string label);
	bool InstructionSearch(std::istream& is, int count);

	std::string ExtractIncbinPath(std::string mappingName, std::string thepath);
	std::string LoadFromBinFile(std::string binpath, int offset, int count, std::vector<unsigned char>& rom);

	bool LineMatchesRegex(const std::string line, const std::regex& rx);
	bool LineContainsRegex(const std::string line, const std::regex& rx);
}
