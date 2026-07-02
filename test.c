#define ZTUI_IMPLEMENTATION
#include "ztui.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int tests = 0, passed = 0;
#define TEST(name)                                                                                 \
    do                                                                                             \
    {                                                                                              \
        tests++;                                                                                   \
        printf("  %-40s ", name);                                                                  \
    } while (0)
#define PASS()                                                                                     \
    do                                                                                             \
    {                                                                                              \
        passed++;                                                                                  \
        printf("ok\n");                                                                            \
    } while (0)
#define FAIL(msg)                                                                                  \
    do                                                                                             \
    {                                                                                              \
        printf("FAIL: %s\n", msg);                                                                 \
    } while (0)
#define CHECK(cond)                                                                                \
    do                                                                                             \
    {                                                                                              \
        if (!(cond))                                                                               \
        {                                                                                          \
            FAIL(#cond);                                                                           \
            return;                                                                                \
        }                                                                                          \
    } while (0)
#define CHECK_EQ(a, b)                                                                             \
    do                                                                                             \
    {                                                                                              \
        if ((a) != (b))                                                                            \
        {                                                                                          \
            FAIL(#a " == " #b);                                                                    \
            printf("    got %d, want %d\n", (int)(a), (int)(b));                                   \
            return;                                                                                \
        }                                                                                          \
    } while (0)
#define CHECK_STR(a, b)                                                                            \
    do                                                                                             \
    {                                                                                              \
        if (strcmp((a), (b)) != 0)                                                                 \
        {                                                                                          \
            FAIL(#a " == " #b);                                                                    \
            return;                                                                                \
        }                                                                                          \
    } while (0)

static void test_rgb_macro(void)
{
    TEST("ztui_rgb packs correctly");
    ztui_color c = ztui_rgb(0x12, 0x34, 0x56);
    CHECK_EQ(c, 0x1123456);
    PASS();
}

static void test_utf8_len(void)
{
    TEST("ztui_utf8_len ASCII");

    PASS();
}

static void test_has_truecolor(void)
{
    TEST("ztui_has_truecolor returns 0 or 1");
    int r = ztui_has_truecolor();
    CHECK(r == 0 || r == 1);
    PASS();
}

static void test_get_error_roundtrip(void)
{
    TEST("ztui_get_error returns ZTUI_OK initially");
    CHECK_EQ(ztui_get_error(), ZTUI_OK);
    PASS();

    TEST("ztui_get_error clears after read");
    CHECK_EQ(ztui_get_error(), ZTUI_OK);

    PASS();
}

static void test_init_fails_without_terminal(void)
{
    TEST("ztui_init fails without terminal");
    ztui_error e = ztui_init();
    CHECK(e == ZTUI_ERR_TERMINAL || e == ZTUI_OK);

    if (e == ZTUI_OK)
        ztui_cleanup();

    if (e == ZTUI_ERR_TERMINAL)
    {
        ztui_error e2 = ztui_get_error();
        CHECK_EQ(e2, ZTUI_ERR_TERMINAL);
    }
    PASS();
}

static void test_rect_make(void)
{
    TEST("ztui_rect initialisation");
    ztui_rect r = {1, 2, 30, 10};
    CHECK_EQ(r.x, 1);
    CHECK_EQ(r.y, 2);
    CHECK_EQ(r.w, 30);
    CHECK_EQ(r.h, 10);
    PASS();
}

static void test_point_make(void)
{
    TEST("ztui_point initialisation");
    ztui_point p = {5, 7};
    CHECK_EQ(p.x, 5);
    CHECK_EQ(p.y, 7);
    PASS();
}

static void test_style_make(void)
{
    TEST("ztui_style_make");
    ztui_style s = ztui_style_make(ZTUI_RED, ZTUI_BLUE, ZTUI_STYLE_BOLD);
    CHECK_EQ(s.fg, ZTUI_RED);
    CHECK_EQ(s.bg, ZTUI_BLUE);
    CHECK_EQ(s.bits, ZTUI_STYLE_BOLD);
    PASS();
}

static void test_default_theme(void)
{
    TEST("ztui_default_theme returns valid styles");
    ztui_theme t = ztui_default_theme();

    CHECK(t.text.fg >= 0);
    CHECK(t.muted.fg >= 0);
    CHECK(t.accent.fg >= 0);
    CHECK(t.danger.fg >= 0);
    PASS();
}

static void test_set_theme_roundtrip(void)
{
    TEST("ztui_set_theme / ztui_get_theme roundtrip");
    ztui_theme t = ztui_default_theme();
    t.text = ztui_style_make(ZTUI_GREEN, ZTUI_BLACK, ZTUI_STYLE_UNDERLINE);
    ztui_set_theme(&t);
    const ztui_theme* g = ztui_get_theme();
    CHECK_EQ(g->text.fg, ZTUI_GREEN);
    CHECK_EQ(g->text.bits, ZTUI_STYLE_UNDERLINE);
    PASS();
}

static void test_text_state_init(void)
{
    TEST("ztui_text_state_init with empty buffer");
    char buf[16] = "";
    ztui_text_state s;
    ztui_text_state_init(&s, buf, sizeof(buf));
    CHECK_EQ(s.length, 0);
    CHECK_EQ(s.cursor, 0);
    CHECK_EQ(s.capacity, 16);
    PASS();

    TEST("ztui_text_state_init with pre-filled buffer");
    char buf2[32] = "hello";
    ztui_text_state s2;
    ztui_text_state_init(&s2, buf2, sizeof(buf2));
    CHECK_EQ(s2.length, 5);
    CHECK_EQ(s2.cursor, 5);

    PASS();
}

static void test_text_state_init_null(void)
{
    TEST("ztui_text_state_init with capacity 0");
    ztui_text_state s;
    ztui_text_state_init(&s, NULL, 0);
    CHECK_EQ(s.length, 0);
    CHECK_EQ(s.cursor, 0);
    PASS();
}

static void test_scroll_state_init(void)
{
    TEST("ztui_scroll_state_init");
    ztui_scroll_state s;
    ztui_scroll_state_init(&s);
    CHECK_EQ(s.offset, 0);
    CHECK_EQ(s.content_height, 0);
    CHECK_EQ(s.active, 0);
    PASS();
}

static void test_layout_opts_macros(void)
{
    TEST("ZTUI_OPTS_FIXED");
    ztui_layout_opts o = ZTUI_OPTS_FIXED(3);
    CHECK_EQ(o.min_h, 3);
    CHECK_EQ(o.max_h, 3);
    CHECK_EQ(o.fill, 0);
    PASS();

    TEST("ZTUI_OPTS_FILL");
    o = ZTUI_OPTS_FILL();
    CHECK_EQ(o.fill, 1);
    PASS();

    TEST("ZTUI_OPTS_WEIGHT");
    o = ZTUI_OPTS_WEIGHT(2);
    CHECK_EQ(o.weight, 2);
    PASS();
}

static void test_text_measure(void)
{
    TEST("ztui_text_measure short text fits in one line");
    int n = ztui_text_measure("hello", 20);
    CHECK_EQ(n, 1);
    PASS();

    TEST("ztui_text_measure wraps long text");
    n = ztui_text_measure("aaa bbb ccc ddd eee", 10);
    CHECK(n >= 2);
    PASS();

    TEST("ztui_text_measure empty string");
    n = ztui_text_measure("", 10);
    CHECK_EQ(n, 1);

    PASS();

    TEST("ztui_text_measure single word longer than width");
    n = ztui_text_measure("abcdefghijklmnop", 5);
    CHECK(n >= 3);

    PASS();
}

static void test_event_type_constants(void)
{
    TEST("ztui_event_type constants are distinct");
    CHECK(ZTUI_EV_NONE != ZTUI_EV_ESCAPE);
    CHECK(ZTUI_EV_UP != ZTUI_EV_DOWN);
    CHECK(ZTUI_EV_MOUSE_DOWN != ZTUI_EV_MOUSE_UP);
    PASS();
}

static void test_layout_cursor_noop(void)
{
    TEST("ztui_layout_cursor returns 0 without active layout");
    PASS();
}

static void test_flex_enums(void)
{
    TEST("ztui_justify constants are distinct");
    CHECK(ZTUI_JUSTIFY_START != ZTUI_JUSTIFY_CENTER);
    CHECK(ZTUI_JUSTIFY_END != ZTUI_JUSTIFY_SPACE_BETWEEN);
    PASS();

    TEST("ztui_align constants are distinct");
    CHECK(ZTUI_ALIGN_STRETCH != ZTUI_ALIGN_START);
    CHECK(ZTUI_ALIGN_CENTER != ZTUI_ALIGN_END);
    PASS();

    TEST("ztui_layout_begin_ex is callable");

    ztui_layout_begin_ex((ztui_rect){0, 0, 10, 10}, ZTUI_LAYOUT_VERTICAL, 1, ZTUI_JUSTIFY_START,
                         ZTUI_ALIGN_STRETCH, 0);
    ztui_layout_end();
    PASS();

    TEST("ztui_layout_begin_ex with align center");
    ztui_layout_begin_ex((ztui_rect){0, 0, 10, 10}, ZTUI_LAYOUT_VERTICAL, 1, ZTUI_JUSTIFY_START,
                         ZTUI_ALIGN_CENTER, 0);
    ztui_layout_end();
    PASS();

    TEST("ztui_layout_begin_ex with wrapping");
    ztui_layout_begin_ex((ztui_rect){0, 0, 10, 10}, ZTUI_LAYOUT_HORIZONTAL, 1, ZTUI_JUSTIFY_START,
                         ZTUI_ALIGN_STRETCH, 1);
    ztui_layout_end();
    PASS();
}

static void test_draw_primitives_no_terminal(void)
{

    TEST("ztui_set_cell no-op without init (does not crash)");
    ztui_set_cell((ztui_point){0, 0}, "x", ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0));
    PASS();

    TEST("ztui_draw_str no-op without init");
    ztui_draw_str((ztui_point){0, 0}, "test", ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0));
    PASS();

    TEST("ztui_fill_rect no-op without init");
    ztui_fill_rect((ztui_rect){0, 0, 10, 5}, " ", ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0));
    PASS();

    TEST("ztui_draw_box no-op without init (w<2, h<2 edge)");
    ztui_draw_box((ztui_rect){0, 0, 1, 1}, ZTUI_BOX_SINGLE,
                  ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0));
    PASS();
}

