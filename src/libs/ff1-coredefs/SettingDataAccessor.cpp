#include "stdafx.h"
#include "SettingDataAccessor.h"
#include "FFH2Project.h"
#include <set>

namespace ffh
{
	namespace acc
	{
		namespace {
			ProjectEditorModuleEntry& FindModule(FFH2Project& proj, const std::string& modulename)
			{
				auto it = proj.modules.entries.find(modulename);
				if (it == cend(proj.modules.entries))
					throw std::runtime_error("SettingDataAccessor couldn't find a module named " + modulename + ".");
				return it->second;
			}
		}

		// === CLASS IMPLEMENTATION
		// * SettingDataAccessor

		SettingDataAccessor::SettingDataAccessor(FFH2Project& proj, const std::string& modulename)
			: m_proj(proj)
			, m_module(FindModule(proj, modulename))
		{
		}

		FFHSetting& SettingDataAccessor::FindValue(const std::string& name) const
		{
			auto it = m_module.settings.find(name);
			if (it == end(m_module.settings))
				throw std::runtime_error("Module '" + m_module.slotName + "' setting '" + name + "' not found.");
			return it->second;
		}


		// * sda_typeconversion_exception

		sda_typeconversion_exception::sda_typeconversion_exception(std::string name, std::string srctype, std::string desttype)
			: std::runtime_error("Value '" + (name)+"' doesn't support conversion from '" + (srctype)+"' to " + (desttype)+".")
			, valueName(name)
			, sourceType(srctype)
			, destType(desttype)
		{
		}


		// === STANDARD CONVERSION IMPLEMENTATIONS

		FFHSetting& operator>>(FFHSetting& stg, bool& value)
		{
			if (stg.type != "bool")
				THROW_SVA_TYPE_ERROR(stg.name, stg.type, "bool");

			value = (stg.data == "true");
			return stg;
		}

		FFHSetting& operator<<(FFHSetting& stg, const bool& value)
		{
			if (stg.type != "bool")
				THROW_SVA_TYPE_ERROR(stg.name, "bool", stg.type);

			stg.data = value ? "true" : "false";
			return stg;
		}

		namespace {
			std::set<std::string> s_intTypes = { "int", "hex", "addr", "rgb" };
		}

		FFHSetting& operator>>(FFHSetting& stg, int& value)
		{
			if (s_intTypes.find(stg.type) == cend(s_intTypes))
				THROW_SVA_TYPE_ERROR(stg.name, stg.type, "int");

			sscanf(stg.data.c_str(), stg.format.c_str(), &value);
			return stg;
		}

		FFHSetting& operator<<(FFHSetting& stg, const int& value)
		{
			if (s_intTypes.find(stg.type) == cend(s_intTypes))
				THROW_SVA_TYPE_ERROR(stg.name, "int", stg.type);

			char buf[24] = { 0 };
			sprintf_s<24>(buf, stg.format.c_str(), stg.data, value);
			stg.data = buf;
			return stg;
		}

		FFHSetting& operator>>(FFHSetting& stg, std::string& value)
		{
			if (stg.type != "str")
				THROW_SVA_TYPE_ERROR(stg.name, stg.type, "str");

			value = stg.data;
			return stg;
		}

		FFHSetting& operator<<(FFHSetting& stg, const std::string& value)
		{
			if (stg.type != "str")
				THROW_SVA_TYPE_ERROR(stg.name, "str", stg.type);

			stg.data = value;
			return stg;
		}

	}
	// end acc
}
// end ffh