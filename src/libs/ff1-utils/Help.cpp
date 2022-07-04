#include "stdafx.h"
#include "Help.h"
#include "FFBaseApp.h"
#include <atlbase.h>
#include <path_functions.h>
#include <ini_functions.h>
#include <rc_functions.h>
#include <string_functions.h>

CHelp::CHelp()
{
}

CHelp::~CHelp()
{
}

//N.B. - as of 7/4/2022, while all HelpTypes are supported at the code level,
//		the UI does not yet expose a way to change them. This was done
//		intentionally to reduce confusion until a good UI system to manage
//		the options is developed.
//		Currently, only HelpType::LocalApp (the AppSettings default) is used.
bool CHelp::Init(HelpType type, CString helppathurl)
{
		ASSERT((int)type >= HelpType::Invalid);
		ASSERT((int)type < HelpType::Last_);
	auto set = false;
	switch (type) {
		case HelpType::None:
			// set type to none, empty the path
			set = SetCfg(type, "", "");
			break;
		case HelpType::LocalApp:
			// help is the /Help folder, config is beside the app exe
			set = SetCfg(type,
				Paths::Combine({ Paths::GetProgramFolder(), "Help", Paths::GetProgramName() + ".helpconfig"}),
				Paths::Combine({ Paths::GetProgramFolder(), "Help" }));
			break;
		case HelpType::Folder:
			// help is a specified folder, and the config is in that folder
			//set = SetCfg(type, Paths::Combine({ helppathurl, Paths::GetProgramName() + ".helpconfig" }), helppathurl);

			//DEVNOTE - amendment, let the user search for the helpconfg file, not the folder.
			set = SetCfg(type,
				Paths::Combine({ helppathurl, Paths::GetProgramName() + ".helpconfig"}),
				helppathurl);
			break;
		case HelpType::Web:
			// help is a web path, config is beside the app
			set = SetCfg(type, Paths::Combine({ Paths::GetProgramFolder(), Paths::GetProgramName() + ".helpconfig" }), helppathurl);
			break;
	}
	if (!set) {
		ErrorHere << "Invalid help type specified '" << GetHelpTypeName(type) << "': the help configuration was not changed." << std::endl;
		return false;
	}
	return set;
}

namespace {
	CString Combine(CString base, CString parentpath, CString childpath, PathCombineFunc combine)
	{
		CString url;
		if (childpath.IsEmpty())
			url = combine({ base, parentpath });
		else if (childpath[0] == '#')
			url = combine({ base, parentpath + childpath });
		else
			url = combine({ base, childpath });
		return url;
	}

	bool DoesFileExist(CString path)
	{
		// strip the anchor if there is one
		auto anc = path.Find('#');
		const auto & checkpath = (anc != -1) ? path.Left(anc) : path;
		return Paths::FileExists(checkpath);
	}
}

//DEVNOTE - default behavior is for context-sensitive help (i.e. for a specific control)
//		to fall back to the parent (i.e. the dialog) if the context isn't tied to a help item.
//		There's currently no option to change this behavior.

#define LogThenFail(msg) (ErrorHere << msg << std::endl, false)

