#include "stdafx.h"
#include "RomAsmMappingCond.h"

#include "FFHacksterProject.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "romasm_functions.h"
#include "string_functions.h"

#include <fstream>

namespace // DECLARATIONS
{
    void StringToRombytes(CString romasmini, CString section, std::string key, std::vector<unsigned char>& bytes, std::vector<std::string>& errors);
    void StringToAsmlines(CString romasmini, CString section, std::string key, std::vector<std::string>& lines, std::vector<std::string>& errors);
}

RomAsmMappingCond::RomAsmMappingCond(CFFHacksterProject& project, std::string mapping)
{
    std::vector<std::string> errors;

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

    StringToRombytes(romasmini, section, "romtrue", romtrue, errors);
    StringToRombytes(romasmini, section, "romfalse", romfalse, errors);
    StringToAsmlines(romasmini, section, "asmtrue", asmtrue, errors);
    StringToAsmlines(romasmini, section, "asmfalse", asmfalse, errors);
    
    if (type != maptype) errors.push_back("type should be " + maptype + ", not '" + edtype + "'");
    if (romoffset < 1) errors.push_back("romoffset must be greater than zero");
    if (source.empty()) errors.push_back("source cannot be blank");
    if (label.empty()) errors.push_back("label cannot be blank");
    if (instoffset < 1) errors.push_back("instoffset must be greater than zero");
    if (!errors.empty())
        throw std::runtime_error("Editor '" + mapping + "' config is malformed: " + Strings::join(errors, "; "));
}

RomAsmMappingCond::~RomAsmMappingCond()
{
}

bool RomAsmMappingCond::Enabled(const std::vector<unsigned char>& rom) const
{
    //TODO - the bytes at rom[romoffset] don't match either romtrue or romfalse
	return false;
}

void RomAsmMappingCond::Load(std::istream& sourcefile, std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    RomAsm::Traverse::SeekToInstruction(sourcefile, name, label, sublabel, instoffset);

    auto bookmark = sourcefile.tellg();

    // Get the line counts for asmtrue and asmfalse
    // For each, read the text and compare to asmtrue/asmfalse
    // Note the match type and line count
    // restore the bookmark
    // if NOT matched:
    // - stop processing, the caller will read the rest of the file will sync-to-current or sync-to-end happens
    // else:
    // - Read the rom bytes for the asm match (romtrue or romfalse)
    // - Convert to bytes
    // - Copy those bytes to rom[romoffset]
   
    throw std::runtime_error(__FUNCTION__ " not implemented yet.");
}

void RomAsmMappingCond::Save(std::istream& sourcefile, std::ostream& destfile, const std::vector<unsigned char>& rom, const std::map<std::string, std::string>& options)
{
    auto bookmark = sourcefile.tellg();
    RomAsm::Traverse::SeekToInstruction(sourcefile, name, label, sublabel, instoffset);
    RomAsm::Modify::SyncDestToCurrent(sourcefile, destfile, bookmark);

    auto spot = sourcefile.tellg();

    // Get the line counts for asmtrue and asmfalse
    // For each, read the text and compare to asmtrue/asmfalse
    // Note the match type and line count
    // restore the bookmark
    // if NOT matched:
    // - stop processing, the caller will read the rest of the file will sync-to-current or sync-to-end happens
    // else:
    // - skip the number of matched lines in source
    // - convert the corresponding asmtrue/asmfalse into a vector<string>
    // - write the strings to dest

    //TODO - this doesn't address runs with a configurable byte, e.g. HoldMP, CapMP, BBMA, etc.
    // I need a way to signal that one or more specific bytes can be masked from the comparison.
    // e.g. CMP #CLS_FT or CMP #0
    // Immediate operand values can be changed (which is largely the point of this mapping) and
    // should be not break equality checks, so for example:
    // - in ROM, C9 01 F0 3C and C9 08 F0 3C should be treated as equal
    // - in ASM, CMP #1\nBEQ @SkipMPGain and CMP #8\nBEQ @SkipMPGain should be treated as equal

    // The #immediate operand shoul dbe allowed to change so that the feature can be manipulated by the UI.
    // Consider something like a mask or placeholder:
    // CMP #?\nBEQ @SkipMPGain

    // The question mark following an immediate value (#?) indicates a term that we'll exclude from equality checks.
    // The term can be any of decimal, binary, hex, or variablename.
    // How can this be noted in ROM?
    // A change to the extraction algorithm to allow ?? in the byte sequence:
    // C9 ?? F0 3C

    // But that won't work!!!
    // I have to actually specify the value.
    // So maybe change this to either prefix or suffix the value with a question mark.
    // That implies that comparieson should be done by converting rom[oromffset] and asmtrue/asmfalse
    // to vector<string> instead of vector<unsigned char>.
    // A routine can then compare the two, ignoring a pair of bytes when one of them is suffixed with ?
    // rom[romoffset] = C9 01 F0 3C
    // romtrue        = C9 01? F0 3C
    
    // Assembly follows suit:
    // CMP #1?\nBEQ @SkipMPGain
    // CMP #CLS_FT? \nBEQ @SkipMPGain

    // Regex like (#\w+)(\?) can be ysed to strip the suffixed ? from the value before writing it to disk.
    // In this case, we'd replace the value with what's at the corresponding rom[] position.
    // But we won't know what that value is...


    throw std::runtime_error(__FUNCTION__ " not implemented yet.");
}

namespace // IMPLEMENTATIONS
{
    void StringToRombytes(CString romasmini, CString section, std::string key, std::vector<unsigned char>& bytes, std::vector<std::string>& errors)
    {
        std::string text = (LPCSTR)Ini::ReadIni(romasmini, section, key.c_str(), "");
        if (text.empty()) {
            errors.emplace_back("'" + key + "' cannot be empty.");
        }
        else {
            auto strings = Strings::split(text, " ");
            if (strings.empty()) {
                errors.emplace_back("'" + key + "' must be a space-separated sequence containing at least one byte, e.g " + key + "=EA 12 7f.");
            }
            else {
                try {
                    std::transform(strings.begin(), strings.end(), std::back_inserter(bytes),
                        [](const std::string& s) -> unsigned char {
                            return (unsigned char)std::strtoul(s.c_str(), nullptr, 16);
                        });
                }
                catch (std::exception& ex) {
                    errors.emplace_back("Key " + key + " failed while converting the string to byte values; " +
                        std::string(ex.what()) + ".Check the format of the values and try again.");
                }
            }
        }
    }

    void StringToAsmlines(CString romasmini, CString section, std::string key, std::vector<std::string>& lines, std::vector<std::string>& errors)
    {
        std::string text = (LPCSTR)Ini::ReadIni(romasmini, section, key.c_str(), "");
        if (text.empty()) {
            errors.emplace_back("'" + key + "' cannot be empty.");
        }
        else {
            auto strings = Strings::split(text, "\n");
            if (strings.empty()) {
                errors.emplace_back("'" + key + "' must be a \n-separated sequence containing at least one entry, e.g " +
                    key + "=LDA ch_class\nCMP $CLS_FT\nBEQ :+");
            }
            else {
                lines = strings;
            }
        }
    }
}