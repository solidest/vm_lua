#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

#include "src/vm_lua/lua_json.h"

static int tests = 0, fails = 0;
#define test(_s) { printf("#%02d ", ++tests); printf(_s); }
#define test_cond(_c) if(_c) printf("\033[0;32mPASSED\033[0;0m\n"); else {printf("\033[0;31mFAILED\033[0;0m\n"); fails++;}


int create_table(lua_State* L) {
    lua_newtable(L);
    auto idx = lua_gettop(L);

    lua_pushboolean(L, true);
    lua_setfield(L, idx, "boolSeg");

    lua_pushinteger(L, 200);
    lua_setfield(L, idx, "intSeg");

    lua_pushnil(L);
    lua_setfield(L, idx, "nilSeg");

    lua_pushnumber(L, -9.887);
    lua_setfield(L, idx, "numberSeg");

    lua_pushstring(L, "segments string");
    lua_setfield(L, idx, "stringSeg");

    lua_newtable(L);
    auto obj_idx = lua_gettop(L);
    lua_pushstring(L, "sub segment string");
    lua_setfield(L, obj_idx, "substringSeg");
    lua_setfield(L, idx, "objectSeg");

    lua_newtable(L);
    auto arr_idx = lua_gettop(L);
    lua_pushnumber(L, 1);
    lua_seti(L, arr_idx, 1);
    lua_pushnumber(L, 2);
    lua_seti(L, arr_idx, 2);
    lua_setfield(L, idx, "arraySeg");

    return idx;
}

string load_json(const string& file) {
    ifstream ifs(file);
    string content( (istreambuf_iterator<char>(ifs) ),
                     (istreambuf_iterator<char>() ) );
    ifs.close();
    return content;
}


int main(int argcs, char **argvs) {

    test("test lua table to json ");
    auto L = luaL_newstate();
    auto idx = create_table(L);
    json jobj = json({});
    int top;

    table2json(L, idx, jobj);
    std::string j_string = jobj.dump();
    //printf("\n%i : %s\n", j_string.length(), j_string.c_str());
    top = lua_gettop(L);
    lua_pop(L, 1);
    test_cond(top==1 && j_string.length()==151);

    test("test json to lua table ");
    jobj["nilSeg"] = nullptr;
    lua_newtable(L);
    idx = lua_gettop(L);
    json2table(jobj, L, idx);
    
    json jobj2 = json::object();
    table2json(L, idx, jobj2);
    auto j_string2 = jobj2.dump();
    //printf("\n%i : %s\n", j_string2.length(), j_string2.c_str());
    top = lua_gettop(L);
    lua_pop(L, 1);
    lua_close(L);
    test_cond(top==1 && j_string2==j_string);


}