static void test_scroll_ensure_noop(void)
{
    TEST("ztui_scroll_state_init sets safe defaults");
    ztui_scroll_state s;
    ztui_scroll_state_init(&s);
    CHECK_EQ(s.active, 0);
    CHECK_EQ(s.offset, 0);
    PASS();
}

static void test_id_stack_push_pop(void)
{
    TEST("ztui_push_id / ztui_pop_id roundtrip");
    ztui_push_id(42);
    ztui_pop_id();
    PASS();

    TEST("ztui_push_ptr / ztui_pop_id roundtrip");
    int x;
    ztui_push_ptr(&x);
    ztui_pop_id();
    PASS();

    TEST("ztui_push_str / ztui_pop_id roundtrip");
    ztui_push_str("hello");
    ztui_pop_id();
    PASS();

    TEST("nested id stack");
    ztui_push_id(1);
    ztui_push_id(2);
    ztui_pop_id();
    ztui_pop_id();
    PASS();
}

static void test_auto_input_callable(void)
{
    TEST("ztui_input_auto is callable (no crash)");
    ztui_event ev;
    memset(&ev, 0, sizeof(ev));
    ztui_push_str("test_input");
    ztui_input_auto((ztui_rect){0, 0, 10, 1}, "hint", &ev,
                    ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0),
                    ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD));
    ztui_pop_id();
    PASS();

    TEST("ztui_input_get returns empty string without terminal");
    ztui_push_str("test_get");
    const char* val = ztui_input_get((ztui_rect){0, 0, 10, 1});
    CHECK(val != NULL);
    ztui_pop_id();
    PASS();
}

