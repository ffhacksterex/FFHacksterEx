#include "stdafx.h"
#include "Editors2.h"
#include "ini_functions.h"
#include "pair_result.h"
#include "path_functions.h"
#include "string_functions.h"

#include "FFHacksterProject.h"
#include "LoadLibraryHandleScope.h"

#include <filesystem>
#include <iterator>
#include <memory>

using namespace Ini;
using namespace Io;
using namespace Strings;

//STDFILESYS - see path_functions.cpp for more info
#if _MSVC_LANG  <= 201402L
namespace fsys = std::experimental::filesystem;
#else
namespace fsys = std::filesystem;
#endif

namespace Editors2
{
	#define EDITORSINIPREFIX "EDITOR-"

	#define INVOKEFUNCNAME "Invoke"
	#define GETEDITORFUNCNAME "GetEditor"
	#define GETALLEDITORSFUNCNAME "GetAllEditors"


	std::pair<EditErrCode, CString> CallExtensionInvoke(EditorInterop & ei, InvokeFunc func)
	{
		std::unique_ptr<char[]> ptext(func(&ei));
		auto responsepair = ExtractResponse(ptext.get());
		return responsepair;
	}

	CString GetInvokeResultString(std::pair<EditErrCode, CString> result)
	{
		if (!IsResponseSuccess(result.first))
			return result.second;

		if (result.first == EditErrCode::ReloadProject)
			return "reload";

		return ""; // success is an empty response string
	}



	// ****************************************************************
	// sEditorInfo

	bool sEditorInfo::operator==(const sEditorInfo & rhs) const
	{
		return
			rhs.show == show &&
			rhs.name == name &&
			rhs.displayname == displayname &&
			rhs.modulepath == modulepath &&
			rhs.description == description;
	}

	bool sEditorInfo::operator!=(const sEditorInfo & rhs) const
	{
		return !operator==(rhs);
	}



	// ****************************************************************
	// sEditorLink

	const sEditorInfo * sEditorLink::GetSelectedNode() const
	{
		return selindex < nodes.size() ? &nodes[selindex] : nullptr;
	}

	sEditorInfo * sEditorLink::GetSelectedNode()
	{
		return selindex < nodes.size() ? &nodes[selindex] : nullptr;
	}

	bool sEditorLink::operator==(const sEditorLink & rhs) const
	{
		return
			rhs.name == name &&
			rhs.selindex == selindex &&
			rhs.nodes == nodes;
	}

	bool sEditorLink::operator!=(const sEditorLink & rhs) const
	{
		return !operator==(rhs);
	}



	// ****************************************************************
	// CEditor

	CEditor::CEditor()
	{
	}

	CEditor::~CEditor()
	{
	}

	CEditor::CEditor(const sEditorInfo & info)
	{
		live = (info.modulepath == EDITORPATH_BUILTIN) || Paths::FileExists(info.modulepath);

		name = info.name;
		displayname = info.displayname;
		modulepath = info.modulepath;
		description = info.description;
	}

	sEditorInfo CEditor::GetInfo() const
	{
		sEditorInfo info;
		info.show = live;
		info.name = name;
		info.displayname = displayname;
		info.modulepath = modulepath;
		info.description = description;
		return info;
	}

	// If the modulepath is missing, the editor is reported as broken until successfully reloaded.
	bool CEditor::Broken() const
	{
		return !live;
	}

	CString CEditor::Init(const char * projectini, EditorAllocatorFunc allocator) const
	{
		if (implfunc != nullptr) return{};

		LoadLibraryHandleScope module(LoadLibrary(modulepath));
		if (module == NULL)
			return GetErrorMessage(EditErrCode::NoModule);

		auto func = (InvokeFunc)GetProcAddress(module, INVOKEFUNCNAME);
		if (func == nullptr)
			return GetErrorMessage(EditErrCode::NoInvokeFunction);

		EditorInterop ei = { 0 };
		ei.allocator = allocator;
		ei.command = INVOKE_INIT;
		ei.projectini = projectini;
		auto result = CallExtensionInvoke(ei, func);
		return GetInvokeResultString(result);
	}

