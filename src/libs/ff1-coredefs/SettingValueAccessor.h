#pragma once

class FFH2Project;
struct FFHSetting;
struct ProjectEditorModuleEntry;

namespace ffh
{
	namespace fda
	{

		// Use extraction >> and insertion << operators on FFHSetting
		// to convert data types instead of type casrs.
		// Requires defining the required operator in either namespace or global scope
		// for the types involved.

		class SettingValueAccessor //TODO - rename class and file to SettingDataAccessor
		{
			FFH2Project& m_proj;
			ProjectEditorModuleEntry& m_module;
		public:
			SettingValueAccessor(FFH2Project& proj, const std::string& modulename);

			FFHSetting& FindValue(const std::string& name) const;

			template <typename T>
			T get(const std::string& name)
			{
				auto& setting = FindValue(name);
				T value;
				setting >> value;
				return value;
			}

			template <typename T>
			void set(const std::string& name, const T& newvalue)
			{
				auto& setting = FindValue(name);
				setting << newvalue;
			}
		};

#define THROW_SVA_TYPE_ERROR(name,srctype,dsttype) \
			throw std::runtime_error("Setting '" + (name) + "' doesn't support conversion from '" + (srctype) + "' to " + (dsttype) + ".")
	}
}
