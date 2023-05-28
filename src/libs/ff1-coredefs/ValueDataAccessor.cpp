#include "stdafx.h"
#include "ValueDataAccessor.h"
#include "FFH2Project.h"
#include <set>

namespace ffh
{
	namespace acc
	{
		// === CLASS IMPLEMENTATION

		ValueDataAccessor::ValueDataAccessor(FFH2Project& prj2)
			: Proj2(prj2)
		{
		}

		FFHValue& ValueDataAccessor::FindValue(FFH2Project& prj2, const std::string& name) const
		{
			auto it = prj2.values.entries.find(name);
			if (it == end(prj2.values.entries))
				throw std::runtime_error("Project value '" + name + "' not found.");
			return it->second;
		}

		vda_typeconversion_exception::vda_typeconversion_exception(std::string name, std::string srctype, std::string desttype)
			: std::runtime_error("Value '" + (name)+"' doesn't support conversion from '" + (srctype)+"' to " + (desttype)+".")
			, valueName(name)
			, sourceType(srctype)
			, destType(destType)
		{
		}


		// === CONVERSION IMPLEMENTATIONS

		//TODO - figure out how to reduce this boilerplate

		FFHValue& operator>>(FFHValue& stg, bool& value)
		{
			const static std::string mytype = "bool";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			value = (stg.data == "true");
			return stg;
		}
		FFHValue& operator<<(FFHValue& stg, const bool& value)
		{
			const static std::string mytype = "bool";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			stg.data = value ? "true" : "false";
			return stg;
		}

		FFHValue& operator>>(FFHValue& stg, std::string& value)
		{
			const static std::string mytype = "str";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			value = stg.data;
			return stg;
		}
		FFHValue& operator<<(FFHValue& stg, const std::string& value)
		{
			const static std::string mytype = "str";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			stg.data = value;
			return stg;
		}

		namespace {
			const std::set<std::string> intTypes = { "int", "hex", "addr", "rgb" };
		}

		FFHValue& operator>>(FFHValue& stg, int& value)
		{
			const static std::string mytype = "int";
			if (intTypes.find(stg.type) == cend(intTypes))
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			sscanf(stg.data.c_str(), stg.format.c_str(), &value);
			return stg;
		}
		FFHValue& operator<<(FFHValue& stg, const int& value)
		{
			const static std::string mytype = "int";
			if (intTypes.find(stg.type) == cend(intTypes))
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, mytype);

			char buf[128] = { 0 };
			sprintf_s(buf, stg.format.c_str(), value);
			stg.data = buf; //TODO - do I need dquotes here? should I use nlohmann here?
			return stg;
		}

	}
	// end namespace acc
}
// end namespace ffh