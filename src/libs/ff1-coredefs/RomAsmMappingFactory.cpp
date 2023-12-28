#include "stdafx.h"
#include "RomAsmMappingFactory.h"

#include "FailedMapping.h"
#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "RomAsmMappingDatFile.h"
#include "RomAsmMappingIncbin.h"
#include "RomAsmMappingBinary.h"
#include "type_support.h"

#include <functional>

RomAsmMappingFactory::RomAsmMappingFactory()
{
}

RomAsmMappingFactory::~RomAsmMappingFactory()
{
}

// For now, this implementation handles INI files.

std::vector<std::unique_ptr<RomAsmMapping>> RomAsmMappingFactory::ReadMappings(CFFHacksterProject& project, std::string key)
{
    std::vector<std::unique_ptr<RomAsmMapping>> mappings;
    auto mapini = project.GetIniFilePath("romasmmappings");
    auto prefix = key.c_str() + CString("-");
    auto altprefix = "*-";
    auto sections = Ini::ReadIniSectionNames(mapini);
    for (const auto& section : sections) {
        // Sections beginning with *- are read by all editors.
        if (section.Find(prefix) != 0 && section.Find(altprefix) != 0)
            continue;

        mappings.push_back(Deserialize(project, (LPCSTR)section));
    }
    return mappings;
}

namespace {
    constexpr const char* NotFoundKey = "@notfound";

    std::unique_ptr<RomAsmMapping> ToDatfile(CFFHacksterProject& project, std::string type, std::string mappingDefinition);
    std::unique_ptr<RomAsmMapping> ToIncbin(CFFHacksterProject& project, std::string type, std::string mappingDefinition);
    std::unique_ptr<RomAsmMapping> ToBinary(CFFHacksterProject& project, std::string type, std::string mappingDefinition);
    std::unique_ptr<RomAsmMapping> ToOpval(CFFHacksterProject& project, std::string type, std::string mappingDefinition);
    std::unique_ptr<RomAsmMapping> NoDeserializer(CFFHacksterProject& project, std::string type, std::string mappingDefinition);

    using Deserializer = std::unique_ptr<RomAsmMapping> (*)(CFFHacksterProject& project, std::string type, std::string mappingDefinition);

    std::map<std::string, Deserializer> l_deserializers = {
        { std::string("datfile"), ToDatfile },
        { std::string("incbin"), ToIncbin},
        { std::string("binary"), ToBinary},
        { std::string("opval"), ToOpval},
        { std::string("@notfound"), NoDeserializer }
    };

    std::string FromDatfile(CFFHacksterProject& project, std::string type, std::string mappingDefinition, RomAsmMapping& mapping);
    std::string NoSerializer(CFFHacksterProject& project, std::string type, std::string mappingDefinition, RomAsmMapping& mapping);
    
    using Serializer = std::string (*)(CFFHacksterProject& project, std::string type, std::string mappingDefinition, RomAsmMapping& mapping);

    std::map<std::string, Serializer> l_serializers = {
        { std::string("datfile"), FromDatfile },
        { std::string("@notfound"), NoSerializer }
    };

    //TODO - move these to a utility header/cpp with a namespace.
    //TODO - find cleaner implementations of these functions?
    CString ReadIniText(CString mapini, CString valini, CString section, CString key);
    int ReadIniInt(CString mapini, CString valini, CString section, CString key);
    std::string ReadSource(CString mapini, CString valini, CString section, CString key);
}

// Deserialize a mapping definition (in text form) into a mapping object.
// This implementation receives the section name as mappingDefinition.
std::unique_ptr<RomAsmMapping> RomAsmMappingFactory::Deserialize(CFFHacksterProject& project, std::string mappingDefinition)
{
    auto mapini = project.GetIniFilePath("romasmmappings");
    auto valini = project.GetIniFilePath(FFHFILE_ValuesPath);
    std::string type = (LPCSTR)Ini::ReadIni(mapini, mappingDefinition.c_str(), "type", "");
    auto iter = l_deserializers.find(type.c_str());
    auto func = iter != std::cend(l_deserializers) ? iter->second : l_deserializers[NotFoundKey];
    return func(project, type, mappingDefinition);
}

// Serialize a mapping object into a mapping definition (in text form).
// This implementation receives the section name as mappingDefinition.
std::string RomAsmMappingFactory::Serialize(CFFHacksterProject& project, const RomAsmMapping& mapping)
{
    //TODO - implement this when we want to create a mappings UI.
    throw std::runtime_error("Saving not implemented yet,");
}

namespace // IMPLEMENTATIONS
{    
    // === DESERIALIZERS

    std::unique_ptr<RomAsmMapping> ToDatfile(CFFHacksterProject& project, std::string type, std::string mappingDefinition)
    {
        ASSERT(type == "datfile");
        auto mapini = project.GetIniFilePath("romasmmappings");
        auto valini = project.GetIniFilePath(FFHFILE_ValuesPath);
        auto section = mappingDefinition.c_str();

        auto m = std::make_unique<RomAsmMappingDatFile>();
        m->name = mappingDefinition;
        m->type = type;
        m->offset = ReadIniInt(mapini, valini, section, "offset");
        m->source = ReadSource(mapini, section, section, "source");
        m->count = ReadIniInt(mapini, valini, section, "count");
        m->readonly = Ini::ReadIni(mapini, section, "readonly", "false") == "true";
        m->projectfolder = (LPCSTR)project.ProjectFolder;
        return m;
    }

