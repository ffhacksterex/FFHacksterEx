#include "stdafx.h"
#include "cnv_std_collections.h"
#include "cnv_primitives.h"

namespace ffh
{
	namespace cnv
	{
		std::vector<unsigned char> hexarray_to_bytevec(const std::string& str)
		{
			auto vstr = cnv::to<std::string>(str);
			std::vector<unsigned char> vbytes;
			vbytes.reserve(vstr.size());
			for (size_t idx = 0; idx < vstr.size(); ++idx) {
				const auto& s = vstr[idx];
				auto i = ffh::cnv::hex(s);
				if (i > 255)
					throw std::domain_error("hexarray_to_bytevec: index " + std::to_string(idx) +
						" value '" + std::to_string(i) + "' is larger than the limit of an unsigned char (255).");
				vbytes.push_back((unsigned char)i);
			}
			return vbytes;
		}

		std::string bytevec_to_hexarray(const std::vector<unsigned char>& vbytes)
		{
			auto format = cnv::build_hex_format(2);
			std::vector<std::string> vhex;
			vhex.reserve(vbytes.size());
			for (unsigned char b : vbytes) {
				vhex.push_back(cnv::hex(b, format));
			}
			auto str = cnv::to(vhex);
			return str;
		}
	}
	// namespace cnv
}
// namespace ffh