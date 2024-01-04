#include "stdafx.h"
#include "RomAsmSerializer.h"
#include "FFHacksterProject.h"
#include "FilePathRemover.h"
#include "path_functions.h"
#include "IRomAsmDiscreteMapping.h"
#include "IRomAsmSequentialMapping.h"
#include "romasm_functions.h"
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

namespace // DECLARATIONS
{
	const RomAsmSerialzerOptions l_defaultOptions;

	void ClassifyMappings(std::string source, RomAsmMappingRefs& mappings,
		RomAsmMappingRefs& discretes, RomAsmMappingRefs& sequentials);

	void ProcessDiscretes(std::string source, const std::vector<unsigned char>& rom,
		const std::map<std::string, std::string>& options, RomAsmMappingRefs& discretes);

	void ProcessSequentials(std::string source, const std::vector<unsigned char>& rom,
		const std::map<std::string, std::string>& options, RomAsmMappingRefs& sequentials,
		std::string projectfolder, std::map<std::string, std::string>& mastersavefilemappings);

	void AddSaveFileMapping(std::map<std::string, std::string>& mappings, std::string liverelfile, std::string temprelfile);

	void AddSaveFileMappings(std::map<std::string, std::string>& mappings, std::string mappingname,
		const std::map<std::string, std::string>& childmappings);

	void CancelSaveFileMappings(std::string projectfolder, const std::map<std::string, std::string>& mappings,
		const RomAsmSerialzerOptions& options, std::vector<std::string>& errors);

	void ConfirmSaveFileMappings(std::string projectfolder, const std::map<std::string, std::string>& mappings,
		const RomAsmSerialzerOptions & options, std::vector<std::string>& errors);
}

RomAsmSerialzerOptions RomAsmSerializer::DefaultOptions()
{
	return l_defaultOptions;
}

// Load data from the source files into the specified ROM buffer.
void RomAsmSerializer::Load(RomAsmGropuedMappingRefs& groupedmappings, std::vector<unsigned char>& rom,
	const RomAsmSerialzerOptions* poptions)
{
	if (groupedmappings.empty())
		throw std::runtime_error("Mappings cannot be empty when loading.");

	std::vector<std::string> errors;
	auto options = poptions != nullptr ? *poptions : l_defaultOptions;
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
	const RomAsmSerialzerOptions* poptions)
{
	if (groupedmappings.empty())
		throw std::runtime_error("Mappings cannot be empty when saving.");

	std::vector<std::string> errors;
	const auto & options = poptions != nullptr ? *poptions : l_defaultOptions;
	std::map<std::string, std::string> mastersavefilemappings;

	for (auto& g : groupedmappings) {
		const auto& source = g.first;
		try {
			// break the mappings into two groups and process them separately.
			std::vector<std::shared_ptr<RomAsmMapping>> discretes;
			std::vector< std::shared_ptr<RomAsmMapping>> sequentials;

			ClassifyMappings(source, g.second, discretes, sequentials);
			ProcessSequentials(source, rom, options, sequentials, (LPCSTR)m_project.ProjectFolder, mastersavefilemappings);
			ProcessDiscretes(source, rom, options, discretes);
		}
		catch (std::exception& ex) {
			errors.push_back(ex.what());
		}
	}

	this->PostSave(options, mastersavefilemappings, errors);
	if (!errors.empty())
		throw std::runtime_error("Save encountered " + std::to_string(errors.size()) + " error(s):\n" + Strings::join(errors, "\n"));
}

void RomAsmSerializer::PostSave(const RomAsmSerialzerOptions & options, SaveFileMapping& mappings, std::vector<std::string>& errors)
{
	if (!mappings.empty()) {
		if (errors.empty()) {
			ConfirmSaveFileMappings((LPCSTR)m_project.ProjectFolder, mappings, options, errors);			
		}
		else {
			CancelSaveFileMappings((LPCSTR)m_project.ProjectFolder, mappings, options, errors);
		}
	}
}


namespace // IMPLEMENTATION
{
	void ClassifyMappings(
		std::string source, RomAsmMappingRefs& mappings,
		RomAsmMappingRefs& discretes, RomAsmMappingRefs& sequentials)
	{
		std::copy_if(mappings.begin(), mappings.end(), std::back_inserter(discretes),
			[](const auto& x) { return dynamic_cast<IRomAsmDiscreteMapping*>(x.get()) != nullptr; });
		std::copy_if(mappings.begin(), mappings.end(), std::back_inserter(sequentials),
			[](const auto& x) { return dynamic_cast<IRomAsmSequentialMapping*>(x.get()) != nullptr; });

		if (!discretes.empty() && !sequentials.empty())
			throw std::runtime_error("Source '" + source + "' hav both discrete and sequential mappings, which is unsupported.");
	}

