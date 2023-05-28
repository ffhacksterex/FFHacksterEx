#pragma once
#include <map>
#include <set>
#include <vector>

namespace ffh
{
	namespace uti
	{
		const std::string DefaultCollectionErrname = "this collection";

		template <typename T>
		inline T& find(std::map<std::string, T>& m, std::string name, const std::string & errname = DefaultCollectionErrname)
		{
			auto it = m.find(name);
			if (it == cend(m))
				throw std::runtime_error("No entry named '" + name + "' was found in " + errname + ".");
			return it->second;
		}
	}
}
