#include "connection.h"
#include "log.h"
#include <string.h>
#include <iostream>
#include "enc.h"
#include <vector>
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
using namespace rapidjson;
using namespace std;
extern bool live_data;

extern bool AUTH;
extern bool FIRSTRUN;
extern bool BEGIN;

EMSCRIPTEN_WEBSOCKET_T *Connection::socket = nullptr;
int Connection::req_key = 0;
map<int, rocket::signal<void(Result)> *> Connection::events = {};
rocket::signal<void(const char *)> Connection::bash_results;

string Connection::pure_origin = "";
string Connection::token = "";

rocket::signal<void(Result)> *Connection::send(const char *req, int param_count, ...)
{
    req_key++;

    auto req_sig = new rocket::signal<void(Result)>;

    va_list args;
    va_start(args, param_count);

    va_end(args);
    //  token: Ha0shedToken
    // req: string
    // params: seq[string]
    // specialparam: string
    // key:string
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    writer.StartObject();
    writer.Key("token");
    writer.String(token.c_str());

    writer.Key("req");
    writer.String(req);

    writer.Key("params");
    writer.StartArray();
    for (size_t i = 0; i < param_count; i++)
    {
        writer.String(va_arg(args, char *));
    }
    writer.EndArray();

    writer.Key("specialparam");
    writer.String("req");
    writer.Key("key");
    char key_buf[10];
    sprintf(key_buf, "%d", req_key);
    writer.String(key_buf);
    writer.EndObject();

    const char *strrep = s.GetString();

    if (socket != nullptr)
    {
        char *send_raw = (char *)strrep;
        char *enc_ed = encrypt(send_raw);
        EMSCRIPTEN_RESULT result;
        result = emscripten_websocket_send_utf8_text(*socket, enc_ed);
        if (result == EMSCRIPTEN_RESULT_SUCCESS)
        {
            console.log("[Socket] [Send] sent:%s", send_raw);
        }
        else
        {
            console.error("[Socket] Falied to send , emscripten_websocket_send_utf8_text(): %d\n", result);

            delete req_sig;
            req_sig = nullptr;
        }
    }
    else
    {
        console.error("[Socket] Falied to send (socket == nullptr)");
        EM_ASM(window.location.reload(););
        delete req_sig;
        req_sig = nullptr;
    }
    events[req_key] = req_sig;
    return req_sig;
}

