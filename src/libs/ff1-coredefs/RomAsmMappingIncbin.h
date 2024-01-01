#pragma once
#include "RomAsmMapping.h"

class CFFHacksterProject;
class RomAsmMappingFactory;

class RomAsmMappingIncbin : public RomAsmMapping
{
public:
	RomAsmMappingIncbin(CFFHacksterProject& project, std::string mapping);
	virtual ~RomAsmMappingIncbin();

	virtual void Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;
	virtual void Save(std::istream& sourcefile, std::ostream& destfile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;

protected:
	const std::string maptype = "incbin";
	std::string projectfolder;
	std::string label;
	std::string sublabel;
	int instoffset = -1;
};

