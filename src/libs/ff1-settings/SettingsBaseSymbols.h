#pragma once

//N.B. - users of this file must include the following in their cpp files:
//#include "ini_functions.h"
//#include "type_support.h"

#define READ_SETTING_STR(name) name = Ini::ReadIni(m_proj.EditorSettingsPath, m_sectionname, #name, name##_default)
#define WRITE_SETTING_STR(name) Ini::WriteIni(m_proj.EditorSettingsPath, m_sectionname, #name, name)

#define READ_SETTING_DEC(name) name = Types::dec(Ini::ReadIni(m_proj.EditorSettingsPath, m_sectionname, #name, Types::dec(name##_default)))
#define WRITE_SETTING_DEC(name) Ini::WriteIni(m_proj.EditorSettingsPath, m_sectionname, #name, Types::dec(name))

#define READ_SETTING_HEX(name) name = Types::hex(Ini::ReadIni(m_proj.EditorSettingsPath, m_sectionname, #name, Types::hex(name##_default)))
#define WRITE_SETTING_HEX(name) Ini::WriteIni(m_proj.EditorSettingsPath, m_sectionname, #name, Types::hex(name))

#define READ_SETTING_BOOL(name) name = Ini::ReadIniBool(m_proj.EditorSettingsPath, m_sectionname, #name, name##_default)
#define WRITE_SETTING_BOOL(name) Ini::WriteIniBool(m_proj.EditorSettingsPath, m_sectionname, #name, name)
