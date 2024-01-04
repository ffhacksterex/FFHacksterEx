#pragma once
#include "RomAsmMapping.h"
#include "IRomAsmSequentialMapping.h"

class CFFHacksterProject;
class RomAsmMappingFactory;

class RomAsmMappingIncbin : public RomAsmMapping, public IRomAsmSequentialMapping
{
public:
	RomAsmMappingIncbin(CFFHacksterProject& project, std::string mapping);
	virtual ~RomAsmMappingIncbin();

	virtual void Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;
	virtual void Save(std::istream& sourcefile, std::ostream& destfile, const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;

protected:
	const std::string maptype = "incbin";
	std::string projectfolder;
	std::string label;
	std::string sublabel;
	int instoffset = -1;
	std::map<std::string, std::string> savefilemappings;  //N.B. - this is cleared and populated with each call to Load()

	virtual std::map<std::string, std::string> GetSaveFileMappings() const override;
};
