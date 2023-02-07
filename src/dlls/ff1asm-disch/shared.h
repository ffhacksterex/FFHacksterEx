#pragma once

#include <regex>
#include <std_assembly.h>

namespace disch_functions
{
	namespace shared
	{
		const std::regex rx_opval_hardcode{ "\\s*\\:?\\s*" RX_OPCODE "\\s+\\#?(" RX_VAL0RVAR ")\\s*" };
		//TODO: unused, remove?
		//const std::regex rx_opval_parameter{ "\\s*\\:?\\s*" RX_OPCODE "\\s+" RX_VAR "\\s*\\+?\\s*([0-3]?)\\s*" };

		bool IsUncommentedDataRow(std::string line, std::string prefix);

		bool has_label(std::string line);
		std::string get_label(std::string line, getlabelstyle style);

		bool is_asmdatarow(std::string line);
	}
}
