#pragma once

//TODO - if not using this file soon, then remove it

#include "RomAsmMapping_t.h"
#include <exception>
#include <vector>

struct sRomAsmMappingFailure
{
	std::string editor;
	std::string name;
	std::string reason;
};

class RomAsmFailedMappingException : std::exception
{
public:
	RomAsmFailedMappingException();
	RomAsmFailedMappingException(RomAsmMappingRefs::iterator ibegin, RomAsmMappingRefs::iterator iend);
	virtual ~RomAsmFailedMappingException();

	virtual const char* what() const;

private:
	std::vector<sRomAsmMappingFailure> m_failures;
};
