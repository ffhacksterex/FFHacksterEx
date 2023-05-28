#pragma once

#include "FFH2Project.h"

namespace ffh
{
	namespace fda
	{
		// FORWARD DECL
		template <typename T> T FromData(const FFHValue& value);
		//template <typename T> void ToData(FFHValue& value, const T& newvalue);

		class DataValueAccessor //TODO - rename class and file to ValueDataAccessor
		{
			FFH2Project& Proj2;
		public:
			DataValueAccessor(FFH2Project& prj2);

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

#define THROW_DVA_TYPE_ERROR(name,srctype,dsttype) \
		throw std::runtime_error("Value '" + (name) + "' doesn't support conversion from '" + (srctype) + "' to " + (dsttype) + ".")

		// DEFAULT IMPLEMENTATION
		template <typename T>
		T FromData(const FFHValue& value)
		{
			//return (T)value.data;
			T data{};
			value >> data;
			return data;
		}

		//template <typename T>
		//void ToData(FFHValue& value, const T& newvalue)
		//{
		//	value.data = (std::string)newvalue;
		//}

	} // end namespace fda

} // end namespace ffh
