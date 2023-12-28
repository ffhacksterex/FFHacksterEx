#pragma once
#include "RomAsmMapping.h"

class RomAsmMappingBinary : public RomAsmMapping
{
public:
	RomAsmMappingBinary();
	virtual ~RomAsmMappingBinary();

	std::string projectfolder;
	std::string label;
	std::string sublabel;
	int instoffset = -1;
	int recwidth = -1;
	std::string format = "$%02X"; //TODO - remove defualt and force mapping to specify the format instead?

	virtual std::string Load(const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom) override;
	virtual std::string Save(const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom) override;
};
