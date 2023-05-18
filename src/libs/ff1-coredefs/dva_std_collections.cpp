#include "stdafx.h"
#include "dva_std_collections.h"
#include "cnv_std_collections.h"
#include "FFHDataValue.h"
#include "DataValueAccessor.h"
#include <vector>
#include <map>
#include <array>

namespace ffh
{
	namespace fda
	{
		namespace {
			const std::string typeStrvec = "std::vector<std::string>";
		}

		const FFHDataValue& operator>>(const FFHDataValue& stg, std::vector<std::string>& value)
		{
			const static std::string mytype = "str[]";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, stg.type, typeStrvec);

			value = cnv::to<std::string>(stg.data);
			return stg;
		}
		FFHDataValue& operator<<(FFHDataValue& stg, const std::vector<std::string>& value)
		{
			const static std::string mytype = "str[]";
			if (stg.type != mytype)
				THROW_DVA_TYPE_ERROR(stg.name, typeStrvec, stg.type);

			stg.data = cnv::to<std::string>(value);
			return stg;
		}
	}
}
