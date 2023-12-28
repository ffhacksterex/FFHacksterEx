#pragma once
#include "RomAsmMapping.h"

class RomAsmMappingDatFile : public RomAsmMapping
{
public:
	RomAsmMappingDatFile();
	virtual ~RomAsmMappingDatFile();

	std::string projectfolder;

	virtual std::string Load(const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom) override;
	virtual std::string Save(const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom) override;
};
