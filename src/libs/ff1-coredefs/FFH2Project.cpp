#include "stdafx.h"
#include "FFH2Project.h"
#include "ini_functions.h"
#include "path_functions.h"
#include "string_functions.h"
#include <fstream>
#include <iomanip>
#include <map>
#include "FFH2Project_IniToJson_Defs.hpp"

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
	p.data = jdata.dump(2);
}


//=== FFHDataValue
void to_json(ojson& j, const FFHDataValue& p)
{
	ojson jdata = ojson::parse(p.data);
	j = ojson{
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

void to_json(ujson& j, const FFHDataValue& p)
{
	ujson jdata = ujson::parse(p.data);
	j = ujson{
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


void FFH2Project::Load(CString filepath)
{
	LogHere << "Loading json project: " << (LPCSTR)filepath << std::endl;

	std::ifstream ifs(filepath);
	auto j = ojson::parse(ifs);
	*this = j.get<FFH2Project>();
	throw std::runtime_error("FFH2Project::Load doesn't fully support JSON loadin gjust yet.");
}