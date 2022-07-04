#include "stdafx.h"
#include "set_types.h"

void read_element(std::istream & is, std::set<std::string> & c, const std::string & separators)
{
	auto value = read_until(is, separators);
	c.insert(value);
}
