// Loading.cpp : implementation file
//

#include "stdafx.h"
#include "resource_subeditors.h"
#include "Loading.h"
#include "ui_helpers.h"
#include <FFBaseApp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoading dialog

CLoading::CLoading(CWnd* pParent /*= nullptr */)
	: CDialog(CLoading::IDD, pParent)
{
}

void CLoading::SetMessage(CString message)
{
	m_messagestatic.SetWindowText(message);
}

void CLoading::Init(int min, int max, int step, CWnd* parent)
{
	if (!IsWindow(GetSafeHwnd())) {
		if (!Create(CLoading::IDD, parent))
			throw Ui::ui_exception(EXCEPTIONPOINT, "Unable to initialize the loading progress dialog.");
	}

	SetIcon(AfxGetFFBaseApp()->GetLargeIcon(), TRUE); //ELJNOTE - small icon taks precendence here

	Ui::CenterToParent(this);
	m_progress.SetRange32(min, max);
	m_progress.SetStep(step);
	ShowWindow(TRUE);
}

void CLoading::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_STATIC1, m_messagestatic);
}

// Inherited via IProgress
void CLoading::DoAddSteps(int numtoadd)
{
	int min = 0, max = 0;
	m_progress.GetRange(min, max);
	m_progress.SetRange32(min, max + numtoadd);
}

void CLoading::DoStep()
{
	m_progress.StepIt();
	m_progress.Invalidate();
}

void CLoading::DoSetProgressText(LPCSTR text)
{
	SetMessage(text);
	Invalidate();
}

void CLoading::DoSetStatusText(LPCSTR text)
{
	UNREFERENCED_PARAMETER(text);
	//FUTURE - re-evaluate. for now, changing status text is not implemented.
}

BEGIN_MESSAGE_MAP(CLoading, CDialog)
END_MESSAGE_MAP()
