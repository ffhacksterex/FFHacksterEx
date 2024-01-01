#pragma once

#include "RomAsmMapping.h"
#include <vector>
#include <map>
#include <memory>

using RomAsmMappingRef = std::shared_ptr<RomAsmMapping>;

using RomAsmMappingRefs = std::vector<RomAsmMappingRef>;

// Map in which a source key is mapped to a list of RomAsmMappingRefs
using RomAsmGropuedMappingRefs = std::map<std::string, RomAsmMappingRefs>;
