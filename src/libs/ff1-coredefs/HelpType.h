#pragma once

enum HelpType { Invalid = -1, None = 0, LocalApp, Folder, Web, Last_ };

const CString HelpTypeNames[] = { "Invalid", "None", "LocalApp", "Folder", "Web" };
constexpr int HelpTypeNamesCount = _countof(HelpTypeNames);

static_assert(_countof(HelpTypeNames) == HelpType::Last_ + 1, "HelpType and HelpTypeNames count mismatch");

inline CString GetHelpTypeName(HelpType h) {
	auto i = 1 + (int)h;
		ASSERT(i >= 0 && i < HelpTypeNamesCount);
	if (i >= 0 && i < HelpTypeNamesCount)
		return HelpTypeNames[i];
	return "Unknown-HelpType";
}