#pragma once
#include "RomAsmMapping.h"

class RomAsmMappingIncbin : public RomAsmMapping
{
public:
	RomAsmMappingIncbin();
	virtual ~RomAsmMappingIncbin();

	std::string projectfolder;
	std::string label;
	std::string sublabel;
	int instoffset = -1;

	virtual std::string Load(const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom) override;
	virtual std::string Save(const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom) override;
};

