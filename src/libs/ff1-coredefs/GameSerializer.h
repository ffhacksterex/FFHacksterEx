#pragma once

#include "std_assembly.h"
#include "set_types.h"
#include "vector_types.h"
#include <functional>
class CFFHacksterProject;
class IProgress;


std::string build_asm_path(CFFHacksterProject & proj, CString relfilename);
std::istream & open_asmstream(std::ifstream & ifs, CFFHacksterProject & proj, CString relfilename);


class GameSerializer
{
public:
	GameSerializer(CFFHacksterProject & proj, CWnd* mainwnd = nullptr);
	~GameSerializer();

	CWnd * MainWindow;

	void LoadAsmBin(CString relfilename, int offset, int expectedlength, bool allowzerolength = false);
	void LoadAsmBin(CString relfilename, int offset, bool allowzerolength = false);  // doesn't verify buffer size
	void SaveAsmBin(CString relfilename, int offset, bool allowzerolength = false);

	void LoadAsmBinBuffer(bytevector & ROM, CString relfilename, int offset, bool allowzerolength = false);  // doesn't verify buffer size
	void SaveAsmBinBuffer(const bytevector & ROM, CString relfilename, int offset, bool allowzerolength = false);

	void LoadInline(CString relfilename, const asmsourcemappingvector & entries);
	void SaveInline(CString relfilename, const asmsourcemappingvector & entries);

	//TODO - normalize params (CString or std::string, but not both)
	//	I didn't change these yet because a lot of other code is mixing both in different places.
	//	It'll be a sweeping change across th eentire codebase, so I'm holding off for now.
	void LoadDialogue(CString filename, std::string mappinglabel, stdstringvector & routinenames, talknamevectormap & routinemappings);
	void SaveDialogue(CString filename, std::string mappinglabel, const stdstringvector & routinenames, const talknamevectormap & routinemappings);

	//N.B. - assumes that no entries are defined on the same line as the label.
	void LoadTalkPointerTable(CString asmfilename);
	void PreloadTalkAsmData(CString asmfilename);

	bool ReadAllVariables(CString projectpath, stdstrintmap & valuesmap, IProgress * progress);

private:
	stdstringset read_varnames();
	stdstringvector read_prefixes();

	static bool read_variables(std::string filepath, const stdstringset & nameset, const stdstringvector & prefixes, std::string numberformat, stdstrintmap & valuesmap);

	stdstringvector DoReadAsmTable(CString asmfile, CString label);
	void DoWriteAsBytes(int hcoffset, stdstrintmap& valuesmap, const talkelement& elem);

	CFFHacksterProject & m_proj;
};
