#pragma once

#include <stdexcept>
#include "FFHValue.h"
class FFH2Project;

namespace ffh
{
	// === CLASS DCLARATION
	namespace acc
	{
		class ValueDataAccessor
		{
			FFH2Project& Proj2;
		public:
			ValueDataAccessor(FFH2Project& prj2);

			FFHValue& FindValue(FFH2Project& prj2, const std::string& name) const;

			template <typename T>
			T get(const std::string& name) const
			{
				auto& value = FindValue(Proj2, name);
				T data{};
				value >> data;
				return data;
			}
			template <typename T>
			void set(const std::string& name, const T & data) const
			{
				auto& value = FindValue(Proj2, name);
				value << data;
			}

			// Direct string access, not typically used for editing purposes.
			std::string str(const std::string& name) const
			{
				auto& value = FindValue(Proj2, name);
				return value.data;
			}

			void str(const std::string& name, const std::string& newdata)
			{
				auto& value = FindValue(Proj2, name);
				value.data = newdata;
			}
		};

		class vda_typeconversion_exception : std::runtime_error
		{
		public:
			vda_typeconversion_exception(std::string name, std::string srctype, std::string desttype);

			const std::string valueName;
			const std::string sourceType;
			const std::string destType;
		};


//TODO - DEPRECATED, throw the exception directly instead...
#define THROW_DVA_TYPE_ERROR(name,srctype,dsttype) throw ffh::acc::vda_typeconversion_exception((name), (srctype), (dsttype))


		// === STANDARD TYPE CONVERSIONS
		// These conversion operators are only used within the context of ValueDataAccessor,
		// so they're defined here instead of in the header with FFHValue.

		FFHValue& operator>>(FFHValue& f, bool& value);
		FFHValue& operator<<(FFHValue& f, const bool& value);

		FFHValue& operator>>(FFHValue& f, std::string& value);
		FFHValue& operator<<(FFHValue& f, const std::string& value);

		FFHValue& operator>>(FFHValue& f, int& value);
		FFHValue& operator<<(FFHValue& f, const int& value);
	}
	// end namespace acc
}
// end namespace ffh
