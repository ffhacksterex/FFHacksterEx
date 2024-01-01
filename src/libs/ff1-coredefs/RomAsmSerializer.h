#pragma once

#include "RomAsmMapping_t.h"
class CFFHacksterProject;

class RomAsmSerializer
{
public:
	RomAsmSerializer(CFFHacksterProject & project);
	~RomAsmSerializer();

	void Load(RomAsmGropuedMappingRefs& groupedmappings, std::vector<unsigned char>& rom,
		const std::map<std::string, std::string>* poptions = nullptr);
	void Save(RomAsmGropuedMappingRefs& groupedmappings, const std::vector<unsigned char>& rom,
		const std::map<std::string, std::string>* poptions = nullptr);

protected:
	CFFHacksterProject& m_project;
};
