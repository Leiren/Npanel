#include "imgui.h"
#include <cmath>
#include "log.h"
#include "colors.h"
#include "server_report_store.h"
#include "connection.h"
#include <stdlib.h>
#include "customwidgets.h"
using std::string;
static bool has_error = false;
static string error = "";
static void samelinehelpmarker(const char *desc)
{
    ImGui::SameLine();
    HelpMarker(desc);
    ImGui::SameLine();
}
static void cursor_pos()
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("x").x;

    ImGui::SetCursorPosX(TEXT_BASE_WIDTH * 30);
}
static void error_popupframe()
{
    if (has_error)
        ImGui::OpenPopup("Error##psettings");
    ImGui::SetNextWindowSize(ImVec2(600, -1));
    bool meaningless = true;
    if (ImGui::BeginPopupModal("Error##psettings", &meaningless, ImGuiWindowFlags_NoSavedSettings))
    {

        has_error = false;
        ImGui::TextWrapped("%s", error.c_str());

        ImGui::EndPopup();
    }
}
static char input_admin_username[80];
static char input_admin_password[80];
static char input_domain[80];
static int input_mainport;
static char input_ws_path[80];
static int input_fake_website_index = 0;
static char input_cert_path[80];
static char input_private_key_path[80];
static bool input_mux = true;
void apply_panel_settingn()
{

    if (strlen(input_admin_username) < 3)
    {
        error = "admin user name too low.";
        has_error = true;
    }

    if (strlen(input_admin_password) < 3)
    {
        error = "admin password too low.";
        has_error = true;
    }
    if (strlen(input_domain) < 3)
    {
        error = "domain too low.";
        has_error = true;
    }
    if (strlen(input_ws_path) < 2)
    {
        error = "ws path too low.";
        has_error = true;
    }
    if (input_ws_path[0] != '/')
    {
        error = "ws path must begin with /";
        has_error = true;
    }
    if (strlen(input_cert_path) < 4)
    {
        error = "cert path too low.";
        has_error = true;
    }
#ifndef win_build
    if (input_cert_path[0] != '/')
    {
        error = "cert path must begin with /";
        has_error = true;
    }
#endif
    if (strlen(input_private_key_path) < 4)
    {
        error = "private key path too low.";
        has_error = true;
    }
#ifndef win_build

    if (input_private_key_path[0] != '/')
    {
        error = "private key path must begin with /";
        has_error = true;
    }
#endif

    if (has_error)
        return;
    char main_domain_buf[10];
    sprintf(main_domain_buf, "%d", input_mainport);
    char fake_website_index_buf[10];
    sprintf(fake_website_index_buf, "%d", input_fake_website_index);

    Connection::send("update-panel-settings", 9,
                     input_admin_username,
                     input_admin_password,
                     input_domain,
                     main_domain_buf,
                     input_ws_path,
                     fake_website_index_buf,
                     input_cert_path,
                     input_private_key_path,
                     input_mux ? "1" : "0")
        ->connect([](Result res)
                  {
                      if (res.success)
                      {
                          console.log("Panel new settings sent.");
                          has_error = false;
                      }
                      else
                      {
                          console.log("Panel new settings Failed. (so the settings aren't applied) info:");
                          console.log("%s",res.info);
                          error = res.info;
                          has_error = true;
                      } });
}

