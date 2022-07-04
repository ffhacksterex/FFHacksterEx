#pragma once

#include <fstream>
#include <regex>
#include <std_assembly.h>

namespace disch_functions
{
	// Specific to the Annotated Disch Assembly
	namespace data {
		int read_assembly_data(const char* asmfile, const char* varmaptext, const char* inputtext, unsigned char * rom, size_t romlength);
		int write_assembly_data(const char* asmfile, const char* varmaptext, const char* inputtext, const unsigned char * rom, size_t romlength);
	}
}
