#pragma once

#include <fstream>
#include <memory>
#include <rc_functions.h>
#include "Help.h"
#include "FontSet.h"
//#include <outputdebugbuf.h>
#include <AppSettings.h>

// CFFBaseApp

class CFFBaseApp : public CWinApp
{
public:
	CFFBaseApp();
	virtual ~CFFBaseApp();

	void SetIcon(UINT idicon);
	HICON GetSmallIcon() const;
	HICON GetLargeIcon() const;

	const Rc::IdDlgDataMap & GetIdDlgDataMap() const;

	void InvokeHelp(UINT dialogid, UINT childid, UINT parentaliasid = -1);
	const AppSettings& GetAppSettings() const;
	AppSettings& GetModifiableAppSettings(); //REFACTOR - provide a wrapper to clients who need write access instead
	CHelp& GetHelpSettings();
	FontSet& GetFonts();

protected:
	AppSettings m_appsettings;
	Rc::IdDlgDataMap m_iddlgdatamap;
	CHelp m_help;
	FontSet m_fonts;

	HICON m_hIconSm;
	HICON m_hIconLg;

	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()

private:
	void EmitStreamsToDebugger();

	//std::unique_ptr<outputdebugbuf> m_odbcout;
	//std::unique_ptr<outputdebugbuf> m_odbcerr;
	//std::unique_ptr<outputdebugbuf> m_odbclog;
};

CFFBaseApp * AfxGetFFBaseApp();
