#include "stdafx.h"
#include "FailedMapping.h"

FailedMapping::FailedMapping(std::string definition, std::string reason)
	: m_definition(definition)
	, m_reason(reason)
{
}

std::string FailedMapping::Load(const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom)
{
	return m_reason;
}

std::string FailedMapping::Save(const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom)
{
	return m_reason;
}