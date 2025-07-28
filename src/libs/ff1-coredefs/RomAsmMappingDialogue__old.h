#pragma once
#include "RomAsmMapping.h"
#include "RomAsmSequentialMappingBase.h"

class CFFHacksterProject;
class RomAsmMappingFactory;

namespace Old
{
	struct TalkElement
	{
		std::string type;
		int instoffset;
		int byteoffset;
		int paramindex;
		std::string comment;
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

		std::vector<TalkHandler> m_talkhandlers;
		std::map<std::string, size_t> m_handlerByName;
		std::map<int, size_t> m_handlerByBankaddr;
		std::vector<int> m_talktable;

		//std::string dialoguepath;
		void Preload(std::string dialoguepath);
	};
}
