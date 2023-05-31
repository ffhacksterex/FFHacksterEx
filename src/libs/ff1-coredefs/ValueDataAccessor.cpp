#include "stdafx.h"
#include "ValueDataAccessor.h"
#include "FFH2Project.h"
#include "cnv_primitives.h"
#include <set>

namespace ffh
{
	namespace acc
	{
		// === CLASS IMPLEMENTATIONS
		// * ValueDataAccessor

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


		// * vda_typeconversion_exception

		vda_typeconversion_exception::vda_typeconversion_exception(std::string name, std::string srctype, std::string desttype)
			: std::runtime_error("Value '" + (name)+"' doesn't support conversion from '" + (srctype)+"' to " + (desttype)+".")
			, valueName(name)
			, sourceType(srctype)
			, destType(desttype)
		{
		}


		// === CONVERSION IMPLEMENTATIONS

		//TODO - figure out how to reduce this boilerplate

		FFHValue& operator>>(FFHValue& f, bool& value)
		{
			const static std::string mytype = "bool";
			if (f.type != mytype)
				THROW_DVA_TYPE_ERROR(f.name, f.type, mytype);

			value = (f.data == "true");
			return f;
		}
		FFHValue& operator<<(FFHValue& f, const bool& value)
		{
			const static std::string mytype = "bool";
			if (f.type != mytype)
				THROW_DVA_TYPE_ERROR(f.name, f.type, mytype);

			f.data = value ? "true" : "false";
			return f;
		}

		FFHValue& operator>>(FFHValue& f, std::string& value)
		{
			const static std::string mytype = "str";
			if (f.type != mytype)
				THROW_DVA_TYPE_ERROR(f.name, f.type, mytype);

			value = f.data;
			return f;
		}
		FFHValue& operator<<(FFHValue& f, const std::string& value)
		{
			const static std::string mytype = "str";
			if (f.type != mytype)
				THROW_DVA_TYPE_ERROR(f.name, f.type, mytype);

			f.data = value;
			return f;
		}

		namespace {
			const std::set<std::string> intTypes = { "int", "hex", "addr", "rgb" };
		}

		FFHValue& operator>>(FFHValue& f, int& value)
		{
			const static std::string mytype = "int";
			if (intTypes.find(f.type) == cend(intTypes))
				THROW_DVA_TYPE_ERROR(f.name, f.type, mytype);

			if (f.type == "hex") {
				value = ffh::cnv::hex(f.data, f.format);
			}
			else {
				sscanf(f.data.c_str(), f.format.c_str(), &value);
			}
			return f;
		}

		FFHValue& operator<<(FFHValue& f, const int& value)
		{
			const static std::string mytype = "int";
			if (intTypes.find(f.type) == cend(intTypes))
				THROW_DVA_TYPE_ERROR(f.name, f.type, mytype);

			if (f.type == "hex") {
				f.data = ffh::cnv::hex(value, f.format);
			} else {
				char buf[128] = { 0 };
				sprintf_s(buf, f.format.c_str(), value);
				f.data = buf;
			}
			return f;
		}

	}
	// end namespace acc
}
// end namespace ffh