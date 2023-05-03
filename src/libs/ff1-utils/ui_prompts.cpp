#include "stdafx.h"
#include "ui_prompts.h"
#include "BrowseForFolderDialog.h"
#include "FFBaseApp.h"
#include <AppSettings.h>
#include <asmdll_impl.h>
#include <path_functions.h>
#include "XPVersionHack.h"

//STDFILESYS - see path_functions.cpp for more info
#include <filesystem>

namespace Ui
{
	// See OpenFilePromptExt for return value info.
	pair_result<CString> OpenFilePrompt(CWnd * parent, CString filter, CString title, CString initialfile)
	{
		return OpenFilePromptExt(parent, filter, nullptr, title, initialfile);
	}

	// Returns a pair_result<CString> as follows:
	// - success returns {true, "C:\path\to\file"}
	// - cancel returns {false, ""}
	// - otherwise, returns {false, "Error reason"}
	// Cancelling the dialog will always return false with an empty string value.
	// Any other result is treated as an error, in which case false is returned
	// and the value will contain some message (even if it's a generic message).
	pair_result<CString> OpenFilePromptExt(CWnd* parent, CString filter, CString defext, CString title, CString initialfile)
	{
		TCHAR initpath[_MAX_PATH] = { 0 };
		CFileDialog dlg(TRUE, defext, nullptr, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | COMMON_OFN, filter, parent);
		dlg.m_pOFN->lpstrTitle = title;
		if (Paths::FileExists(initialfile)) {
			strncpy(initpath, initialfile, _MAX_PATH);
			dlg.m_pOFN->lpstrFile = initpath;
		}
		else if (Paths::DirExists(initialfile)) {
			strncpy(initpath, initialfile, _MAX_PATH);
			dlg.m_pOFN->lpstrInitialDir = initpath;
		}

		auto modalresult = dlg.DoModal();
		if (modalresult == IDOK)
			return pair_result<CString>(true, dlg.GetPathName());

		return pair_result<CString>(false, modalresult == IDCANCEL ? "" : "An error occurred while attempting to prompt for an open filename.");
	}

	// See OpenFilePromptExt for return value info.
	pair_result<CString> SaveFilePrompt(CWnd * parent, CString filter, CString title, CString initialfile) //REFACTOR
	{
		return SaveFilePromptExt(parent, filter, nullptr, title, initialfile);
	}

	// See OpenFilePromptExt for return value info.
	pair_result<CString> SaveFilePromptExt(CWnd* parent, CString filter, CString defext, CString title, CString initialfile)
	{
		char initpath[_MAX_PATH + 1] = { 0 };
		CFileDialog dlg(FALSE, defext, nullptr, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, filter, parent);
		dlg.m_pOFN->lpstrTitle = title;
		dlg.m_pOFN->lpstrFile = initpath;
		if (Paths::IsDir(initialfile)) initialfile += "\\"; // this sets it as initial dir instead
		strncpy(initpath, initialfile, _MAX_PATH);

		auto modalresult = dlg.DoModal();
		if (modalresult == IDOK)
			return pair_result<CString>(true, dlg.GetPathName());

		return pair_result<CString>(false, modalresult == IDCANCEL ? "" : "An error occurred while attempting to prompt for a save filename.");
	}

	CString PromptToSaveByFilter(CWnd * parent, CString defext, CString filter, CString defaultfilename)
	{
		LPCSTR pszfilename = !defaultfilename.IsEmpty() ? (LPCSTR)defaultfilename : nullptr;
		CFileDialog dlg(FALSE, defext, pszfilename, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
			filter, parent);

		auto modalresult = dlg.DoModal();
		if (modalresult == IDOK)
			return dlg.GetPathName();
		return "";
	}

	CString PromptForEXE(CWnd * parent, CString defaultfilename)
	{
		LPCSTR pszfilename = !defaultfilename.IsEmpty() ? (LPCSTR)defaultfilename : nullptr;
		CFileDialog dlg(TRUE, nullptr, pszfilename, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | UiTemp::COMMON_OFN,
			"NES Executable (*.exe)|*.exe||", parent);

		auto modalresult = dlg.DoModal();
		if (modalresult == IDOK)
			return dlg.GetPathName();
		return "";
	}

