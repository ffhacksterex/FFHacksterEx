#pragma once

#include <memory>
#include <vector>
class RomAsmMapping;
class CFFHacksterProject;

class RomAsmMappingFactory
{
public:
	RomAsmMappingFactory();
	~RomAsmMappingFactory();
	RomAsmMappingFactory(const RomAsmMappingFactory& rhs) = delete;
	RomAsmMappingFactory& operator=(const RomAsmMappingFactory& rhs) = delete;

	std::vector<std::unique_ptr<RomAsmMapping>> ReadMappings(CFFHacksterProject & project, std::string key);

protected:
	std::unique_ptr<RomAsmMapping> Deserialize(CFFHacksterProject& project, std::string mappingDefinition);
	std::string Serialize(CFFHacksterProject& project, const RomAsmMapping & mapping);
};