void Connection::_login()
{
    const char *cookie_token = (const char *)EM_ASM_PTR(
        var token = getCookie("token");
        if (token == null) {
            return "";
        } var buffer = Module._malloc(token.length + 1);
        Module.writeStringToMemory(token, buffer);
        return buffer;);
    if (strlen(cookie_token) > 5)
    {
        token = cookie_token; // dose it work?
        Connection::send("client-hello", 0)->connect([](Result result)
                                                     {
                                                         if (result.success)
                                                         {
                                                             // loged in
                                                             //  token = result.info; !!!!!!!!!!!!!!!!!!
                                                             //  EM_ASM({
                                                             //      const str = UTF8ToString($0);

                                                             //      alert(str);
                                                             //      setCookie("token", str, 3);
                                                             //  },
                                                             //         token.c_str());

                                                             AUTH = true;
                                                             BEGIN = true;
                                                         }
                                                         else
                                                         {
                                                             // token expired
                                                             AUTH = false;
                                                             BEGIN = true;
                                                         }

                                                         // else go to login page
                                                     });
    }
    else
    {
        EM_ASM(setCookie("token", "", 3););
        AUTH = false;
        BEGIN = true;
    }
}
EM_BOOL Connection::onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData)
{
    Connection::socket = (int *)&(websocketEvent->socket);

    console.log("[Socket] connection established.");
    // const char *toenc = "{\
    // \"token\":{\"token\":\"\",\"expire\":{\"seconds\":123,\"nanosecond\":415}},\
    // \"req\":\"Auth\",\
    // \"params\":[\"\",\"\"],\
    // \"specialparam\":\"\",\
    // \"key\":\"a\"\
    // }";

    // EM_ASM(window.location.reload(););

    Connection::send("Auth", 1, "")->connect([](Result result)
                                             {
                                                 if (result.success)
                                                 {
                                                     token = result.info;
                                                     FIRSTRUN = true;
                                                     AUTH = true;
                                                     BEGIN = true;
                                                 }
                                                 else
                                                 { // not first run, username and password present
                                                     FIRSTRUN = false;
                                                     AUTH = false;

                                                     _login();
                                                 }

                                                 // else go to login page
                                             });

    return EM_TRUE;
}
EM_BOOL Connection::onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData)
{
    Connection::socket = nullptr;
    console.log("[Socket] Error !");

    return EM_TRUE;
}
EM_BOOL Connection::onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData)
{
    Connection::socket = nullptr;
    console.log("[Socket] Closed !");

    return EM_TRUE;
}
EM_BOOL Connection::onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData)
{
    // console.log("onmessage");
    if (websocketEvent->isText)
    {
        char buf[websocketEvent->numBytes + 1];
        memcpy(buf, (char *)websocketEvent->data, websocketEvent->numBytes);
        buf[websocketEvent->numBytes] = 0;
        char *dec = decrypt(buf);

        Document resobj;
        if (resobj.Parse<0, ASCII<>>(dec).HasParseError())
        {
            console.log("[ERROR] Could not parse server message. Something is definitely wrong!");
            console.log("%d", resobj.GetParseError());
        }

        Result result;
        if (resobj.HasMember("key"))
        {
            if ((ApiErrorCode)atoi(resobj["code"].GetString()) == ApiErrorCode::token_expired)
            {
                AUTH = false;
            }

            console.log("[Socket] [Recv] message: %s", dec);

            if (strcmp(resobj["key"].GetString(), "bash") == 0)
            {
                bash_results(resobj["info"].GetString());
                return EM_TRUE;
            }

            result.code = (ApiErrorCode)atoi(resobj["code"].GetString());

            result.info = (char *)malloc(resobj["info"].GetStringLength());
            strcpy(result.info, resobj["info"].GetString());

            result.success = resobj["success"].GetBool();
            result.key = atoi(resobj["key"].GetString());

            if (events[result.key] != nullptr)
            {

                (*events[result.key])(result);
                delete result.info;
                delete events[result.key];
                events.erase(result.key);
            }
        }
        else
        {
            if (!live_data)
                return EM_TRUE;
            // console.log("[Socket] [Recv] message: %s", dec);

            // update data report
            ServerReport new_report;
            new_report.cpu_percent = resobj["cpu_percent"].GetInt();
            new_report.memory_used = resobj["memory_used"].GetInt();
            new_report.memory_total = resobj["memory_total"].GetInt();

            new_report.cores = resobj["cores"].GetInt();
            new_report.uptime = resobj["uptime"].GetInt();

            new_report.server_capacity = resobj["server_capacity"].GetInt();
            new_report.users_count = resobj["users_count"].GetInt();
            new_report.active_users_count = resobj["active_users_count"].GetInt();

            // if ( ServerReportStore::last_report.users_count != new_report.users_count)
            // {
            //     if (ServerReportStore::last_report.users == nullptr)
            //     {
            //         console.log("c");
            //         new_report.users = (User *)new User[new_report.users_count];
            //     }
            //     else
            //     {
            //         console.log("d");

            //         delete ServerReportStore::last_report.users;

            //         new_report.users = (User *)new User[new_report.users_count];
            //     }
            // }else
            //     new_report.users = (User *)new User[new_report.users_count];
            // this works , pls dont touch it !!! fuck it
            if (ServerReportStore::last_report.users == nullptr)
                ServerReportStore::last_report.users = (User *)new User[999];
            new_report.users = ServerReportStore::last_report.users;

            for (int i = 0; i < new_report.users_count; i++)
            {
                // console.log("E");

                User user;
                user.id = i;
                user.name = resobj["users"].GetArray()[i].GetObject()["name"].GetString();
                user.password = resobj["users"].GetArray()[i].GetObject()["password"].GetString();
                user.speed_limit.upload = resobj["users"].GetArray()[i].GetObject()["speed_limit"].GetObject()["upload"].GetInt();
                user.speed_limit.download = resobj["users"].GetArray()[i].GetObject()["speed_limit"].GetObject()["download"].GetInt();
                user.speed_current.upload = resobj["users"].GetArray()[i].GetObject()["speed_current"].GetObject()["upload"].GetInt();
                user.speed_current.download = resobj["users"].GetArray()[i].GetObject()["speed_current"].GetObject()["download"].GetInt();
                user.traffic_limit.upload = resobj["users"].GetArray()[i].GetObject()["traffic_limit"].GetObject()["upload"].GetInt();
                user.traffic_limit.download = resobj["users"].GetArray()[i].GetObject()["traffic_limit"].GetObject()["download"].GetInt();
                user.traffic_total.upload = resobj["users"].GetArray()[i].GetObject()["traffic_total"].GetObject()["upload"].GetInt();
                user.traffic_total.download = resobj["users"].GetArray()[i].GetObject()["traffic_total"].GetObject()["download"].GetInt();
                user.ip_limit = resobj["users"].GetArray()[i].GetObject()["ip_limit"].GetInt();
                user.enable = resobj["users"].GetArray()[i].GetObject()["enable"].GetBool();
                user.days_left = resobj["users"].GetArray()[i].GetObject()["days_left"].GetInt();
                user.day_limit = resobj["users"].GetArray()[i].GetObject()["day_limit"].GetBool();
                user.minutes_left = resobj["users"].GetArray()[i].GetObject()["minutes_left"].GetInt();
                user.protocol = resobj["users"].GetArray()[i].GetObject()["protocol"].GetInt();
                user.note = resobj["users"].GetArray()[i].GetObject()["note"].GetString();

                new_report.users[i] = user;
            }

            new_report.panelsettings.admin_username = resobj["panelsettings"].GetObject()["admin_username"].GetString();
            new_report.panelsettings.admin_password = resobj["panelsettings"].GetObject()["admin_password"].GetString();
            new_report.panelsettings.domain = resobj["panelsettings"].GetObject()["domain"].GetString();
            new_report.panelsettings.mainport = resobj["panelsettings"].GetObject()["mainport"].GetInt();
            new_report.panelsettings.websocket_path = resobj["panelsettings"].GetObject()["websocket_path"].GetString();
            new_report.panelsettings.fakewebsite_template = resobj["panelsettings"].GetObject()["fakewebsite_template"].GetInt();
            new_report.panelsettings.cert_path = resobj["panelsettings"].GetObject()["cert_path"].GetString();
            new_report.panelsettings.private_key_path = resobj["panelsettings"].GetObject()["private_key_path"].GetString();
            new_report.panelsettings.mux = resobj["panelsettings"].GetObject()["mux"].GetBool();

            ServerReportStore::last_report = new_report;
            (ServerReportStore::signal)(&new_report);
        }

        delete dec;
    }

    // EMSCRIPTEN_RESULT result;
    // result = emscripten_websocket_close(websocketEvent->socket, 1000, "no reason");
    // if (result)
    // {
    //     console.error("Failed to emscripten_websocket_close(): %d\n", result);
    // }
    return EM_TRUE;
}

