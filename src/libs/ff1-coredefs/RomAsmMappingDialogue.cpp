#include "stdafx.h"
#include "RomAsmMappingDialogue.h"

#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "regex_helpers.h"
#include "romasm_functions.h"
#include "string_functions.h"
#include "type_support.h"

#include <fstream>
#include <map>
#include <set>

namespace {
    constexpr int UNKNOWN_BANKADDR = 0xFFFF;

    //std::regex rx_constant("\\s*(\\w+)\\s+=\\s+([\\$%]?\\w+)\\s*(?:;.*)?");
    std::regex rx_constant("\\s*(\\w+)\\s+=\\s+([^;]+)(?:\\s*;.*)?");

    //std::regex rx_constant_xplusy("\\s*(\\w+)\\s+=\\s+([\\$%]?\\w+)\\s*\\+\\s*([\\$%]?\\w+)\\s*(?:;.*)?");


    const auto NUMBERFORMAT = "(\\#?\\-?[$|%]?[0-9A-Fa-f]+)";
    //auto result = regex_helpers::eval_formula(value, constants, NUMBERFORMAT);

    TalkHandler LoadEntry(CString dialoguepath, std::string term);
    std::vector<std::string> ReadRoutinePointerRow(std::string line, std::streampos pos);

    template <typename K, typename V>
    size_t FindHandlerIndex(const std::map<K, V>& m, K k)
    {
        auto i = m.find(k);
        return i != m.end() ? i->second : -1;
    }

    template <typename T>
    void EraseByValue(std::set<T>& s, const T& t)
    {
        auto i = s.find(t);
        if (i != s.end())
            s.erase(i);
    }

    void ToRom8(int data, size_t destromoffset, std::vector<unsigned char>& rom);
    void ToRom16(int data, size_t destromoffset, std::vector<unsigned char>& rom);
    void ToRomFromBytestring(std::string bytestring, size_t destromoffset, std::vector<unsigned char>& rom);

    std::string GetHandlerName(std::string line, const std::regex& rx);
    std::string MapLookup(const std::map<std::string, std::string>& constants, std::string value);
    int GetLookupMapIndex(const TalkElement& elem);

    //using RomLoadRoutineFunc = void(*)(const TalkElement& elem, std::string line, size_t destbaseoffset,
    //    std::vector<unsigned char>& rom, const std::map<std::string, int>& symbols);
    //RomLoadRoutineFunc GetRomLoadRoutine(const TalkElement& elem);

    //RomLoadRoutineFunc GetRomLoadRoutine(const TalkElement& elem);
}

RomAsmMappingDialogue::RomAsmMappingDialogue(CFFHacksterProject& project, std::string mapping)
    : m_fanfareEnabled(false)
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
    m_fanfareEnabled = m_fanfareCond.FromString((LPCSTR)Ini::ReadIni(romasmini, section, "fanfarecond", ""));

    Preload(project);

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
    if (!m_fanfareEnabled) errors.push_back("fanfarecond failed to load properly");
    //if (!Paths::FileExists(dialoguepath.c_str())) errors.push_back("dialogue file '" + dialoguepath + "'does not exist in the project");

    if (!errors.empty())
        throw std::runtime_error("Editor '" + mapping + "' config is malformed: " + Strings::join(errors, "; "));

    m_romloadroutines =
    {
        { "hcobj", { &RomAsmMappingDialogue::LoadOpval, 1 }},
        { "hcspr", { &RomAsmMappingDialogue::LoadOpval, 1 }},
        { "hcbyte", { &RomAsmMappingDialogue::LoadOpval, 1 }},
        { "hcbattle", { &RomAsmMappingDialogue::LoadOpval, 1 }},
        { "hcweapon", { &RomAsmMappingDialogue::LoadOpval, 1 }},
        { "hcnntele", { &RomAsmMappingDialogue::LoadOpval, 1 }},
        { "hctext", { &RomAsmMappingDialogue::LoadOpval, 1 }},
        { "hcevent", { &RomAsmMappingDialogue::LoadOpval, 1 }},
        { "hcitemormap", { &RomAsmMappingDialogue::LoadSymbol, 2 }},
        { "hcitemorcanoe", { &RomAsmMappingDialogue::LoadSymbol, 2 }},
        { "hcitem", { &RomAsmMappingDialogue::LoadSymbol, 2 }},
        { "hcmap", { &RomAsmMappingDialogue::LoadSymbol, 2 }},
        { "hcfanfare", { &RomAsmMappingDialogue::LoadFanfare, 2 }},
    };
}

