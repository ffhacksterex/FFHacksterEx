#pragma once
#include "RomAsmMapping.h"
#include "RomAsmSequentialMappingBase.h"

class CFFHacksterProject;
class RomAsmMappingFactory;

class RomAsmMappingImmed : public RomAsmSequentialMappingBase
{
public:
	RomAsmMappingImmed(CFFHacksterProject& project, std::string mapping);
	virtual ~RomAsmMappingImmed();

	virtual void Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;
	virtual void Save(std::istream& sourcefile, std::ostream& destfile, const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;

protected:
	const std::string maptype = "immed";
	std::string projectfolder;
	std::string label;
	std::string sublabel;
	int instoffset = -1;
	std::string format = "$%02X"; //TODO - remove defualt and force mapping to specify the format instead?
};