	CString CEditor::Edit(const char * projectini, EditorAllocatorFunc allocator) const
	{
		CString command = INVOKE_EDIT;

		if (implfunc != nullptr) {
			bool succeeded = implfunc(command, name, "");
			return succeeded ? "" : "the internal editor failed.";
		}
		
		// Even if the editor is live, the LoadLibrary call could fail for other reasons
		LoadLibraryHandleScope module(LoadLibrary(modulepath));
		if (module == NULL)
			return GetErrorMessage(EditErrCode::NoModule);

		auto func = (InvokeFunc)GetProcAddress(module, INVOKEFUNCNAME);
		if (func == nullptr)
			return GetErrorMessage(EditErrCode::NoInvokeFunction);

		EditorInterop ei = { 0 };
		ei.allocator = allocator;
		ei.command = command;
		ei.name = name;
		ei.projectini = projectini;
		auto result = CallExtensionInvoke(ei, func);
		return GetInvokeResultString(result);
	}

	CString CEditor::Rclick(const char * projectini, EditorAllocatorFunc allocator, HWND howner, int x, int y) const
	{
		CString command = INVOKE_RCLICK;
		CString params;
		params.Format("0x%08X %d %d", howner, x, y);

		if (implfunc != nullptr) {
			implfunc(command, name, params);
			return FormatEditorResponse(EditErrCode::Success, "");
		}

		LoadLibraryHandleScope module(LoadLibrary(modulepath));
		if (module == NULL)
			return GetErrorMessage(EditErrCode::NoModule);

		auto func = (InvokeFunc)GetProcAddress(module, INVOKEFUNCNAME);
		if (func == nullptr)
			return GetErrorMessage(EditErrCode::NoInvokeFunction);

		EditorInterop ei = { 0 };
		ei.allocator = allocator;
		ei.command = command;
		ei.name = name;
		ei.projectini = projectini;
		ei.params = params;
		auto result = CallExtensionInvoke(ei, func);
		return GetInvokeResultString(result);
	}



	// ****************************************************************
	// INTERNAL IMPEMENTATION

	// Make this std::function, since it will most often be a lambda (instead of a raw function pointer)
	using GetModuleTextFunc = std::function<char*(HMODULE module)>;


	CString GetModuleText(CString modulepath, GetModuleTextFunc func)
	{
		if (!Paths::FileExists(modulepath)) return{};
		if (Paths::GetFileExtension(modulepath) != ".dll") return{};
		LoadLibraryHandleScope module(LoadLibrary(modulepath));
		if (module == NULL) return{};

		std::unique_ptr<char[]> ptext(func(module));
		auto responsepair = ExtractResponse(ptext.get());
		return IsResponseSuccess(responsepair.first) ? responsepair.second : "";
	}

	char* LocalAllocator(int64_t bytes)
	{
		return new char[(size_t)bytes];
	}


	namespace {
		sEditorInfoVector GetBuiltinEditorInfos()
		{
			const static sEditorInfoVector infos{
				{ true, ARMOREDIT, "Armor", EDITORPATH_BUILTIN, "Edit armor attributes, prices, and permissions." },
				{ true, ATTACKSEDIT, "Special Attacks", EDITORPATH_BUILTIN, "Edit special attacks used exclusively by enemies." },
				{ true, BATTLESEDIT, "Battles", EDITORPATH_BUILTIN, "Edit enemy formations, palettes, and properties for battles. Right-click for settings." },
				{ true, CLASSESEDIT, "Classes", EDITORPATH_BUILTIN, "Edit class info, including level-up data. Right-click for settings." },
				{ true, ENEMIESEDIT, "Enemies", EDITORPATH_BUILTIN, "Edit enemy stats and AI patterns. Right-click for settings." },
				{ true, SHOPSEDIT, "Shops", EDITORPATH_BUILTIN, "Edit item lists and prices for all shops." },
				{ true, MAGICEDIT, "Magic", EDITORPATH_BUILTIN, "Edit magic spells. Right-click for settings." },
				{ true, WEAPONSEDIT, "Weapons", EDITORPATH_BUILTIN, "Edit weapon attiribyes, prices, graphics, and permissions." },
				{ true, TEXTEDIT, "Text", EDITORPATH_BUILTIN, "Edit text in various parts of the game. Right-click for settings." },
				{ true, STARTITEMSEDIT, "Starting Items", EDITORPATH_BUILTIN, "Edit starting items and events for a new game." },
				{ true, DIALOGUEEDIT, "Sprite Dialogue", EDITORPATH_BUILTIN, "Edit sprite dialog handlers. Right-click for settings." },
				{ true, STDMAPSEDIT, "Standard Maps", EDITORPATH_BUILTIN, "Edit standard (local) maps." },
				{ true, OVERWORLDEDIT, "Overworld Map", EDITORPATH_BUILTIN, "Edit the overworld map." },
				{ true, PARTYSETUPEDIT, "Party Setup", EDITORPATH_BUILTIN, "Edit the new game party configuration options." },
			};
			return infos;
		}
	}