bool CHelp::Invoke(int parentid, int controlid, int parentaliasid)
{
	if (this->m_helptype == HelpType::None) return false;
	if (this->m_helptype == HelpType::Invalid) return ErrorReturn("Cannot use an invalid help type.", false);
	if ((int)this->m_helptype >= (int)HelpType::Last_) return ErrorReturn("Help type out of range.", false);

	auto ffapp = AfxGetFFBaseApp();
	//REFACTOR - if it works, use ErrorReturnFalse et al in other places.
	if (ffapp == nullptr) return ErrorReturnFalse("The app interface is inaccessible."); //false;
	if (parentid == 0) return ErrorReturnFalse("No help ID was specified."); //false;
	if (!Paths::FileExists(m_configpath)) return ErrorReturnFalse("The help config file cannot be found."); //false;
	if (m_helpurl.IsEmpty()) return ErrorReturnFalse("No help path or URL was specified."); //false;

	const auto & themap = ffapp->GetIdDlgDataMap();
	const auto iter = themap.find(parentid);
	auto inmap = iter != cend(themap);
	auto csparent = inmap ? iter->second.name : "";
	auto rcname = inmap ? iter->second.rcname : "";
	auto cscontrol = (controlid != -1) ? Rc::GetControlNameFromId(AfxGetResourceHandle(), rcname, csparent, controlid) : "";

	// If the parent is empty, we can't process anything here
	if (csparent.IsEmpty()) {
		ErrorHere << "Can't invoke help, not help item found for parent ID '" << parentid << "'." << std::endl;
		return false;
	}

	// If there's an alias name, use it in place of the parent name.
	if (parentaliasid != -1 && parentid != parentaliasid) {
		CString alias;
		if (!alias.LoadString(AfxGetResourceHandle(), parentaliasid)) {
			// Since the alias and original are treated as unrelated, don't fall back to the original
			ErrorHere << "Unable to find id for alias " << parentaliasid << std::endl;
			return false;
		}
		csparent = alias;
	}

	// Look up the values in the help config for the parent and (if applicable) child control
	auto rawparentpath = Ini::TrimIniValue(Ini::ReadIni(m_configpath, csparent, csparent, ""));
	if (rawparentpath.IsEmpty()) {
		ErrorHere << "Can't find a help path for this dialog: " << (LPCSTR)csparent << std::endl;
		return false;
	}

	// Parent might have an anchor, split it for now
	auto parentparts = Strings::split(rawparentpath, "#");
	auto parentpath = parentparts[0];
	auto parentanchor = parentparts.size() > 1 ? "#" + parentparts[1] : "";
	auto isfile = m_helptype == HelpType::LocalApp || m_helptype == HelpType::Folder;
	auto isweb = m_helptype == HelpType::Web;

	auto fnCombine = isfile ? Paths::Combine : Urls::Combine;
	auto childpath = Ini::TrimIniValue(Ini::ReadIni(m_configpath, csparent, cscontrol, ""));
	CString base = isfile ? Paths::RebaseToAppPath(m_helpurl) : m_helpurl;
	auto childstem = !childpath.IsEmpty() ? childpath : parentanchor;
	auto url = Combine(base, parentpath, childstem, fnCombine);

	if (url.IsEmpty()) {
		ErrorHere << "No URL specified. can't launch a help file." << std::endl;
		return false;
	}

	// Normalize the path/URL
	url.MakeLower();
	url.Replace("\\\\", "\\");
	url.Replace("//", "/");
	if (isweb) {
		// The web site doesn't use .html file extensions for pages.
		url.Replace(".html", "");
	}

	auto found = isfile ? DoesFileExist(url) : Urls::UrlIsAccessible(url);
	auto allowworkaround = false;
	if (!found && allowworkaround) {
		// Second chance when debugging, try to use a copy in the solution folder
		BOOL bToDebugger = IsDebuggerPresent();
		if (bToDebugger) {
			auto scroot = Paths::GetAncestorFolder(Paths::GetCWD(), 3);
			auto scpath = Paths::Combine({ scroot, "Help" });
			LogHere << "Second Chance Help CWD is " << (LPCSTR)scpath << std::endl;
			url = Combine(scpath, parentpath, childstem, Paths::Combine);
			// Since we're using a local path, force isfile to true
			isfile = true;
			found = DoesFileExist(url);
			LogHere << "  -> Second Chance file = " << (LPCSTR)url << std::endl;
		}
	}

	if (!found) {
		if (isfile)
			ErrorHere << "Can't find specified help file path '" << (LPCSTR)url << "'." << std::endl;
		else
			ErrorHere << "Can't access specified help file url '" << (LPCSTR)url << "'." << std::endl;
		AfxMessageBox("Unable to find help article " + Paths::Combine({ parentpath, childstem }) + ".");
		return false;
	}
	else if (isfile) {
		url = Urls::FilePathToUrl(url);
	}
	return ShowInBrowser(url);
}

//STATIC
bool CHelp::ShowInBrowser(CString url)
{
	try {
		return InvokeBrowser(url);
	}
	catch (std::exception& ex) {
		ErrorHere << "Unable to launch help page due to an exception: " << ex.what() << std::endl;
	}
	catch (...) {
		ErrorHere << "Unable to launch help page due to an unknown exception." << std::endl;
	}
	return false;
}

bool CHelp::SetCfg(HelpType type, CString configpath, CString helppathurl)
{
	if ((int)type >= HelpType::Invalid && (int)type < HelpType::Last_) {
		m_helptype = type;
		m_configpath = configpath;
		m_helpurl = helppathurl;
		return true;
	}
	return false;
}

//STATIC
CString CHelp::GetUserBrowserProgId()
{
	//REFACTOR - put the reg key and value names in the app config
	static const CString keypath = R"_(Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.html\UserChoice)_";
	static const LPCSTR szkeyname = "ProgId";
	CString progid;

	try {
		bool foundSetting = false;
		CRegKey key;

		if (key.Open(HKEY_CURRENT_USER, keypath, KEY_READ) == ERROR_SUCCESS)
			foundSetting = true;
		else if (key.Open(HKEY_LOCAL_MACHINE, keypath, KEY_READ) == ERROR_SUCCESS)
			foundSetting = true;

		if (foundSetting) {
			CHAR tszkey[_MAX_PATH] = { 0 };
			ULONG chars = _countof(tszkey);
			key.QueryStringValue(szkeyname, tszkey, &chars);
			key.Close();
			progid = tszkey;
		}
	}
	catch (...) {
		ErrorHere << "Unable to get user's default browser progid" << std::endl;
	}
	return progid;
}

//STATIC
CString CHelp::GetBrowserCmd(CString progid)
{
	CString cmd;
	try {
		CString keypath;
		keypath.Format(R"_(%s\shell\open\command)_", (LPCSTR)progid);

		CRegKey key;
		if (key.Open(HKEY_CLASSES_ROOT, keypath, KEY_READ) == ERROR_SUCCESS) {
			TCHAR tszvalue[_MAX_PATH * 2] = { 0 };
			ULONG chars = _countof(tszvalue) - 1;
			if (key.QueryStringValue(nullptr, tszvalue, &chars) == ERROR_SUCCESS) {
				cmd = tszvalue;
			}
		}
	}
	catch (...) {
		ErrorHere << "Unable to get browser cmd from progid '" << (LPCSTR)progid << "'" << std::endl;
	}
	return cmd;
}

