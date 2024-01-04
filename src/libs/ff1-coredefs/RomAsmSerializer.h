#pragma once

#include "RomAsmMapping_t.h"
class CFFHacksterProject;

using RomAsmSerialzerOptions = std::map<std::string, std::string>;

class RomAsmSerializer
{
public:
	RomAsmSerializer(CFFHacksterProject & project);
	virtual ~RomAsmSerializer();

	static RomAsmSerialzerOptions DefaultOptions();

	virtual void Load(RomAsmGropuedMappingRefs& groupedmappings, std::vector<unsigned char>& rom,
		const RomAsmSerialzerOptions* poptions = nullptr);
	virtual void Save(RomAsmGropuedMappingRefs& groupedmappings, const std::vector<unsigned char>& rom,
		const RomAsmSerialzerOptions* poptions = nullptr);

protected:
	CFFHacksterProject& m_project;

	using SaveFileMapping = std::map<std::string, std::string>;

	virtual void PostSave(const RomAsmSerialzerOptions & options, SaveFileMapping & mappings,
		std::vector<std::string> & errors);
};
