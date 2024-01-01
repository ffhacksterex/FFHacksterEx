#pragma once

class CFFHacksterProject;
#include "RomAsmMapping_t.h"

class RomAsmMappingFactory
{
public:
	RomAsmMappingFactory();
	~RomAsmMappingFactory();

	RomAsmMappingRefs ReadMappings(CFFHacksterProject& project);
	RomAsmMappingRefs ReadMappings(CFFHacksterProject& project, std::string editor);
	RomAsmGropuedMappingRefs ReadGroupedMappings(CFFHacksterProject& project, std::string editor);
};

