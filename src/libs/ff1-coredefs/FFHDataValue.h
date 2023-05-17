#pragma once

#include <string>

//TODO - rename to FFHValue
struct FFHDataValue {
	std::string name;
	std::string type;
	std::string format;
	std::string data;
	std::string label;
	std::string desc;
	std::string group;
	bool internal = false;
	bool readonly = false;
	bool hidden = false;
};