	pair_result<CString> PromptForAsmDll(CWnd * parent, LPCSTR initialdir)
	{
		if (parent == nullptr) parent = AfxGetMainWnd();

		auto result = OpenFilePrompt(parent,
			"FFHacksterEX Assembly parsing DLL (ff1asm-*.dll)|ff1asm-*.dll||", "Locate FFHacksterEX assembly DLL", initialdir);

		if (!result)
			return result; // user likely cancelled

		bool isvalid = asmdll_impl::is_ff1_asmdll(result.value);
		if (!isvalid)
			return { false, "The selected file is not an FF1 Assembly DLL." };

		return result;
	}

	pair_result<CString> BrowseForProject(CWnd* parent, CString title, CString initialfileordir)
	{
		//TODO - put these in a resource and load from there, minimizes code changes
		CString filter =
			"FF1 Projects (*.ff1r,*.ff1rom,*.ff1asm)|*.ff1r;*.ff1rom;*.ff1asm|" //TODO - ff1r is temporary
			"FF1 JSON Projects (*.ff1r)|*.ff1r|" //TODO - ff1r is temporary
			"FF1 ROM Projects (*.ff1rom)|*.ff1rom|"
			"FF1 Assembly Projects (*.ff1asm)|*.ff1asm|"
			"|";
		if (parent == nullptr) parent = AfxGetMainWnd();
		return OpenFilePrompt(parent, filter, title, initialfileordir);
	}


	namespace {
		CString BrowseForFolder(CWnd * parent, LPCSTR title, LPCSTR initialdir)
		{
			// The callback will attempt to expand to the selected folder once the
			// browse dialog is initialized.
			const auto BrowseCallback = [](HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
				UNREFERENCED_PARAMETER(lParam);

				if (uMsg == BFFM_INITIALIZED) {
					LPCSTR path = reinterpret_cast<LPCSTR>(lpData);
					CStringW wpath(path);
					::SendMessage(hwnd, BFFM_SETEXPANDED, TRUE, (LPARAM)(LPCWSTR)wpath);
				}
				return 0;
			};

			// We need to leave the desktop as the root of the browse dialog,
			// Use a callback to expand to the initialdir if it exists.
			CString returnedpath;
			const static LPCSTR deftitle = "Select a folder.";
			BROWSEINFO bi = { 0 };
			bi.ulFlags = BIF_NEWDIALOGSTYLE;
			bi.hwndOwner = parent != nullptr ? parent->GetSafeHwnd() : nullptr;
			bi.lpszTitle = title != nullptr ? title : deftitle;
			bi.lParam = reinterpret_cast<LPARAM>(initialdir);
			if (initialdir != nullptr) bi.lpfn = BrowseCallback;

			// The Windows Shell will allocate and return an LPITEMIDLIST for the selected folder.
			auto idlist = SHBrowseForFolder(&bi);
			if (idlist != nullptr) {
				// Get the path from that idlist - its absolute since the dialog used the desktop as the root.
				// Use the Shell to delete the idlist if it allocated one.
				char szpath[_MAX_PATH + 1] = { 0 };
				SHGetPathFromIDList(idlist, szpath);
				CoTaskMemFree(idlist);
				returnedpath = szpath;
			}
			return returnedpath;
		}

		pair_result<CString> SelectFolder(CWnd* parent, CString title, CString initialdir)
		{
			CBrowseForFolderDialog dlg(parent, initialdir, title);
			auto modalresult = dlg.DoModal();
			if (modalresult == IDOK)
				return pair_result<CString>(true, Paths::GetDirectoryPath(dlg.GetPathName()));

			return pair_result<CString>(false, modalresult == IDCANCEL ? "" : "An error occurred while attempting to prompt for an open filename.");
		}
	}

	// Prompts for a path. Returns {true, <path>} if successful.
	pair_result<CString> PromptForFolder(CWnd * parent, CString title, CString initialdir)
	{
		auto appstgs = AfxGetFFBaseApp()->GetAppSettings();
		if (appstgs.UseFileDialogToBrowseFolders)
			return SelectFolder(parent, title, initialdir);

		auto filepath = BrowseForFolder(parent, title, initialdir);
		auto ok = filepath.IsEmpty() || Paths::DirExists(filepath);
		return { ok, filepath };
	}

}