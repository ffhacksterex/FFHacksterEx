// CBrowseForFolderDialog.cpp : implementation file
//

#include "stdafx.h"
#include "BrowseForFolderDialog.h"
#include <Dlgs.h>
#include <path_functions.h>
#include "XPVersionHack.h"

//#define FORCE_ON_VISTAPLUS

// CBrowseForFolderDialog

IMPLEMENT_DYNAMIC(CBrowseForFolderDialog, CFileDialog)

CBrowseForFolderDialog::CBrowseForFolderDialog(CWnd * parent, CString initialdir, CString title)
	: CBrowseForFolderDialog(parent, initialdir, title,
#ifdef FORCE_ON_VISTAPLUS
		FALSE
#else
		IsWindowsVistaOrGreater() == true
#endif
	)
{
}

CBrowseForFolderDialog::CBrowseForFolderDialog(CWnd * parent, CString initialdir, CString title, BOOL isvistaplus)
	: CFileDialog(TRUE, nullptr, nullptr,
		OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLEHOOK | OFN_NOCHANGEDIR,
		nullptr, parent, 0, isvistaplus)
{
	if (Paths::DirExists(initialdir))
		m_ofn.lpstrInitialDir = initialdir;
	m_ofn.lpstrTitle = title;

	ZeroMemory(m_filename, _countof(m_filename));

	if (isvistaplus) {
		m_bPickFoldersMode = TRUE;
	}
	else {
		// In XP mode, use a hook function to handle messages sent to the file dialog
		m_ofn.lpfnHook = (LPOFNHOOKPROC)CBrowseForFolderDialog::OFNHookFolderSearch;

		// To get the folder name to show up in the ex combo edit. format initialdir so that the foldername
		// is appended to the path, e.g. "C:\path\to\folder\" would become "C:\path\to\folder\folder".
		// The the trailing slash must be removed or the dialog will put a dot (.) in the combo edit.
		//DEVNOTE - *** see the notes at the end of this function on why initialdir has to be formatted like this.
		auto cleanpath = Paths::RemoveTrailingSlash(initialdir);
		auto filename = Paths::Combine({ cleanpath, Paths::GetFileName(cleanpath) });
		m_ofn.lpstrFile = m_filename;
		strncpy(m_filename, filename, _MAX_PATH);
	}

	//DEVNOTE - *** Bear in mind that this code was done as a hack for Win XP to circumvent the need
	//		for the SHBrowseForFolder dialog, which generated usability complaints. Vista and later
	//		use IFileDialog in FOS_PICKFOLDERS mode by default (CFileDialog implements it internally),
	//		so Vista-forward Oses won't hit this code unless the FORCE_ON_VISTAPLUS symbol is defined.

	// To get the combo edit to show the folder name when the dialog first opens, we have to trick the
	//		OS implementation into thinking that our initialdir is a file instead of a folder.
	//		That takes advantage of the fact that the OS doesn't (currently) validate the combo edit on entry.

	// We need to set the filename to the initialdir with the foldername appended to it and NOT trailing slash.
	// e.g. if using "C:\path\to\folder" as initialdir, consider these outcomes:

	// m_ofn.lpstrFile              dialog current path       cmb13 edit contents
	// ---------------              -------------------       -------------------
	// C:\path\to\folder\           C:\path\to\folder         .  (single dot)       ***BAD***
	// C:\path\to\folder            C:\path\to                folder                ***BAD***
	// C:\path\to\folder\folder     C:\path\to\folder         folder                OK

	// We have to set lpstrFile to "C:\path\to\folder\folder", which will set the current path to
	// "C:\path\to\folder" and the combo edit to "folder"; clicking Select or pressing ENTER will
	// then select the path and dismiss the dialog.

	// Of course, this won't work if initialdir is invalid or empty, and you'll be back in the
	// situation of the ex combo edit being empty (and having to click Select twice to pick
	// the initial directory - once to populate the ex combo edit and again to accept it).

	//DEVNOTE - if initialdir is empty or invalid, the OS reads the registry to determine what
	//		initial directory the file dialog will use. On XP, this is a set of keys contained in
	//			HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\ComDlg32.
	//		\OpenSaveMRU contains strings paths for various file extensions.
	//		\LastVisitedMRu, however, contains ITEMIDLISTs in REG_BINARY format, and those
	//			might actually be the values needed here (meaning you'll need to use the
	//			Shell ITEMIDLIST APIs to use those values).
	//		It's just not worth the effort for me, but I hope that info helps anyone who wants to try it.
}

