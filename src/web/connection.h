#pragma once
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <emscripten/fetch.h>
#include "rocket.hpp"
#include "server_report_store.h"
#include <map>
#include <string>
#include <stdarg.h>

using std::map;
using std::string;
enum class ApiErrorCode{  general = 0,token_expired};

struct Result
{
    bool success = false;
    ApiErrorCode code = ApiErrorCode::general;
    char* info;
    int key;
};

class Connection
{
private:
    static string token;
    /* data */
    Connection(/* args */);
    ~Connection();
    static map<int, rocket::signal<void(Result)>* > events;
  
    static int req_key;

    static EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData);
    static EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData);
    static EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData);
    static EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData);

    static void _login();
    friend class LoginWindow;
public:
    static string pure_origin;

    static EMSCRIPTEN_WEBSOCKET_T *socket;

    static rocket::signal<void(const char*)> bash_results;

    static void init();
    static rocket::signal<void(Result)>* send(const char* req ,int param_count,...);
    
    //user api
    static rocket::signal<void(Result)>* createUser(const User& user);
    static rocket::signal<void(Result)>* updateUser(const User& user);
    static rocket::signal<void(Result)>* deleteUser(const User& user);
};

