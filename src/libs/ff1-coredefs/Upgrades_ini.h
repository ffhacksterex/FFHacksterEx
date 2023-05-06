#pragma once

#include "pair_result.h"

// Handles INI format project upgrades from the initial unversioned projects to v0.9.8.1.
class Upgrades_ini
{
public:
	static bool NeedsConversion(CString projectpath);
	static void Upgrade(std::string projectpath);

private:
	static pair_result<CString> UpgradeProject_none_to_1(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_1_to_2(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_2_to_3(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_3_to_4(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_4_to_5(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_5_to_6(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_6_to_7(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_7_to_8(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_8_to_9(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_9_to_10(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_10_to_971(int oldver, int newver, CString projectfolder, CString projectini);
	static pair_result<CString> UpgradeProject_971_to_9810(int oldver, int newver, CString projectfolder, CString projectini);
};

