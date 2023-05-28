#pragma once

class FFH2Project;
struct FFHSetting;
struct ProjectEditorModuleEntry;

namespace ffh
{
	namespace acc
	{
		// Use extraction >> and insertion << operators on FFHSetting
		// to convert data types instead of type casrs.
		// Requires defining the required operator in either namespace or global scope
		// for the types involved.

		// === CLASS DECLARATION

		class SettingDataAccessor //TODO - rename class and file to SettingDataAccessor
		{
			FFH2Project& m_proj;
			ProjectEditorModuleEntry& m_module;
		public:
			SettingDataAccessor(FFH2Project& proj, const std::string& modulename);

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

		class sda_typeconversion_exception : std::runtime_error
		{
		public:
			sda_typeconversion_exception(std::string name, std::string srctype, std::string desttype);

			const std::string valueName;
			const std::string sourceType;
			const std::string destType;
		};

//TODO - DEPRECATED, THROW THE EXCEPTION DIRECTLY
#define THROW_SVA_TYPE_ERROR(name,srctype,dsttype) throw ffh::acc::sda_typeconversion_exception((name), (srctype), (dsttype))

		// === STANDARD CONVERSIONS

		FFHSetting& operator>>(FFHSetting& stg, bool& value);
		FFHSetting& operator<<(FFHSetting& stg, const bool& value);

		FFHSetting& operator>>(FFHSetting& stg, int& value);
		FFHSetting& operator<<(FFHSetting& stg, const int& value);

		FFHSetting& operator>>(FFHSetting& stg, std::string& value);
		FFHSetting& operator<<(FFHSetting& stg, const std::string& value);
	}
}
