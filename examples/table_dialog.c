#define ZTUI_IMPLEMENTATION
#include "../ztui.h"

int main(void)
{
    static const char* headers[] = {"Service", "State", "CPU"};
    static const ztui_column columns[] = {
        {16, ZTUI_TEXT_LEFT}, {10, ZTUI_TEXT_CENTER}, {6, ZTUI_TEXT_RIGHT}};
    static const char* cells[] = {
        "api-gateway", "healthy",  "18%", "worker-east", "healthy", "35%",
        "worker-west", "degraded", "79%", "billing",     "healthy", "24%",
        "sessions",    "healthy",  "13%", "scheduler",   "paused",  "03%"};

    ztui_scroll_state table_scroll;
    int running = 1;
    int selected = 0;
    int confirm = 0;

    if (ztui_init() != ZTUI_OK)
        return 1;
    ztui_scroll_state_init(&table_scroll);

    while (running)
    {
        int w, h;
        ztui_event ev;
        ztui_get_size(&w, &h);
        ztui_frame_begin(ZTUI_BLACK);
        ztui_poll(&ev);

        if (!confirm && (ev.type == ZTUI_EV_ESCAPE || ev.key == 'q'))
            running = 0;

        ztui_panel_begin((ztui_rect){(w - 42) / 2, (h - 14) / 2, 42, 14}, " Table + Dialog ",
                         ZTUI_BOX_DOUBLE, 1);
        ztui_label_muted("Press ENTER on a row, then confirm.");

        if (ztui_table(ztui_layout_next_h(8), ZTUI_AUTO_ID, headers, columns, 3, cells, 6,
                       &selected, &table_scroll, &ev, ztui_get_theme()->muted,
                       ztui_get_theme()->text, ztui_get_theme()->selection,
                       ztui_get_theme()->field_focus))
        {
            confirm = 1;
        }

        ztui_panel_end();

        if (confirm)
        {
            int result = ztui_dialog_confirm(" Confirm ", "Restart selected service?",
                                             "[ Restart ]", "[ Cancel ]", &ev);
            if (result == 1 || result == -1)
                confirm = 0;
        }

        ztui_frame_end();
    }

    ztui_cleanup();
    return 0;
}
