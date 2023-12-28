#pragma once
#include <map>
#include <vector>

class RomAsmMapping
{
protected:
	RomAsmMapping() {}
public:
	RomAsmMapping(const RomAsmMapping& rhs) = delete;
	RomAsmMapping& operator=(const RomAsmMapping& rhs) = delete;

	std::string name;
	std::string type;
	int offset = -1;
	std::string source;
	int count = 0;
	bool readonly = false;

	virtual ~RomAsmMapping() {}

	// Returns a string indicating the status, with an empty string indicating success.
	virtual std::string Load(const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom) = 0;
	virtual std::string Save(const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom) = 0;

protected:
	std::string GetProjectType(const std::map<std::string, std::string>& options) const;
};
