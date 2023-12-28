#pragma once
#include "RomAsmMapping.h"

// Represents a RomAsm mapping definition that could not convert into a RomAsmMapping-ederived object.

class FailedMapping : public RomAsmMapping
{
public:
	FailedMapping(std::string definition, std::string reason);
	virtual ~FailedMapping() {}

	virtual std::string Load(const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom) override;
	virtual std::string Save(const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom) override;

private:
	std::string m_definition;
	std::string m_reason;
};
