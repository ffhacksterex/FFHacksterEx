#include "stdafx.h"
#include "shared.h"

namespace disch_functions
{
	namespace shared
	{
		// Returns true if the prefix is not embedded in a comment.
		bool IsUncommentedDataRow(std::string line, std::string prefix)
		{
			auto at = line.find(prefix);
			if (at == std::string::npos) return false; // not a data row

			// it has the prefix, but is it located after a comment marker?
			auto comm = line.find(';');
			return comm == std::string::npos || comm > at;
		}

		//TODO - use dialogue_helpers::HasLabel instead.
		// Checks for a label that is neither local (preceeded by @) nor embedded in a comment.
		bool has_label(std::string line)
		{
			const std::regex rx{ RX_VARNAME "\\:" };
			std::sregex_iterator end;
			std::sregex_iterator iter(cbegin(line), cend(line), rx);
			if (iter != end) {
				auto pos = iter->position();
				if (pos == 0 || line[pos - 1] != '@') {
					auto comm = line.find(';');
					return comm == std::string::npos || (size_t)pos < comm;
				}
			}
			return false;
		}

		// Gets the non-local label on this line, if any.
		std::string get_label(std::string line, getlabelstyle style)
		{
			const std::regex rx{ RX_VARNAME "\\:" };
			std::sregex_iterator end;
			std::sregex_iterator iter(cbegin(line), cend(line), rx);
			if (iter != end) {
				auto pos = iter->position();
				if (pos == 0 || line[pos - 1] != '@') {
					auto comm = line.find(';');
					if (comm == std::string::npos || (size_t)pos < comm) {
						auto length = iter->length() - (style == getlabelstyle::includecolon ? 0 : 1);
						return line.substr(pos, length);
					}
				}
			}
			return std::string();
		}

		bool is_asmdatarow(std::string line)
		{
			return IsUncommentedDataRow(line, ".BYTE") ||
				IsUncommentedDataRow(line, ".WORD") ||
				IsUncommentedDataRow(line, ".FARADDR");
		}
	}
}