	// ****************************************************************
	// EDITOR PARSING FUNCTIONS

	// Forward declarations
	CString GetKnownErrorMessage(EditErrCode ec);


	// Responses from the DLL come in this format:
	//		0x<code>:responsetext
	// If not in that format, some error has occurred.
	// This function breaks the text and returns a code.
	// If it can;t, it formats an error message stating so.

	#define EDITRESPONSEFORMAT "EC0x%08zX%c" //REFACTOR - ensure that the z prefix works (C++11)

	// Responses from the DLL come in this format: 0x<code>:responsetext
	CString FormatEditorResponse(EditErrCode ec, CString errdesc)
	{
		// if we have an unknown error code, we don't want to print "unknown error code (unable to do X)".
		auto codemsg = GetKnownErrorMessage(ec);
		if (!errdesc.IsEmpty()) {
			// put the message in parens if codemsg isn't empty
			if (!codemsg.IsEmpty()) codemsg.AppendFormat(" (%s)", (LPCSTR)errdesc);
			else codemsg = errdesc;
		}

		CString msg;
		msg.Format(EDITRESPONSEFORMAT, (size_t)ec, ':');
		msg.Append(codemsg);
		return msg;
	}

	// Breaks a response into a pair <errcode, responsetext>. Synthesizes a response if an error is detected.
	std::pair<EditErrCode, CString> ExtractResponse(const char* responsetext)
	{
		if (responsetext == nullptr || *responsetext == 0)
			return{ EditErrCode::NoResponse, "no response was provided" };

		size_t errcode = 0;
		char colon = 0;
		auto result = std::sscanf(responsetext, EDITRESPONSEFORMAT, &errcode, &colon);
		if (result != 2 || colon != ':')
			return{ EditErrCode::BadResponseFormat, "bad response format, can't determine the return code" };

		auto code = static_cast<EditErrCode>(errcode); //HACK - casting could lead to undefined behavior if errcode is out of enum range
		auto data = strchr(responsetext, ':') + 1;
		return{ code, data };
	}

	CString GetKnownErrorMessage(EditErrCode ec)
	{
		switch (ec) {
		case EditErrCode::Success:
		case EditErrCode::ReloadProject:
			return{};

		case EditErrCode::NoModule:  return "no module specified";
		case EditErrCode::NoInvokeFunction: return "no Invoke function found";
		case EditErrCode::NoAllocator:  return "no allocator specified";
		case EditErrCode::NoResponse: return "no response received";
		case EditErrCode::BadResponseFormat: return "bad response format";
		case EditErrCode::NotFound: return "a resource wasn't found";
		case EditErrCode::Failure: return "general failure";
		}
		return{};
	}

	CString GetErrorMessage(EditErrCode ec)
	{
		auto msg = GetKnownErrorMessage(ec);
		if (msg.IsEmpty())
			msg = "unknown error code";
		return msg;
	}

	bool IsResponseSuccess(EditErrCode ec)
	{
		return ec == EditErrCode::Success || ec == EditErrCode::ReloadProject;
	}

	// Returns a list of zero or more editor info structs built from 'text'.
	// 'text' is a pipe-delimited, newline-record-separated string, with the fields laid out like this:
	//		show|name|displayname|modulepath|description\n
	//		show|name|displayname|modulepath|description\n
	//		...
	// the show field is represented by a single character, t = true, f = false
	sEditorInfoVector TextToEditorInfos(CString text)
	{
		sEditorInfoVector infos;
		auto lines = split(text, "\n");
		for (auto line : lines) {
			try {
				auto fields = split(line, "|");
				sEditorInfo info;
				info.show = fields[0] == 't';
				info.name = fields[1];
				info.displayname = fields[2];
				info.modulepath = fields[3];
				info.description = fields[4];
				infos.push_back(info);
			}
			catch (...) {
				ErrorHere << "unable to create EditorInfo from line " << line << std::endl;
			}
		}
		return infos;
	}

