#pragma once

#include "RomAsmMapping.h"
#include "IRomAsmSequentialMapping.h"

class RomAsmSequentialMappingBase : public RomAsmMapping, public IRomAsmSequentialMapping
{
	// By default, sequential mappings don't use any additional files.
	virtual std::map<std::string, std::string> GetSaveFileMappings() const override
	{
		return std::map<std::string, std::string>();
	}
};
