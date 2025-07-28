#include "stdafx.h"
#include "RomAsmMappingDialogue__old.h"

#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "romasm_functions.h"
#include "string_functions.h"
#include "type_support.h"

#include <fstream>
#include <map>
#include <set>

namespace Old
{
    namespace {
        bool IsNumericTerm(const std::string& term);
        //TalkEntry LoadEntry(CString dialoguepath, std::string term);

        TalkHandler LoadEntry(CString dialoguepath, std::string term);

        template <typename K, typename V>
        void ThrowIfDuplicate(const std::map<K, V>& m, const K& key, const V& value);

        void ToRom8(int data, size_t destromoffset, std::vector<unsigned char>& rom);
        void ToRom16(int data, size_t destromoffset, std::vector<unsigned char>& rom);

        bool IsRoutine(std::string line, std::string prefix); //TODO - NOT USED
        std::string GetRoutineName(std::string line, const std::regex& rx);

        //void LoadEntries(std::string dialoguepath, std::map<std::string, TalkEntry> & entries,
        //    std::map<int, std::string> & bankaddrToName);
        //void MapElements(std::istream& is, int routinecount, std::map<std::string, TalkEntry> & talkentries);

        void LoadHandlerAddress(int bankoffset, size_t destromoffset, std::vector<unsigned char>& rom);
        void LoadHardcode(const TalkElement& elem, std::istream& is, size_t routinestartoffset, std::vector<unsigned char>& rom);
    }

    RomAsmMappingDialogue::RomAsmMappingDialogue(CFFHacksterProject& project, std::string mapping)
    {
        CString romasmini = project.GetIniFilePath(FFHFILE_RomAsmMappingsPath);
        CString section = mapping.c_str();
        std::string edtype = (LPCSTR)Ini::ReadIni(romasmini, section, "type", "");
        ASSERT(edtype == maptype);

        CString valini = project.GetIniFilePath(FFHFILE_ValuesPath);
        name = mapping;
        type = edtype;
        romoffset = RomAsm::ReadIniInt(romasmini, valini, section, "offset");
        source = Strings::trimleft(RomAsm::ReadSource(romasmini, section, section, "source"));
        readonly = Ini::ReadIni(romasmini, section, "readonly", "false") == "true";
        projectfolder = (LPCSTR)project.ProjectFolder;
        label = Ini::ReadIni(romasmini, section, "label", "");
        sublabel = Ini::ReadIni(romasmini, section, "sublabel", "");
        instoffset = atol(Ini::ReadIni(romasmini, section, "instoffset", "-1"));
        tablecount = RomAsm::ReadIniInt(romasmini, valini, section, "tablecount");
        talkoffset = RomAsm::ReadIniInt(romasmini, valini, section, "offset");
        talkptradd = RomAsm::ReadIniInt(romasmini, valini, section, "ptradd");
        stopatfunc = Ini::ReadIni(romasmini, section, "stopatfunc", "");

        std::string dialoguepath = project.GetIniFilePath(FFHFILE_DialoguePath);
        Preload(dialoguepath);

        std::vector<std::string> errors;
        if (type != maptype) errors.push_back("type should be " + maptype + ", not '" + edtype + "'");
        if (romoffset < 1) errors.push_back("romoffset must be greater than zero");
        if (source.empty()) errors.push_back("source cannot be blank");
        if (label.empty()) errors.push_back("label cannot be blank");
        if (instoffset < 1) errors.push_back("instoffset must be greater than zero");
        if (tablecount < 1) errors.push_back("tablecount must be greater than zero");
        if (talkoffset < 1) errors.push_back("talkoffset must be greater than zero");
        if (talkptradd < 1) errors.push_back("talkptradd must be greater than zero");
        if (stopatfunc.empty()) errors.push_back("stopatfunc cannot be blank");
        if (!Paths::FileExists(dialoguepath.c_str())) errors.push_back("dialogue file '" + dialoguepath + "'does not exist in the project");

        if (!errors.empty())
            throw std::runtime_error("Editor '" + mapping + "' config is malformed: " + Strings::join(errors, "; "));
    }

    RomAsmMappingDialogue::~RomAsmMappingDialogue()
    {
    }

