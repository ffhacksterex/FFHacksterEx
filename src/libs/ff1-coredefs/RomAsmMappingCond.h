#pragma once
#include "RomAsmMapping.h"
#include <vector>
class CFFHacksterProject;

class RomAsmMappingCond : public RomAsmMapping
{
public:
	RomAsmMappingCond(CFFHacksterProject& project, std::string mapping);
	virtual ~RomAsmMappingCond();

	bool Enabled(const std::vector<unsigned char>& rom) const;

protected:
	const std::string maptype = "binary";
	std::string projectfolder;
	std::string label;
	std::string sublabel;
	int instoffset = -1;
	std::vector<unsigned char> romtrue;    // bytes to write if feature is on
	std::vector<unsigned char> romfalse;   // bytes to write if feature is off
	std::vector<std::string> asmtrue;      // asm source to write if feature is on
	std::vector<std::string> asmfalse;     // asm source to write if feature is off

	virtual void Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;
	virtual void Save(std::istream& sourcefile, std::ostream& destfile, const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;
};
