#define ZTUI_IMPLEMENTATION
#include "../ztui.h"

int main(void)
{
    int running = 1;
    float progress = 0.25f;

    if (ztui_init() != ZTUI_OK)
        return 1;

    while (running)
    {
        int w, h;
        ztui_event ev;
        ztui_get_size(&w, &h);
        ztui_frame_begin(ZTUI_BLACK);
        ztui_poll(&ev);

        if (ev.type == ZTUI_EV_ESCAPE || ev.key == 'q')
            running = 0;
        if (ev.type == ZTUI_EV_LEFT && progress > 0.0f)
            progress -= 0.05f;
        if (ev.type == ZTUI_EV_RIGHT && progress < 1.0f)
            progress += 0.05f;
        if (progress < 0.0f)
            progress = 0.0f;
        if (progress > 1.0f)
            progress = 1.0f;

        ztui_panel_begin((ztui_rect){(w - 34) / 2, (h - 8) / 2, 34, 8}, " Minimal ",
                         ZTUI_BOX_DOUBLE, 1);
        ztui_label_next("Press LEFT/RIGHT to adjust progress.");
        ztui_label_muted("Press Q or ESC to quit.");
        ztui_progress_next(progress);
        ztui_panel_end();

        ztui_frame_end();
    }

    ztui_cleanup();
    return 0;
}