    //TODO - trap exceptions and instead turn them into failed mapping text.
    //      ReadIniInit and ReadSource both throw.
    //TODO - pass the name along with the mappingDefinition
    std::unique_ptr<RomAsmMapping> ToIncbin(CFFHacksterProject& project, std::string type, std::string mappingDefinition)
    {
        ASSERT(type == "incbin");
        auto mapini = project.GetIniFilePath("romasmmappings");
        auto valini = project.GetIniFilePath(FFHFILE_ValuesPath);
        auto section = mappingDefinition.c_str();

        //TODO - replace this with a Validate virtual method and throw with the relevant error message
        auto label = Ini::ReadIni(mapini, section, "label", "");
        auto instoffset = atol(Ini::ReadIni(mapini, section, "instoffset", "-1")); //TODO - is atol safe here?
        auto offset = ReadIniInt(mapini, valini, section, "offset");
        std::string errtext;
        if (label.IsEmpty()) errtext = "The label cannot be empty. ";
        if (instoffset < 1) errtext = "The instoffset must be greater than zero. ";
        if (offset < 1) errtext = "The offset must be greater than 0."; //TODO - greater than ROM header length instead?
        if (!errtext.empty())
            return std::make_unique<FailedMapping>(mappingDefinition, errtext);

        auto m = std::make_unique<RomAsmMappingIncbin>();
        m->name = mappingDefinition;
        m->type = type;
        m->offset = offset;
        m->source = ReadSource(mapini, section, section, "source");
        m->label = label;
        m->sublabel = Ini::ReadIni(mapini, section, "sublabel", "");
        m->instoffset = instoffset;
        m->count = ReadIniInt(mapini, valini, section, "count");
        m->readonly = Ini::ReadIni(mapini, section, "readonly", "false") == "true";
        m->projectfolder = (LPCSTR)project.ProjectFolder;            
        return m;
    }

    std::unique_ptr<RomAsmMapping> ToBinary(CFFHacksterProject& project, std::string type, std::string mappingDefinition)
    {
        // Binary reads a series of bytes from data rows (.BYTE/.WORD/.FARADDR)
        // use strnicmp functions to compare the strings (write a wrapper for it)

        ASSERT(type == "binary");
        auto mapini = project.GetIniFilePath("romasmmappings");
        auto valini = project.GetIniFilePath(FFHFILE_ValuesPath);
        auto section = mappingDefinition.c_str();

        auto m = std::make_unique<RomAsmMappingBinary>();
        m->name = mappingDefinition;
        m->type = type;
        m->offset = ReadIniInt(mapini, valini, section, "offset");
        m->source = ReadSource(mapini, section, section, "source");
        m->label = Ini::ReadIni(mapini, section, "label", "");
        m->sublabel = Ini::ReadIni(mapini, section, "sublabel", "");
        m->instoffset = atol(Ini::ReadIni(mapini, section, "instoffset", "-1")); //TODO - is atol safe here?
        m->count = ReadIniInt(mapini, valini, section, "count");
        m->recwidth = atol(Ini::ReadIni(mapini, section, "recwidth", ""));
        m->format = Ini::ReadIni(mapini, section, "format", "");
        m->readonly = Ini::ReadIni(mapini, section, "readonly", "false") == "true";
        m->projectfolder = (LPCSTR)project.ProjectFolder;

        ASSERT(m->count > 0);
        ASSERT(m->recwidth > 0);
        ASSERT(m->instoffset > 0);
        return m;
    }

    std::unique_ptr<RomAsmMapping> ToOpval(CFFHacksterProject& project, std::string type, std::string mappingDefinition)
    {
        throw std::runtime_error(__FUNCTION__ " not yet implemented(" + mappingDefinition + ").");
    }

    std::unique_ptr<RomAsmMapping> NoDeserializer(CFFHacksterProject& project, std::string type, std::string mappingDefinition)
    {
        return std::make_unique<FailedMapping>(mappingDefinition,
            "No deserializer was found for '" + type + "' mapping '" + mappingDefinition + "'.");
    }


    // === SERIALIZERS

    std::string FromDatfile(CFFHacksterProject& project, std::string type, std::string mappingDefinition, RomAsmMapping& mapping)
    {
        throw std::runtime_error(__FUNCTION__ " not yet implemented(" + mappingDefinition + ").");
    }

    std::string NoSerializer(CFFHacksterProject& project, std::string type, std::string mappingDefinition, RomAsmMapping& mapping)
    {
        return "No serializer was found for '" + type + "' mapping '" + mappingDefinition + "'.";
    }

    CString ReadIniText(CString mapini, CString valini, CString section, CString key)
    {
        auto text = Ini::ReadIni(mapini, section, key, "");
        // If it's a @valini ref, then read the value from there using text as the key
        if (text.GetLength() > 0) {
            if (text[0] == '@')
                text = Ini::ReadIni(valini, text.Mid(1), "value", "");
        }

        return text;
    }

    int ReadIniInt(CString mapini, CString valini, CString section, CString key)
    {
        using namespace Types;

        auto text = ReadIniText(mapini, valini, section, key);

        if (is_dec(text))
            return dec(text);
        if (is_addr(text))
            return addr(text);
        if (is_hex(text))
            return hex(text);

        throw std::runtime_error(key + " is not an integer.");
    }

    std::string ReadSource(CString mapini, CString valini, CString section, CString key)
    {
        auto text = Ini::ReadIni(mapini, section, key, "");
        if (!text.IsEmpty()) {
            // If it's a !mapini or @valini ref, then read the value from there using text as the key
            // If it's a @valini ref, then read the value from there using text as the key
            //TODO - for !, do the @ action if the result is blank as a fallback
            if (text[0] == '!')
                text = Ini::ReadIni(mapini, "sources", text.Mid(1), "");
            else if (text[0] == '@')
                text = Ini::ReadIni(valini, text.Mid(1), "value", "");
        }

        if (text.IsEmpty())
            throw std::runtime_error(key + " is a source reference and cannot be empty.");
        return (LPCSTR)text;
    }
}