RomAsmMappingDialogue::~RomAsmMappingDialogue()
{
}

void RomAsmMappingDialogue::Preload(CFFHacksterProject& project)
{
    //TODO - load the dialogue file helpers:
    // 1) load in declared order, e.g. vector<TalkHandler>
    //  - load from the project .dialogue file
    // 2) map name to index (fail on dupe)
    // 3) map bankaddr to index (fail on dupe)

    //TODO - are the rest of these actually mapping responsibilities or editor tasks?
    // 4) load symbols from constants.inc
    //  - load in order as key/value tuples, e.g. vector<pair<string,string>>
    //  - map key vector to index

    //TODO - load in parallel
    LoadHandlerFromDialogFile((LPCSTR)project.GetIniFilePath(FFHFILE_DialoguePath));
    LoadConstants(project);
    LoadSymbols(project);
    LoadFanfare(project);
}

void RomAsmMappingDialogue::Load(std::istream& sourcefile, std::vector<unsigned char>& rom,
	const std::map<std::string, std::string>& options)
{
    // 1) load handler table from asm source (since we know the handler entries, we can load from either name or bankaddr)
    // 2) load hardcoded elements for each handler from asm source
    //    - need symbol mapping from constants.inc
    // the BIN_OBJECTDATA mapping loads the params for us

    RomAsm::Traverse::SeekToInstruction(sourcefile, name, label, sublabel, instoffset);
    LoadHandlerTable(sourcefile, rom);
    LoadHardcodedRoutineValues(sourcefile, rom);

    throw std::runtime_error(__FUNCTION__ " not implemented yet");
}

void RomAsmMappingDialogue::Save(std::istream& sourcefile, std::ostream& destfile,
	const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    // 1) save handler table to asm source (copy the bankaddrs/routinenames from to asm handler table)
    // 2) save hardcoded elements for each handler to asm source
    // the BIN_OBJECTDATA mapping saves the params for us

    throw std::runtime_error(__FUNCTION__ " not implemented yet");
}

void RomAsmMappingDialogue::LoadHandlerFromDialogFile(std::string dialogpath)
{
    m_handlerdefs.clear();
    m_handlerByName.clear();
    m_handlerByBankaddr.clear();

    auto dialogini = dialogpath.c_str();
    auto sections = Ini::ReadIniSectionNames(dialogini);
    for (const auto& section : sections) {
        if (Ini::ReadIni(dialogini, section, "bankaddr", "").IsEmpty())
            continue;

        std::string term = (LPCSTR)section;
        if (m_handlerByName.find(term) != m_handlerByName.end())
            throw std::runtime_error(__FUNCTION__ ": cannot load talk handler '" + term + "' due to it's name being a duplicate.");

        auto entry = LoadEntry(dialogini, term);
        if (m_handlerByBankaddr.find(entry.bankoffset) != m_handlerByBankaddr.end())
            throw std::runtime_error(__FUNCTION__ ": cannot load talk handler '" + term + "' due to its bankaddr being a duplicate.");

        auto index = m_handlerdefs.size();
        m_handlerdefs.push_back(entry);
        m_handlerByName[term] = index;
        m_handlerByBankaddr[entry.bankoffset] = index;
    }
}

namespace {
    //TODO - add to collection helpers

    //TODO - won't compile, doesn't like the return type
    //template <typename V>
    //std::map<std::string, V>::iterator FindByPrefix(std::map<std::string, V>& m, std::string prefix)
    //{
    //    auto it = m.lower_bound(prefix);
    //    if (it != std::end(m) && it->first.compare(0, prefix.size(), prefix) == 0)
    //        return it;

