#include "stdafx.h"
#include "FFHExtensionModule.h"

#include <path_functions.h>

#include <Editors2.h>
#include <FFHacksterProject.h>
#include <FFH2Project.h>
#include <IniOverrider.h>
#include "StrikeCheck.h"
#include "ProjectLoader.h"

using namespace Editors2;
using namespace Ini;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CFFHExtensionModule::CFFHExtensionModule()
{
}

CFFHExtensionModule::~CFFHExtensionModule()
{
}

BOOL CFFHExtensionModule::InitInstance()
{
	if (!CFFBaseApp::InitInstance())
		return FALSE;

	UpdateSharedDisplayProperties();
	return TRUE;
}

CString CFFHExtensionModule::Invoke(const EditorInterop * ei)
{
	if (ei == nullptr) return{};

	ASSERT(ei->allocator != nullptr);
	if (ei->allocator == nullptr) return{};

	std::pair<EditErrCode, CString> response = EDITCMD_IGNORED;
	try {
		CString command = ei->command;
		if (command == INVOKE_INIT) {
			response = OverrideValues(ei->projectini);
			if (IsResponseSuccess(response.first))
				response = HandleInit(*ei);
		}
		else if (command == INVOKE_GETEDITOR) {
			response = GetEditor(ei->name);
		}
		else if (command == INVOKE_GETALLEDITORS) {
			response = GetAllEditors();
		}
		else {
			CFFHacksterProject proj;
			FFH2Project prj2;
			proj.AppSettings = &m_appsettings;
			if (ei->projectini == nullptr) {
				response = { EditErrCode::NoProject, "no project was specified" };
			}
			else if (!Paths::FileExists(ei->projectini)) {
				response = { EditErrCode::NoProject, "couldn't find the project file '" + CString(ei->projectini) + "'" };
			}
			else if (!LoadProject(prj2, proj, ei->projectini)) {
				response = { EditErrCode::NoProject, "failed to load the project" };
			}
			else {
				response = HandleCommand(proj, *ei);
			}
		}
	}
	catch (std::exception & ex) {
		response = { EditErrCode::ExceptionCaught, "an unexpected exception was caught (" + CString(ex.what()) + ")" };
	}
	catch (...) {
		response = { EditErrCode::ExceptionCaught, "an unexpected exception was caught" };
	}

	return FormatEditorResponse(response.first, response.second);
}

std::pair<EditErrCode, CString> CFFHExtensionModule::OverrideValues(CString projectini)
{
	return EDITCMD_IGNORED;
}

std::pair<EditErrCode, CString> CFFHExtensionModule::HandleInit(const EditorInterop & ei)
{
	UNREFERENCED_PARAMETER(ei);
	return EDITCMD_IGNORED;
}

std::pair<EditErrCode, CString> CFFHExtensionModule::HandleCommand(CFFHacksterProject & proj, const EditorInterop & ei)
{
	UNREFERENCED_PARAMETER(proj);
	UNREFERENCED_PARAMETER(ei);
	return EDITCMD_IGNORED;
}

std::pair<EditErrCode, CString> CFFHExtensionModule::GetAllEditors()
{
	return{ EditErrCode::Success, EditorInfoToText(GetEditorInfos()) };
}

std::pair<EditErrCode, CString> CFFHExtensionModule::GetEditor(const char* name)
{
	return RetrieveEditorAsResponse(GetEditorInfos(), name);
}

void CFFHExtensionModule::UpdateSharedDisplayProperties()
{
	//N.B. - this must be done for this extension module, as the app that launched this module loaded it with LoadLibrary.
	CStrikeCheck::RenderAsCheckbox = m_appsettings.DisplayStrikeChecksAsNormalCheckboxes == true;
	CStrikeCheck::SharedCheckedColor = m_appsettings.StrikeCheckedColor;
}