	// Returns a string with the editor's fields. The string is laid out with these fields:
	//		show|name|displayname|modulepath|description
	// The show field is represented by a single character: t = true, f = false
	CString EditorInfoToText(const sEditorInfo & info)
	{
		CString text;
		text.Format("%s|%s|%s|%s|%s",
			info.show ? "t" : "f",
			(LPCSTR)info.name, (LPCSTR)info.displayname, (LPCSTR)info.modulepath, (LPCSTR)info.description);
		return text;
	}

	CString EditorInfoToText(const sEditorInfoVector & infos)
	{
		CString result;
		for (const auto & info : infos) {
			if (!result.IsEmpty()) result += "\n";
			result += EditorInfoToText(info);
		}
		return result;
	}

	sEditorInfoVector GetDefaultEditorInfos()
	{
		return GetBuiltinEditorInfos();
	}

	// Returns a list of zero or more editor info structs built from 'text'.
	CEditorVector TextToEditors(CString text)
	{
		CEditorVector editors;
		auto infos = TextToEditorInfos(text);
		for (const auto & info : infos) {
			if (!info.show) continue;

			// A check for the existence of the module path is made, but we assume that the module was previously loaded.
			editors.push_back(CEditor(info));
		}
		return editors;
	}

	sEditorInfoVector GetEditorInfo(CString modulepath, CString name)
	{
		const auto ReadNamedEditor = [name](HMODULE module) -> char* {
			auto invoke = (InvokeFunc)GetProcAddress(module, INVOKEFUNCNAME);
			if (invoke == nullptr) return{};

			EditorInterop ei = { 0 };
			ei.allocator = LocalAllocator;
			ei.command = INVOKE_GETEDITOR;
			ei.name = name;
			return invoke(&ei);
		};

		sEditorInfoVector infos = TextToEditorInfos(GetModuleText(modulepath, ReadNamedEditor));
		ASSERT(infos.size() < 2); // it should either find one entry or nothing
		if (infos.size() > 1) infos.resize(1);
		return infos;
	}

	sEditorInfoVector GetEditorInfos(CString modulepath)
	{
		const auto ReadAllEditors = [](HMODULE module) -> char* {
			auto invoke = (InvokeFunc)GetProcAddress(module, INVOKEFUNCNAME);
			if (invoke == nullptr) return{};

			EditorInterop ei = { 0 };
			ei.allocator = LocalAllocator;
			ei.command = INVOKE_GETALLEDITORS;
			return invoke(&ei);
		};

		auto infos = TextToEditorInfos(GetModuleText(modulepath, ReadAllEditors));
		return infos;
	}

	//REFACTOR - THIS SHOULD RECEIVE A CFFHacksterProject, NOT THE INI PATH TO THE PROJECT
	//		Even if a temp CFFHacksterProject has to be created, that will be
	//		more flexible than manipulating the projectini path here.
	//		Also allows using the path properties instead of the RefDir functions.
	sEditorInfoVector GetAllEditorInfos(CString projectini)
	{
		std::vector<CString> foldersets = {
			 Paths::GetProgramFolder(),
			 Paths::GetCWD(),
			 Paths::GetDirectoryPath(projectini)
		};
		auto addlmodfolder = CFFHacksterProject::GetIniRefDir(projectini, FFHREFDIR_AddlModFolder);
		if (Paths::DirExists(addlmodfolder))
			foldersets.push_back(addlmodfolder);

		// Collect the editors from each location, then build the links from that list
		sEditorInfoVector infos = GetBuiltinEditorInfos();

		for (const auto& fset : foldersets) {
			CString folder = fset;
			auto dlls = Paths::GetFiles(folder, "*.dll", false);
			for (const auto& dll : dlls) {
				try {				
					auto newinfos = GetEditorInfos(dll);
					append(infos, newinfos);
				} catch (std::exception& ex) {
					ErrorHere << "Unable to retrieve editors from '" << (LPCSTR)dll << "\n"
						<< " -> " << ex.what() << std::endl;
				}
				catch (...) {
					ErrorHere << "Unknown exception: unable to retrieve editors from '" << (LPCSTR)dll << std::endl;
				}
			}
		}

		return infos;
	}