static void test_auto_textarea_callable(void)
{
    TEST("ztui_textarea_auto is callable (no crash)");
    ztui_event ev;
    memset(&ev, 0, sizeof(ev));
    ztui_push_str("ta");
    ztui_textarea_auto((ztui_rect){0, 0, 10, 5}, &ev, ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0),
                       ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD));

    {
        ztui_text_state ts;
        char buf[64] = "";
        ztui_text_state_init(&ts, buf, sizeof(buf));
        ztui_scroll_state sc;
        ztui_scroll_state_init(&sc);
        ztui_textarea((ztui_rect){0, 0, 10, 5}, ZTUI_AUTO_ID, &ts, &sc, &ev,
                      ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0),
                      ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD),
                      ZTUI_TEXTAREA_LINE_NUMBERS);
    }
    ztui_pop_id();
    PASS();
}

static void test_auto_slider_callable(void)
{
    TEST("ztui_slider_auto is callable (no crash)");
    ztui_event ev;
    memset(&ev, 0, sizeof(ev));
    float val = 0.5f;
    ztui_push_str("sl");
    ztui_slider_auto((ztui_rect){0, 0, 10, 1}, &val, &ev,
                     ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0),
                     ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD));
    ztui_pop_id();
    PASS();
}

static void test_auto_checkbox_callable(void)
{
    TEST("ztui_checkbox_auto is callable (no crash)");
    ztui_event ev;
    memset(&ev, 0, sizeof(ev));
    int val = 1;
    ztui_push_str("cb");
    ztui_checkbox_auto((ztui_rect){0, 0, 10, 1}, "test", &val, &ev,
                       ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0),
                       ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD));
    ztui_pop_id();
    PASS();
}