void Connection::init()
{

    if (!emscripten_websocket_is_supported())
    {
        console.error("[Fatal]  bidirectional communications not supported, update your browser.");
        emscripten_exit_with_live_runtime();
    }

    const char *origin = (const char *)EM_ASM_PTR({
        var str = window.location.href;
        var buffer = Module._malloc(str.length + 1);
        Module.writeStringToMemory(str, buffer);
        return buffer;
    });

    char wsurl[100] = {0};
    bool ssl = false;
    if (strstr(origin, "https") != nullptr)
        ssl = true;

    const char *after_slashes = strstr(origin, "//") + 2;
    if (after_slashes == nullptr)
    {

        console.error("[Fatal] unexpected origin; either http or https was expcted.");
        emscripten_exit_with_live_runtime();
    }
    pure_origin = after_slashes;
    snprintf(wsurl, 100, "ws%s://%sstream", ssl ? "s" : "", after_slashes);

    EmscriptenWebSocketCreateAttributes ws_attrs = {
        wsurl,
        NULL,
        EM_TRUE};
    console.log("[Connect] initiate: %s", wsurl);

    EMSCRIPTEN_WEBSOCKET_T ws = emscripten_websocket_new(&ws_attrs);
    emscripten_websocket_set_onopen_callback(ws, NULL, onopen);
    emscripten_websocket_set_onerror_callback(ws, NULL, onerror);
    emscripten_websocket_set_onclose_callback(ws, NULL, onclose);
    emscripten_websocket_set_onmessage_callback(ws, NULL, onmessage);
}

