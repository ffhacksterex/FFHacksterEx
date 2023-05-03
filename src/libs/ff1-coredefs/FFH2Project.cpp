#include "stdafx.h"
#include "FFH2Project.h"
#include "DataValueAccessor.h"
#include "FFH2Project_IniToJson_Defs.hpp"
#include "FFHDataValue_dva.hpp"
#include "ini_functions.h"
#include "io_functions.h"
#include "path_functions.h"
#include "string_conversions.hpp"
#include "string_functions.h"

#include <fstream>
#include <iomanip>
#include <map>


FFH2Project::FFH2Project()
{
}

FFH2Project::~FFH2Project()
{
}

// Conversions to and from JSON (in this case, ordered_json)
// Declared externally in the json header above.

//=== FFHSettingValue
void to_json(ojson& j, const FFHSettingValue& p)
{
	ojson jdata = ojson::parse(p.data);
	j = ojson{
		{"type", p.type},
		{"format", p.format},
		{"data", jdata}
	};
}

void from_json(const ojson& j, FFHSettingValue& p)
{
	ojson jdata;
	j.at("type").get_to(p.type);
	j.at("format").get_to(p.format);
	j.at("data").get_to(jdata);
	p.data = jdata.dump(2);
}

void to_json(ujson& j, const FFHSettingValue& p)
{
	ujson jdata = ujson::parse(p.data);
	j = ujson{
		{"type", p.type},
		{"format", p.format},
		{"data", jdata}
	};
}

void from_json(const ujson& j, FFHSettingValue& p)
{
	ujson jdata;
	j.at("type").get_to(p.type);
	j.at("format").get_to(p.format);
	j.at("data").get_to(jdata);

	//TODO - to get the dquotes from around strings, check for the string type and call get instead of dump.
	//p.data = jdata.dump(2);
	if (jdata.type() == ojson::value_t::string)
		p.data = jdata.get<std::string>();
	else
		p.data = jdata.dump(2);
}


//=== FFHDataValue
void to_json(ojson& j, const FFHDataValue& p)
{
	ojson jdata = ojson::parse(p.data);
	j = ojson{
		{"name", p.name},
		{"label", p.label},
		{"desc", p.desc},
		{"group", p.group},
		{"internal", p.internal},
		{"readonly", p.readonly},
		{"hidden", p.hidden},
		{"type", p.type},
		{"format", p.format},
		{"data", jdata}
	};
}

void from_json(const ojson& j, FFHDataValue& p)
{
	ojson jdata;
	j.at("name").get_to(p.name);
	j.at("label").get_to(p.label);
	j.at("desc").get_to(p.desc);
	j.at("group").get_to(p.group);
	j.at("internal").get_to(p.internal);
	j.at("readonly").get_to(p.readonly);
	j.at("hidden").get_to(p.hidden);
	j.at("type").get_to(p.type);
	j.at("format").get_to(p.format);
	j.at("data").get_to(jdata);

	//TODO - to get the dquotes from around strings, check for the string type and call get instead of dump.
	//p.data = jdata.dump(2);
	if (jdata.type() == ojson::value_t::string)
		p.data = jdata.get<std::string>();
	else
		p.data = jdata.dump(2);
}

void to_json(ujson& j, const FFHDataValue& p)
{
	ujson jdata = ujson::parse(p.data);
	j = ujson{
		{"name", p.name},
		{"label", p.label},
		{"desc", p.desc},
		{"group", p.group},
		{"internal", p.internal},
		{"readonly", p.readonly},
		{"hidden", p.hidden},
		{"type", p.type},
		{"format", p.format},
		{"data", jdata}
	};
}

void from_json(const ujson& j, FFHDataValue& p)
{
	ujson jdata;
	j.at("name").get_to(p.name);
	j.at("label").get_to(p.label);
	j.at("desc").get_to(p.desc);
	j.at("group").get_to(p.group);
	j.at("internal").get_to(p.internal);
	j.at("readonly").get_to(p.readonly);
	j.at("hidden").get_to(p.hidden);
	j.at("type").get_to(p.type);
	j.at("format").get_to(p.format);
	j.at("data").get_to(jdata);
	p.data = jdata.dump(2);
}

void FFH2Project::Load(std::string projectpath)
{
	LogHere << "Loading JSON project to " << projectpath << std::endl;

	std::ifstream ifs(projectpath);
	auto j = ojson::parse(ifs);
	*this = j.get<FFH2Project>();
	ProjectPath = projectpath;
	ProjectFolder = Paths::GetDirectoryPath(projectpath);
	WorkRomPath = Paths::Combine({ tomfc(ProjectFolder), tomfc(this->info.workrom) });
	LogHere << "  -> finished loading JSON project to " << projectpath << std::endl;
}

void FFH2Project::Save(std::string alternateprojectpath)
{
	auto& projpath = alternateprojectpath.empty() ? this->ProjectPath : alternateprojectpath;
	LogHere << " Saving JSON project to: " << projpath << std::endl;

	std::ostringstream ostr;
	ojson oj = *this;
	ostr << oj.dump(2);

	std::string s = ostr.str();
	std::ofstream ofs(projpath);
	ofs << s;
	LogHere << "  -> finished saving JSON project to " << projpath << std::endl;
}

bool FFH2Project::IsRom() const
{
	return info.type == "rom";
}

bool FFH2Project::IsAsm() const
{
	return info.type == "asm";
}

const std::string* FFH2Project::GetTable(int index)
{
	ASSERT(index >= 0 && index < (int)session.textViewInDTE.size());
	if (index < 0 || index >= (int)session.textViewInDTE.size())
		throw std::runtime_error("Can't find a text table with index '" + std::to_string(index) + "'.");

	return data.tables[session.textViewInDTE[index]].data();
}

bool FFH2Project::ClearROM()
{
	ff1coredefs::DataValueAccessor d(*this);
	auto ROM_SIZE = d.get<int>("ROM_SIZE");
	if (ROM_SIZE == 0)
		throw std::runtime_error("ROM_SIZE shouldn't be 0");

	ROM.clear();
	ROM.resize(ROM_SIZE, 0);
	return true;
}

void FFH2Project::LoadROM()
{
	ASSERT(info.type == "rom");
	Io::load_binary(tomfc(WorkRomPath), ROM);
}
