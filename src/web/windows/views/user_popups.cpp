#include "imgui.h"
#include "imgui_internal.h"
#include "server_report_store.h"
#include <cmath>
#include "log.h"
#include "colors.h"
#include "connection.h"
#include "QrToPng.h"
#include "imageloader.h"
#include "emscripten/bind.h"
void HelpMarker(const char *desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
void error_popupframe();

static bool has_error = false;
static std::string last_error_msg;
struct nup_state
{
    char name[16] = "";
    int protocol = 0;
    int speed_limited = 0; // 0 - 1
    int speed_limited_upload = 0;
    int speed_limited_download = 0;
    int traffic_limited = 0; // 0 - 1
    int traffic_limited_upload = 0;
    int traffic_limited_download = 0;
    int duration_limited = 0; // 0 - 1
    int duration_limited_amount = 0;
    int ip_limited = 0; // 0 - 1
    int ip_limited_amount = 0;
    char notes[512] = "";
};

static void create_user(nup_state &cst)
{

    if (cst.speed_limited == 0)
    {
        cst.speed_limited_upload = 0;
        cst.speed_limited_download = 0;
    }
    else
    {
        const char *err_msg = "Never Enter value 0 for a speed limit\n if you want to disable a config, use Enable/Disable button or enter a number higher than 0 like 1.";
        if (cst.speed_limited_upload == 0 | cst.speed_limited_download == 0)
        {
            has_error = true;
            last_error_msg = err_msg;
            return;
        }
    }

    if (cst.traffic_limited == 0)
    {
        cst.traffic_limited_upload = 0;
        cst.traffic_limited_download = 0;
    }
    else
    {
        const char *err_msg = "Never Enter value 0 for a traffic limit\n if you want to disable a config, use Enable/Disable button or enter a number higher than 0 like 1.";

        if (cst.traffic_limited_upload == 0 | cst.traffic_limited_download == 0)
        {
            has_error = true;
            last_error_msg = err_msg;
            return;
        }
    }
    if (cst.ip_limited == 0)
    {
        cst.ip_limited_amount = 0;
    }
    else
    {
        const char *err_msg = "Never Enter value 0 for a ip limit\n if you want to disable a config, use Enable/Disable button.";

        if (cst.ip_limited_amount == 0)
        {
            has_error = true;
            last_error_msg = err_msg;
            return;
        }
    }
    if (cst.duration_limited == 0)
    {
        cst.duration_limited_amount = 0;
    }
    else
    {
        const char *err_msg = "Never Enter value 0 for a ip limit\n if you want to disable a config, use Enable/Disable button.";

        if (cst.duration_limited_amount == 0)
        {
            has_error = true;
            last_error_msg = err_msg;
            return;
        }
    }

    char speed_limited_upload[16];
    char speed_limited_download[16];
    char traffic_limited_upload[16];
    char traffic_limited_download[16];
    char duration_limited_amount[16];
    char ip_limited_amount[16];
    char protocol[16];

    sprintf(speed_limited_upload, "%d", cst.speed_limited_upload);
    sprintf(speed_limited_download, "%d", cst.speed_limited_download);
    sprintf(traffic_limited_upload, "%d", cst.traffic_limited_upload);
    sprintf(traffic_limited_download, "%d", cst.traffic_limited_download);
    sprintf(duration_limited_amount, "%d", cst.duration_limited_amount);
    sprintf(ip_limited_amount, "%d", cst.ip_limited_amount);
    sprintf(protocol, "%d", cst.protocol);
    User cru;
    cru.name = cst.name;
    Connection::createUser(cru)->connect([=](Result res)
                                         {
        if (!res.success)
        {
            has_error = true;
            last_error_msg = res.info;
            return;
        }
        User upu;
        upu.name = cst.name;
        upu.password = res.info;
        upu.speed_limit.upload = cst.speed_limited_upload;
        upu.speed_limit.download = cst.speed_limited_download;
        upu.traffic_limit.upload = cst.traffic_limited_upload;
        upu.traffic_limit.download = cst.traffic_limited_download;
        upu.ip_limit = cst.ip_limited_amount;
        upu.enable = "1";//enable
        upu.days_left = cst.duration_limited_amount;
        upu.protocol = cst.protocol;
        upu.note = cst.notes;
       
          Connection::updateUser(upu)->connect([=](Result res)
                                                          {
                if (!res.success)
                {
                            has_error = true;

                            last_error_msg = res.info;
                            return;
                }

                has_error = false;

                                                          }); });
    cst = {};
}
static void update_user(nup_state &cst, const char *user_password, bool user_enable)
{
    if (cst.speed_limited == 0)
    {
        cst.speed_limited_upload = 0;
        cst.speed_limited_download = 0;
    }
    else
    {
        const char *err_msg = "Never Enter value 0 for a speed limit\n if you want to disable a config, use Enable/Disable button or enter a number higher than 0 like 1.";
        if (cst.speed_limited_upload == 0 | cst.speed_limited_download == 0)
        {
            has_error = true;
            last_error_msg = err_msg;
            return;
        }
    }

    if (cst.traffic_limited == 0)
    {
        cst.traffic_limited_upload = 0;
        cst.traffic_limited_download = 0;
    }
    else
    {
        const char *err_msg = "Never Enter value 0 for a traffic limit\n if you want to disable a config, use Enable/Disable button or enter a number higher than 0 like 1.";

        if (cst.traffic_limited_upload == 0 | cst.traffic_limited_download == 0)
        {
            has_error = true;
            last_error_msg = err_msg;
            return;
        }
    }

    if (cst.ip_limited == 0)
    {
        cst.ip_limited_amount = 0;
    }
    else
    {
        const char *err_msg = "Never Enter value 0 for a ip limit\n if you want to disable a config, use Enable/Disable button.";

        if (cst.ip_limited_amount == 0)
        {
            has_error = true;
            last_error_msg = err_msg;
            return;
        }
    }
    if (cst.duration_limited == 0)
    {
        cst.duration_limited_amount = 0;
    }
    else
    {
        const char *err_msg = "Never Enter value 0 for a date limit\n if you want to disable a config, use Enable/Disable button.";

        if (cst.duration_limited_amount == 0)
        {
            has_error = true;
            last_error_msg = err_msg;
            return;
        }
    }
    User upu;
    upu.name = cst.name;
    upu.password = user_password;
    upu.speed_limit.upload = cst.speed_limited_upload;
    upu.speed_limit.download = cst.speed_limited_download;
    upu.traffic_limit.upload = cst.traffic_limited_upload;
    upu.traffic_limit.download = cst.traffic_limited_download;
    upu.traffic_total.upload = -1;
    upu.traffic_total.download = -1;
    upu.ip_limit = cst.ip_limited_amount;
    upu.enable = user_enable; // enable
    upu.days_left = cst.duration_limited_amount;
    upu.protocol = cst.protocol;
    upu.note = cst.notes;

    Connection::updateUser(upu)->connect([=](Result res)
                                         {
                                             if (!res.success)
                                             {
                                                 has_error = true;

                                                 last_error_msg = res.info;
                                                 return;
                                             }
                                         

                                                 has_error = false; });

    cst = {};
}
static void delete_user(User &user)
{
    Connection::deleteUser(user)
        ->connect([&](Result res)
                  {
        if (!res.success)
        {
            has_error = true;

            last_error_msg = res.info;
            return;
        }
        has_error = false; });
}

void new_user_popup_frame(bool *new_state)
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    static nup_state state;
    if (*new_state)
    {
        // state = {};
        *new_state = false;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(600, -1));
    }

    bool show = true; // yes meaningless
    const ImGuiViewport *viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Enter Information##new_user_popup", &show, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::SetWindowSize(ImVec2(600, -1), ImGuiCond_Once);
        // ImGui::SetNextWindowPos(ImVec2(-1,-1),ImGuiCond_Once);
        // name
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Name");
        ImGui::SameLine();
        ImGui::InputText("##name_input_text", state.name, IM_ARRAYSIZE(state.name));
        ImGui::SameLine();
        HelpMarker("Enter the user name.\ncan have space.\nmaximum 16 characters.\nmultiple configs can have same names.");

        // protocol
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Protocol:");
        ImGui::Indent();
        ImGui::RadioButton("Trojan Tcp", &state.protocol, 0);
        ImGui::RadioButton("Trojan Tcp +  Websocket", &state.protocol, 1);
        ImGui::SameLine();
        HelpMarker("Generate 2 configs: 1 for Tcp and 1 for Websocket.\n"
                   "The Websocket config is only used for CDN , otherwise it has no special difference or advantage over regular TCP.\n"
                   "you should only use the Websockt config when you are using a CDN, becuse TCP won't work with CDN.");
        ImGui::Unindent();

        ImGui::NewLine();
        // // speed limit
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Speed Limit:");
        ImGui::SameLine();
        HelpMarker("Maximum download / upload speed the user will be able to reach.\n"
                   "Unit: (KBytes/sec)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - TEXT_BASE_WIDTH * 22);
        ImGui::RadioButton("Unlimited##sl", &state.speed_limited, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Limited##sl", &state.speed_limited, 1);
        if (state.speed_limited == 1)
        {
            ImVec2 content_avail = ImGui::GetContentRegionAvail();
            const float space = ImGui::GetStyle().ItemSpacing.x / 2;
            if (ImGui::BeginTable("table##sl", 2))
            {

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Upload:");
                ImGui::SameLine();
                if (ImGui::InputInt("##table_sl_up", &state.speed_limited_upload, 0))
                    state.speed_limited_upload = abs(state.speed_limited_upload);
                ImGui::TableSetColumnIndex(1);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Download:");
                ImGui::SameLine();
                if (ImGui::InputInt("##table_sl_dl", &state.speed_limited_download, 0))
                    state.speed_limited_download = abs(state.speed_limited_download);
                ImGui::EndTable();
            }
        }

        ImGui::NewLine();
        // Traffic Limit
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Traffic Limit:");
        ImGui::SameLine();
        HelpMarker("Maximum download / upload total traffic the user will be able to reach.\n"
                   "Unit: (GBytes)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - TEXT_BASE_WIDTH * 22);
        ImGui::RadioButton("Unlimited##tl", &state.traffic_limited, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Limited##tl", &state.traffic_limited, 1);
        if (state.traffic_limited == 1)
        {
            ImVec2 content_avail = ImGui::GetContentRegionAvail();
            const float space = ImGui::GetStyle().ItemSpacing.x / 2;
            if (ImGui::BeginTable("table##tl", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Upload:");
                ImGui::SameLine();
                if (ImGui::InputInt("##table_tl_up", &state.traffic_limited_upload, 0))
                    state.traffic_limited_upload = abs(state.traffic_limited_upload);
                ImGui::TableSetColumnIndex(1);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Download:");
                ImGui::SameLine();
                if (ImGui::InputInt("##table_tl_dl", &state.traffic_limited_download, 0))
                    state.traffic_limited_download = abs(state.traffic_limited_download);
                ImGui::EndTable();
            }
        }

        ImGui::NewLine();
        // Duration Limit
        ImGui::Text("Duration Limit:");
        ImGui::SameLine();
        HelpMarker("How many days this config works? you can ofcourse extend it later.\n"
                   "Unit: (days)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - TEXT_BASE_WIDTH * 22);
        ImGui::RadioButton("Unlimited##dl", &state.duration_limited, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Limited##dl", &state.duration_limited, 1);

        if (state.duration_limited == 1)
        {
            ImGui::Text("Days:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputInt("##dl_input", &state.duration_limited_amount, 0))
                state.duration_limited_amount = abs(state.duration_limited_amount);
        }

        ImGui::NewLine();
        // Duration Limit
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Ip Limit:");
        ImGui::SameLine();
        HelpMarker("How many devices can connect at same time?\n"
                   "Indeed, the user can connect multiple devices behind a NAT\n(wifi forexample) and all of them count as 1 device\n"
                   "because they have 1 ip.\n"
                   "Unit: (IP Count)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - TEXT_BASE_WIDTH * 22);
        ImGui::RadioButton("Unlimited##il", &state.ip_limited, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Limited##il", &state.ip_limited, 1);
        if (state.ip_limited == 1)
        {
            ImGui::AlignTextToFramePadding();

            ImGui::Text("Ips:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1);

            if (ImGui::InputInt("##ipl_input", &state.ip_limited_amount, 0))
                state.ip_limited_amount = abs(state.ip_limited_amount);
        }
        ImGui::AlignTextToFramePadding();

        ImGui::Text("Optional Notes:");
        ImGui::SameLine();
        HelpMarker("If you want to remember something about this user, write it here, its just a note pad and you can leave it empty.");
        ImGui::InputTextMultiline("##nup_notes", state.notes, IM_ARRAYSIZE(state.notes), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4));

        ImGui::Separator();
        // ImGui::SetItemDefaultFocus();
        if (ImGui::Button("OK", ImVec2(120, 0)) && 0 < strlen(state.name))
        {
            create_user(state);
            if (!has_error)
                ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        error_popupframe();

        ImGui::EndPopup();
    }
}
void edit_user_popupframe(User **_user)
{
    static nup_state state;
    static User user;
    if (_user != nullptr && *_user != nullptr)
    {
        user = **_user;
        *_user = nullptr;
        strncpy(state.name, user.name.c_str(), sizeof(state.name));
        strncpy(state.notes, user.note.c_str(), sizeof(state.notes));
        state.speed_limited_upload = user.speed_limit.upload;
        state.speed_limited_download = user.speed_limit.download;
        state.traffic_limited_upload = user.traffic_limit.upload;
        state.traffic_limited_download = user.traffic_limit.download;
        state.duration_limited_amount = user.days_left;
        state.ip_limited_amount = user.ip_limit;
        state.protocol = user.protocol;

        if (user.speed_limit.upload != 0 | user.speed_limit.download != 0)
        {
            state.speed_limited_upload = 1;
        }
        if (user.traffic_limit.upload != 0 | user.traffic_limit.upload != 0)
        {
            state.traffic_limited = 1;
        }
        if (user.ip_limit != 0)
        {
            state.ip_limited = 1;
        }
        if (user.days_left != 0)
        {
            state.duration_limited = 1;
        }
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(600, -1));
    }
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    bool show = true; // yes meaningless

    if (ImGui::BeginPopupModal("Enter Information##edit_user_popup", &show, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings))
    {

        ImGui::SetWindowSize(ImVec2(600, -1), ImGuiCond_Once);
        // ImGui::SetNextWindowPos(ImVec2(-1,-1),ImGuiCond_Once);
        // name
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Name");
        ImGui::SameLine();
        ImGui::InputText("##name_input_text", state.name, IM_ARRAYSIZE(state.name));
        ImGui::SameLine();
        HelpMarker("Enter the user name.\ncan have space.\nmaximum 16 characters.\nmultiple configs can have same names.");

        // protocol
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Protocol:");
        ImGui::Indent();
        ImGui::RadioButton("Trojan Tcp", &state.protocol, 0);
        ImGui::RadioButton("Trojan Tcp +  Websocket", &state.protocol, 1);
        ImGui::SameLine();
        HelpMarker("Generate 2 configs: 1 for Tcp and 1 for Websocket.\n"
                   "The Websocket config is only used for CDN , otherwise it has no special difference or advantage over regular TCP.\n"
                   "you should only use the Websockt config when you are using a CDN, becuse TCP won't work with CDN.");
        ImGui::Unindent();

        ImGui::NewLine();
        // // speed limit
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Speed Limit:");
        ImGui::SameLine();
        HelpMarker("Maximum download / upload speed the user will be able to reach.\n"
                   "Unit: (KBytes/sec)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - TEXT_BASE_WIDTH * 22);
        ImGui::RadioButton("Unlimited##sl", &state.speed_limited, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Limited##sl", &state.speed_limited, 1);
        if (state.speed_limited == 1)
        {
            ImVec2 content_avail = ImGui::GetContentRegionAvail();
            const float space = ImGui::GetStyle().ItemSpacing.x / 2;
            if (ImGui::BeginTable("table##sl", 2))
            {

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Upload:");
                ImGui::SameLine();
                if (ImGui::InputInt("##table_sl_up", &state.speed_limited_upload, 0))
                    state.speed_limited_upload = abs(state.speed_limited_upload);
                ImGui::TableSetColumnIndex(1);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Download:");
                ImGui::SameLine();
                if (ImGui::InputInt("##table_sl_dl", &state.speed_limited_download, 0))
                    state.speed_limited_download = abs(state.speed_limited_download);
                ImGui::EndTable();
            }
        }

        ImGui::NewLine();
        // Traffic Limit
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Traffic Limit:");
        ImGui::SameLine();
        HelpMarker("Maximum download / upload total traffic the user will be able to reach.\n"
                   "Unit: (GBytes)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - TEXT_BASE_WIDTH * 22);
        ImGui::RadioButton("Unlimited##tl", &state.traffic_limited, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Limited##tl", &state.traffic_limited, 1);
        if (state.traffic_limited == 1)
        {
            ImVec2 content_avail = ImGui::GetContentRegionAvail();
            const float space = ImGui::GetStyle().ItemSpacing.x / 2;
            if (ImGui::BeginTable("table##tl", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Upload:");
                ImGui::SameLine();
                if (ImGui::InputInt("##table_tl_up", &state.traffic_limited_upload, 0))
                    state.traffic_limited_upload = abs(state.traffic_limited_upload);
                ImGui::TableSetColumnIndex(1);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Download:");
                ImGui::SameLine();
                if (ImGui::InputInt("##table_tl_dl", &state.traffic_limited_download, 0))
                    state.traffic_limited_download = abs(state.traffic_limited_download);
                ImGui::EndTable();
            }
        }

        ImGui::NewLine();
        // Duration Limit
        ImGui::Text("Duration Limit:");
        ImGui::SameLine();
        HelpMarker("How many days this config works? you can ofcourse extend it later.\n"
                   "Unit: (days)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - TEXT_BASE_WIDTH * 22);
        ImGui::RadioButton("Unlimited##dl", &state.duration_limited, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Limited##dl", &state.duration_limited, 1);

        if (state.duration_limited == 1)
        {
            ImGui::Text("Days:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputInt("##dl_input", &state.duration_limited_amount, 0))
                state.duration_limited_amount = abs(state.duration_limited_amount);
        }

        ImGui::NewLine();
        // Duration Limit
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Ip Limit:");
        ImGui::SameLine();
        HelpMarker("How many devices can connect at same time?\n"
                   "Indeed, the user can connect multiple devices behind a NAT\n(wifi forexample) and all of them count as 1 device\n"
                   "because they have 1 ip.\n"
                   "Unit: (IP Count)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - TEXT_BASE_WIDTH * 22);
        ImGui::RadioButton("Unlimited##il", &state.ip_limited, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Limited##il", &state.ip_limited, 1);
        if (state.ip_limited == 1)
        {
            ImGui::AlignTextToFramePadding();

            ImGui::Text("Ips:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1);

            if (ImGui::InputInt("##ipl_input", &state.ip_limited_amount, 0))
                state.ip_limited_amount = abs(state.ip_limited_amount);
        }
        ImGui::AlignTextToFramePadding();

        ImGui::Text("Optional Notes:");
        ImGui::SameLine();
        HelpMarker("If you want to remember something about this user, write it here, its just a note pad and you can leave it empty.");
        ImGui::InputTextMultiline("##nup_notes", state.notes, IM_ARRAYSIZE(state.notes), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4));

        ImGui::Separator();
        // ImGui::SetItemDefaultFocus();

        if (ImGui::Button("OK", ImVec2(120, 0)) && 0 < strlen(state.name))
        {
            update_user(state, user.password.c_str(), user.enable);
            if (!has_error)
                ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        error_popupframe();

        ImGui::EndPopup();
    }
}
void delete_user_popupframe(User **_user)
{
    static User user;
    bool first_time = true;

    if (_user != nullptr && *_user != nullptr)
    {
        user = **_user;
        *_user = nullptr;
        first_time = false;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(0, 0));
    }

    bool show = true;
    if (ImGui::BeginPopupModal("Confirm##delete_user_popup", &show, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Delete user: %s?", user.name.c_str());

        ImGui::Separator();
        // ImGui::SetItemDefaultFocus();
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            delete_user(user);
            if (!has_error)
                ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        error_popupframe();

        ImGui::EndPopup();
    }
}
void error_popupframe()
{
    if (has_error)
        ImGui::OpenPopup("Error##nup");
    ImGui::SetNextWindowSize(ImVec2(450, -1));
    bool meaningless = true;
    if (ImGui::BeginPopupModal("Error##nup", &meaningless, ImGuiWindowFlags_NoSavedSettings))
    {

        has_error = false;
        ImGui::TextWrapped(last_error_msg.c_str());

        ImGui::EndPopup();
    }
}
void view_notes_popupframe(User **_user)
{
    static User user;

    if (_user != nullptr && *_user != nullptr)
    {
        user = **_user;
        *_user = nullptr;

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(600, -1));
    }

    bool show = true;
    if (ImGui::BeginPopupModal("Notes##show_notes_user_popup", &show, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::TextWrapped("User %s notes: \n %s", user.name.c_str(), user.note.c_str());

        ImGui::EndPopup();
    }
}
void show_user_configs(User **_user)
{
    static User user;

    int imgSize = 300;
    int minModulePixelSize = 3;
    static int my_image_width = 0;
    static int my_image_height = 0;
    static GLuint tcp_texture = 0;
    static GLuint ws_texture = 0;
    const int frame_pad = ImGui::GetStyle().FramePadding.y;
    if (_user != nullptr && *_user != nullptr)
    {
        user = **_user;
        *_user = nullptr;

        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(600, 0));

        // tcp:
        char qtextbuf[150];
        sprintf(qtextbuf, "trojan://%s@%s:%d?security=tls&type=tcp#%s", user.password.c_str(), ServerReportStore::last_report.panelsettings.domain.c_str(),
                ServerReportStore::last_report.panelsettings.mainport, user.name.c_str());
        std::string qrText = qtextbuf;
        std::string fileName = "qr-tcp.png";
        auto QrPng = QrToPng(fileName, imgSize, minModulePixelSize, qrText, true, qrcodegen::QrCode::Ecc::MEDIUM);
        if (QrPng.writeToPNG())
            std::cout << "Success!" << std::endl;
        else
            console.log("Failure in generating qr code!");
        bool ret = LoadTextureFromFile("qr-tcp.png", &tcp_texture, &my_image_width, &my_image_height);
        IM_ASSERT(ret);
        // ws:
        if (user.protocol == 1)
        {
            sprintf(qtextbuf, "trojan://%s@%s:%d?security=tls&type=ws&path=%s&#%s", user.password.c_str(), ServerReportStore::last_report.panelsettings.domain.c_str(),
                    ServerReportStore::last_report.panelsettings.mainport, ServerReportStore::last_report.panelsettings.websocket_path.c_str(), user.name.c_str());
            std::string qrText = qtextbuf;
            std::string fileName = "qr-ws.png";
            auto QrPng = QrToPng(fileName, imgSize, minModulePixelSize, qrText, true, qrcodegen::QrCode::Ecc::MEDIUM);
            if (QrPng.writeToPNG())
                std::cout << "Success!" << std::endl;
            else
                console.log("Failure in generating qr code!");
            bool ret = LoadTextureFromFile("qr-ws.png", &ws_texture, &my_image_width, &my_image_height);
            IM_ASSERT(ret);
        }
    }
    bool show = true;

    if (ImGui::BeginPopupModal("Configs##share_user_popup", &show, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings))
    {
        const int child_H = my_image_height + 16;
        ImGui::BeginChild("Child1", ImVec2(0, child_H + 2 * frame_pad), true);
        if (ImGui::BeginTable("table_sorting", 2, ImGuiTableFlags_BordersInnerV, ImVec2(0.0f, 0), 0.0f))
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGui::Image((void *)(intptr_t)tcp_texture, ImVec2(my_image_width, my_image_height));
            ImGui::TableNextColumn();
            ImGui::Text("Text 1");
            if(ImGui::Button("Download QR-Code")){
                  emscripten::val::global("window").call<void>(
                    "offerFileAsDownload",
                    string("filename.ext"),
                    string("mime/type")
                );
            }
            ImGui::Text("Text 3");
            ImGui::EndTable();
        }
        ImGui::EndChild();

        if (user.protocol != 1)
        {
            ImGui::EndPopup();
            return;
        }
        ImGui::BeginChild("Child2", ImVec2(0, child_H + 2 * frame_pad), true);
        if (ImGui::BeginTable("table_sorting", 2, ImGuiTableFlags_BordersInnerV, ImVec2(0.0f, 0), 0.0f))
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGui::Image((void *)(intptr_t)ws_texture, ImVec2(my_image_width, my_image_height));
            ImGui::TableNextColumn();
            ImGui::Text("Text 1");
            ImGui::Text("Text 2");
            ImGui::Text("Text 3");
            ImGui::EndTable();
        }
        ImGui::EndChild();

        ImGui::EndPopup();
        // std::cout << "Writing Example QR code 3 (huge) to " << fileName << " with text: '" << qrText << "', size: " << imgSize << "x" << imgSize << ", qr module pixel size: " << minModulePixelSize << ". " << std::endl;
    }
}