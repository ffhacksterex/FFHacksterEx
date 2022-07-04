#pragma once

#include <fstream>
#include <regex>
#include <std_assembly.h>

namespace disch_functions
{
	namespace dialogue
	{
		std::string read_dialogue_data(const char* asmfile, const char* dialoguepath, const char* szmappinglabel, const char* varmaptext);
		void write_dialogue_data(const char* asmfile, const char* dialoguepath, const char* varmaptext, const char* inputtext);
	}
}