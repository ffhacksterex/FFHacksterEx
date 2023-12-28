#pragma once

class RomAsmMapping;
class RomAsmMappingFactory;
#include <memory>
#include <vector>
#include <map>

class RomAsmSerializer
{
public:
	RomAsmSerializer(RomAsmMappingFactory& factory, std::string projectfolder, std::vector<unsigned char> & rom);
	~RomAsmSerializer();

	std::string Load(std::string projetype, const std::vector<std::unique_ptr<RomAsmMapping>> & mappings,
		const std::map<std::string, std::string>& options, std::vector<unsigned char> & rom);
	std::string Save(std::string projetype, const std::vector<std::unique_ptr<RomAsmMapping>>& mappings,
		const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom);

private:
	RomAsmMappingFactory& m_factory;
	std::string m_projectfolder;
	std::vector<unsigned char>& m_rom;
};
