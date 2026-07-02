#define ZTUI_IMPLEMENTATION
#include "../ztui.h"

int main(void)
{
    char command_buffer[64] = "deploy --dry-run";
    char notes_buffer[160] = "Checklist:\n- verify nodes\n- review logs";
    ztui_text_state command_state;
    ztui_text_state notes_state;
    ztui_scroll_state notes_scroll;
    int running = 1;
    int autoscale = 1;
    float throttle = 0.30f;

    if (ztui_init() != ZTUI_OK)
        return 1;
    ztui_text_state_init(&command_state, command_buffer, (int)sizeof(command_buffer));
    ztui_text_state_init(&notes_state, notes_buffer, (int)sizeof(notes_buffer));
    ztui_scroll_state_init(&notes_scroll);

    while (running)
    {
        int w, h;
        ztui_event ev;
        ztui_get_size(&w, &h);
        ztui_frame_begin(ZTUI_BLACK);
        ztui_poll(&ev);

        if (ev.type == ZTUI_EV_ESCAPE || ev.key == 'q')
            running = 0;

        ztui_panel_begin((ztui_rect){(w - 46) / 2, (h - 16) / 2, 46, 16}, " Form Demo ",
                         ZTUI_BOX_DOUBLE, 1);

        /* Command row */
        {
            ztui_rect row = ztui_layout_next_h(1);
            ztui_layout_begin(row, ZTUI_LAYOUT_HORIZONTAL, 1);
            ztui_label(ztui_layout_next_wh(12, 1), "Command", ztui_get_theme()->muted);
            ztui_input(ztui_layout_next_fill(), ZTUI_AUTO_ID, &command_state, &ev,
                       ztui_get_theme()->field, ztui_get_theme()->field_focus);
            ztui_layout_end();
        }

        /* Throttle row */
        {
            ztui_rect row = ztui_layout_next_h(1);
            ztui_layout_begin(row, ZTUI_LAYOUT_HORIZONTAL, 1);
            ztui_label(ztui_layout_next_wh(12, 1), "Throttle", ztui_get_theme()->muted);
            ztui_slider(ztui_layout_next_fill(), ZTUI_AUTO_ID, &throttle, &ev,
                        ztui_get_theme()->text, ztui_get_theme()->field_focus);
            ztui_layout_end();
        }

        ztui_checkbox_next("Enable autoscale", &autoscale, &ev);
        ztui_label_muted("Notes");
        ztui_textarea(ztui_layout_next_h(6), ZTUI_AUTO_ID, &notes_state, &notes_scroll, &ev,
                      ztui_get_theme()->field, ztui_get_theme()->field_focus, 0);

        ztui_panel_end();
        ztui_frame_end();
    }

    ztui_cleanup();
    return 0;
}
