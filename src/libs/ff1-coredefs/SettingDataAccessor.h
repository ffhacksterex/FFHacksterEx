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
		public:
			SettingDataAccessor(FFH2Project& proj, const std::string& modulename);

			FFHSetting& EnsureSetting(std::string name, std::string type, std::string data, std::string format = "") const;

			template <typename T>
			T get(const std::string& name)
			{
				auto& setting = FindSetting(name);
				T value;
				setting >> value;
				return value;
			}

			template <typename T>
			void set(const std::string& name, const T& newvalue)
			{
				auto& setting = FindSetting(name);
				setting << newvalue;
			}

			template <typename T>
			T tryget(const std::string& name, T& defultValue)
			{
				auto* setting = TryFindSetting(name);
				if (setting == nullptr)
					return defaultValue;
				T value;
				*setting >> value;
				return value;
			}

		private:
			FFH2Project& m_proj;
			ProjectEditorModuleEntry& m_module;

			FFHSetting& FindSetting(const std::string& name) const;
			FFHSetting* TryFindSetting(const std::string& name) const;
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

		FFHSetting& operator>>(FFHSetting& f, bool& value);
		FFHSetting& operator<<(FFHSetting& f, const bool& value);

		FFHSetting& operator>>(FFHSetting& f, int& value);
		FFHSetting& operator<<(FFHSetting& f, const int& value);

		FFHSetting& operator>>(FFHSetting& f, std::string& value);
		FFHSetting& operator<<(FFHSetting& f, const std::string& value);
	}
}
