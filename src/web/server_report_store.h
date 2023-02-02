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
    int upload;
    int download;
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
    int id; //only client
    string name;
    string password;
    SpeedCurrent speed_current;
    SpeedLimit speed_limit;
    TrafficTotal traffic_total;
    TrafficLimit traffic_limit;
    bool enable;
    int ip_limit;
    int days_left;
    int protocol;
    string note;
};

struct PanelConfStructure
{
    string admin_username;
    string admin_password;
    string domain;
    int mainport;
    string websocket_path;
    string fakewebsite_url;
    string cert_path;
    string private_key_path;
    bool mux;
    bool first_launch;
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
    char last_software_update[35];  
};
class   ServerReportStore
{
private:
    /* data */
    ServerReportStore(/* args */);
    ~ServerReportStore();

public:
    static rocket::signal<void(ServerReport *)> signal;
    static ServerReport last_report;
};
