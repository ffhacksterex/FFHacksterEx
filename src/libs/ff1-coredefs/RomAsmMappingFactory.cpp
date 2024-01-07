#include "stdafx.h"
#include "RomAsmMappingFactory.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "string_functions.h"
#include "collection_helpers.h"

#include "RomAsmMappingDatfile.h"
#include "RomAsmMappingIncbin.h"
#include "RomAsmMappingBinary.h"
#include "RomAsmMappingImmed.h"
#include "RomAsmMappingCond.h"

RomAsmMappingFactory::RomAsmMappingFactory()
{
}

RomAsmMappingFactory::~RomAsmMappingFactory()
{
}

namespace // LOCAL HELPER DECLARATIONS
{
    RomAsmMappingRef DeserializeMapping(CFFHacksterProject& project, CString romasmini, CString editorname);

    RomAsmMappingRef NoSerializer(CFFHacksterProject& project, CString romasmini, CString editorname);
    RomAsmMappingRef ToDatfile(CFFHacksterProject& project, CString romasmini, CString editorname);
    RomAsmMappingRef ToIncbin(CFFHacksterProject& project, CString romasmini, CString editorname);
    RomAsmMappingRef ToBinary(CFFHacksterProject& project, CString romasmini, CString editorname);
    RomAsmMappingRef ToImmed(CFFHacksterProject& project, CString romasmini, CString editorname);
    RomAsmMappingRef ToCond(CFFHacksterProject& project, CString romasmini, CString editorname);
}

// Read all mappiongs from the file.
RomAsmMappingRefs RomAsmMappingFactory::ReadMappings(CFFHacksterProject& project)
{
    throw std::runtime_error(__FUNCTION__ " not implemented yet.");
}

// Read all mappings from the file that are tied to the specified editor.
RomAsmMappingRefs RomAsmMappingFactory::ReadMappings(CFFHacksterProject& project, std::string editor)
{
    throw std::runtime_error(__FUNCTION__ " not implemented yet.");
}

// Read all mappings from the file that are tied to the specified editor.
// Return a map of source to RomAsmMappingRefs.
// Order the map by source ascending, and each RomAsmMappingRefs by offset ascending.
RomAsmGropuedMappingRefs RomAsmMappingFactory::ReadGroupedMappings(CFFHacksterProject& project, std::string editor)
{
    std::map<std::string, RomAsmMappingRefs> groups;
    CString romasmini = project.GetIniFilePath(FFHFILE_RomAsmMappingsPath);
    auto sectionnames = Strings::split(Ini::ReadIni(romasmini, editor.c_str(), "mappings", ""), " ");

    std::vector<std::string> failures;
    for (const auto& section: sectionnames) {
        try {
            auto mapping = DeserializeMapping(project, romasmini, section);
            const auto & source = mapping->source;
            auto iter = groups.find(source);
            if (iter == groups.end())
                groups[source] = RomAsmMappingRefs();
            groups[source].push_back(mapping);
        }
        catch (std::exception & ex) {
            failures.push_back(ex.what());
        }
    }

    //TODO - use a custom exception here instead of simple text?
    if (!failures.empty())
        throw std::runtime_error(std::to_string(failures.size()) + " mappings failed.\n" + Strings::join(failures, "\n"));

    // Sort each group's refs by romoffset ascending.
    for (auto& g: groups) {
        std::sort(g.second.begin(), g.second.end(), [](const auto& left, const auto& right) { return left->romoffset < right->romoffset; });
    }
    return groups;
}

namespace // LOCAL HELPER IMPLEMENTATION
{
    using Serializer = RomAsmMappingRef(*)(CFFHacksterProject& project, CString romasmini, CString editorname);

    std::map<std::string, Serializer> l_serializers = {
        { "datfile", ToDatfile },
        { "incbin", ToIncbin },
        { "binary", ToBinary },
        { "immed", ToImmed },
        { "cond", ToCond }
    };

    RomAsmMappingRef DeserializeMapping(CFFHacksterProject& project, CString romasmini, CString editorname)
    {
        std::string type = (LPCSTR)Ini::ReadIni(romasmini, editorname, "type", "");
        auto iter = l_serializers.find(type);
        auto serializer = iter != l_serializers.end() ? iter->second : NoSerializer;
        return serializer(project, romasmini, editorname);
    }

    RomAsmMappingRef NoSerializer(CFFHacksterProject& project, CString romasmini, CString editorname)
    {
        std::string type = (LPCSTR)Ini::ReadIni(romasmini, editorname, "type", "<unspecified>");
        std::string editor = (LPCSTR)editorname;
        auto reason = "Editor '" + editor + "' failed beacuase no serializer was specified for its mapping type " + type + ".";
        throw std::runtime_error(reason);
    }

    RomAsmMappingRef ToDatfile(CFFHacksterProject& project, CString romasmini, CString editorname)
    {
        return std::make_shared<RomAsmMappingDatfile>(project, (LPCSTR)editorname);
    }

    RomAsmMappingRef ToIncbin(CFFHacksterProject& project, CString romasmini, CString editorname)
    {
        return std::make_shared<RomAsmMappingIncbin>(project, (LPCSTR)editorname);
    }

    RomAsmMappingRef ToBinary(CFFHacksterProject& project, CString romasmini, CString editorname)
    {
        return std::make_shared<RomAsmMappingBinary>(project, (LPCSTR)editorname);
    }

    RomAsmMappingRef ToImmed(CFFHacksterProject& project, CString romasmini, CString editorname)
    {
        return std::make_shared<RomAsmMappingImmed>(project, (LPCSTR)editorname);
    }

    RomAsmMappingRef ToCond(CFFHacksterProject& project, CString romasmini, CString editorname)
    {
        return std::make_shared< RomAsmMappingCond>(project, (LPCSTR)editorname);
    }
}