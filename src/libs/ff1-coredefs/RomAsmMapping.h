#pragma once

#include <iosfwd>
#include <map>
#include <vector>

class RomAsmMappingFactory;

class RomAsmMapping
{
	friend class RomAsmMappingFactory;
public:
	RomAsmMapping();
	virtual ~RomAsmMapping();

	virtual void Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) = 0;
	virtual void Save(std::istream& sourcefile, std::ostream & destfile, const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) = 0;

	std::string Name() const;

protected:
	std::string name;
	std::string type;
	int romoffset = -1;
	std::string source;
	int count = -1;
	bool readonly = false;
};