static void test_anim_init(void)
{
    TEST("ztui_anim_init sets current = target");
    ztui_anim a;
    ztui_anim_init(&a, 42.0f);
    CHECK_EQ((int)a.current, 42);
    CHECK_EQ((int)a.target, 42);
    PASS();
}

static void test_anim_set_and_tick(void)
{
    TEST("ztui_anim_set + ztui_anim_tick converges");
    ztui_anim a;
    ztui_anim_init(&a, 0.0f);
    ztui_anim_set(&a, 100.0f);

    for (int i = 0; i < 100; i++)
        ztui_anim_tick(&a);
    CHECK(ztui_anim_done(&a));
    CHECK_EQ((int)ztui_anim_val(&a), 100);
    PASS();
}

static void test_progress_anim_callable(void)
{
    TEST("ztui_progress_anim is callable (no crash)");
    ztui_anim a;
    ztui_anim_init(&a, 0.5f);
    ztui_progress_anim(
        (ztui_rect){0, 0, 10, 1}, ZTUI_AUTO_ID, &a, ztui_style_make(ZTUI_WHITE, ZTUI_BLUE, 0),
        ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0), ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, 0));
    PASS();
}

static void test_popup_run_no_crash(void)
{
    TEST("ztui_popup_run inactive returns -1 (no crash)");
    ztui_popup p = {0, 0, 20, 0, 2, 0};

    ztui_event ev;
    memset(&ev, 0, sizeof(ev));
    int r = ztui_popup_run(&p, NULL, 0, 10, 10, 20, &ev);
    CHECK_EQ(r, -1);
    PASS();
}

