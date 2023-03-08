#pragma once
#include <string>
#include "rocket.hpp"

using std::string;
struct TrafficTotal
{
    int upload;
    int download;
};
struct TrafficLimit
{
    int max;
};
struct SpeedCurrent
{
    int upload;
    int download;
};
struct SpeedLimit
{
    int upload;
    int download;
};
struct User
{
    int id; // only client
    string name;
    string password;
    SpeedCurrent speed_current;
    SpeedLimit speed_limit;
    TrafficTotal traffic_total;
    TrafficLimit traffic_limit;
    bool enable;
    int ip_limit;
    int days_left;
    bool day_limit;
    int minutes_left;
    int protocol;
    string note;
};

struct EandN
{
    bool enable;
    bool notify;
    // EandN(bool e,bool n):enable(e),notify(n){}
};
struct SellerOptions
{
    EandN notif_panel_login_fail;
    EandN notif_panel_login_success;
    EandN notif_panel_information_changed;
    EandN notif_panel_cpu_usage_high;
    EandN notif_panel_mem_usage_high;
    EandN notif_panel_server_reboot;
    EandN notif_panel_start;
    EandN notif_user_reach_duration_limit;
    EandN notif_user_reach_traffic_limit;
    EandN notif_user_added;
    EandN notif_user_disabled;
    EandN notif_user_enable;
    EandN notif_user_removed;
    EandN notif_user_support;
};
struct BotUserOptions
{
    bool can_ask_info;
    bool info_include_traffic_used;
    bool info_include_ip_limit;
    bool info_include_speed_limit;
    bool info_include_traffic_limit;
    bool info_include_days_left;
    bool info_include_user_note;
};
struct BotOverrideOptions
{
    string domain;
    string sni;
    string ws_host;
    int port;
   
};
struct PanelConfStructure
{
    string admin_username;
    string admin_password;
    string domain;
    int mainport;
    string websocket_path;
    int fakewebsite_template;
    string cert_path;
    string private_key_path;
    bool mux;
    bool first_launch;
    string telegram_bot_key;
    BotOverrideOptions botoverrides;

};


struct ServerReport
{
    int cpu_percent;
    int memory_used;
    int memory_total;
    int cores;
    int uptime;
    int server_capacity;
    int users_count = 0;
    int active_users_count;
    User *users = nullptr;
    PanelConfStructure panelsettings;
    int days_left;
    SellerOptions selleroptions;
    BotUserOptions useroptions;
};
class ServerReportStore
{
private:
    /* data */
    ServerReportStore(/* args */);
    ~ServerReportStore();

public:
    static rocket::signal<void(ServerReport *)> signal;
    static ServerReport last_report;
};
