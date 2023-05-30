#include "stdafx.h"
#include "SettingDataAccessor.h"
#include "FFH2Project.h"
#include "cnv_primitives.h"
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

		// Returns the named setting.
		// If it doesn't exist, creates it with the specified name, type, data, and optional format.
		// Data is in string format, so an object would have to be serialized as a string.
		FFHSetting& SettingDataAccessor::EnsureSetting(std::string name, std::string type, std::string data, std::string format) const
		{
			auto it = m_module.settings.find(name);
			if (it != end(m_module.settings))
				return it->second;

			auto& s = (m_module.settings[name] = FFHSetting{});
			s.name = name;
			s.type = type;
			s.data = data;
			s.format = format;
			return s;
		}

		FFHSetting& SettingDataAccessor::FindSetting(const std::string& name) const
		{
			auto it = m_module.settings.find(name);
			if (it == end(m_module.settings))
				throw std::runtime_error("Module '" + m_module.slotName + "' setting '" + name + "' not found.");
			return it->second;
		}

		FFHSetting* SettingDataAccessor::TryFindSetting(const std::string& name) const
		{
			auto it = m_module.settings.find(name);
			if (it == end(m_module.settings))
				return nullptr;
			return &it->second;
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

		FFHSetting& operator>>(FFHSetting& f, bool& value)
		{
			if (f.type != "bool")
				THROW_SVA_TYPE_ERROR(f.name, f.type, "bool");

			value = (f.data == "true");
			return f;
		}

		FFHSetting& operator<<(FFHSetting& f, const bool& value)
		{
			if (f.type != "bool")
				THROW_SVA_TYPE_ERROR(f.name, "bool", f.type);

			f.data = value ? "true" : "false";
			return f;
		}

		namespace {
			std::set<std::string> s_intTypes = { "int", "hex", "addr", "rgb" };
		}

		FFHSetting& operator>>(FFHSetting& f, int& value)
		{
			if (s_intTypes.find(f.type) == cend(s_intTypes))
				THROW_SVA_TYPE_ERROR(f.name, f.type, "int");

			if (f.type == "hex") {
				value = ffh::cnv::hex(f.data, f.format);
			}
			else {
				sscanf(f.data.c_str(), f.format.c_str(), &value);
			}

			return f;
		}

		FFHSetting& operator<<(FFHSetting& f, const int& value)
		{
			if (s_intTypes.find(f.type) == cend(s_intTypes))
				THROW_SVA_TYPE_ERROR(f.name, "int", f.type);

			if (f.type == "hex") {
				f.data = ffh::cnv::hex(value, f.format);
			}
			else {
				char buf[128] = { 0 };
				sprintf_s(buf, f.format.c_str(), value);
				f.data = buf;
			}
			return f;
		}

		FFHSetting& operator>>(FFHSetting& f, std::string& value)
		{
			if (f.type != "str")
				THROW_SVA_TYPE_ERROR(f.name, f.type, "str");

			value = f.data;
			return f;
		}

		FFHSetting& operator<<(FFHSetting& f, const std::string& value)
		{
			if (f.type != "str")
				THROW_SVA_TYPE_ERROR(f.name, "str", f.type);

			f.data = value;
			return f;
		}

	}
	// end acc
}
// end ffh