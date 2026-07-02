#define ZTUI_IMPLEMENTATION
#include "../ztui.h"
#include <stdio.h>
#include <string.h>

static char filebuf[65536];
static char filename[256] = "";
static int dirty = 0;

static void editor_load(const char* path)
{
    strncpy(filename, path, sizeof(filename) - 1);
    FILE* f = fopen(path, "r");
    if (!f)
    {
        filebuf[0] = '\0';
        return;
    }
    size_t n = fread(filebuf, 1, sizeof(filebuf) - 1, f);
    filebuf[n] = '\0';
    fclose(f);
    dirty = 0;
}

static void editor_save(void)
{
    if (!filename[0])
        return;
    FILE* f = fopen(filename, "w");
    if (!f)
        return;
    fwrite(filebuf, 1, strlen(filebuf), f);
    fclose(f);
    dirty = 0;
}

int main(int argc, char** argv)
{
    ztui_text_state text;
    ztui_scroll_state scroll;
    ztui_status status = {"Ctrl+S save | Ctrl+Q quit | Ctrl+Z undo | Ctrl+Y redo", -1};

    if (argc > 1)
        editor_load(argv[1]);
    else
        snprintf(filename, sizeof(filename), "untitled.txt");

    ztui_text_state_init(&text, filebuf, (int)sizeof(filebuf));
    ztui_scroll_state_init(&scroll);

    if (ztui_init() != ZTUI_OK)
        return 1;

    int running = 1;
    while (running)
    {
        const ztui_theme* th = ztui_get_theme();
        ztui_event ev;
        int w, h;
        ztui_get_size(&w, &h);
        ztui_frame_begin(ZTUI_BLACK);
        ztui_poll(&ev);

        /* keyboard shortcuts */
        if (ev.type == ZTUI_EV_KEY)
        {
            if (ev.key == 19)
            { /* Ctrl+S */
                editor_save();
                continue;
            }
            if (ev.key == 17)
            { /* Ctrl+Q */
                running = 0;
                continue;
            }
        }

        /* editor area */
        ztui_rect editor_area = {0, 0, w, h - 2};
        ztui_panel_begin(editor_area, filename, ZTUI_BOX_SINGLE, 0);

        if (ztui_textarea((ztui_rect){_ly.area.x, _ly.area.y, _ly.area.w, _ly.area.h}, ZTUI_AUTO_ID,
                          &text, &scroll, &ev, th->field, th->field_focus,
                          ZTUI_TEXTAREA_LINE_NUMBERS | ZTUI_TEXTAREA_NO_UNDO))
            dirty = 1;

        ztui_panel_end();

        /* status bar */
        {
            int cl = text.cursor, lines = 1;
            for (int ci = 0; ci < cl && ci < text.length; ci++)
                if (text.buffer[ci] == '\n')
                    lines++;
            char status_msg[384];
            snprintf(status_msg, sizeof(status_msg), "%s%s  line %d  %s", filename,
                     dirty ? " [modified]" : "", lines, "Ctrl+S save | Ctrl+Q quit");
            ztui_status_set(&status, status_msg, -1);
            ztui_status_draw(&status, (ztui_rect){0, h - 2, w, 2}, th->muted);
        }

        ztui_frame_end();
    }

    ztui_cleanup();
    return 0;
}
