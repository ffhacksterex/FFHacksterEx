#pragma once

#include <iosfwd>
#include <deque>
#include <vector>
#include <regex>

namespace RomAsm
{
	CString ReadIniText(CString mapini, CString valini, CString section, CString key);
	int ReadIniInt(CString mapini, CString valini, CString section, CString key);
	std::string ReadSource(CString mapini, CString valini, CString section, CString key);

	std::deque<std::string> BuildLabelQueue(std::string label, std::string sublabel);
	bool LabelSearch(std::istream& is, std::string label);
	bool InstructionSearch(std::istream& is, int count);

	std::string ExtractIncbinPath(std::string mappingName, std::string thepath);
	std::string LoadFromBinFile(std::string binpath, int offset, int count, std::vector<unsigned char>& rom);

	bool LineMatchesRegex(const std::string line, const std::regex& rx);
	bool IsStdOrLocalLabel(const std::string& line);
	bool IsComment(const std::string& line);
}
