#pragma once
#include "RomAsmMapping.h"
#include "RomAsmSequentialMappingBase.h"

class CFFHacksterProject;
class RomAsmMappingFactory;

class RomAsmMappingBinary : public RomAsmSequentialMappingBase
{
public:
	RomAsmMappingBinary(CFFHacksterProject& project, std::string mapping);
	virtual ~RomAsmMappingBinary();

	virtual void Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;
	virtual void Save(std::istream& sourcefile, std::ostream& destfile, const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;

protected:
	const std::string maptype = "binary";
	std::string projectfolder;
	std::string label;
	std::string sublabel;
	int instoffset = -1;
	int recwidth = -1;
	std::string format = "$%02X"; //TODO - remove defualt and force mapping to specify the format instead?
	int groupsize = 0;            // 0 means no grouping, number of items per group (e.g. 4 yields "1,1,1,1,  1,1,1,1")
	int groupspacecount = 0;      // 0 means no grouping, number of space characters (e.g. 8 yields "1,1,1,1,        1,1,1,1")

	std::string FormatAsmSourceLine(std::string line, const std::vector<unsigned char>& rom, int& offset);
};
