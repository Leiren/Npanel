#include "imgui.h"
#include "imgui_internal.h"
#include "server_report_store.h"
#include <cmath>
#include "log.h"
#include "colors.h"
#include "connection.h"
#include "customwidgets.h"
#include "imageloader.h"
#include "enc.h"
#include <stdio.h>
static bool re_fetch = false;

static bool has_error = false;
static bool loading = false;

static string error = "";
static int state = 0;

static SellerOptions selleroptions;
static BotUserOptions useroptions;

static void error_popupframe()
{
    if (has_error)
        ImGui::OpenPopup("Error##tbotpage");
    ImGui::SetNextWindowSize(ImVec2(600, -1));
    bool meaningless = true;
    if (ImGui::BeginPopupModal("Error##tbotpage", &meaningless, ImGuiWindowFlags_NoSavedSettings))
    {

        has_error = false;
        ImGui::TextWrapped("%s", error.c_str());

        ImGui::EndPopup();
    }
}

void not_activated_view()
{
    static char bkey_input[80] = {0};
    static char bot_name[80] = {0};

    if (state == 0)
    {
        const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("x").x;
        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        ImGui::Begin("Activate", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
        ImGui::Text("Telegram bot\n\n"
                    "In order to setup the bot, First step is to create your bot, give it a name and take the bot token.\n"
                    "For that open your telegram and create a bot using Bot-Father\n"
                    "And Then you can bring your bot-token here to connect the bot.");
        static int my_image_width = 0;
        static int my_image_height = 0;
        static GLuint my_image_texture = 0;
        static bool ret = LoadTextureFromFile("resources/bf.png", &my_image_texture, &my_image_width, &my_image_height);
        IM_ASSERT(ret);
        ImGui::SameLine();
        ImGui::Image((void *)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
        ImGui::NewLine();
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x / 2 - TEXT_BASE_WIDTH * 22);

        if (ImGui::SmallButton("link to BotFather"))
        {
            EM_ASM(window.open("https://t.me/BotFather", '_blank').focus(););
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("simple youtube tutorial"))
        {
            EM_ASM(window.open("https://www.youtube.com/watch?v=aNmRNjME6mE", '_blank').focus(););
        }

        ImGui::Text("\n");
        ImGui::Text("Note that telegram bot should not be connected to 2 servers as same time, disconnect it from old server first, then connect it to new server.");
        ImGui::Text("\n");
        ImGui::Text("\n");
        ImGui::Text("Bot features:");
        ImGui::BulletText("Users can check their stats and take their configs with latest domain that this server is runnig on.");
        ImGui::BulletText("You can change server domain and re-send updated configs to users.");
        ImGui::BulletText("You can send message to single or all users from panel.");
        ImGui::BulletText("You can receive admin notifications for the topics like server reboot , panel login , user expiration , etc.. ");
        // ImGui::BulletText("You add/delete users directly from the bot.");

        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 120 - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("Next", ImVec2(120, 0)))
        {
            state = 1;
        }
        ImGui::End();
    }
    if (state == 1)
    {
        ImGui::Begin("Key", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

        ImGui::Text("HTTP Token");
        ImGui::InputText("##input_bkey", bkey_input, IM_ARRAYSIZE(bkey_input));
        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine();
        if (loading)
            Spinner("loading", 8, 2, IM_COL32(66, 150, 250, 255));
        if (has_error)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, Color::red);
            ImGui::TextUnformatted(error.c_str());
            ImGui::PopStyleColor();
        }

        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240 - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("Back", ImVec2(120, 0)))
            state--;
        ImGui::SameLine();
        if (ImGui::Button("Next", ImVec2(120, 0)) && !loading)
        {
            loading = true;
            has_error = false;
            Connection::send("bot-activate-key", 1, bkey_input)->connect([](Result res)
                                                                         {
                loading = false;
                if(res.success)
                {
                    state =2;
                    strcpy(bot_name,res.info);
                }
                else{
                    has_error = true;
                    error = res.info;
                } });
        }
        ImGui::End();
    }
    if (state == 2)
    {
        ImGui::Begin(bot_name, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
        ImGui::Text("This link allows the admin to access the bot.");
        ImGui::SameLine();
        if (ImGui::Button("copy", ImVec2(120, 0)))
        {
            char admin_link[120] = {0};
            sprintf(admin_link, "https://telegram.me/%s?start=%s", bot_name,  ServerReportStore::last_report.panelsettings.admin_password.c_str());
            ImGui::SetClipboardText((const char *)admin_link);
        }
        ImGui::NewLine();
        ImGui::Text(
            "Now, you can use the users tab to share each user telegram bot link.\n\n"
            "Once you do this, you will never need to do it again even if you change your server ip or even domain\n "
            "because you can resend the updated configs to all users."

        );

        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 120 - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("Done", ImVec2(120, 0)))
        {
            state = 0;
        }
        ImGui::End();
    }
}