    //    return m.end();
    //}

    std::map<std::string, std::string>::iterator FindByPrefix(std::map<std::string, std::string>& m, std::string prefix)
    {
        auto it = m.lower_bound(prefix);
        if (it != std::end(m) && it->first.compare(0, prefix.size(), prefix) == 0)
            return it;

        return m.end();
    }
} // end namespace (unnamed)



void RomAsmMappingDialogue::LoadConstants(CFFHacksterProject& project)
{
    // load constants into a map
    // since we need to support partial searches, either:
    // - segment the map
    // - do a partial binary search for a specific starting letter

    //TODO -  Note the case of Constants.inc in the original assembly. If
    //  this is an issue, then make the filename configurable as a value.
    auto constpath = Paths::Combine({project.ProjectFolder, "asm", "Constants.inc"});

    //CATEGORY_UNKNOWN  = $01
    //CHAN_SQ1          = CHAN_START
    //CHAN_SQ2          = CHAN_START + CHAN_BYTES
    //CHAN_TRI          = CHAN_START + (2 * CHAN_BYTES)

    ////std::regex rx_constant("^(\\w+)\\s+=\\s+([\\$%]?\\w+)");
    //std::regex rx_constant("\\s*(\\w+)\\s+=\\s+([\\$%]?\\w+)\\s*(?:;.*)?");

    //m_constants.clear();
    m_constants2.clear();

    std::ifstream ifs((LPCSTR)constpath);
    std::string line;
    while (ifs && !ifs.eof()) {
        std::getline(ifs, line);
        std::smatch m;
        if (std::regex_match(line, m, rx_constant))
        {
            auto term = m[1].str();
            auto value = Strings::trimright(m[2].str()); //TODO - until I figure out how to shave trailing spaces in regex
            if (!value.empty()) {
                //m_constants[term] = MapLookup(m_constants, value);
                m_constants2[term] = regex_helpers::eval_formula(value, m_constants2, NUMBERFORMAT);
            }
        }
    }
}

void RomAsmMappingDialogue::LoadSymbols(CFFHacksterProject& project)
{
    auto varspath = Paths::Combine({ project.ProjectFolder, "asm", "variables.inc" });

    //CATEGORY_UNKNOWN  = $01
    //CHAN_SQ1          = CHAN_START
    //bridge_vis        = unsram + $08
    //CHAN_SQ2          = CHAN_START + CHAN_BYTES
    //CHAN_TRI          = CHAN_START + (2 * CHAN_BYTES)
    //math_hitchance    = btl_mathbuf + (MATHBUF_HITCHANCE*2) 

    //m_symbols.clear();
    m_symbols2.clear();

    std::ifstream ifs((LPCSTR)varspath);
    std::string line;
    while (ifs && !ifs.eof()) {
        std::getline(ifs, line);
        std::smatch m;
        if (std::regex_match(line, m, rx_constant))
        {
            auto term = m[1].str();
            auto value = m[2].str();
            if (!value.empty()) {
                //m_symbols[term] = MapLookup(m_symbols, value);
                m_symbols2[term] = regex_helpers::eval_formula(value, m_symbols2, NUMBERFORMAT);
            }
        }
    }
}

//TODO - REMOVE
void RomAsmMappingDialogue::LoadFanfare(CFFHacksterProject& project)
{
    //m_fanfareEnabled = false;

    ////TODO - change this to use fanfare on/off values defined in the dialogue mapping itself
    //auto valini = project.GetIniFilePath(FFHFILE_ValuesPath);
    //auto talkfanfare = strtol(Ini::ReadIni(valini, "talkfanfare", "Value", "-0x2"), nullptr, 16);
    //auto notalksnd = strtol(Ini::ReadIni(valini, "notalksnd", "Value", "-0x2"), nullptr, 16);
    //if (talkfanfare == -2 || notalksnd == -2) {
    //    //TODO - visibly warn that hcfanfare will be disabled without both talkfanfare and notalksnd
    //    //  ... or should two different values be used
    //    //  ... and should these be defined by the mapping itself, or in the values file?
    //    // if in the mapping, the intent is clearer and is associated to the dialog handler directly,
    //    // and this is a memory mapping issue.
    //}
    //else {
    //    m_constants2["talkfanfare"] = talkfanfare;
    //    m_constants2["notalksnd"] = notalksnd;
    //    m_fanfareEnabled = true;
    //}
}

