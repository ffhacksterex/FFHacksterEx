#include "stdafx.h"
#include "RomAsmSerializer.h"
#include "FFHacksterProject.h"
#include "path_functions.h"
#include "string_functions.h"

#include <fstream>
#include <stdexcept>
#include <string>

RomAsmSerializer::RomAsmSerializer(CFFHacksterProject& project)
	: m_project(project)
{
}

RomAsmSerializer::~RomAsmSerializer()
{
}

namespace {
	const std::map<std::string, std::string> defaultOptions;
}

// Load data from the source files into the specified ROM buffer.
void RomAsmSerializer::Load(RomAsmGropuedMappingRefs& groupedmappings, std::vector<unsigned char>& rom,
	const std::map<std::string, std::string>* poptions)
{
	if (groupedmappings.empty())
		throw std::runtime_error("Mappings cannot be empty when loading.");

	std::vector<std::string> errors;
	auto options = poptions != nullptr ? *poptions : defaultOptions;
	for (auto& g : groupedmappings) {
		try {
			std::string sourcefile = (LPCSTR)Paths::Combine({ m_project.ProjectFolder, g.first.c_str()});
			std::ifstream ifs(sourcefile);
			for (auto& m : g.second) {
				m->Load(ifs, rom, options);
			}
		}
		catch (std::exception& ex) {
			errors.push_back(ex.what());
		}
	}

	if (!errors.empty())
		throw std::runtime_error("Load encountered " + std::to_string(errors.size()) + " error(s):\n" + Strings::join(errors, "\n"));
}

// Save data from the specified ROM buffer to the source files.
void RomAsmSerializer::Save(RomAsmGropuedMappingRefs& groupedmappings, const std::vector<unsigned char>& rom,
	const std::map<std::string, std::string>* poptions)
{
	if (groupedmappings.empty())
		throw std::runtime_error("Mappings cannot be empty when saving.");

	throw std::runtime_error(__FUNCTION__ " is not implemented yet");
}