#include "stdafx.h"
#include "vda_std_collections.h"
#include "cnv_std_collections.h"
#include "FFHValue.h"
#include "ValueDataAccessor.h"
#include <vector>
#include <map>
#include <array>
#include <set>

namespace ffh
{
	namespace acc
	{
		namespace {
			const std::string typeStrvec = "std::vector<std::string>";

			const static std::set<std::string> BytevecTypes = { "byte[]", "hex[]" };
			const std::string typeBytevec = "std::vector<unsigned char>";
		}

		const FFHValue& operator>>(const FFHValue& stg, std::vector<std::string>& value)
		{
			const static std::string mytype = "str[]";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, typeStrvec);

			value = cnv::to<std::string>(stg.data);
			return stg;
		}
		FFHValue& operator<<(FFHValue& stg, const std::vector<std::string>& value)
		{
			const static std::string mytype = "str[]";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, typeStrvec, stg.type);

			stg.data = cnv::to<std::string>(value);
			return stg;
		}

		const FFHValue& operator>>(const FFHValue& stg, std::vector<unsigned char>& value)
		{
			if (BytevecTypes.find(stg.type) == cend(BytevecTypes))
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, typeBytevec);

			value = cnv::hexarray_to_bytevec(stg.data);
			return stg;
		}
		FFHValue& operator<<(FFHValue& stg, const std::vector<unsigned char>& value)
		{
			if (BytevecTypes.find(stg.type) == cend(BytevecTypes))
				THROW_DVA_TYPE_ERROR(stg.name, typeBytevec, stg.type);

			stg.data = cnv::bytevec_to_hexarray(value);
			return stg;
		}
	}
}
