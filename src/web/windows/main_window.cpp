#include "main_window.h"

extern void server_status_frame();
extern void bash_view_frame();
extern void users_view_frame();
extern void panel_settings_frame(bool);
extern void tunnels_view_frame();
extern bool live_data;
static bool panel_settings_activated = false;
void MainWindow::onFrame()
{

    LeftSideBarWindow::on_tab_changed.connect(
        [this](int index)
        {   
            panel_settings_activated = index == 2;
            this->running_index = index;
            live_data = true;
        });
    switch (running_index)
    {
    case 0:
        server_status_frame();
        break;

    case 1:
        users_view_frame();
        break;
    case 2:
        panel_settings_frame(panel_settings_activated);
        panel_settings_activated = false;
        break;
    case 3:
        bash_view_frame();
            break;

    case 4:
        tunnels_view_frame();
            break;
    default:
        break;
    }
}