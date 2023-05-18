#include "stdafx.h"
#include "DataValueAccessor.h"

namespace ffh
{
	namespace fda
	{

		DataValueAccessor::DataValueAccessor(FFH2Project& prj2)
			: Proj2(prj2)
		{
		}

		FFHDataValue& DataValueAccessor::FindValue(FFH2Project& prj2, const std::string& name) const
		{
			auto it = prj2.values.entries.find(name);
			if (it == end(prj2.values.entries))
				throw std::runtime_error("Project value '" + name + "' not found.");
			return it->second;
		}

	} // end namespace fda
} // end namespace ffh