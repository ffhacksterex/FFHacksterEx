#pragma once
#include <vector>
#include "nlohmann/nlohmann-integration.hpp"

namespace cnv
{
	template <typename T>
	std::vector<T> to(const std::string& str)
	{
		std::vector<T> v;
		auto j = ujson::parse(str);
		j.get_to(v);
		return v;
	}

	template <typename T>
	std::string to(const std::vector<T>& v)
	{
		ojson j = v;
		std::string str = j.dump(2);
		return str;
	}
}