void RomAsmMappingDialogue::LoadHandlerTable(std::istream& is, std::vector<unsigned char>& rom)
{
    // When the marker loads, we'll already be at the start of the table.
    // This routine loads the handlers and returns.
    // Stop once we hit a non-data row, non blank, non whole-line comment.
    // If we don't read the expected count of handlers is read, it's an error.

    auto bookmark = is.tellg();
    int livecount = 0;

    //m_handlerpointers.clear();

    // Considerations:
    // if a routine is in ROM/Asm but NOT in the .dialogue file, use UNKNOWN_BANKADDR instead.
    // - when saving, that entry will be skipped, keaving the original value intact

    try {
        while (is && !is.eof() && livecount < tablecount) {
            bookmark = is.tellg();

            // for each row:
            // - read the data row, and produce a list of names/bankaddrs
            // - for each entry
            //   > look up the handler by name or bankaddr
            //     * if found, add to the pointertable
            //     * if it's a bankaddr that's not in the known list, add UNKNOWN_BANKADDR instead
            //       this will be ignored by the editor when saving (i.e. there's a handler in ROM/assembly
            //       that isn't in the .dialogue file).

            std::string rawline;
            std::getline(is, rawline);
            if (RomAsm::Query::IsBlankOrWhitespaceLine(rawline))
                continue;

            //DEVNOTE - supported data rows start with .WORD, so we don't require the flexibility of LooksLikeDataRow here
            std::string line = rawline;
            if (RomAsm::Query::IsApparentDataRow(line)) {
                // Copy each bank addr into rom[]
                auto terms = ReadRoutinePointerRow(line, bookmark);
                for (const auto& term : terms) {
                    auto bankaddr = LookupBankAddress(term, bookmark);
                    auto destoffset = talkoffset + ((size_t)2 * livecount);
                    ToRom16(bankaddr, destoffset, rom);
                    ++livecount;
                }
            }
            else if (!RomAsm::Query::IsComment(rawline)) {
                // the comment check only happens if the data row check failed first.
                // If this row is anything but a data row or comment at this point, we're done.
                break;
            }
        }
    }
    catch (std::exception& ex) {
        throw std::runtime_error("Mapping '" + this->name + "' encounter the following processing error starting at file position "
            + std::to_string(bookmark) + ": " + ex.what());
    }

    if (livecount != tablecount) {
        throw std::runtime_error("Mapping '" + this->name + "' retrieved " + std::to_string(livecount)
            + " bytes instead of the expected " + std::to_string(tablecount) + ".");
    }
}