	void ProcessDiscretes(
		std::string source, const std::vector<unsigned char>& rom,
		const std::map<std::string, std::string>& options, RomAsmMappingRefs& discretes)
	{
		if (!discretes.empty()) {
			if (discretes.size() > 1)
				throw std::runtime_error("Source '" + source + "' has " + std::to_string(discretes.size()) + " discrete mappings, "
					"but at most 1 is supported.");

			std::ifstream ifs;
			std::ofstream ofs;
			for (auto& m : discretes)
				m->Save(ifs, ofs, rom, options);
		}
	}

	void ProcessSequentials(
		std::string source, const std::vector<unsigned char>& rom,
		const std::map<std::string, std::string>& options, RomAsmMappingRefs& sequentials,
		std::string projectfolder, std::map<std::string, std::string>& mastersavefilemappings)
	{
		if (!sequentials.empty()) {
			// Set up a save file entry for this source.
			std::string reltemp = source + ".~new";
			AddSaveFileMapping(mastersavefilemappings, source, reltemp);

			std::string sourcefile = (LPCSTR)Paths::Combine({ projectfolder.c_str(), source.c_str() });
			std::string destfile = (LPCSTR)Paths::Combine({ projectfolder.c_str(), reltemp.c_str() });
			std::ifstream ifs(sourcefile);
			std::ofstream ofs(destfile);
			for (auto& m : sequentials) {
				auto pseq = dynamic_cast<IRomAsmSequentialMapping*>(m.get());
				if (pseq == nullptr)
					throw std::runtime_error("Mapping '" + m->Name() + "' is not a sequential mapping.");

				AddSaveFileMappings(mastersavefilemappings, m->Name(),
					pseq->GetSaveFileMappings());
				m->Save(ifs, ofs, rom, options);
				ofs.flush();
			}
			RomAsm::Modify::SyncDestToEnd(ifs, ofs);
		}
	}

	void AddSaveFileMapping(std::map<std::string, std::string>& mappings, std::string liverelfile, std::string temprelfile)
	{
		auto iter = mappings.find(liverelfile);
		if (iter != mappings.end())
			throw std::runtime_error("Cannot add savefile mapping '" + liverelfile + "'='" + temprelfile + "' "
				"because it is already mapped to '" + iter->second + "'.");

		mappings[liverelfile] = temprelfile;
	}

	void AddSaveFileMappings(
		std::map<std::string, std::string>& mappings, std::string mappingname,
		const std::map<std::string, std::string>& childmappings)
	{
		std::vector<std::string> dupes;
		for (const auto& c : childmappings) {
			const auto& liverel = c.first;
			const auto& temprel = c.second;
			auto iter = mappings.find(liverel);
			if (iter != mappings.end())
				dupes.emplace_back(liverel);
			else
				mappings[liverel] = temprel;
		}

		if (!dupes.empty())
			throw std::runtime_error("Mapping '" + mappingname + "' failed due to the following " + std::to_string(dupes.size())
				+ " savefile mappings having duplicate entries: " + Strings::join(dupes, ", "));
	}

	void ConfirmSaveFileMappings(
		std::string projectfolder, const std::map<std::string, std::string>& mappings,
		const RomAsmSerialzerOptions& options, std::vector<std::string>& errors)
	{
		//N.B. - don't discard save file mappings due to a failure140
		if (!mappings.empty() && errors.empty()) {
			// We have some save file mappings and not errors, so begin updating the live files.
			bool updateErors = false;
			for (const auto& m : mappings) {
				const auto& liverel = m.first;
				const auto& temprel = m.second;
				auto live = Paths::Combine({ projectfolder.c_str(), liverel.c_str() });
				auto temp = Paths::Combine({ projectfolder.c_str(), temprel.c_str() });
				if (Paths::FileExists(temp) && Paths::FileExists(live)) {
					Io::FilePathRemover rem(temp);
					Paths::FileMove(temp, live);
					LogHere << "Moved " << (LPCSTR)temp << " to " << (LPCSTR)live << 'n';
				}
				else {
					errors.emplace_back("Unable to overwrite live file " + live + " with the changes from " + temp);
					updateErors = true;
				}			
			}

			LogFlush;

			if (updateErors) {
				errors.emplace_back("Data corruption has likely happened, please restore from a backup before proceeding!");
			}
		}
	}

	// Remove the temp files created by the mapping (but does nto remove the mappings themselves).
	void CancelSaveFileMappings(
		std::string projectfolder, const std::map<std::string, std::string>& mappings,
		const RomAsmSerialzerOptions& options, std::vector<std::string>& errors)
	{
		if (!mappings.empty()) {
			if (!RomAsm::Options::On(options, "preserveSaveFileMappingsOnFailure")) {
				LogHere << "Discarding " << mappings.size() << " save file mapping(s) due to errors.\n";
				for (const auto& m : mappings) {
					const auto& temprel = m.second;
					auto temp = Paths::Combine({ projectfolder.c_str(), temprel.c_str() });
					if (Paths::FileExists(temp))
						Paths::FileDelete(temp);
				}
			}
		}
	}
} // end namespace IMPLEMENTATION