#pragma once

#include "Editors2.h"
class CFFHacksterProject;
struct ProjectEditorModuleEntry;
class FFH2Project;

namespace Editors2
{

	class EditorSettingsSerializer
	{
	public:
		EditorSettingsSerializer();
		~EditorSettingsSerializer();

		CString EditorSettingsPath;

		CString AppFolder;
		CString ProjectFolder;
		CString ProjectAdditionModulesFolder;

		sEditorInfoVector ReadEditorInfo(CString name);

		sEditorInfoVector ReadAllEditorInfos(FFH2Project & prj2);
		[[deprecated]]
		sEditorInfoVector ReadAllEditorInfos();
		void WriteAllEditorInfos(const sEditorInfoVector & editors);

	private:
		sEditorInfoVector BuildEditorFromIni(CString section);
		CString DecodeModulePath(CString path);
		CString EncodeModulePath(CString path);

		sEditorInfoVector BuildEditorFromModuleEntry(ProjectEditorModuleEntry& entry);
	};

}