//STATIC
std::pair<CString, CString> CHelp::SplitBrowserCmd(CString cmd)
{
	CString exe;
	CString rest;
	auto quote = cmd.Find('"');
	if (quote == 0) {
		auto endquote = cmd.Find('"', 1);
		exe = cmd.Mid(1, endquote - 1);
		rest = cmd.Mid(endquote + 1);
	}
	else {
		auto space = cmd.Find(' ');
		exe = space != -1 ? cmd.Left(space) : cmd;
		rest = space != -1 ? cmd.Mid(space + 1) : "";
	}

	return { exe, rest };
}

//STATIC
bool CHelp::OutputToIE(CString path)
{
	//DEVNOTE - default IE behavior is to display an error message if the URL can't be resolved.
	//		I'm not going to take any action to avoid that at this point.
	//DEVNOTE - didn't have time to look into Automation or similar options for other
	//		browsers, and there seems to be no way to generically open a browser in
	//		an existing tab externally (e.g. from a program, not another page in the web browser).

	IShellWindows* psw;
	HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL, IID_IShellWindows, (void**)&psw);
	if (!SUCCEEDED(hr))
		return false;

	IWebBrowser2* pBrowser2 = nullptr;
	long count = 0;
	hr = psw->get_Count(&count);
	bool found = false;
	if (SUCCEEDED(hr)) {
		// Loop through the open shell windows until we find one for IE.
		for (long i = 0; i < count && !found; ++i) {
			_variant_t va = (long)i;
			IDispatch* spDisp = nullptr;
			hr = psw->Item(va, &spDisp);
			hr = spDisp->QueryInterface(IID_IWebBrowserApp, (void**)&pBrowser2);
			if (SUCCEEDED(hr)) {
				BSTR bsname;
				pBrowser2->get_FullName(&bsname);
				CString csname(bsname);
				if (csname.Find("IEXPLORE") == -1)
					pBrowser2->Release();
				else
					found = true;
			}
		}
		psw->Release();
	}

	// Create a new IE instance if we didn't find one.
	if (!found)
		hr = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pBrowser2);

	bool visited = false;
	if (SUCCEEDED(hr)) {
		// We have an IE instance, now open the URL with the "_self" target to reuse an open tab.
		VARIANT vEmpty;
		VariantInit(&vEmpty);

		_variant_t Flag = (int)(found ? 0x800 : 0);
		_variant_t URL = (LPCSTR)path;
		_variant_t Target = "_self";
		hr = pBrowser2->Navigate2(&URL, &Flag, &Target, &vEmpty, &vEmpty);
		if (SUCCEEDED(hr)) {
			pBrowser2->put_Visible(TRUE);
			visited = true;

			//DEVNOTE - if we had to create an instance, then the anchor won't be visited.
			//		If there's an anchor in the URL, simply navigate to the browser a second time.
			//		Since the initial navigation worked, don't bother recording the return value;
			//		if it fails here, at least the main page has displayed.
			if (!found && path.Find('#') != -1)
				pBrowser2->Navigate2(&URL, &Flag, &Target, &vEmpty, &vEmpty);
		}
		else {
			pBrowser2->Quit();
		}
		pBrowser2->Release();
	}
	return visited;
}

//STATIC
bool CHelp::OutputToShell(CString path)
{
	// Fall back to the browser opened using the "open" verb.
	// This executes the browser's default behavior (could be new window, new tab, etc.).
	auto hreturn = (LONG_PTR)ShellExecute(NULL, "open", path, nullptr, nullptr, SW_SHOWNORMAL);
	return hreturn > 32;
}

//STATIC
bool CHelp::InvokeBrowser(CString url)
{
	// Get the browser and invoke the file using the command in the registry.
	// IE supports an automation interface, so it's singled out here.
	auto progid = GetUserBrowserProgId();
	if (progid == "htmlfile")
		return OutputToIE(url);

	auto browsercmd = GetBrowserCmd(progid);
	if (browsercmd.Find("%1") != -1) {
		CString exe, rest;
		std::tie(exe, rest) = SplitBrowserCmd(browsercmd);
		exe = Paths::QuoteUrl(exe);
		//DEVNOTE - quoting the url seems to cause a problem, so if there are spaces
		//		in the url path then that might also cause an problem.
		//		However, these would usually be uuencoded (e.g. space = %20)
		//		so it might never be a real issue.
		//url = Paths::QuoteUrl(url);
		rest.Replace("%1", url);

		auto hreturn = (LONG_PTR)ShellExecute((LONG_PTR)NULL, "open", exe, rest, nullptr, SW_SHOWNORMAL);
		return hreturn > 32;
	}
	else {
		return OutputToShell(url);
	}
}