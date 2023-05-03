// FFHackster.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FFHackster.h"
#include "FFHacksterGenerator.h"
#include "DlgWelcome.h"
#include "path_functions.h"
#include "imaging_helpers.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>

using namespace Imaging;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFFHacksterApp

BEGIN_MESSAGE_MAP(CFFHacksterApp, CFFBaseApp)
END_MESSAGE_MAP()

//ON_COMMAND(ID_HELP, CFFBaseApp::OnHelp) //N.B. - disabled to suppress default F1 key behavior
//		CFFBaseDlgs handle help directly, which points to web pages.

/////////////////////////////////////////////////////////////////////////////
// CFFHacksterApp construction

CFFHacksterApp::CFFHacksterApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFFHacksterApp object

CFFHacksterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFFHacksterApp initialization

BOOL CFFHacksterApp::InitInstance()
{
	CString progname;
	progname.LoadStringA(IDS_PROGRAMCAPTION);
	LogMsg << "Starting " << (LPCSTR)progname << std::endl;

	if (!CFFBaseApp::InitInstance())
		return FALSE;

	AfxEnableControlContainer();
	srand((unsigned int)time(nullptr));

	if (!AfxOleInit()) return ErrorReturn("AfxOleInit failed", FALSE);
	if (!InitGdiplus()) return ErrorReturn("InitGdiPlus failed", FALSE);

	SetIcon(IDR_MAINFRAME);

	auto appstgs = this->GetModifiableAppSettings();
	CFFHacksterGenerator ffgen(appstgs);
	CDlgWelcome dlgwelcome;
	dlgwelcome.AppStgs = &appstgs;

	CString action = GetIniitalAction();
	while (!action.IsEmpty() && action != "quit") {
		if (action == "prompt") {
			if (dlgwelcome.DoModal() == IDOK) {
				appstgs.Write();
				action = ffgen.ProcessAction(dlgwelcome.Action, AppIni, dlgwelcome.ActionParam);
			}
			else {
				action = "quit";
			}
		}
		else {
			action = ffgen.ProcessAction(action, AppIni, dlgwelcome.ActionParam);
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CFFHacksterApp::ExitInstance()
{
	FreeGdiplus();
	return CFFBaseApp::ExitInstance();
}

CString CFFHacksterApp::GetIniitalAction()
{
	CString action = "prompt";

	//HACK - flag to allow the rest of the command line to be unconditionally ignored
	if (__argc > 1 && CString(__argv[1]) == "-ignore")
		return action;

	switch (__argc) {
	case 2:
		if (Paths::FileExists((std::string)__argv[1])) {
			// open|projectpath
			action.Format("open|%s", __argv[1]);
		}
		else {
			ErrorHere << "Cannot find the file " << __argv[1] << std::endl;
			LogMsg << "    the file cannot be launched in the editor." << std::endl;
		}
		break;

	case 3:
		if (Paths::FileExists((std::string)__argv[2])) {
			action.Format("%s|%s", __argv[1], __argv[2]);
		}
		else {
			ErrorHere << "Cannot find the file " << __argv[2] << std::endl;
			LogMsg << "    the command " << __argv[1] << "  cannot be executed." << std::endl;
		}
		break;

	case 4:
		action.Format("edit|%s|%s", __argv[2], __argv[3]);
		break;
	}
	return action;
}