	sEditorLinkVector BuildEditorLinks(const sEditorInfoVector & infos)
	{
		sEditorLinkVector links;
		for (const auto & info : infos) {
			auto itlink = std::find_if(begin(links), end(links), [info](const sEditorLink & link) { return info.name == link.name; });
			if (itlink == end(links)) {
				// There's no link for this editor name, so add one and make this info its first node
				sEditorLink newlink;
				newlink.name = info.name;
				newlink.selindex = 0;
				newlink.nodes.push_back(info);
				links.push_back(newlink);
			}
			else {
				// If this info's modulepath isn't part of this link, add it as a node (modulepaths must be unique filepaths).
				auto & link = *itlink;
				if (!std::any_of(cbegin(link.nodes), cend(link.nodes), [info](const sEditorInfo & node) { return info.modulepath == node.modulepath;} )) {
					link.nodes.push_back(info);
				}
			}
		}
		return links;
	}

	// This will select links matching the supplied current editors (if any).
	// Any single-node NON-BUILTIN links will be unselected by default, such that the user can opt-in to the selection process.
	// It will also add a remove option to any node that doesn't have a built-in ootion AND lacks a remove node.
	void SelectCurrentEditorLinks(const sEditorInfoVector & cureditors, sEditorLinkVector & links)
	{
		// a consideration for NON-BUILTIN nodes:
		// if these nodes only have a single entry and are NOT represented by an editor, hide them.
		// This will allow the user to opt-in, instead of having to disable a bunch of editors for new projects.
		for (auto & link : links) {
			bool hasany = std::any_of(cbegin(cureditors), cend(cureditors), [link](const CEditor & ed) { return link.nodes.front().name == ed.name; });
			if (!hasany) {
				// This link isn't tied to a current editor, so turn it off by default.
				// If none of it's nodes are set to hidden, add a hidden node and select that one
				auto ithidden = std::find_if(begin(link.nodes), end(link.nodes), [](const auto & node) { return !node.show;});
				if (ithidden != end(link.nodes)) {
					link.selindex = std::distance(begin(link.nodes), ithidden);
				}
				else {
					// create a hidden node.
					link.nodes.push_back({ false, link.name, "--" + link.name, EDITORPATH_REMOVED, "Remove this editor" });
					link.selindex = link.nodes.size() - 1;
				}
			}
			else {
				// it's tied to an editor.
				// add a remove node if there are no builtin nodes.
				if (!std::any_of(cbegin(link.nodes), cend(link.nodes), [](const sEditorInfo & node) { return node.modulepath == EDITORPATH_BUILTIN; })) {
					link.nodes.push_back({ false, link.name, "--" + link.name, EDITORPATH_REMOVED, "Remove this editor" });
					// select the remove node if this link only has a single external node (i.e. we just added a second node)
					if (link.nodes.size() == 2)
						link.selindex = 1;
				}
			}
		}

		// Current editors should be turned on.
		// Note that this might activate an external only-child node that was deactivated by the previous loop, which is fine.
		for (const auto & editor : cureditors) {
			// if this editor name is represented by a link ...
			auto itlink = std::find_if(begin(links), end(links), [editor](const sEditorLink & link) { return editor.name == link.name;});
			if (itlink != end(links)) {
				// and this link has a node with this editor's modulepath (which is a unique filepath)...
				auto & nodes = itlink->nodes;
				auto itnode = std::find_if(begin(nodes), end(nodes), [editor](const sEditorInfo & info) { return editor.modulepath == info.modulepath; });
				if (itnode != end(nodes)) {
					// then set the link selindex to that node and force it visible (but don't change the status)
					itlink->selindex = std::distance(begin(nodes), itnode);
				}
			}
		}
	}

	sEditorInfoVector TakeSelectedEditorLinks(const sEditorLinkVector & links)
	{
		sEditorInfoVector editors;
		for (const auto & link : links) {
			auto * selnode = link.GetSelectedNode();
			if (selnode == nullptr) continue;
			if (!selnode->show) continue;

			// Don't take hidden editors, but if a shown editor is broken, the client can report it.
			editors.push_back(*selnode);
		}
		return editors;
	}