void panel_settings_frame(bool tab_changed)
{
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    if (tab_changed || ServerReportStore::last_report.panelsettings.domain == "")
    {

        strcpy(input_admin_username, ServerReportStore::last_report.panelsettings.admin_username.c_str());
        strcpy(input_admin_password, ServerReportStore::last_report.panelsettings.admin_password.c_str());
        strcpy(input_domain, ServerReportStore::last_report.panelsettings.domain.c_str());
        strcpy(input_ws_path, ServerReportStore::last_report.panelsettings.websocket_path.c_str());
        strcpy(input_cert_path, ServerReportStore::last_report.panelsettings.cert_path.c_str());
        strcpy(input_private_key_path, ServerReportStore::last_report.panelsettings.private_key_path.c_str());

        input_mainport = ServerReportStore::last_report.panelsettings.mainport;
        input_fake_website_index = ServerReportStore::last_report.panelsettings.fakewebsite_template;
        input_mux = ServerReportStore::last_report.panelsettings.mux;
    }

    ImGui::Text("login username:");
    samelinehelpmarker(
        "User name for login.\n"
        "It is used for base path of panel url! if you change it you must change your url to new username.\n");
    cursor_pos();
    ImGui::InputText("##input_admin_username", input_admin_username, IM_ARRAYSIZE(input_admin_username));
    ImGui::Text("login password:");
    samelinehelpmarker(
        "Password for login.\n"
        "Note:\nIf you forget your username or password, you find it in file /opt/Npanel/panel.json\n");
    cursor_pos();

    ImGui::InputText("##input_admin_password", input_admin_password, IM_ARRAYSIZE(input_admin_password), ImGuiInputTextFlags_Password);
    ImGui::Text("domain:");
    samelinehelpmarker("Your website domain.\n"
                       "Note: your ssl certifcate & private key must match the domain.\n"
                       "Therefore If you change it, you must change ssl certificate as well.\n"
                       "example: sub.mydomain.com or mydomain.com ");
    cursor_pos();
    ImGui::InputText("##input_domain", input_domain, IM_ARRAYSIZE(input_domain));

    ImGui::Text("mainport:");
    samelinehelpmarker("Trojan-go and Npanel use this port\n"
                       "Note: if you set anything other than 443 , your are at risk of being blocked by gfw, i suggest you to never do this!\n"
                       "Note: Changing port will NOT unblock your server, you should change ip.\n"
                       "Note: Changing port will NOT improve your connection speed/bandwidth.\n"
                       "Note: Npanel will try to free the port (kill) before it starts. make sure this port it not used by anyother proccess.");
    cursor_pos();
    if (ImGui::InputInt("##input_mainport", &input_mainport, 0))
    {
        if (input_mainport == 80 || input_mainport == 2060 || input_mainport == 2061)
            input_mainport = 0;
    }
    input_mainport = std::min(65535, input_mainport);
    input_mainport = std::max(10, input_mainport);

    ImGui::Text("websocket path:");
    samelinehelpmarker("Used for Websocket configs\n"
                       "If you change this value, all websocket configs must be re-generated and be sent to the user again.\n"
                       "Must start with slash \"/\" and a string\n"
                       "Examples: /firebase /fetch /users /ws etc..\n");

    cursor_pos();
    ImGui::InputText("##input_websocket_path", input_ws_path, IM_ARRAYSIZE(input_ws_path));

    ImGui::Text("fakewebsite template:");
    samelinehelpmarker("choose a template, when a stranger opens your domain he will see a fake website.\n"
                       "this is why you should run this pannel on port 443, otherwise the browser will not show the fake website!\n"
                       "Note: you are recommended to customize the html/css , files are in\n /opt/Npanel/templates\n\n"
                       "More templates will come in next versions");
    const char *items[] = {"Social Media1", "creative studio", "Pizza seler", "Direct TV", "Covido"};
    cursor_pos();

    ImGui::Combo("##input_fake_website_index", &input_fake_website_index, items, IM_ARRAYSIZE(items));

    ImGui::Text("certificate path:");
    samelinehelpmarker("Certificate file path\n"
#ifndef win_build
                       "Start your path with /  and as you know it's full path!\n"
                       "This certificate must match the domain you enetered.\n"
                       "Examples: /etc/letsencrypt/live/my.domain.com/fullchain.pem\n"
                       "Examples: /etc/letsencrypt/live/my.domain.com/cert.pem"
#else
                       "Examples: C:\\ssl\\fullchain.pem \n"
                       "Examples: C:\\ssl\\cert.pem"
#endif
    );

    cursor_pos();
    ImGui::InputText("##input_cert_path", input_cert_path, IM_ARRAYSIZE(input_cert_path));

    ImGui::Text("private key path:");
    samelinehelpmarker("Private key file path\n"
#ifndef win_build

                       "Start your path with /  and as you know it's full path!\n"
                       "This key must match the domain you enetered.\n"
                       "Examples: /etc/letsencrypt/live/my.domain.com/privkey.pem"
#else
                       "Examples: C:\\ssl\\key.pem"
#endif
    );

    cursor_pos();
    ImGui::InputText("##input_private_key_path", input_private_key_path, IM_ARRAYSIZE(input_private_key_path));

    ImGui::Text("Mux:");
    samelinehelpmarker("my suggestion: keep it on (server side) \n but on client side it should be off(better speed)");
    cursor_pos();
    ToggleButton("##input_mux", &input_mux);
    ImGui::NewLine();

    if (ImGui::Button("Apply", ImVec2(280, TEXT_BASE_HEIGHT * 1.5f)))
        ImGui::OpenPopup("Confirm##apply_and_restart");
    ImGui::SameLine();

    if (ImGui::Button("Backup Panel", ImVec2(280, TEXT_BASE_HEIGHT * 1.5f)))
        ImGui::OpenPopup("Backup##how_to_backup");

    if (ImGui::BeginPopupModal("Confirm##apply_and_restart", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text("Panel will restart, and comes up with new settings.\nin-case panel did not start\n"
#ifndef win_build

                    "1- check panel logs \"service npanel status\"\n"
                    "2- correct your mistakes in /opt/Npanel/panel.json\n"
                    "3- restart \"service npanel restart\"\n"
#else
                    "1- check panel logs from console window\n"
                    "2- correct your mistakes in Npanel/panel.json\n"
                    "3- restart the program"
#endif
                    "\n You should backup before changing settings.\n\n"
                    "\n\n");
        ImGui::Separator();

        // static int unused_i = 0;
        // ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

        static bool dont_ask_me_next_time = false;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
        ImGui::PopStyleVar();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            apply_panel_settingn();
            if (!has_error)
            {
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        error_popupframe();
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Backup##how_to_backup", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text(
#ifndef win_build

            "In order to backup your users: /opt/Npanel/users.db\n"
            "In order to backup your settings: /opt/Npanel/panel.json\n"
            "But it is suggested to Backup the folder /opt/Npanel entirely.\n\n"
            // "You can transfer that folder to your new server without any problem, but \n"
            // "before that install npanel from github script, then move users.db and panel.jso from your backup\n to destination server.\n"
            // "then service npanel start.\n\n"
            "For security reasons, you should download those files yourself, we don't provide you any links."
#else
            "In order to backup your users: Npanel/users.db\n"
            "In order to backup your settings: Npanel/panel.json\n"
            "But I suggest you to Backup the folder Npanel/ entirely.\n\n"
            "For security reasons, you should download those files yourself, we don't provide you any links."
#endif


        );
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    error_popupframe();
}