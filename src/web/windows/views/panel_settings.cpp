#include "imgui.h"
#include <cmath>
#include "log.h"
#include "colors.h"
#include "server_report_store.h"
#include "connection.h"
#include <stdlib.h>
extern void HelpMarker(const char *desc);
extern void ToggleButton(const char *str_id, bool *v);

void samelinehelpmarker(const char *desc)
{
    ImGui::SameLine();
    HelpMarker(desc);
    ImGui::SameLine();
}
void cursor_pos()
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

    ImGui::SetCursorPosX(TEXT_BASE_WIDTH * 30);
}

void panel_settings_frame()
{
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    static char input_admin_username[80];
    static char input_admin_password[80];
    static char input_domain[80];
    static int input_mainport;
    static char input_ws_path[80];
    static int input_fake_website_index = 0;
    static char input_cert_path[80];
    static char input_private_key_path[80];
    static bool input_mux = true;

    static bool first_time = true;
    if (first_time)
    {
        first_time = false;

        strcpy(input_admin_username, ServerReportStore::last_report.panelsettings.admin_username.c_str());
        strcpy(input_admin_password, ServerReportStore::last_report.panelsettings.admin_password.c_str());
        strcpy(input_domain, ServerReportStore::last_report.panelsettings.domain.c_str());
        strcpy(input_ws_path, ServerReportStore::last_report.panelsettings.websocket_path.c_str());
        strcpy(input_cert_path, ServerReportStore::last_report.panelsettings.cert_path.c_str());
        strcpy(input_private_key_path, ServerReportStore::last_report.panelsettings.private_key_path.c_str());

        input_mainport =  ServerReportStore::last_report.panelsettings.mainport;
        input_fake_website_index =  ServerReportStore::last_report.panelsettings.fakewebsite_template;
        input_mux =  ServerReportStore::last_report.panelsettings.mux;
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

    ImGui::InputText("##input_admin_password", input_admin_password, IM_ARRAYSIZE(input_admin_password));
    ImGui::Text("domain:");
    samelinehelpmarker("Your website domain.\n"
                       "Note: your ssl certifcate & private key must match the domain.\n"
                       "Therefore If you change it, you must change ssl certificate as well.");
    cursor_pos();
    ImGui::InputText("##input_domain", input_domain, IM_ARRAYSIZE(input_domain));

    ImGui::Text("mainport:");
    samelinehelpmarker("Trojan-go and Npanel use this port\n"
                       "Note: if you set anything other than 443 , your are at risk of being blocked by gfw, i suggest you to never do this!\n"
                       "Note: Changing port will NOT unblock your server, you should change ip.\n"
                       "Note: Changing port will NOT improve your connection speed/bandwidth.");
    cursor_pos();
    ImGui::InputInt("##input_mainport", &input_mainport, 0);

    ImGui::Text("websocket path:");
    samelinehelpmarker("Used for Websocket configs\n"
                       "If you change this value, all websocket configs must be re-generated and be sent to the user again.\n"
                       "Must start with slash \"/\" and a string\n"
                       "Examples: /firebase /fetch /users /ws etc..\n");

    cursor_pos();
    ImGui::InputText("##input_websocket_path", input_ws_path, IM_ARRAYSIZE(input_ws_path));

    ImGui::Text("fakewebsite template:");
    samelinehelpmarker("choose a template, when a stranger opens your domain it will see a fake website.\n"
                       "this is why you should run this pannel on port 443, otherwise the browser will not show the fake website!\n"
                       "Note: you are recommended to customize the html/css , files are in\n /opt/Npanel/templates\n");
    const char *items[] = {"Social Media1", "Social Media2", "Pizza seler", "Direct TV", "Covido"};
    cursor_pos();

    ImGui::Combo("##input_fake_website_index", &input_fake_website_index, items, IM_ARRAYSIZE(items));

    ImGui::Text("certificate path:");
    samelinehelpmarker("Certificate file path\n"
                       "Start your path with /  and as you know it's complete path!\n"
                       "This certificate must match the domain you enetered.\n"
                       "Examples: /etc/letsencrypt/live/my.domain.com/fullchain.pem\n"
                       "Examples: /etc/letsencrypt/live/my.domain.com/cert.pem");

    cursor_pos();
    ImGui::InputText("##input_cert_path", input_cert_path, IM_ARRAYSIZE(input_cert_path));

    ImGui::Text("private key path:");
    samelinehelpmarker("Private key file path\n"
                       "Start your path with /  and as you know it's complete path!\n"
                       "This key must match the domain you enetered.\n"
                       "Examples: /etc/letsencrypt/live/my.domain.com/privkey.pem\n");

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

        ImGui::Text("Panel will restart, and comes up with new settings.\n in-case panel did not start\n"
                    "first check logs with \"service npanel status\" \n"
                    "then change your wrong settings in /opt/Npanel/panel.json\n"
                    "then \"service npanel restart\"\n"
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
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Backup##how_to_backup", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {

        ImGui::Text("In order to backup your users: /opt/Npanel/users.db\n"
                    "In order to backup your settings: /opt/Npanel/panel.json\n"
                    "But I suggest you to Backup the folder /opt/Npanel completely.\n\n"
                    "for security reasons, you should dwonload those files yourself, we don't provide you any links."

        );
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}