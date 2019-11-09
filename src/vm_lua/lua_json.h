/*
** lua2json.h for cpt in /home/pi/cpt/src
**
** Made by solidest
** Login   <>
**
** Started on  Fri Nov 1 11:03:09 AM 2019 solidest
** Last update Sun Nov 9 10:37:59 PM 2019 solidest
*/

#ifndef _LUA2JSON_H_
#define _LUA2JSON_H_

#include <lua.hpp>
#include "../json.hpp"

using namespace nlohmann;

//from lua table to json
void table2json(lua_State* L, int table_index, json& result, int maxLevel=5);

//from json to lua table
void json2table(const json& jobject, lua_State* L, int table_index);

//push json in lua stack, return the stack index
int pushlua(const json& value, lua_State* L);

//get json from lua stack
void getjson(lua_State* L, int index, json& result);

#endif