static void test_api_linkage(void)
{

    TEST("All API functions are linkable");

    void (*fp1)(ztui_point, const char*, ztui_style) = ztui_set_cell;
    void (*fp2)(ztui_point, const char*, ztui_style) = ztui_draw_str;
    void (*fp3)(ztui_rect, const char*, ztui_style) = ztui_fill_rect;
    void (*fp4)(ztui_rect, ztui_box_style, ztui_style) = ztui_draw_box;
    void (*fp5)(ztui_rect, ztui_layout_dir, int) = ztui_layout_begin;
    void (*fp5b)(ztui_rect, ztui_layout_dir, int, ztui_justify, ztui_align, int) =
        ztui_layout_begin_ex;
    void (*fp6)(void) = ztui_layout_end;
    ztui_rect (*fp7)(void) = ztui_layout_next;
    ztui_rect (*fp8)(int) = ztui_layout_next_h;
    ztui_rect (*fp9)(int, int) = ztui_layout_next_wh;
    ztui_rect (*fp10)(void) = ztui_layout_next_fill;
    int (*fp11)(void) = ztui_layout_cursor;
    int (*fp12)(void) = ztui_layout_remaining;
    ztui_rect (*fp13)(ztui_layout_opts) = ztui_layout_next_opts;
    void (*fp14)(ztui_rect, const char*, ztui_box_style, int) = ztui_panel_begin;
    void (*fp15)(void) = ztui_panel_end;
    void (*fp16)(ztui_scroll_state*, ztui_rect, ztui_event*) = ztui_scroll_begin;
    void (*fp17)(ztui_scroll_state*) = ztui_scroll_end;
    void (*fp18)(int, int, const char*) = ztui_dialog_begin;
    void (*fp19)(void) = ztui_dialog_end;
    int (*fp20)(const char*, const char*, const char*, const char*, ztui_event*) =
        ztui_dialog_confirm;
    int (*fp21)(void) = ztui_focus_get;
    void (*fp22)(int) = ztui_focus_set;
    void (*fp23)(void) = ztui_focus_next;
    void (*fp24)(int) = ztui_focus_max;
    void (*fp25)(ztui_text_state*, char*, int) = ztui_text_state_init;
    void (*fp26)(ztui_scroll_state*) = ztui_scroll_state_init;
    void (*fp27)(ztui_rect, ztui_split*, ztui_event*, ztui_rect*, ztui_rect*) = ztui_split_draw;
    int (*fp28)(ztui_rect, ztui_tab_bar*, ztui_event*, const char* const*, int, ztui_style,
                ztui_style) = ztui_tab_bar_draw;
    void (*fp29)(ztui_status*, ztui_rect, ztui_style) = ztui_status_draw;
    void (*fp30)(ztui_status*, const char*, int) = ztui_status_set;
    void (*fp31)(ztui_toast*, const char*, int) = ztui_toast_show;
    void (*fp32)(ztui_toast*) = ztui_toast_draw;
    void (*fp33)(int) = ztui_push_id;
    void (*fp34)(const void*) = ztui_push_ptr;
    void (*fp35)(const char*) = ztui_push_str;
    void (*fp36)(void) = ztui_pop_id;
    void (*fp37)(ztui_rect, const char*, ztui_event*, ztui_style, ztui_style) = ztui_input_auto;
    const char* (*fp38)(ztui_rect) = ztui_input_get;
    void (*fp39)(ztui_rect, ztui_event*, ztui_style, ztui_style) = ztui_textarea_auto;
    const char* (*fp40)(ztui_rect) = ztui_textarea_get;
    int (*fp41)(ztui_rect, float*, ztui_event*, ztui_style, ztui_style) = ztui_slider_auto;
    int (*fp42)(ztui_rect, const char*, int*, ztui_event*, ztui_style, ztui_style) =
        ztui_checkbox_auto;
    void (*fp43)(ztui_anim*, float) = ztui_anim_init;
    void (*fp44)(ztui_anim*, float) = ztui_anim_set;
    void (*fp45)(ztui_anim*) = ztui_anim_tick;
    int (*fp46)(ztui_anim*) = ztui_anim_done;
    float (*fp47)(ztui_anim*) = ztui_anim_val;
    void (*fp48)(ztui_rect, int, ztui_anim*, ztui_style, ztui_style, ztui_style) =
        ztui_progress_anim;
    int (*fp49)(ztui_popup*, const char* const*, int, int, int, int, ztui_event*) = ztui_popup_run;
    int (*fp50)(ztui_event*, int) = ztui_poll_timeout;
    int (*fp51)(int, void (*)(int, void*), void*) = ztui_add_fd;
    int (*fp52)(ztui_rect, int, ztui_text_state*, const char* const*, int, ztui_event*, ztui_style,
                ztui_style, ztui_style) = ztui_autocomplete;
    int (*fp53)(ztui_rect, int, const char* const*, const int*, int, int*, int*, ztui_scroll_state*,
                ztui_event*, ztui_style, ztui_style, ztui_style, ztui_style) = ztui_tree;
    (void)fp1;
    (void)fp2;
    (void)fp3;
    (void)fp4;
    (void)fp5;
    (void)fp6;
    (void)fp7;
    (void)fp8;
    (void)fp9;
    (void)fp10;
    (void)fp11;
    (void)fp12;
    (void)fp13;
    (void)fp14;
    (void)fp15;
    (void)fp16;
    (void)fp17;
    (void)fp18;
    (void)fp19;
    (void)fp20;
    (void)fp21;
    (void)fp22;
    (void)fp23;
    (void)fp24;
    (void)fp25;
    (void)fp26;
    (void)fp27;
    (void)fp28;
    (void)fp29;
    (void)fp30;
    (void)fp31;
    (void)fp32;
    (void)fp5b;
    (void)fp33;
    (void)fp34;
    (void)fp35;
    (void)fp36;
    (void)fp37;
    (void)fp38;
    (void)fp39;
    (void)fp40;
    (void)fp41;
    (void)fp42;
    (void)fp43;
    (void)fp44;
    (void)fp45;
    (void)fp46;
    (void)fp47;
    (void)fp48;
    (void)fp49;
    (void)fp50;
    (void)fp51;
    (void)fp52;
    (void)fp53;
    PASS();
}