void RomAsmMappingDialogue::LoadHardcodedRoutineValues(std::istream& is, std::vector<unsigned char>& rom)
{
    // traverse to the start at routine
    // walk through the file, stopping at each top level routine label
    // - if it's the stop at routine, stop
    // - if it's NOT a talk routine, skip it
    // - if it isn't in the handler by name map, note it as unused
    // - else, process it, for each element
    //   * move to instoffset and read the line
    //   * pick the extract routine based on type
    //   * use that routine to move elem.bytecount bytes
    //     > throw if the bytecount and routine don't match

    m_unusedhandlers.clear();

    //TODO - for now, no start at routine, so start from where we are.

    std::streampos bookmark = is.tellg();
    std::string prefix = "Talk_";
    std::regex rc_talkroutine("^(" + prefix + "\\w+)");

    std::set<std::string> allhandlers;
    for (const auto& kv : m_handlerByName) allhandlers.insert(kv.first);

    try {
        while (is && !is.eof()) {
            bookmark = is.tellg();

            std::string rawline;
            std::getline(is, rawline);
            if (RomAsm::Query::IsBlankOrWhitespaceLine(rawline))
                continue;

            auto term = GetHandlerName(rawline, rc_talkroutine);
            if (term.empty())
                continue;

            auto index = FindHandlerIndex(m_handlerByName, term);
            //TODO - are unused/unused handlers a mapping concern or an editor concern?
            if (index == -1) {
                //TODO - this handler is NOT in the .dialogue file
            }
            else {
                EraseByValue(allhandlers, term);                
                auto& entry = m_handlerdefs[index];
                auto basedestoffset = talkptradd + entry.bankoffset;
                std::string contextname = "Handler " + term;

                for (const auto& elem : entry.elements) {
                    if (!elem.hardcoded) continue;

                    is.seekg(bookmark);
                    RomAsm::Traverse::SeekToInstruction(is, elem.instoffset, contextname);

                    std::string elemline;
                    std::getline(is, elemline);

                    //TODO - next, add support for hcfanfare
                    auto routine = GetRomLoadRoutine(elem);

                    auto& lookup = GetLookupMap(elem);
                    (this->*routine)(elem, elemline, basedestoffset, rom, lookup);
                }
            }
        }
    }
    catch (std::exception& ex) {
        throw std::runtime_error(__FUNCTION__ " failure during loading: " + std::string(ex.what()));
    }

    m_unusedhandlers = allhandlers; // in the .dialogue file, but not asm source

    throw std::runtime_error(__FUNCTION__ " not implemnted yet");
}

int RomAsmMappingDialogue::LookupBankAddress(std::string term, std::streampos pos)
{
    if (term.empty())
        throw std::runtime_error("Pointer table row starting at pos " + std::to_string(pos) + " contains an empty entry name/bankaddr.");

    int bankaddr = -2;
    if (term[0] == '$') {
        auto s = (int)strtoul(term.c_str() + 1, nullptr, 16);
        auto index = FindHandlerIndex(m_handlerByBankaddr, s);
        bankaddr = index != -1 ? m_handlerdefs[index].bankoffset : UNKNOWN_BANKADDR;
    }
    else {
        auto index = FindHandlerIndex(m_handlerByName, term);
        if (index != -1)
            bankaddr = m_handlerdefs[index].bankoffset;
    }
    if (bankaddr == -2)
        throw std::runtime_error("No handler address match found for '" + term + "' on row starting at pos " + std::to_string(pos) + ".");

    return bankaddr;
}

//const std::map<std::string, std::string>& RomAsmMappingDialogue::GetLookupMap(const TalkElement& elem)
const std::map<std::string, int>& RomAsmMappingDialogue::GetLookupMap(const TalkElement& elem)
{
    //static const std::map<std::string, std::string>* lookups[] = { &m_constants, &m_symbols };
    static const std::map<std::string, int>* lookups[] = { &m_constants2, &m_symbols2 };
    auto index = GetLookupMapIndex(elem);
    auto* plookup = lookups[index];
    return *plookup;
}


namespace {

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

    std::vector<std::string> ReadRoutinePointerRow(std::string line, std::streampos pos)
    {
        // if it's not a WORD followed by comma-separated values, we're done
        int recwidth = RomAsm::Parse::ExtractDataRowRecWidth(line);
        if (recwidth != 2)
            throw std::runtime_error("On the line at position " + std::to_string(pos) +
                ", the data row is not .WORD and cannot be processed as a bank address list.");

        auto terms = Strings::splitrx(line, "\\s*,\\s*", true);
        return terms;
    }

    void ToRom8(int data, size_t destromoffset, std::vector<unsigned char>& rom)
    {
        rom[destromoffset] = (data) & 0xFF;
    }

    void ToRom16(int data, size_t destromoffset, std::vector<unsigned char>& rom)
    {
        rom[destromoffset] = (data) & 0xFF;
        rom[destromoffset + 1] = (data >> 8) & 0xFF;
    }

    void ToRomFromBytestring(std::string bytestring, size_t destromoffset, std::vector<unsigned char>& rom)
    {
        auto v = Strings::Convert::ToBytvector(bytestring, ' ', true);
        overwrite(rom, v, destromoffset, v.size());
    }

