#include "stdafx.h"
#include "SettingValueAccessor.h"
#include "FFH2Project.h"

namespace ffh
{
	namespace fda
	{
		namespace {
			ProjectEditorModuleEntry& FindModule(FFH2Project& proj, const std::string& modulename)
			{
				auto it = proj.modules.entries.find(modulename);
				if (it == cend(proj.modules.entries))
					throw std::runtime_error("SettingValueAccessor couldn't find a module named " + modulename + ".");
				return it->second;
			}
		}

		SettingValueAccessor::SettingValueAccessor(FFH2Project& proj, const std::string& modulename)
			: m_proj(proj)
			, m_module(FindModule(proj, modulename))
		{
		}

		FFHSetting& SettingValueAccessor::FindValue(const std::string& name) const
		{
			auto it = m_module.settings.find(name);
			if (it == end(m_module.settings))
				throw std::runtime_error("Module '" + m_module.slotName + "' setting '" + name + "' not found.");
			return it->second;
		}

	}
}