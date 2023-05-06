#pragma once

#include "nlohmann-json.hpp"

#define EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(jclass,Type, ...)  \
    inline void to_json(jclass& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const jclass& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }

#define EXP_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(jclass,Type, ...)  \
    inline void to_json(jclass& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const jclass& nlohmann_json_j, Type& nlohmann_json_t) { const Type nlohmann_json_default_obj{}; NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM_WITH_DEFAULT, __VA_ARGS__)) }

using ujson = nlohmann::json;
using ojson = nlohmann::ordered_json;