    void RomAsmMappingDialogue::Load(std::istream& sourcefile, std::vector<unsigned char>& rom,
        const std::map<std::string, std::string>& options)
    {
        // The marker is the start of the routine table.
        // To load the routines, we also need the dialogue file.
        // Each table entry is either the address or name of a routine.

        // 1) Gather the dialogue file entries into two maps: by address (int -> entry) and by name (string -> entry).
        // 2) for each entry, build the offset and map the data according of each element in the entry
        // 3) keep these mappings persistent for the duration of the editor.

        RomAsm::Traverse::SeekToInstruction(sourcefile, name, label, sublabel, instoffset);

        auto& ifs = sourcefile;
        auto bookmark = sourcefile.tellg();

        m_talktable.clear();
        m_talktable.reserve(tablecount);

        // PASS 1: load the talk table index
        // PASS 2: move hardcoded values
        // The BIN_TALKOBJECT mapping moves the talk object params block for us

        try {
            // PASS 1 - load the talk table index
            while (ifs && !ifs.eof() && m_talktable.size() < tablecount) {
                bookmark = ifs.tellg();

                std::string rawline;
                std::getline(ifs, rawline);
                if (RomAsm::Query::IsBlankOrWhitespaceLine(rawline))
                    continue;

                //DEVNOTE - supported data rows start with .WORD, so we don't require the flexibility of LooksLikeDataRow here
                std::string line = rawline;
                if (RomAsm::Query::IsApparentDataRow(line)) {
                    // if it's not a WORD followed by comma-separated values, we're done
                    int recwidth = RomAsm::Parse::ExtractDataRowRecWidth(line);
                    if (recwidth != 2)
                        throw std::runtime_error("On the line at position " + std::to_string(bookmark) +
                            ", the data row is not .WORD and cannot be processed as a bank address list.");

                    // the terms could be names or bank addresses.
                    // if it's a name, then we have to look up the name from the dialogue file
                    auto terms = Strings::splitrx(line, "\\s*,\\s*", true);
                    for (const auto& term : terms) {
                        //TODO - FIRST PASS DOESN'T SUPPORT BANK ADDRESSES IN THE TALK ROUTINE TABLE
                        auto isnum = IsNumericTerm(term);
                        if (isnum)
                            throw std::runtime_error(__FUNCTION__ ": bank addresses are not yet supported in the talk routine table.");

                        // Read this entry from the dialogue file, then map it
                        //TODO - for now, not supporting bank addresses here
                        auto iter = m_handlerByName.find(term);
                        if (iter == m_handlerByName.end() || iter->second >= m_talkhandlers.size())
                            throw std::runtime_error(__FUNCTION__ ": no handler found by the name '" + term + "'.");

                        // Now move the bank address into this slot in the rom[] handler ptr table.
                        auto& entry = m_talkhandlers[iter->second];
                        auto destoffset = (int)talkoffset + (2 * m_talktable.size());
                        LoadHandlerAddress(entry.bankoffset, destoffset, rom);
                        m_talktable.push_back(entry.bankoffset);
                    }
                }
                else if (!RomAsm::Query::IsComment(rawline)) {
                    // the comment check only happens if the data row check failed first.
                    // If this row is anything but a data row or comment at this point, we're done.
                    break;
                }
            }

            ASSERT(m_talkhandlers.size() == tablecount);

            // PASS 2 - load hardcoded data

            //TODO - if we add a startafterfunc, then move to that function label here
            std::string prefix = "Talk_";
            std::regex rc_talkroutine("^(" + prefix + "\\w+)");

            while (ifs && !ifs.eof() && m_talktable.size() < tablecount) {
                bookmark = ifs.tellg();

                std::string rawline;
                std::getline(ifs, rawline);
                if (RomAsm::Query::IsBlankOrWhitespaceLine(rawline))
                    continue;

                std::string line = rawline;

                // Find the next talk routine
                //if (!IsRoutine(line, "Talk_"))
                auto term = GetRoutineName(line, rc_talkroutine);
                if (term.empty())
                    continue;

                auto iterm = m_handlerByName.find(term);
                if (iterm == m_handlerByName.end())
                    throw std::runtime_error(__FUNCTION__ ": no handler found for routine name " + term + ".");

                //TODO - elements aren't currently ordered by instoffset, might be a good idea to either
                //  order them or index them and walk through them via the index.
                //  since we currently don't enforce ordering, don't assume they're in order.

                auto& handler = m_talkhandlers[iterm->second];
                std::string contextname = "Dialogue Handler " + term;
                auto routinestart = talkptradd + handler.bankoffset;
                for (const auto& elem : handler.elements) {
                    if (!elem.hardcoded) continue;

                    // Move to the instruction offset from the start of the routine.
                    ifs.clear(std::ios::eofbit);
                    ifs.seekg(bookmark);
                    RomAsm::Traverse::SeekToInstruction(ifs, elem.instoffset, contextname);

                    LoadHardcode(elem, ifs, romoffset, rom);
                }

                // for each element, subract the previous instoffset from its instoffset, then walk than number of instructions.
                // -> remember, instructions, not lines
                // for the instruction, read the line based on type and content:
                // hcfanfare, e.g. INC dlgsfx
                // 8-bit hardcoded, e.g. LDA #27 or LDA #CLS_FT
                // 16-bit hardcoded, e.g. LDA bridge_vis or LDA $6028 (uncommon)
                // 
                // params, e.g. tmp+3
            }
        }
        catch (std::exception& ex) {
            throw std::runtime_error("Mapping '" + this->name + "' encounter the following processing error starting at file position "
                + std::to_string(bookmark) + ": " + ex.what());
        }

        if (m_talkhandlers.size() != tablecount) {
            throw std::runtime_error("Mapping '" + this->name + "' retrieved " + std::to_string(m_talkhandlers.size())
                + " bytes instead of the expected " + std::to_string(tablecount) + ".");
        }
    }

