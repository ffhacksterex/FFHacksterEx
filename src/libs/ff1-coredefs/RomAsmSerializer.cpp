#include "stdafx.h"
#include "RomAsmSerializer.h"
#include "RomAsmMappingFactory.h"
#include "RomAsmMapping.h"
#include "ini_functions.h"

RomAsmSerializer::RomAsmSerializer(RomAsmMappingFactory& factory, std::string projectfolder, std::vector<unsigned char>& rom)
    : m_factory(factory)
    , m_projectfolder(projectfolder)
    , m_rom(rom)
{
}

RomAsmSerializer::~RomAsmSerializer()
{
}

//TODO - REMOVE
//std::vector<std::unique_ptr<RomAsmMapping>> RomAsmSerializer::ReadMappings(std::string mappingsini, std::string valuesini, std::string key)
//{
//    //throw std::runtime_error(__FUNCTION__ " is not implemented yet.");
//    
//    // Select the sections beginngin with "<key>-"
//    // Transform each section into a mapping by passing the text of the section to the factory
//    // In this case, we'll just pas the section name?
//
//    std::vector<std::unique_ptr<RomAsmMapping>> mappings;
//
//    //TOD O- this is INI-specific code. Likely have to move this into the factory, which implies that
//    //      thji load function DOESN'T belong in the serializer.
//    auto sects = Ini::ReadIniSectionNames(mappingsini.c_str());
//    auto marker = CString(key.c_str()) + "-";
//    for (const auto& sect : sects) {
//        if (sect.Find(marker) != 0)
//            continue;
//
//        mappings.push_back(m_factory.Deserialize(
//    }
//    return mappings;
//}

//TODO - prefer ROM as a member or param? currently using the member
std::string RomAsmSerializer::Load(std::string projetype, const std::vector<std::unique_ptr<RomAsmMapping>>& mappings, 
    const std::map<std::string, std::string>& options, std::vector<unsigned char>& rom)
{
    std::string result;
    //std::string firstresult;
    for (auto& m : mappings) {
        auto res = m->Load(options, m_rom);
        if (!res.empty()) {
            result += res + "\n";
            //if (firstresult.empty())
            //    firstresult = res;
        }
    }

    LogHere << result << std::endl;

    //TODO - might be too many to log, so log the count and the first error
    // or just return vector<String> instead?
    return result;

    //return __FUNCTION__ " is not implemented yet.";
}

//TODO - prefer ROM as a member or param? currently using the member
std::string RomAsmSerializer::Save(std::string projetype, const std::vector<std::unique_ptr<RomAsmMapping>>& mappings, 
    const std::map<std::string, std::string>& options, const std::vector<unsigned char>& rom)
{
    std::string result;
    //std::string firstresult;
    for (auto& m : mappings) {
        auto res = m->Save(options, m_rom);
        if (!res.empty()) {
            result += res + "\n";
            //if (firstresult.empty())
            //    firstresult = res;
        }
    }

    LogHere << result << std::endl;

    //TODO - might be too many to log, so log the count and the first error
    // or just return vector<String> instead?
    return result;
}
