/*
** lua2json.h for cpt in /home/pi/cpt/src
**
** Made by solidest
** Login   <>
**
** Started on  Fri Nov 1 11:03:09 AM 2019 solidest
** Last update Fri Nov 1 11:03:09 AM 2019 solidest
*/

#ifndef _LUA2JSON_H_
#define _LUA2JSON_H_

#include <lua.hpp>
#include "../json.hpp"

using namespace nlohmann;

void table2json(lua_State* L, int table_index, json& result, int maxLevel=5);
void json2table(json& jobject, lua_State* L, int table_index);

#endif


