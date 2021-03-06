// CFFBaseApp.cpp : implementation file
//

#include "stdafx.h"
#include "FFBaseApp.h"
#include "path_functions.h"

CFFBaseApp * AfxGetFFBaseApp()
{
	return dynamic_cast<CFFBaseApp*>(AfxGetApp());
}


// CFFBaseApp

CFFBaseApp::CFFBaseApp()
	: m_appsettings(AppSettings::GetAppSettingsPath())
	, m_hIconSm(NULL)
	, m_hIconLg(NULL)
{
	if (IsDebuggerPresent())
		EmitStreamsToDebugger();
	m_fonts.SetDefaults();
}

CFFBaseApp::~CFFBaseApp()
{
	if (m_hIconSm != NULL) DestroyIcon(m_hIconSm);
	if (m_hIconLg != NULL) DestroyIcon(m_hIconLg);
}

void CFFBaseApp::SetIcon(UINT idicon)
{
	if (m_hIconSm != NULL) DestroyIcon(m_hIconSm);
	if (m_hIconLg != NULL) DestroyIcon(m_hIconLg);

	m_hIconSm = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(idicon),
		IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT);
	m_hIconLg = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(idicon),
		IMAGE_ICON, 32, 32, LR_LOADTRANSPARENT);
}

HICON CFFBaseApp::GetSmallIcon() const
{
	return m_hIconSm;
}

HICON CFFBaseApp::GetLargeIcon() const
{
	return m_hIconLg;
}

const Rc::IdDlgDataMap & CFFBaseApp::GetIdDlgDataMap() const
{
	return m_iddlgdatamap;
}

void CFFBaseApp::InvokeHelp(UINT dialogid, UINT childid, UINT parentaliasid)
{
	m_help.Invoke(dialogid, childid, parentaliasid);
}

const AppSettings& CFFBaseApp::GetAppSettings() const
{
	return m_appsettings;
}

AppSettings& CFFBaseApp::GetModifiableAppSettings()
{
	return m_appsettings;
}

CHelp& CFFBaseApp::GetHelpSettings()
{
	return m_help;
}

FontSet& CFFBaseApp::GetFonts()
{
	return m_fonts;
}

BOOL CFFBaseApp::InitInstance()
{
	if (!CWinApp::InitInstance())
		return FALSE;

	m_appsettings.Read();
	m_iddlgdatamap = Rc::GetIdToDlgNmmeMap(AfxGetResourceHandle());
	m_help.Init(m_appsettings.HelpTypeId, m_appsettings.HelpPath);
	return TRUE;
}

int CFFBaseApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

BEGIN_MESSAGE_MAP(CFFBaseApp, CWinApp)
END_MESSAGE_MAP()


void CFFBaseApp::EmitStreamsToDebugger()
{
	//m_odbcout = std::make_unique<outputdebugbuf>(std::cout);
	//m_odbcerr = std::make_unique<outputdebugbuf>(std::cerr);
	//m_odbclog = std::make_unique<outputdebugbuf>(std::clog);
}