#include "main_window.h"

extern void server_status_frame();
extern void bash_view_frame();
extern void users_view_frame();
extern void panel_settings_frame();
extern bool live_data;
void MainWindow::onFrame()
{

    LeftSideBarWindow::on_tab_changed.connect(
        [this](int index)
        {
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
        panel_settings_frame();
        break;
    case 3:
        bash_view_frame();
    default:
        break;
    }
}