#pragma once

// CBrowseForFolderDialog

class CBrowseForFolderDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CBrowseForFolderDialog)

public:
	CBrowseForFolderDialog(CWnd * parent, CString initialdir, CString title = "Choose a folder");
	virtual ~CBrowseForFolderDialog();

	//virtual INT_PTR DoModal() override; //REFACTOR - not sure this has any effect

private:
	CBrowseForFolderDialog(CWnd * parent, CString initialdir, CString title, BOOL isvistaplus);

protected:
	CHAR m_filename[_MAX_PATH + 1];

	static UINT_PTR OFNHookFolderSearch(HWND hwnd, UINT msg, WPARAM, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};
