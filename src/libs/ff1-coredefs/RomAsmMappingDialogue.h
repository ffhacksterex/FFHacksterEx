#pragma once
#include "RomAsmSequentialMappingBase.h"
#include "romasm_functions.h"
#include <set>

class CFFHacksterProject;
class RomAsmMappingFactory;

struct TalkElement
{
	// required
	std::string type;
	int instoffset;
	int byteoffset;
	int paramindex;
	std::string comment;
	// extended
	std::string hint;  // e.g. CLS_, OBJID_, etc.
	// unserialized
	bool hardcoded;
	int datasizebytes;
};

struct TalkHandler
{
	std::string desc;
	int bankoffset;
	std::vector<TalkElement> elements;
};

class RomAsmMappingDialogue : public RomAsmSequentialMappingBase
{
public:
	RomAsmMappingDialogue(CFFHacksterProject& project, std::string mapping);
	virtual ~RomAsmMappingDialogue();

	virtual void Load(std::istream& sourcefile, std::vector<unsigned char>& rom,
		const std::map<std::string, std::string>& options) override;
	virtual void Save(std::istream& sourcefile, std::ostream& destfile,
		const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options) override;

protected:
	const std::string maptype = "dialogue";
	std::string projectfolder;
	std::string label;
	std::string sublabel;
	int instoffset = -1;
	int tablecount = -1;
	int talkoffset = -1;
	int talkptradd = -1;
	std::string stopatfunc;

	std::vector<TalkHandler> m_handlerdefs;
	std::map<std::string, size_t> m_handlerByName;
	std::map<int, size_t> m_handlerByBankaddr;
	//std::vector<int> m_handlerpointers;
	std::set<std::string> m_unusedhandlers;

	//TODO - use these instead of string-to-string mappings.
	std::map<std::string, int> m_constants2;
	std::map<std::string, int> m_symbols2;
	//std::map<std::string, int> m_allsymbols; //TODO - combine constants and variables into one map?

	void Preload(CFFHacksterProject& project);
	void LoadHandlerFromDialogFile(std::string dialogpath);
	void LoadConstants(CFFHacksterProject& project);
	void LoadSymbols(CFFHacksterProject& project);
	void LoadFanfare(CFFHacksterProject& project);

	void LoadHandlerTable(std::istream & is, std::vector<unsigned char>& rom);
	void LoadHardcodedRoutineValues(std::istream& is, std::vector<unsigned char>& rom);
	int LookupBankAddress(std::string term, std::streampos pos);
	//const std::map<std::string, std::string>& GetLookupMap(const TalkElement& elem);
	const std::map<std::string, int>& GetLookupMap(const TalkElement& elem);

	using RomLoadRoutineFunc = void(RomAsmMappingDialogue::*)(
		const TalkElement& elem, std::string line, size_t destbaseoffset,
		std::vector<unsigned char>& rom, const std::map<std::string, int>& symbols);

	struct RoutineFuncRef // works around C4503 //TODO - find a cleaner solution)
	{
		RomAsmMappingDialogue::RomLoadRoutineFunc first;
		int second;
	};
	std::map<std::string, RoutineFuncRef> m_romloadroutines;
	bool m_fanfareEnabled;
	RomAsm::CondValues m_fanfareCond;

	RomLoadRoutineFunc GetRomLoadRoutine(const TalkElement& elem);
	void LoadOpval(const TalkElement& elem, std::string line, size_t destbaseoffset,
		std::vector<unsigned char>& rom, const std::map<std::string, int>& constants);
	void LoadSymbol(const TalkElement& elem, std::string line, size_t destbaseoffset,
		std::vector<unsigned char>& rom, const std::map<std::string, int>& constants);
	void LoadFanfare(const TalkElement& elem, std::string line, size_t destbaseoffset,
		std::vector<unsigned char>& rom, const std::map<std::string, int>& constants);
};