    void RomAsmMappingDialogue::Save(std::istream& sourcefile, std::ostream& destfile, const std::vector<unsigned char>& rom,
        const std::map<std::string, std::string>& options)
    {
        throw std::runtime_error(__FUNCTION__ " not implemented yet");
    }

    void RomAsmMappingDialogue::Preload(std::string dialoguepath)
    {
        // load the handlers and lookups
        auto dialogini = dialoguepath.c_str();

        auto sections = Ini::ReadIniSectionNames(dialogini);
        for (const auto& section : sections) {
            if (Ini::ReadIni(dialogini, section, "bankaddr", "").IsEmpty())
                continue;

            std::string term = (LPCSTR)section;
            if (m_handlerByName.find(term) != m_handlerByName.end())
                throw std::runtime_error(__FUNCTION__ ": cannot load duplicate talk handler '" + term + "'.");

            auto index = m_talkhandlers.size();
            m_talkhandlers.emplace_back(LoadEntry(dialogini, term));

            //if (m_handlerByBankaddr.find(term) != m_handlerByBankaddr.end())
            //    throw std::runtime_error(__FUNCTION__ ": duplicate talk handler '" + term + "' cannot be loaded.");

            m_handlerByName[term] = index;
            m_handlerByBankaddr[m_talkhandlers.back().bankoffset] = index;
        }
    }

    namespace {
        bool IsNumericTerm(const std::string& term)
        {
            if (term.empty()) return false;

            char c = term[0];
            return c == '$' || c == '%' || isdigit(c);
        }

        //TalkEntry ParseToEntry(std::string line, std::streampos bookmark, std::map<std::string, TalkEntry>& talkentries)
        //{
        //    // if it's not a WORD followed by comma-separated values, we're done
        //    int recwidth = RomAsm::Parse::ExtractDataRowRecWidth(line);
        //    if (recwidth != 2)
        //        throw std::runtime_error("On the line at position " + std::to_string(bookmark) +
        //            ", the data row is not .WORD and cannot be processed as a bank address list.");

        //    // the terms could be names or bank addresses.
        //    // If it's a name, then we have to look up the name from the dialogue file
        //    auto terms = Strings::splitrx(line, "\\s*,\\s*", true);
        //    for (const auto& term : terms) {
        //        //TODO - FIRST PASS DOESN'T SUPPORT BANK ADDRESSES IN THE TALK ROUTINE TABLE
        //        auto isnum = IsNumericTerm(term);
        //        if (isnum)
        //            throw std::runtime_error(__FUNCTION__ ": bankaddresses are not yet supported in the talk routine table.");

        //        ThrowIfDuplicate(nametobankaddr, term, entry.bankoffset);
        //        ThrowIfDuplicate(bankaddrtoname, entry.bankoffset, term);
        //    }
        //}

        TalkHandler LoadEntry(CString dialoguepath, std::string term)
        {
            auto section = term.c_str();
            TalkHandler t;
            t.desc = Ini::ReadIni(dialoguepath, section, "desc", "");
            t.bankoffset = strtoul(Ini::ReadIni(dialoguepath, section, "bankaddr", ""), nullptr, 16);

            int index = 0;
            CString key = "elem0";
            CString raw = Ini::ReadIni(dialoguepath, section, key, "");
            while (!raw.IsEmpty()) {
                auto parts = Strings::splitrx(raw, "|", false);
                if (parts.size() < 5)
                    throw std::runtime_error("'" + term + "' has " + std::to_string(parts.size()) + " terms instead of the expected 5.");

                TalkElement e;
                e.type = parts[0];
                e.instoffset = atol(parts[1]);
                e.byteoffset = strtoul(parts[2], nullptr, 16);
                e.paramindex = atol(parts[3]);
                e.comment = parts[4];
                e.hardcoded = e.type.find("hc") == 0;
                e.datasizebytes = RomAsm::Dialogue::GetByteCountByElementType(e.type);

                //TODO - the first 5 parts are standard, the rest are optional extended properties.
                // index 3 (say) has parts[5] == "cond", then read key + parts[5], which would be "elem3cond"
                // for now, there is only one supported extended property

                t.elements.push_back(e);
                //---
                key.Format("elem%d", ++index);
                raw = Ini::ReadIni(dialoguepath, section, key, "");
            }

            return t;
        }

