#pragma once

#include <functional>
#include <afx.h>
#include "datanode.h"

class CFFHacksterProject;
class FFH2Project;
class CListBox;
class CComboBox;

using LoadLabelFunc = std::function<CString(CFFHacksterProject&, CListBox*, CComboBox*, bool showindex)>;
using LoadEntryFunc = std::function<dataintnodevector (CFFHacksterProject&, bool showindex)>;
using LoadSingleEntryFunc = std::function<dataintnode(CFFHacksterProject & proj, int index, bool showindex)>;

using LoadLabelFunc2 = std::function<CString(FFH2Project&, CListBox*, CComboBox*, bool showindex)>;
using LoadEntryFunc2 = std::function<dataintnodevector(FFH2Project&, bool showindex)>;
using LoadSingleEntryFunc2 = std::function<dataintnode(FFH2Project& proj, int index, bool showindex)>;

using BoolAction = std::function<bool()>;

using InplaceEditOKFunc = bool(CString newtext);
using InplaceCheckOKFunc = bool(bool checked);

using StringTransformFunc = std::function<CString(CString str)>;

// Used to refer to functions that load text from the project ROM or labels.
using GetTextEntryFunc = std::function<CString (CFFHacksterProject & proj, int index, bool showindex)>;
using GetTextEntryFunc2 = std::function<CString(FFH2Project& proj, int index, bool showindex)>;

using DataNodeReadFunc = std::function<dataintnode(CFFHacksterProject & proj, int index, bool showindex)>;
using IniWriteFunc = std::function<void (CFFHacksterProject & proj, int index, CString newvalue)>;
using DataNodeReadFunc2 = std::function<dataintnode(FFH2Project& proj, int index, bool showindex)>;
using IniWriteFunc2 = std::function<void(FFH2Project& proj, int index, CString newvalue)>;

using StrToIntFunc = std::function<int(CString text)>;
using IntToStrFunc = std::function<CString(int value)>;
