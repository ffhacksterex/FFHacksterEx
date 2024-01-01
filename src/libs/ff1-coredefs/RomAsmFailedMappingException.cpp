#include "stdafx.h"
#include "RomAsmFailedMappingException.h"

//TODO - if not using this file soon, then remove it

RomAsmFailedMappingException::RomAsmFailedMappingException()
{
}

RomAsmFailedMappingException::RomAsmFailedMappingException(RomAsmMappingRefs::iterator ibegin, RomAsmMappingRefs::iterator iend)
{
}

RomAsmFailedMappingException::~RomAsmFailedMappingException()
{
}

const char* RomAsmFailedMappingException::what() const
{
    return nullptr;
}