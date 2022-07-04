#include "stdafx.h"
#include "stream_ops.h"


bool stream_tag_info::load(std::string separators)
{
	if (separators.length() < 6)
		valid = false;
	else {
		openhead = separators[0];
		closehead = separators[1];
		openbody = separators[2];
		closebody = separators[3];
		elemsep = separators[4];
		tag = separators.substr(5);
		valid = !tag.empty();
	}
	return valid;
}


std::string read_until(std::istream & is, const std::string & separators)
{
	std::string result;
	while (is && !is.eof() && separators.find((std::string::value_type)is.peek()) == std::string::npos)
		result += (std::string::value_type)is.get();
	return result;
}

void init_stringstream(std::istringstream & istr, const std::string & str)
{
	istr.str(str);
	istr.clear();
	istr.seekg(0);
}

void init_stringstream(std::ostringstream & ostr)
{
	ostr.clear();
	ostr.seekp(0);
}
