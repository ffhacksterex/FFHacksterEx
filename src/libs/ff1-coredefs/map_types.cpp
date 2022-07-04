#include "stdafx.h"
#include "map_types.h"

void read_element(std::istream & is, std::map<std::string, std::string> & c, const std::string & separators)
{
	auto thekey = read_until(is, "=" + separators);
	auto assign = is.get();
	if (assign != '=') throw std::runtime_error("missing equal sign between key and value at position " + std::to_string(is.tellg()));

	auto theval = read_until(is, separators);
	c[thekey] = theval;
}