    std::string GetHandlerName(std::string line, const std::regex& rx)
    {
        std::smatch m;
        if (!std::regex_search(line, m, rx))
            return {};

        return m[1].str();
    }

    std::string MapLookup(const std::map<std::string, std::string>& constants, std::string value)
    {
        if (value[0] != '$' && value[0] != '%' && !isdigit(value[0])) {
            auto i = constants.find(value);
            while (i != constants.end()) {
                value = i->second;
                if (value[0] == '$' || value[0] == '%' || isdigit(value[0]))
                    break;

                i = constants.find(value);
            }
        }
        return value;
    }

    const std::map<std::string, int> l_lookupmaptypes = {
        { "hcmap", 1 },
        { "hcitem", 1 },
        { "hcitemorcanoe", 1 },
        { "hcitemormap", 1 },
        { "item", 1 },
    };

    int GetLookupMapIndex(const TalkElement& elem)
    {
        auto i = l_lookupmaptypes.find(elem.type);
        auto index = (i != l_lookupmaptypes.end()) ? i->second : 0;
        return index;
    }


    std::regex rx_inst_opval("\\s*\\w{3}\\s+#(\\w+)");
    std::regex rx_inst_param("\\s*\\w{3}\\s+(\\w+(?:\\+\\d+))"); // LDA tmp+2
    std::regex rx_inst_symbol("\\s*\\w{3}\\s+(\\w+)");

    void LoadOpval(const TalkElement& elem, std::string line, size_t destbaseoffset,
        std::vector<unsigned char>& rom,
        //const std::map<std::string, std::string> & constants
        const std::map<std::string, int>& constants
    )
    {
        ASSERT(elem.datasizebytes == 1);

        if (elem.type == "hcevent") {
            int i = 0;
        }

        std::smatch m;
        if (!std::regex_search(line, m, rx_inst_opval))
            throw std::runtime_error("cannot read opcode value for type '" + elem.type + "' on this line: " + line);

        //TODO - need to handle symbols here, meaning we need to have read Constants.inc
        auto strvalue = m[1].str();
        //auto value = MapLookup(constants, strvalue);

        //auto intvalue = Types::to_int(value);

        //--
        //TODO - strvalue or line?
        auto intvalue = regex_helpers::eval_formula(strvalue, constants, NUMBERFORMAT);
        //--
        auto destoffset = destbaseoffset + elem.byteoffset;
        ToRom8(intvalue, destoffset, rom);
    }

    //TODO - this works, but it likely isn't needed since params (which are not hardcoded) are handled elsewhere
    //void LoadParam(const TalkElement& elem, std::string line, size_t destbaseoffset,
    //    std::vector<unsigned char>& rom, const std::map<std::string, std::string>& constants)
    //{
    //    ASSERT(elem.datasizebytes == 1);

    //    std::smatch m;
    //    if (!std::regex_search(line, m, rx_inst_param))
    //        throw std::runtime_error("cannot read opcode value for type '" + elem.type + "' on this line: " + line);

    //    auto value = m[1].str();
    //    auto plus = value.find('+');
    //    auto index = (plus != std::string::npos) ? atol(value.c_str() + plus) : 0;

    ////TODO - don't modify the hardcoded index, instead use the index to access the data param
    //    auto destoffset = destbaseoffset + elem.byteoffset;
    //    ToRom8(index, destoffset, rom);
    //}