enum AdminTable
{
    AdminTable_Name,
    AdminTable_Enable,
    AdminTable_Notify

};
enum UserTable
{
    UserTable_Name,
    UserTable_Enable,

};

const char *true_val = "1";
const char *false_val = "0";
static const char *strval(bool x)
{
    return x ? true_val : false_val;
}
static void send_newoptions()
{
    Connection::send("bot-update-settings", 35,
                     strval(selleroptions.notif_panel_login_fail.enable),
                     strval(selleroptions.notif_panel_login_fail.notify),
                     strval(selleroptions.notif_panel_login_success.enable),
                     strval(selleroptions.notif_panel_login_success.notify),
                     strval(selleroptions.notif_panel_information_changed.enable),
                     strval(selleroptions.notif_panel_information_changed.notify),
                     strval(selleroptions.notif_panel_cpu_usage_high.enable),
                     strval(selleroptions.notif_panel_cpu_usage_high.notify),
                     strval(selleroptions.notif_panel_mem_usage_high.enable),
                     strval(selleroptions.notif_panel_mem_usage_high.notify),
                     strval(selleroptions.notif_panel_server_reboot.enable),
                     strval(selleroptions.notif_panel_server_reboot.notify),
                     strval(selleroptions.notif_panel_start.enable),
                     strval(selleroptions.notif_panel_start.notify),
                     strval(selleroptions.notif_user_reach_duration_limit.enable),
                     strval(selleroptions.notif_user_reach_duration_limit.notify),
                     strval(selleroptions.notif_user_reach_traffic_limit.enable),
                     strval(selleroptions.notif_user_reach_traffic_limit.notify),
                     strval(selleroptions.notif_user_added.enable),
                     strval(selleroptions.notif_user_added.notify),
                     strval(selleroptions.notif_user_disabled.enable),
                     strval(selleroptions.notif_user_disabled.notify),
                     strval(selleroptions.notif_user_enable.enable),
                     strval(selleroptions.notif_user_enable.notify),
                     strval(selleroptions.notif_user_removed.enable),
                     strval(selleroptions.notif_user_removed.notify),
                     strval(selleroptions.notif_user_support.enable),
                     strval(selleroptions.notif_user_support.notify),
                     strval(useroptions.can_ask_info),
                     strval(useroptions.info_include_traffic_used),
                     strval(useroptions.info_include_ip_limit),
                     strval(useroptions.info_include_speed_limit),
                     strval(useroptions.info_include_traffic_limit),
                     strval(useroptions.info_include_days_left),
                     strval(useroptions.info_include_user_note)

                         )
        ->connect([](Result res)
                  {
                      if (res.success)
                      {
                          console.log("Bot new settings sent.");
                          has_error = false;
                      }
                      else
                      {
                          console.log("Bot new settings Failed. (so the settings aren't applied) info:");
                          console.log("%s",res.info);
                          error = res.info;
                          has_error = true;
                      } });
}

static int admin_row_i = 0;
static void admin_row(const char *name, EandN *en)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::PushID(admin_row_i++);
    ImGui::TextUnformatted(name);
    ImGui::TableNextColumn();
    bool o_enable = en->enable;
    bool o_notify = en->notify;
    ToggleButton("enable", &(en->enable));
    ImGui::TableNextColumn();
    ToggleButton("notify", &(en->notify));
    if (o_enable != en->enable || o_notify != en->notify)
    {
        send_newoptions();
    }

    ImGui::PopID();
}

static int user_row_i = 0;
static void user_row(const char *name, bool *en)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::PushID(user_row_i++);
    ImGui::TextUnformatted(name);
    ImGui::TableNextColumn();
    bool o_enable = *en;

    ToggleButton("enable", en);
    if (o_enable != *en)
    {
        send_newoptions();
    }

    ImGui::PopID();
}

