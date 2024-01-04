#pragma once

#include "RomAsmMapping.h"

// This class is as interface, used by the RomAsmSerializer during processing.
class IRomAsmSequentialMapping
{
public:
	// A mapping of relative live file paths to relative temp file paths.
	//   Descendants specify the relative file path of a live file and map it to a temp file
	// that the serializer will use during a save operation.
	//   The serializer writes to the all temp files, and if no errors occur will
	//   use these mappings to update the live files accordingly,
	//   then remove the temp files.
	virtual std::map<std::string, std::string> GetSaveFileMappings() const = 0;
};