    void LoadSymbol(const TalkElement& elem, std::string line, size_t destbaseoffset,
        std::vector<unsigned char>& rom,
        //const std::map<std::string, std::string>& constants
        const std::map<std::string, int>& constants
    )
    {
        std::smatch m;
        if (!std::regex_search(line, m, rx_inst_symbol))
            throw std::runtime_error("cannot read symbol value for type '" + elem.type + "' on this line: " + line);

        //TODO - need to handle symbols here, meaning we need to have read variables.inc
        auto strvalue = m[1].str();
        //auto value = MapLookup(constants, strvalue);

        //auto intvalue = Types::to_int(value);

        //--
        //TODO - strvalue or line?
        auto intvalue = regex_helpers::eval_formula(strvalue, constants, NUMBERFORMAT);
        //--

        auto destoffset = destbaseoffset + elem.byteoffset;
        switch (elem.datasizebytes) {
        case 1:
            ToRom8(intvalue, destoffset, rom);
            break;
        case 2:
            ToRom16(intvalue, destoffset, rom);
            break;
        default:
            throw std::runtime_error("Element '" + elem.type + "' uses unsupported byte size " + std::to_string(elem.datasizebytes) + ".");
        }        
    }

    void LoadFanfare(const TalkElement& elem, std::string line, size_t destbaseoffset,
        std::vector<unsigned char>& rom, const std::map<std::string, int>& constants)
    {
        if (elem.datasizebytes != 2)
            throw std::runtime_error("Element '" + elem.type + "' uses unsupported byte size " + std::to_string(elem.datasizebytes) + ".");

        // three cases - the line (sans whitespace) should:
        // equal talkfanfare, set on
        // equal talknosnd, set off
        // neither, disabled
    }

    void LoadCond(const TalkElement& elem, std::string line, size_t destbaseoffset,
        std::vector<unsigned char>& rom, const std::map<std::string, int>& constants)
    {
        if (elem.datasizebytes != 2)
            throw std::runtime_error("Element '" + elem.type + "' uses unsupported byte size " + std::to_string(elem.datasizebytes) + ".");

        std::smatch m;
        if (!std::regex_search(line, m, rx_inst_symbol))
            throw std::runtime_error("cannot read symbol value for type '" + elem.type + "' on this line: " + line);

        //TODO - This should be conditional, so this element needs to lookup the condition for this element.
        //  that should the asmtrue/asmfalse choices as follows:
        //  - if the source line matches asmtrue, load the corresponding romtrue bytes
        //  - if the source line matches asmfalse, load the corresponding romfalse bytes
        //  - else, disable the condition and don't load the bytes (we'll ignore them when saving)

        //Alternative: the current editor uses two values, talkfanfare and notalksnd.
        // Both are 16-bit hex values.
        // hcfanfare could use those as a special case (th emapping could preload the values).
        // if both aren't found, then perhaps hcfanfare would be unsupported (and the element would be ignored).

        //auto* pcond = this->GetCondition("TODO: handlername", elem.slot); // maybe elem.handlername
        //if (pcond != nullptr) {
        //    auto destoffset = destbaseoffset + elem.byteoffset;
        //    bool enable = true;
        //    if (line == pcond->asmtrue) {
        //        ToRomFromBytestring(pcond->romtrue, destoffset, rom);
        //    } 
        //    else if (line == "pcond->asmfalse") {
        //        ToRomFromBytestring(pcond->romfalse, destoffset, rom);
        //    }
        //    else {
        //        enable = false;
        //    }
        //    pcond->SetEnabled(enable);
        //}

        // the above implies that there's an interface to acess conditions for an element
        // so in the editor, we'd see code somewhat like this:

        //auto handlerref = GetHandlerRef(GetCurSelData(m_combo)); // 0x9297, bankaddr of Talk_KingConeria
        //auto* pcond = m_dialogmapping.GetCondition(handlerref, elem.slot); // handlerref is "Talk_KingConeria"
        //if (pcond != nullptr) {
        //    pcond->IsEnabled();
        //    bool on = pcond->Get();
        //    // .. do something to change the value of 'on', hence the editor never knows the underlying value
        //    pcond->Set(on);
        //    // the editor only knows to access the element according to a slot
        //}
    }

    //std::map<std::string, std::pair<RomLoadRoutineFunc, int>> l_romloadroutines
    //{
    //    { "hcobj", { LoadOpval, 1 }},
    //    //{ "text", { LoadParam, 1 }}, //TODO - not needed, params are handled elsewhere
    //    { "hcitemormap", { LoadSymbol, 2 }},
    //    { "hctext", { LoadOpval, 1 }},
    //    { "hcfanfare", { LoadCond, 2 }},
    //};

