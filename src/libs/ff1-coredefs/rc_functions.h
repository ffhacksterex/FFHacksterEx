#pragma once

#include <afx.h>
#include <map>
#include <vector>

namespace Rc
{
	struct sIdToDlgData {
		CString name;
		CString rcname;
	};

	using IdDlgDataMap = std::map<int, sIdToDlgData>;
	using dialogmappings = std::map<CString, UINT>;

	IdDlgDataMap GetIdToDlgNmmeMap(HMODULE hmod);
	const sIdToDlgData* GetDialogData(const IdDlgDataMap & imap, int dlgid);
	dialogmappings GetDialogControlMapping(const IdDlgDataMap& imap, int dlgid, HMODULE hmod);
	dialogmappings GetDialogControlMapping(HMODULE hmod, const sIdToDlgData & dlgdata);

	CString GetControlNameFromId(HMODULE hmod, CString rcname, CString dialogname, UINT childid);
}
