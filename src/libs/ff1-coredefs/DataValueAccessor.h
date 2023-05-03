#pragma once

#include "FFH2Project.h"

namespace ff1coredefs
{

	// FORWARD DECL
	template <typename T> T FromData(const FFHDataValue& value);

	class DataValueAccessor
	{
		FFH2Project& Proj2;
	public:
		DataValueAccessor(FFH2Project& prj2);

		FFHDataValue& FindValue(FFH2Project& prj2, const std::string& name) const;

		template <typename T>
		T get(const std::string& name) const
		{
			auto& value = FindValue(Proj2, name);
			return FromData<T>(value);
		}
	};

	// DEFAULT IMPLEMENTATION
	template <typename T>
	T FromData(const FFHDataValue& value)
	{
		return (T)value.data;
	}

} // end namespace ff1coredefs