CBrowseForFolderDialog::~CBrowseForFolderDialog()
{
}

//INT_PTR CBrowseForFolderDialog::DoModal() //REFACTOR - not sure this has any effect
//{
//	//DEVNOTE - according to the documentation, OFN_NOCHANGEDIR is ineffective for OpenFileName,
//	//		which is how we're using this dialog. If this dialog is extended for saving,
//	//		then remember that this action is not needed for saving, just opening.
//	auto cwd = Paths::GetCWD();
//	auto retval = CFileDialog::DoModal();
//	if (m_ofn.Flags & OFN_NOCHANGEDIR) {
//		auto newcwd = Paths::GetCWD();
//		if (newcwd != cwd)
//			SetCurrentDirectory(cwd);
//	}
//	return retval;
//}


BEGIN_MESSAGE_MAP(CBrowseForFolderDialog, CFileDialog)
END_MESSAGE_MAP()


// CBrowseForFolderDialog message handlers

namespace {
	const LPCSTR FILENEWLABEL = "&Folder to open";

	CString GetCDNText(HWND hwnd, UINT message)
	{
		CHAR szfile[_MAX_PATH + 1] = { 0 };
		SendMessage(hwnd, message, _MAX_PATH, (LPARAM)szfile);
		return CString(szfile);
	}

	void UpdateCDNDisplay(HWND hparent)
	{
		// Ignore the file that's picked or typed and put the current folder name in the comboedit.
		// Then write a message with the selected path into the caption to inform the user.
		auto folderpath = ::GetCDNText(hparent, CDM_GETFOLDERPATH);
		::SetWindowText(::GetDlgItem(hparent, cmb13), Paths::GetFileName(folderpath));

		auto caption = "Click Select to choose '" + Paths::GetFileName(folderpath) + "'";
		::SetWindowText(hparent, caption);
	}
}

// This hook function handles messages sent to the file dialog
UINT_PTR CBrowseForFolderDialog::OFNHookFolderSearch(HWND hwnd, UINT msg, WPARAM, LPARAM lparam)
{
	// This hook is only set for pre-Vista systems that use the old-style file dialog.
	if (msg == WM_NOTIFY) {
		auto hparent = ::GetParent(hwnd);
		auto nmhdr = reinterpret_cast<LPNMHDR>(lparam);
		ASSERT(nmhdr != nullptr);
		switch (nmhdr->code) {
		case CDN_INITDONE:
		{
			::SendMessage(hparent, CDM_HIDECONTROL, stc2, 0);
			::SendMessage(hparent, CDM_HIDECONTROL, cmb1, 0);
			::SendMessage(hparent, CDM_SETCONTROLTEXT, stc3, (LPARAM)FILENEWLABEL);
			::SendMessage(hparent, CDM_SETCONTROLTEXT, IDOK, (LPARAM)"&Select");

			//DEVNOTE - Don't attempt to set the text of the cmb13 (the filenamne combo edit) here.
			//		That's set according the file dialog's to m_ofn.lpstrFile; see the constructor
			//		for details on how we set that up when the dialog first appears.
			break;
		}

		case CDN_SELCHANGE:
			UpdateCDNDisplay(hparent);
			break;

		case CDN_FOLDERCHANGE:
			UpdateCDNDisplay(hparent);
			break;
		}
	}
	else if (msg == WM_NCDESTROY) {
		// For OFN_EXPLORER | OFN_ENABLEHOOK dialogs, the system destroys the file dialog resource but
		// CFileDialog::DoModal() neglects to NULL pthreadstate->m_pAlternateWndInit afterwards.
		// That causes an unneccesary ASSERT when the dialog is dismissed.
		// It isn't well documented, but that pointer can safely be NULLed here to avoid the assert
		// if the window has already been destroyed.
		auto pthreadstate = AfxGetThreadState();
		if (!::IsWindow(pthreadstate->m_pAlternateWndInit->m_hWnd)) {
			pthreadstate->m_pAlternateWndInit = nullptr;
		}
	}
	return FALSE;
}