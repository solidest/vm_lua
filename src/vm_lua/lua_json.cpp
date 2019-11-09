

#include "lua_json.h"

static void _table2jsonObject(lua_State* L, int table_idx, json& parent, int* parent_level, int maxLevel);
static void _table2jsonArray(lua_State* L, int table_idx, json& parent_array, int* parent_level, int maxLevel);


static bool _table_is_array(lua_State* L, int table_idx) {
    lua_pushnil(L);  /* first key */
    while (lua_next(L, table_idx) != 0) {
       /* uses 'key' (at index -2) and 'value' (at index -1) */
       if(lua_type(L, -2) != LUA_TNUMBER) {
           lua_pop(L, 2);
           return false;
       }
       /* removes 'value'; keeps 'key' for next iteration */
       lua_pop(L, 1);
    }
    return true;
}

static void _table2jsonArray(lua_State* L, int table_idx, json& parent_array, int* parent_level, int maxLevel) {
    
    lua_pushnil(L);  /* first key */

    while (lua_next(L, table_idx) != 0) {
       /* uses 'key' (at index -2) and 'value' (at index -1) */
        auto t = lua_type(L, -1);
        switch (t)
        {
        case LUA_TNIL:
            parent_array.push_back(nullptr);
            break;

        case LUA_TNUMBER:
            parent_array.push_back(lua_tonumber(L, -1));
            break;

        case LUA_TBOOLEAN:
            parent_array.push_back((bool)lua_toboolean(L, -1));
            break;

        case LUA_TSTRING:
            parent_array.push_back(lua_tostring(L, -1));
            break;

        case LUA_TTABLE:
            if(*parent_level < maxLevel) {
                *parent_level += 1;
                lua_len(L, -1);
                int len = lua_tonumber(L, -1);
                lua_pop(L, 1);
                lua_checkstack(L, len+2);
                if(_table_is_array(L, lua_gettop(L))) {
                    parent_array.push_back(json::array());
                    _table2jsonArray(L, lua_gettop(L), parent_array.back(), parent_level, maxLevel);
                }else {
                    parent_array.push_back(json::object());
                    _table2jsonObject(L, lua_gettop(L), parent_array.back(), parent_level, maxLevel);
                }
            }
            break;

        default:
            break;
        }

       /* removes 'value'; keeps 'key' for next iteration */
       lua_pop(L, 1);
    }
}


static void _table2jsonObject(lua_State* L, int table_idx, json& parent, int* parent_level, int maxLevel) {

    lua_pushnil(L);  /* first key */
    while (lua_next(L, table_idx) != 0) {
       /* uses 'key' (at index -2) and 'value' (at index -1) */
       if(lua_type(L, -2) == LUA_TSTRING) {
           auto token = lua_tostring(L, -2);
           auto t = lua_type(L, -1);
           switch (t)
           {
            case LUA_TNIL:
                parent[token] = nullptr;
                break;

            case LUA_TNUMBER:
                parent[token] = lua_tonumber(L, -1);
                break;

            case LUA_TBOOLEAN:
                parent[token] = (bool)lua_toboolean(L, -1);
                break;

            case LUA_TSTRING:
                parent[token] = lua_tostring(L, -1);
                break;

            case LUA_TTABLE:
                if(*parent_level < maxLevel) {
                    *parent_level += 1;
                    if(_table_is_array(L, lua_gettop(L))) {
                        parent[token] = json::array();
                        _table2jsonArray(L, lua_gettop(L), parent[token], parent_level, maxLevel);
                    }else {
                        parent[token] = json::object();
                        _table2jsonObject(L, lua_gettop(L), parent[token], parent_level, maxLevel);
                    }
                }
                break;

            default:
                break;
           }
       }

       /* removes 'value'; keeps 'key' for next iteration */
       lua_pop(L, 1);
     }


}


void table2json(lua_State* L, int table_index, json& result, int maxLevel) {
    int level = 1;
    _table2jsonObject(L, table_index, result, &level, maxLevel);
}


static void _array2table(json& arr, lua_State* L, int table_idx) {
    int lua_tid = 1;
    for (json::iterator it = arr.begin(); it != arr.end(); ++it) {

        auto v = (*it);
        auto t = v.type();

        switch (t)
        {
        case json::value_t::null:
            lua_pushnil(L);
            lua_seti(L, table_idx, lua_tid);
            lua_tid++;
            break;

        case json::value_t::boolean:
            lua_pushboolean(L, v.get<bool>()?true:false);
            lua_seti(L, table_idx, lua_tid);
            lua_tid++;
            break;

        case json::value_t::number_float:
            lua_pushnumber(L, v.get<double>());
            lua_seti(L, table_idx, lua_tid);
            lua_tid++;
            break;

        case json::value_t::number_integer:
            lua_pushinteger(L, v.get<long long>());
            lua_seti(L, table_idx, lua_tid);
            lua_tid++;
            break;

        case json::value_t::number_unsigned:
            lua_pushinteger(L, v.get<unsigned long long>());
            lua_seti(L, table_idx, lua_tid);
            lua_tid++;
            break;

        case json::value_t::string:
            lua_pushstring(L, v.get<std::string>().c_str());
            lua_seti(L, table_idx, lua_tid);
            lua_tid++;
            break;

        case json::value_t::array: {
                lua_newtable(L);
                auto idx = lua_gettop(L);
                _array2table(v, L, idx);
                lua_seti(L, table_idx, lua_tid);
                lua_tid++;
            }
            break;

        case json::value_t::object: {
                lua_newtable(L);
                auto idx = lua_gettop(L);
                json2table(v, L, idx);
                lua_seti(L, table_idx, lua_tid);
                lua_tid++;
            }
            break;
        
        default:
            break;
        }
    }
}

void json2table(json& jobject, lua_State* L, int table_index) {
    for (json::iterator it = jobject.begin(); it != jobject.end(); ++it) {
        auto k = it.key().c_str();
        auto v = it.value();
        auto t = v.type();
   
        switch (t)
        {
        case json::value_t::null:
            lua_pushnil(L);
            lua_setfield(L, table_index, k);
            break;

        case json::value_t::boolean:
            lua_pushboolean(L, v.get<bool>()?true:false);
            lua_setfield(L, table_index, k);
            break;

        case json::value_t::number_float:
            lua_pushnumber(L, v.get<double>());
            lua_setfield(L, table_index, k);
            break;

        case json::value_t::number_integer:
            lua_pushinteger(L, v.get<long long>());
            lua_setfield(L, table_index, k);
            break;

        case json::value_t::number_unsigned:
            lua_pushinteger(L, v.get<unsigned long long>());
            lua_setfield(L, table_index, k);
            break;

        case json::value_t::string:
            lua_pushstring(L, v.get<std::string>().c_str());
            lua_setfield(L, table_index, k);
            break;

        case json::value_t::array: {
                lua_newtable(L);
                auto idx = lua_gettop(L);
                _array2table(v, L, idx);
                lua_setfield(L, table_index, k);
            }
            break;

        case json::value_t::object: {
                lua_newtable(L);
                auto idx = lua_gettop(L);
                json2table(v, L, idx);
                lua_setfield(L, table_index, k);
            }
            break;
        
        default:
            break;
        }
    }
}