        template <typename K, typename V>
        void ThrowIfDuplicate(const std::map<K, V>& m, const K& key, const V& value)
        {
            auto i = m.find(key);
            if (i != m.end() && i->second != value) {
                std::ostringstream o;
                o << "Cannot overwrite existing key '" << key << "' with value '" << value << "'.";
                throw std::runtime_error(o.str());
            }
        }

        ////std::map<std::string, TalkEntry> LoadEntries(std::string dialoguepath)
        //void LoadEntries(std::string dialoguepath, std::map<std::string, TalkEntry>& talkentries,
        //    std::map<int, std::string>& bankaddrToName)
        //{
        //    //std::map<std::string, TalkEntry> talkentries;

        //    talkentries.clear();
        //    bankaddrToName.clear();

        //    auto dialogini = dialoguepath.c_str();
        //    {
        //        auto sections = Ini::ReadIniSectionNames(dialogini);
        //        for (const auto& section : sections) {
        //            if (Ini::ReadIni(dialogini, section, "bankaddr", "").IsEmpty())
        //                continue;

        //            std::string term = (LPCSTR)section;
        //            if (talkentries.find(term) != talkentries.end())
        //                throw std::runtime_error(__FUNCTION__ ": duplicate talk handler '" + term + "' cannot be loaded.");

        //            auto entry = LoadEntry(dialogini, term);
        //            talkentries[term] = entry;

        //            if (bankaddrToName.find(entry.bankoffset) != bankaddrToName.end()) {
        //                auto i = talkentries.find(bankaddrToName[entry.bankoffset]);
        //                if (i != talkentries.end())
        //                    throw std::runtime_error(__FUNCTION__ ": talk handler '" + term +
        //                        "' bankaddr cannot be the same as handler '" + i->first + "'.");
        //                else
        //                    throw std::runtime_error(__FUNCTION__ ": talk handler '" + term +
        //                        "' bankaddr " + std::to_string(entry.bankoffset) + " conflicts with an orphaned handler index.");
        //            }

        //            bankaddrToName[entry.bankoffset] = term;
        //        }
        //    }

        //    //return talkentries;
        //}

        //void MapElements(std::istream& ifs, int routinecount, std::map<std::string, TalkEntry>& talkentries)
        //{
        //    auto bookmark = ifs.tellg();

        //    std::vector<int> talkhandlerindex;
        //    talkhandlerindex.reserve(routinecount);

        //    while (ifs && !ifs.eof() && talkhandlerindex.size() < routinecount) {
        //        bookmark = ifs.tellg();

        //        std::string rawline;
        //        std::getline(ifs, rawline);
        //        if (RomAsm::Query::IsBlankOrWhitespaceLine(rawline))
        //            continue;

        //        //DEVNOTE  - each usable starts with .WORD, so we don't require the flexibility of LooksLikeDataRow here
        //        std::string line = rawline;

        //    }
        //}


        void ToRom8(int data, size_t destromoffset, std::vector<unsigned char>& rom)
        {
            rom[destromoffset] = (data) & 0xFF;
        }

        void ToRom16(int data, size_t destromoffset, std::vector<unsigned char>& rom)
        {
            rom[destromoffset] = (data) & 0xFF;
            rom[destromoffset + 1] = (data >> 8) & 0xFF;
        }

        //TODO - not used
        bool IsRoutine(std::string line, std::string prefix)
        {
            std::regex rx_routineprefix("^" + prefix);
            std::smatch m;
            if (!std::regex_search(line, m, rx_routineprefix))
                return false;

            return true;
        }

        std::string GetRoutineName(std::string line, const std::regex& rx)
        {
            std::smatch m;
            if (!std::regex_search(line, m, rx))
                return {};

            return m[1].str();;
        }

        void LoadHandlerAddress(int bankoffset, size_t destromoffset, std::vector<unsigned char>& rom)
        {
            ToRom16(bankoffset, destromoffset, rom);
        }

        void LoadHardcode(const TalkElement& elem, std::istream& is, size_t routinestartoffset, std::vector<unsigned char>& rom)
        {
            // romoffset coming in is the offset to the start of the routine.
            // each element has a byte offset (ROM) and instruction offset (ASM).
            // here, we add the byte offset and translate the value

            // add the bankoffset, then move the byte over
            // bear in mnid that this is an ASM-to-ROM conversion.

            std::string line;
            std::getline(is, line);
        }
    }

} // end namespace Old