rocket::signal<void(Result)> *Connection::createUser(const User &user)
{
    auto result = Connection::send("create-user", 1, user.name.c_str());
    result->connect([&](Result res)
                    { if(res.success) console.log("User %s created.", user.name.c_str()); });

    return result;
}
rocket::signal<void(Result)> *Connection::updateUser(const User &_user, bool reseting)
{
    User user = _user;
    char speed_limited_upload[16];
    char speed_limited_download[16];
    char traffic_limited_upload[16];
    char traffic_limited_download[16];
    char ip_limited_amount[16];
    char enable[16];
    char duration_limited_amount[16];
    char duration_limited_bool[16];
    char protocol[16];


    char traffic_total_upload[16];
    char traffic_total_download[16];

    if (!reseting)
    {
        user.traffic_total.upload = -1;
        user.traffic_total.download = -1;
    }

    sprintf(speed_limited_upload, "%d", user.speed_limit.upload);
    sprintf(speed_limited_download, "%d", user.speed_limit.download);
    sprintf(traffic_limited_upload, "%d", user.traffic_limit.upload);
    sprintf(traffic_limited_download, "%d", user.traffic_limit.download);
    sprintf(ip_limited_amount, "%d", user.ip_limit);
    sprintf(enable, "%d", user.enable ? 1 : 0);
    sprintf(duration_limited_amount, "%d", user.days_left);
    sprintf(duration_limited_bool, "%d", user.day_limit ? 1 : 0);
    sprintf(protocol, "%d", user.protocol);

    sprintf(traffic_total_upload, "%d", user.traffic_total.upload);
    sprintf(traffic_total_download, "%d", user.traffic_total.download);

    auto result = Connection::send("update-user", 14,
                                   user.name.c_str(),
                                   user.password.c_str(),
                                   speed_limited_upload,
                                   speed_limited_download,
                                   traffic_limited_upload,
                                   traffic_limited_download,
                                   ip_limited_amount,
                                   enable, // enable
                                   duration_limited_amount,
                                   duration_limited_bool,
                                   protocol,
                                   user.note.c_str(),traffic_total_upload,traffic_total_download);
    result->connect([=](Result res)
                    { if(res.success) console.log("User %s updated.", user.name.c_str()); });

    return result;
}
rocket::signal<void(Result)> *Connection::deleteUser(const User &user)
{
    auto result = Connection::send("delete-user", 1, user.password.c_str());
    result->connect([=](Result res)
                    { if(res.success)console.log("User %s deleted.", user.name.c_str()); });
    return result;
}