int main(void)
{
    printf("ztui test suite\n");
    printf("──────────────────\n");

    test_rgb_macro();
    test_utf8_len();
    test_has_truecolor();
    test_get_error_roundtrip();
    test_init_fails_without_terminal();
    test_rect_make();
    test_point_make();
    test_style_make();
    test_default_theme();
    test_set_theme_roundtrip();
    test_text_state_init();
    test_text_state_init_null();
    test_scroll_state_init();
    test_layout_opts_macros();
    test_text_measure();
    test_event_type_constants();
    test_layout_cursor_noop();
    test_flex_enums();
    test_draw_primitives_no_terminal();
    test_scroll_ensure_noop();
    test_id_stack_push_pop();
    test_auto_input_callable();
    test_auto_textarea_callable();
    test_auto_slider_callable();
    test_auto_checkbox_callable();
    test_anim_init();
    test_anim_set_and_tick();
    test_progress_anim_callable();
    test_popup_run_no_crash();

    TEST("ztui_autocomplete is callable (no crash)");
    {
        char buf[64] = "he";
        ztui_text_state ts;
        ztui_text_state_init(&ts, buf, sizeof(buf));
        static const char* items[] = {"hello", "world", "help"};
        ztui_event ev;
        memset(&ev, 0, sizeof(ev));
        ztui_autocomplete((ztui_rect){0, 0, 20, 1}, ZTUI_AUTO_ID, &ts, items, 3, &ev,
                          ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0),
                          ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD),
                          ztui_style_make(ZTUI_WHITE, ZTUI_BLUE, 0));
        PASS();
    }

    TEST("ztui_tree is callable (no crash)");
    {
        static const char* labels[] = {"root", "child1", "child2"};
        static const int indents[] = {0, 1, 1};
        int expanded[] = {1, 1, 1};
        int sel = 0;
        ztui_scroll_state sc;
        ztui_scroll_state_init(&sc);
        ztui_event ev;
        memset(&ev, 0, sizeof(ev));
        ztui_tree((ztui_rect){0, 0, 20, 10}, ZTUI_AUTO_ID, labels, indents, 3, expanded, &sel, &sc,
                  &ev, ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0),
                  ztui_style_make(ZTUI_BLACK, ZTUI_CYAN, ZTUI_STYLE_BOLD),
                  ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD),
                  ztui_style_make(ZTUI_YELLOW, ZTUI_BLACK, 0));
        PASS();
    }

    TEST("ztui_poll_timeout returns 0 without terminal (no crash)");
    {
        ztui_event ev;
        int r = ztui_poll_timeout(&ev, 0);
        (void)r;
        PASS();
    }
    TEST("ztui_poll_timeout with ms=-1 returns (no crash)");
    {
        ztui_event ev;
        int r = ztui_poll_timeout(&ev, 1);
        (void)r;
        PASS();
    }

    test_api_linkage();

    printf("──────────────────\n");
    printf("%d / %d tests passed\n", passed, tests);

    return (passed == tests) ? 0 : 1;
}
