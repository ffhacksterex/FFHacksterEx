#pragma once

#include <map>
#include <CString>
#include <HelpType.h>

class CHelp
{
public:
	CHelp();
	~CHelp();

	bool Init(HelpType type, CString helppathurl);
	bool Invoke(int parentid, int controlid, int parentaliasid = -1);

	static bool ShowInBrowser(CString url);

private:
	HelpType m_helptype = HelpType::Invalid;
	CString m_helpurl;
	CString m_configpath;

	bool SetCfg(HelpType type, CString configpath, CString helppathurl);

	static CString GetUserBrowserProgId();
	static CString GetBrowserCmd(CString progid);
	static std::pair<CString, CString> SplitBrowserCmd(CString cmd);
	static bool OutputToIE(CString path);
	static bool OutputToShell(CString path);
	static bool InvokeBrowser(CString url);
};
