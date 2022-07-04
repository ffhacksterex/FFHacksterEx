#pragma once

#include "FFBaseApp.h"
#include <EditorInterop.h>
#include <Editors2.h>
#include <rc_functions.h>
#include <map>
#include <functional>
class CFFHacksterProject;


class CFFHExtensionModule : public CFFBaseApp
{
public:
	CFFHExtensionModule();
	virtual ~CFFHExtensionModule();

	virtual BOOL InitInstance();

	// No need to override this unless you want to break from default processing.
	virtual CString Invoke(const EditorInterop * ei);

protected:
	using QueryInvokeFunc = std::function<INT_PTR()>;
	using QueryHandlerMap = std::map<std::pair<CString, CString>, QueryInvokeFunc>;

private:
	Rc::IdDlgDataMap m_iddlgdatamap;

	virtual Editors2::sEditorInfoVector GetEditorInfos() = 0;

	// These are optional: by default, they do nothing and simply return success (command ignored).
	virtual std::pair<Editors2::EditErrCode, CString> OverrideValues(CString projectini);
	virtual std::pair<Editors2::EditErrCode, CString> HandleInit(const EditorInterop & ei);
	virtual std::pair<Editors2::EditErrCode, CString> HandleCommand(CFFHacksterProject & proj, const EditorInterop & ei);

	std::pair<Editors2::EditErrCode, CString> GetAllEditors();
	std::pair<Editors2::EditErrCode, CString> GetEditor(const char* name);

	void UpdateSharedDisplayProperties();
};
