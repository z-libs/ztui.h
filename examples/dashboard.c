#define ZTUI_IMPLEMENTATION
#include "../ztui.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int read_proc_int(const char* path)
{
    FILE* f = fopen(path, "r");
    if (!f)
        return 0;
    char buf[256];
    if (!fgets(buf, sizeof(buf), f))
    {
        fclose(f);
        return 0;
    }
    fclose(f);
    int val = 0;
    sscanf(buf, "%d", &val);
    return val;
}

static void read_cpu(float* user, float* nice, float* system, float* idle)
{
    FILE* f = fopen("/proc/stat", "r");
    if (!f)
        return;
    char buf[256];
    if (!fgets(buf, sizeof(buf), f))
    {
        fclose(f);
        return;
    }
    fclose(f);
    long long u, n, s, i;
    sscanf(buf, "cpu %lld %lld %lld %lld", &u, &n, &s, &i);
    *user = (float)u;
    *nice = (float)n;
    *system = (float)s;
    *idle = (float)i;
}

#if 0
static int read_proc_count(void) {
    FILE* f = popen("ps -eo pid,comm,%cpu,%mem --no-headers | head -20", "r");
    if (!f) return 0;
    int count = 0;
    char buf[256];
    while (fgets(buf, sizeof(buf), f)) count++;
    pclose(f);
    return count;
}
#endif

static int read_procs(char* lines, int max_lines, int line_sz)
{
    FILE* f = popen("ps -eo pid,comm,%cpu,%mem --no-headers | head -20", "r");
    if (!f)
        return 0;
    int n = 0;
    while (n < max_lines && fgets(lines + n * line_sz, line_sz, f))
    {
        int len = (int)strlen(lines + n * line_sz);
        while (len > 0 && lines[n * line_sz + len - 1] == '\n')
            lines[n * line_sz + --len] = '\0';
        n++;
    }
    pclose(f);
    return n;
}

int main(void)
{
    ztui_split split = {40, 10, 10, 0, -1};
    ztui_tab_bar tabs = {0, 3, -1};
    ztui_status status = {"Dashboard ready. Press Q to quit.", -1};
    int running = 1;

    if (ztui_init() != ZTUI_OK)
        return 1;

    float cpu_u = 0, cpu_n = 0, cpu_s = 0, cpu_i = 0;
    float cpu_u_prev = 0, cpu_n_prev = 0, cpu_s_prev = 0, cpu_i_prev = 0;
    int mem_total = 0, mem_free = 0;
    int frame = 0;

    while (running)
    {
        const ztui_theme* th = ztui_get_theme();
        ztui_event ev;
        int w, h;
        ztui_get_size(&w, &h);
        ztui_frame_begin(ZTUI_BLACK);

        /* poll with 500ms timeout — enables live updates */
        ztui_poll_timeout(&ev, 500);

        if (ev.key == 'q' || ev.type == ZTUI_EV_ESCAPE)
            running = 0;

        /* refresh data every 2 frames (~1 second) */
        if (frame % 2 == 0)
        {
            cpu_u_prev = cpu_u;
            cpu_n_prev = cpu_n;
            cpu_s_prev = cpu_s;
            cpu_i_prev = cpu_i;
            read_cpu(&cpu_u, &cpu_n, &cpu_s, &cpu_i);
            mem_total = read_proc_int("/proc/meminfo");
            mem_free = read_proc_int("/proc/meminfo");
        }
        frame++;

        float total_prev = cpu_u_prev + cpu_n_prev + cpu_s_prev + cpu_i_prev;
        float total = cpu_u + cpu_n + cpu_s + cpu_i;
        float cpu_pct = total_prev > 0 ? ((total - total_prev) / total_prev) * 100 : 0;
        float mem_pct = mem_total > 0 ? (float)(mem_total - mem_free) / mem_total : 0;
        if (cpu_pct < 0)
            cpu_pct = 0;
        if (cpu_pct > 100)
            cpu_pct = 100;

        /* split: sidebar + main area */
        ztui_rect left, right;
        ztui_split_draw((ztui_rect){0, 0, w, h - 2}, &split, &ev, &left, &right);

        /* ── left: CPU + memory bars ── */
        ztui_panel_begin(left, " System ", ZTUI_BOX_DOUBLE, 1);
        ZTUI_LABEL("CPU Usage");
        ztui_progress_next(cpu_pct / 100.0f);
        {
            char cpu_text[32];
            snprintf(cpu_text, sizeof(cpu_text), "  %.1f%% user", cpu_pct);
            ztui_label_next(cpu_text);
        }
        ZTUI_LABEL("Memory");
        ztui_progress_next(mem_pct);
        {
            char mem_text[32];
            snprintf(mem_text, sizeof(mem_text), "  %d%% used", (int)(mem_pct * 100));
            ztui_label_next(mem_text);
        }
        ztui_panel_end();

        /* ── right: tabbed content ── */
        ztui_rect content = {right.x + 1, right.y + 1, right.w - 2, right.h - 2};
        ztui_draw_box((ztui_rect){right.x, right.y, right.w, right.h}, ZTUI_BOX_SINGLE, th->border);
        ztui_fill_rect((ztui_rect){right.x + 1, right.y + 1, right.w - 2, right.h - 2}, " ",
                       th->panel);
        ztui_tab_bar_draw((ztui_rect){content.x, content.y, content.w, 1}, &tabs, &ev,
                          (const char* const[]){" CPU ", " Memory ", " Processes "}, 3, th->muted,
                          th->accent);

        ztui_rect tc = {content.x, content.y + 1, content.w, content.h - 1};

        if (tabs.active == 0)
        {
            /* CPU tab */
            ztui_anim cpu_a;
            ztui_anim_init(&cpu_a, cpu_pct / 100.0f);
            ztui_progress_anim(tc, ZTUI_AUTO_ID, &cpu_a, th->accent, th->muted, th->field_focus);
            char info[128];
            snprintf(info, sizeof(info), "CPU: %.1f%%  (idle: %.0f)", cpu_pct, cpu_i - cpu_i_prev);
            ztui_label((ztui_rect){tc.x, tc.y + 1, tc.w, 1}, info, th->text);
        }
        else if (tabs.active == 1)
        {
            /* Memory tab */
            ztui_progress(tc, ZTUI_AUTO_ID, mem_pct, th->accent, th->muted, th->field_focus);
            char info[128];
            snprintf(info, sizeof(info), "Memory: %d%%  (%d MB / %d MB)", (int)(mem_pct * 100),
                     (mem_total - mem_free) / 1024, mem_total / 1024);
            ztui_label((ztui_rect){tc.x, tc.y + 1, tc.w, 1}, info, th->text);
        }
        else
        {
            /* Processes tab */
            static char procs[20][128];
            static const char* proc_ptrs[20];
            static ztui_scroll_state proc_scroll;
            static int proc_init = 0;
            if (!proc_init)
            {
                ztui_scroll_state_init(&proc_scroll);
                proc_init = 1;
            }

            int np = read_procs((char*)procs, 20, 128);
            for (int i = 0; i < np; i++)
                proc_ptrs[i] = procs[i];
            ztui_list(tc, ZTUI_AUTO_ID, proc_ptrs, np, &(int){0}, &proc_scroll, &ev, th->text,
                      th->selection, th->field_focus);
        }

        ztui_status_draw(&status, (ztui_rect){0, h - 2, w, 2}, th->muted);
        ztui_frame_end();
    }

    ztui_cleanup();
    return 0;
}
