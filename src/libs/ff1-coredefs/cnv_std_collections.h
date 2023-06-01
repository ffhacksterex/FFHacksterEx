#pragma once
#include <vector>
#include "nlohmann/nlohmann-integration.hpp"

namespace ffh
{
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

		std::vector<unsigned char> hexarray_to_bytevec(const std::string& str);
		std::string bytevec_to_hexarray(const std::vector<unsigned char>& vbytes);
	}
	// end namespace cnv
}
// end namespace ffh
