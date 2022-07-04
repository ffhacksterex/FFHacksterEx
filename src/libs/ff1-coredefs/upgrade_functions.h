#pragma once

#include "pair_result.h"

namespace Upgrades
{
	bool NeedsConversion(CString projectpath);
	pair_result<CString> UpgradeProject(CString projectpath);
}