    //RomLoadRoutineFunc GetRomLoadRoutine(const TalkElement& elem)
    //{
    //    auto i = l_romloadroutines.find(elem.type);
    //    if (i == l_romloadroutines.end())
    //        throw std::runtime_error("no ROM element load routine found for type '" + elem.type + ".");

    //    if (i->second.second != elem.datasizebytes)
    //        throw std::runtime_error("ROM element load routine for type '" + elem.type + ". expects " + std::to_string(i->second.second)
    //            + " bytes, but the element byte size is " + std::to_string(elem.datasizebytes) + ".");

    //    return i->second.first;
    //}
}

RomAsmMappingDialogue::RomLoadRoutineFunc RomAsmMappingDialogue::GetRomLoadRoutine(const TalkElement& elem)
{
    auto i = m_romloadroutines.find(elem.type);
    if (i == m_romloadroutines.end())
        throw std::runtime_error("no ROM element load routine found for type '" + elem.type + ".");

    if (i->second.second != elem.datasizebytes)
        throw std::runtime_error("ROM element load routine for type '" + elem.type + ". expects " + std::to_string(i->second.second)
            + " bytes, but the element byte size is " + std::to_string(elem.datasizebytes) + ".");

    return i->second.first;
}

void RomAsmMappingDialogue::LoadOpval(const TalkElement& elem, std::string line, size_t destbaseoffset,
    std::vector<unsigned char>& rom, const std::map<std::string, int>& constants)
{
    if (elem.datasizebytes != 1)
        throw std::runtime_error("Element '" + elem.type + "' uses unsupported byte size " + std::to_string(elem.datasizebytes) + ".");

    std::smatch m;
    if (!std::regex_search(line, m, rx_inst_opval))
        throw std::runtime_error("cannot read opcode value for type '" + elem.type + "' on this line: " + line);

    auto strvalue = m[1].str();
    auto intvalue = regex_helpers::eval_formula(strvalue, constants, NUMBERFORMAT);
    auto destoffset = destbaseoffset + elem.byteoffset;
    ToRom8(intvalue, destoffset, rom);
}

void RomAsmMappingDialogue::LoadSymbol(const TalkElement& elem, std::string line, size_t destbaseoffset,
    std::vector<unsigned char>& rom, const std::map<std::string, int>& constants)
{
    std::smatch m;
    if (!std::regex_search(line, m, rx_inst_symbol))
        throw std::runtime_error("cannot read symbol value for type '" + elem.type + "' on this line: " + line);

    auto strvalue = m[1].str();
    auto intvalue = regex_helpers::eval_formula(strvalue, constants, NUMBERFORMAT);
    auto destoffset = destbaseoffset + elem.byteoffset;
    switch (elem.datasizebytes) {
    case 1:
        ToRom8(intvalue, destoffset, rom);
        break;
    case 2:
        ToRom16(intvalue, destoffset, rom);
        break;
    default:
        throw std::runtime_error("Element '" + elem.type + "' uses unsupported byte size " + std::to_string(elem.datasizebytes) + ".");
    }
}

void RomAsmMappingDialogue::LoadFanfare(const TalkElement& elem, std::string line, size_t destbaseoffset,
    std::vector<unsigned char>& rom, const std::map<std::string, int>& constants)
{
    if (elem.datasizebytes != 2)
        throw std::runtime_error("Element '" + elem.type + "' uses unsupported byte size " + std::to_string(elem.datasizebytes) + ".");

    if (!m_fanfareEnabled)
        return; // fanfare is disabled, so skip processing the line

    auto destoffset = destbaseoffset + elem.byteoffset;
    const auto content = RomAsm::Parse::ExtractLineContent(line, true);
    if (content == m_fanfareCond.asmtrue)
        ToRomFromBytestring(m_fanfareCond.romtrue, destbaseoffset, rom);
    else if (content == m_fanfareCond.asmfalse)
        ToRomFromBytestring(m_fanfareCond.romfalse, destbaseoffset, rom);
}