void activated_view(bool tab_changed)
{

    // if (tab_changed || re_fetch)
    // {
    selleroptions = ServerReportStore::last_report.selleroptions;
    useroptions = ServerReportStore::last_report.useroptions;
    // }
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("x").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    static ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ScrollY;

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

    ImGui::TextUnformatted("Admin Options");
    ImGui::SameLine();
    if (ImGui::Button("copy access link", ImVec2(280, 0)))
    {
        Connection::send("bot-name", 0)->connect([&](Result res)
                                                 {
                loading = false;
                if(res.success)
                {
                       char admin_link[120] = {0};
                    sprintf(admin_link, "https://telegram.me/%s?start=%s", res.info, ServerReportStore::last_report.panelsettings.admin_password.c_str());
                    ImGui::SetClipboardText((const char *)admin_link);
                }
                else{
                    has_error = true;
                    error = res.info;
                } });
    }
    ImGui::SameLine();
    static bool dc_btn_check = false;
    if (ImGui::Button("disconnect bot", ImVec2(160, 0)))
    {
        if (dc_btn_check == false)
            dc_btn_check = true;
        else
            Connection::send("bot-disconnect", 0)->connect([&](Result res)
                                                           {
                loading = false;
                if(res.success)
                {
                    EM_ASM(window.location.reload(););
                    
                }
                else{
                    has_error = true;
                    error = res.info;
                } });
    }
    ImGui::Separator();
    if (ImGui::BeginTable("tg_admin_options", 3, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 25), 0.0f))
    {

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.0f, AdminTable_Name);
        ImGui::TableSetupColumn("Enable", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 6, AdminTable_Enable);
        ImGui::TableSetupColumn("Notification Sound", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18, AdminTable_Notify);

        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();
        admin_row("Notify on panel login fail (incorrect password)", &selleroptions.notif_panel_login_fail);
        admin_row("Notify on panel login succeed", &selleroptions.notif_panel_login_success);
        admin_row("Notify on panel settings changed (domain,port,etc..)", &selleroptions.notif_panel_information_changed);
        admin_row("Notify on high cpu usage", &selleroptions.notif_panel_cpu_usage_high);
        admin_row("Notify on high memory usage", &selleroptions.notif_panel_mem_usage_high);
        admin_row("Notify on server reboot", &selleroptions.notif_panel_server_reboot);
        admin_row("Notify on panel start", &selleroptions.notif_panel_start);
        admin_row("Notify when a user expires (days-left = 0) and becomes disabled", &selleroptions.notif_user_reach_duration_limit);
        admin_row("Notify when a user reaches traffic limit and becomes disabled", &selleroptions.notif_user_reach_traffic_limit);
        admin_row("Notify on a new user created", &selleroptions.notif_user_added);
        admin_row("Notify on a user becomes disabled for any reason", &selleroptions.notif_user_disabled);
        admin_row("Notify on a user becomes enabled for any reason", &selleroptions.notif_user_enable);
        admin_row("Notify on a user gets deleted", &selleroptions.notif_user_removed);
        admin_row("Notify when a user asks for support", &selleroptions.notif_user_support);

        ImGui::EndTable();
    }
    ImGui::NewLine();

    ImGui::TextUnformatted("Users Options");
    ImGui::Separator();
    if (ImGui::BeginTable("tg_users_options", 2, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 13), 0.0f))
    {

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.0f, UserTable_Name);
        ImGui::TableSetupColumn("Enable", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 6, UserTable_Enable);

        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        user_row("Users can ask for any information (rules all below options)", &useroptions.can_ask_info);
        user_row("Users can see their traffic usage", &useroptions.info_include_traffic_used);
        user_row("Users can see their ip limit amount", &useroptions.info_include_ip_limit);
        user_row("Users can see their speed limit", &useroptions.info_include_speed_limit);
        user_row("Users can see their maximum allowed traffic", &useroptions.info_include_traffic_limit);
        user_row("Users can see their expiration days-left", &useroptions.info_include_days_left);
        user_row("Users can see their notes", &useroptions.info_include_user_note);

        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
}

void telegram_bot_view_frame(bool tab_changed)
{

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(ImGui::GetStyle().CellPadding.x, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 5));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    bool is_bot_activated = ServerReportStore::last_report.panelsettings.telegram_bot_key != "";
    if (is_bot_activated && state != 2)
    {
        admin_row_i = 0;
        user_row_i = 0;
        activated_view(tab_changed);
    }
    else
    {
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.75f));

        not_activated_view();
    }
    ImGui::PopStyleVar(3);

    // error_popupframe();
}