#pragma once

#include <map>
#include <vector>
using stdstringintmap = std::map<std::string, int>;

namespace regex_helpers
{
	long eval_formula(std::string formula, const stdstringintmap & valuemap);
	long eval_formula(std::string formula, const stdstringintmap & valuemap, std::string numberformatrxpattern);
}
