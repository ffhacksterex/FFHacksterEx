#pragma once

#include "EditorInterop.h"
#include <vector>
#include <functional>
#include "vector_types.h"
class CFFHacksterProject;

namespace Editors2
{

	enum class EditErrCode
	{
		Success = 0,
		ReloadProject,
		NoModule,
		NoInvokeFunction,
		NoAllocator,
		NoProject,
		NoResponse,
		BadResponseFormat,
		NotFound,
		ExceptionCaught,
		FileMissing,
		Failure,
	};

	CString FormatEditorResponse(EditErrCode ec, CString errdesc);
	std::pair<EditErrCode, CString> ExtractResponse(const char* responsetext);
	CString GetErrorMessage(EditErrCode ec);
	bool IsResponseSuccess(EditErrCode ec);


	struct sEditorInfo
	{
		bool show;
		CString name;
		CString displayname;
		CString modulepath;
		CString description;

		bool operator==(const sEditorInfo & rhs) const; // ... CStrings seem to require this
		bool operator!=(const sEditorInfo & rhs) const;
	};

	using sEditorInfoVector = std::vector<sEditorInfo>;

	struct sEditorLink
	{
		CString name;
		size_t selindex = (size_t)-1;
		sEditorInfoVector nodes;

		const sEditorInfo * GetSelectedNode() const;
		sEditorInfo * GetSelectedNode();

		bool operator==(const sEditorLink & rhs) const;
		bool operator!=(const sEditorLink & rhs) const;
	};

	using sEditorLinkVector = std::vector<sEditorLink>;

	struct sEditorIniRef
	{
		CString name;
		CString modulepath;
	};

	using sEditorIniRefVector = std::vector<sEditorIniRef>;

	using EditorImplFunc = std::function<bool(CString command, CString name, CString params)>;

	class CEditor
	{
	public:
		CEditor();
		~CEditor();
		CEditor(const sEditorInfo & info);

		bool live = false; // only set to true if modulepath is valid
		CString name;
		CString displayname;
		CString modulepath;
		CString description;
		EditorImplFunc implfunc = nullptr;

		sEditorInfo GetInfo() const;
		bool Broken() const;

		CString Init(const char* projectini, EditorAllocatorFunc allocator) const;
		CString Edit(const char* projectini, EditorAllocatorFunc allocator) const;
		CString Rclick(const char* projectini, EditorAllocatorFunc allocator, HWND howner, int x, int y) const;
	};

	using CEditorVector = std::vector<CEditor>;



	// ****************************************************************
	// Names for built-in editors

	#define ARMOREDIT       "armor"
	#define ATTACKSEDIT     "attacks"
	#define BATTLESEDIT     "battles"
	#define CLASSESEDIT     "classes"
	#define ENEMIESEDIT     "enemies"
	#define SHOPSEDIT       "shops"
	#define TEXTEDIT        "text"
	#define MAGICEDIT       "magic"
	#define WEAPONSEDIT     "weapons"
	#define INGAMETEXTEDIT  "text"
	#define STARTITEMSEDIT  "startitems"
	#define DIALOGUEEDIT    "dialogue"
	#define STDMAPSEDIT     "stdmaps"
	#define OVERWORLDEDIT   "overworld"
	#define PARTYSETUPEDIT  "partysetup"

	// Preview editors for 0.9.7.9 and later

	#define PREV_LOCALMAPEDIT "$preview-localmap"
	#define PREV_WORLDMAPEDIT "$preview-worldmap"
	


	// ****************************************************************
	// INTERFACE FUNCTIONS

	sEditorInfoVector TextToEditorInfos(CString text);
	CString EditorInfoToText(const sEditorInfo & info);
	CString EditorInfoToText(const sEditorInfoVector & infos);

	sEditorInfoVector GetDefaultEditorInfos();
	sEditorInfoVector GetEditorInfo(CString modulepath, CString name);

	sEditorInfoVector GetAllEditorInfos(CString projectini);
	sEditorLinkVector BuildEditorLinks(const sEditorInfoVector & infos);
	void SelectCurrentEditorLinks(const sEditorInfoVector & cureditors, sEditorLinkVector & links);
	sEditorInfoVector TakeSelectedEditorLinks(const sEditorLinkVector & links);

	sEditorInfoVector RemoveInvalidEditorInfos(sEditorInfoVector existingeditors, sEditorInfoVector * pinvalideditors = nullptr);

	sEditorInfoVector EditorsToInfos(const CEditorVector & editors);

	mfcstringvector InitializeExternalEditors(const CEditorVector & editors, CString projectini);

	#define EDITCMD_IGNORED std::pair<EditErrCode, CString>{EditErrCode::Success, "command ignored"}

	#define EDITORLEVEL_BUILTIN "Built-in"
	#define EDITORLEVEL_MISSING "Missing"
	#define EDITORLEVEL_CUSTOM "Custom"
	#define EDITORLEVEL_REMOVED "Excluded"

	#define EDITORPATH_REMOVED "*excluded"      // this is a placeholder that is not written to settings
	#define EDITORPATH_BUILTIN "*builtin"
	#define EDITORPATH_APPDIR "*app"
	#define EDITORPATH_PROJECT "*project"       // modules in the project folder
	#define EDITORPATH_PROJADD "*projadd"       // project additional modules folder
	#define EDITORPATH_BINPREVIEW "*binpreview" // optional that can be shown or hidden at will

	bool IsPathBuiltin(CString path);
	bool IsPathBuiltinPrefixed(CString path);

	CString GetLevelFromPath(CString projectini, CString testpath);
	CString GetLevelFromPath(const CFFHacksterProject & project, CString testpath);
	CString GetEditorLiveStatus(bool show); 

	bool IsEditorSectionName(CString keyname);
	CString GetEditorSectionName(CString editorname);


	// ****************************************************************
	// INTEROP SUPPORT FUNCTIONS
	// These are part of the static libraries compiled into the EXE and DLL, but they aren't passed
	// across process bounds, so it's safe to use them in both place.

	char* AllocReturnValue(EditorAllocatorFunc allocator, CString value);
	CString GetEditorResponse(const sEditorInfoVector & infos, const char* name);
	std::pair<EditErrCode, CString> RetrieveEditorAsResponse(const sEditorInfoVector & infos, const char* name);
}
