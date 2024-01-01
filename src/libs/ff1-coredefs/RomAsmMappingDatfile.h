#pragma once
#include "RomAsmMapping.h"

class CFFHacksterProject;
class RomAsmMappingFactory;

// Moves data between a data file (typically representing an entire bank) and working ROM.
class RomAsmMappingDatfile : public RomAsmMapping
{
public:
	RomAsmMappingDatfile(CFFHacksterProject & project, std::string mapping);
	virtual ~RomAsmMappingDatfile();

	virtual void Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;
	virtual void Save(std::istream& sourcefile, std::ostream& destfile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;

private:
	std::string m_destfile;
};
