#pragma once

#include <pair_result.h>

namespace Ui
{
	constexpr UINT COMMON_OFN = OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;

	CString PromptForRomPath(CWnd * parent);
	CString PromptForProject(CWnd * parent);
	CString PromptToSaveProject(CWnd * parent, bool isrom, const char* promptcaption = nullptr);
	CString PromptToSaveProjectByFilter(CWnd * parent, CString defext, CString filter, CString defaultfilename = "");
	CString PromptForEXE(CWnd * parent, CString defaultfilename = "");

	pair_result<CString> PromptForAsmDll(CWnd* parent, LPCSTR initialdir = nullptr);
	pair_result<CString> BrowseForProject(CWnd* parent, CString title = "Open Project");
	pair_result<CString> OpenFilePrompt(CWnd* parent, CString filter, CString title = "Open", CString initialfile = "");
	pair_result<CString> OpenFilePromptExt(CWnd* parent, CString filter, CString defext, CString title, CString initialfile = "");
	pair_result<CString> SaveFilePrompt(CWnd* parent, CString filter, CString title = "Save", CString initialfile = "");
	pair_result<CString> SaveFilePromptExt(CWnd* parent, CString filter, CString defext, CString title = "Save", CString initialfile = "");

	pair_result<CString> PromptForFolder(CWnd * parent, CString title = "", CString initialdir = "");
}