	sEditorInfoVector RemoveInvalidEditorInfos(sEditorInfoVector existingeditors, sEditorInfoVector * pinvalideditors)
	{
		const auto IsInvalid = [](const CEditor & ed) { return !ed.live; };

		sEditorInfoVector invalideditors;
		if (pinvalideditors == nullptr) pinvalideditors = &invalideditors;

		// copy the invalids to another vector, then remove them via remove_if/erase.
		std::copy_if(begin(existingeditors), end(existingeditors), std::back_inserter(*pinvalideditors), IsInvalid);
		auto iter = std::remove_if(begin(existingeditors), end(existingeditors), IsInvalid);
		existingeditors.erase(iter, end(existingeditors));

		return existingeditors;
	}

	sEditorInfoVector EditorsToInfos(const CEditorVector & editors)
	{
		sEditorInfoVector infos;
		for (const auto & editor : editors)
			infos.push_back(editor.GetInfo());
		return infos;
	}

	mfcstringvector InitializeExternalEditors(const CEditorVector & editors, CString projectini)
	{
		mfcstringvector failures;
		mfcstringset unique;
		for (const auto & editor : editors) {
			if (unique.find(editor.modulepath) != cend(unique)) continue;

			unique.insert(editor.modulepath);
			auto result = editor.Init(projectini, LocalAllocator);
			if (!result.IsEmpty())
				failures.push_back(result);
		}
		return failures;
	}


	//REFACTOR - THIS SHOULD RECEIVE A CFFHacksterProject, NOT THE INI PATH TO THE PROJECT
	//		Even if a temp CFFHacksterProject has to be created, that will be
	//		more flexible than manipulating the projectini path here.
	//		Also allows using the path properties instead of the RefDir functions.
	CString GetLevelFromPath(CString projectini, CString testpath)
	{
		CString testdir = Paths::GetDirectoryPath(testpath);

		if (testdir == EDITORPATH_BUILTIN) return EDITORLEVEL_BUILTIN;
		if (testdir == EDITORPATH_REMOVED) return EDITORLEVEL_REMOVED;
		if (!Paths::FileExists(testpath)) return EDITORLEVEL_MISSING;

		std::vector<std::pair<CString, CString>> foldersets = {
			{ Paths::GetProgramFolder(), "App "},
			{ Paths::GetCWD(), "Cwd", },
			{ Paths::GetDirectoryPath(projectini), "Project "},
			{ CFFHacksterProject::GetIniRefDir(projectini, FFHREFDIR_AddlModFolder), "ProjExtFolder" }
		};

		for (const auto & fset : foldersets) {
			if (fset.first == testdir)
				return fset.second;
		}

		return EDITORLEVEL_CUSTOM;
	}

	CString GetLevelFromPath(const CFFHacksterProject & project, CString testpath)
	{
		return GetLevelFromPath(project.ProjectPath, testpath);
	}

	CString GetEditorLiveStatus(bool show)
	{
		return show ? "show" : "hide";
	}

	bool IsEditorSectionName(CString keyname)
	{
		return keyname.Find(EDITORSINIPREFIX) == 0;
	}

	CString GetEditorSectionName(CString editorname)
	{
		return EDITORSINIPREFIX + editorname;
	}

	char* AllocReturnValue(EditorAllocatorFunc allocator, CString value)
	{
		size_t length = value.GetLength();
		char* buffer = allocator(length + 1);
		if (buffer != nullptr) {
			memset(buffer, 0, length + 1);
			strncpy(buffer, value, length);
		}
		return buffer;
	}

	CString GetEditorResponse(const sEditorInfoVector & infos, const char* name)
	{
		auto itinfo = std::find_if(cbegin(infos), cend(infos), [name](const sEditorInfo & info) { return info.name == name; });

		CString result;
		if (itinfo != cend(infos))
			result = FormatEditorResponse(EditErrCode::Success, EditorInfoToText(*itinfo));
		else
			result = FormatEditorResponse(EditErrCode::NotFound, "couldn't find an editor named " + CString(name));

		return result;
	}

	std::pair<EditErrCode, CString> RetrieveEditorAsResponse(const sEditorInfoVector & infos, const char* name)
	{
		auto itinfo = std::find_if(cbegin(infos), cend(infos), [name](const sEditorInfo & info) { return info.name == name; });

		if (itinfo != cend(infos))
			return{ EditErrCode::Success, EditorInfoToText(*itinfo) };

		return{ EditErrCode::NotFound, "couldn't find an editor named " + CString(name) };
	}

}