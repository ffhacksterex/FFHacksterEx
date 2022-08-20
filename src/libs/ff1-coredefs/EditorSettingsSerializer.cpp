#include "stdafx.h"
#include "EditorSettingsSerializer.h"
#include "ini_functions.h"
#include "path_functions.h"
using namespace Ini;

namespace Editors2
{

	EditorSettingsSerializer::EditorSettingsSerializer()
		: AppFolder(Paths::GetProgramFolder())
	{
	}

	EditorSettingsSerializer::~EditorSettingsSerializer()
	{
	}

	// Reas the info for a single editor, but for convenience returns it as an array.
	// It either contains the item read or is empty on failure.
	sEditorInfoVector EditorSettingsSerializer::ReadEditorInfo(CString name)
	{
		CString section = GetEditorSectionName(name);
		auto infos = BuildEditorFromIni(section);
		ASSERT(infos.size() <= 1);
		if (infos.size() > 1) infos.resize(1);
		return infos;
	}

	sEditorInfoVector EditorSettingsSerializer::ReadAllEditorInfos()
	{
		sEditorInfoVector infos;
		auto sections = ReadIniSectionNames(EditorSettingsPath);
		for (auto section : sections) {
			if (IsEditorSectionName(section))
				append(infos, BuildEditorFromIni(section));
		}
		return infos;
	}

	void EditorSettingsSerializer::WriteAllEditorInfos(const sEditorInfoVector & editors)
	{
		// Remove the sections so there are no leftover editor references
		auto sections = ReadIniSectionNames(EditorSettingsPath);
		for (auto section : sections) {
			if (IsEditorSectionName(section)) RemoveSection(EditorSettingsPath, section);
		}

		// Now write the visible sections
		for (const auto & editor : editors) {
			if (!editor.show) continue;

			auto section = Editors2::GetEditorSectionName(editor.name);
			WriteIni(EditorSettingsPath, section, "name", editor.name);
			WriteIni(EditorSettingsPath, section, "modulepath", EncodeModulePath(editor.modulepath));
		}
	}

	CEditor GetDefaultEditor(CString name)
	{
		auto infos = GetDefaultEditorInfos();
		auto itinfo = std::find_if(cbegin(infos), cend(infos), [name](const auto & info) { return info.name == name; });
		if (itinfo != cend(infos))
			return CEditor(*itinfo);

		return CEditor({ false, name, "--" + name, EDITORLEVEL_MISSING, "Default editor not found" });
	}

	CEditor GetBadPathEditor(CString name, CString badeditorpath)
	{
		return CEditor({ false, name, "--" + name, badeditorpath, "Module not found" });
	}

	sEditorInfoVector EditorSettingsSerializer::BuildEditorFromIni(CString section)
	{
		sEditorInfoVector infos;
		auto name = ReadIni(EditorSettingsPath, section, "name", "");
		if (!name.IsEmpty()) {
			
			// Normalize the path
			auto modulepath = ReadIni(EditorSettingsPath, section, "modulepath", "");
			if (Editors2::IsPathBuiltin(modulepath))
			{
				infos.push_back(GetDefaultEditor(name).GetInfo());
			}
			else {
				auto normalpath = DecodeModulePath(modulepath);
				if (!Paths::FileExists(normalpath)) {
					infos.push_back(GetBadPathEditor(name, normalpath).GetInfo());
				}
				else {
					auto newinfos = GetEditorInfo(normalpath, name);
					append(infos, newinfos);
				}
			}
		}
		return infos;
	}

	CString EditorSettingsSerializer::DecodeModulePath(CString path)
	{
		//N.B. - CWD entries are NOT currently decoded
		CString newpath = path;
		if (path.Find(EDITORPATH_APPDIR) == 0) {
			newpath = Paths::Combine({AppFolder, Paths::GetFileName(path)});
		}
		else if (path.Find(EDITORPATH_PROJECT) == 0) {
			newpath = Paths::Combine({ProjectFolder, Paths::GetFileName(path) });
		}
		else if (path.Find(EDITORPATH_PROJADD) == 0) {
			newpath = Paths::Combine({ ProjectAdditionModulesFolder, Paths::GetFileName(path) });
		}
		else if (path.Find(EDITORPATH_REMOVED) == 0) {
			throw std::invalid_argument(EDITORPATH_REMOVED + std::string(" cannot decode from a module path"));
		}
		return newpath;
	}

	//N.B. - CWD entries are NOT currently encoded
	CString EditorSettingsSerializer::EncodeModulePath(CString path)
	{
		CString newpath = path;
		CString directory = Paths::GetDirectoryPath(path);
		if (directory.CompareNoCase(AppFolder) == 0) {
			newpath = Paths::Combine({ EDITORPATH_APPDIR, Paths::GetFileName(path) });
		}
		else if (directory.CompareNoCase(ProjectFolder) == 0) {
			newpath = Paths::Combine({ EDITORPATH_PROJECT, Paths::GetFileName(path) });
		}
		else if (directory.CompareNoCase(ProjectAdditionModulesFolder) == 0) {
			newpath = Paths::Combine({ EDITORPATH_PROJADD, Paths::GetFileName(path) });
		}
		else if (Editors2::IsPathBuiltinPrefixed(path)) {
			//DEVNOTE - special case, I thought about forcing special handling by throwing here, but that might backfire
			//		due to code mushrooming to handle the exception...
			//		for now, just return the path as is.
			//throw std::runtime_error("Built-in cannot encode to a module path");
		}
		else if (path.Find(EDITORPATH_REMOVED) == 0) {
			throw std::invalid_argument(EDITORPATH_REMOVED + std::string(" cannot encode to a module path"));
		}
		return newpath;
	}

}