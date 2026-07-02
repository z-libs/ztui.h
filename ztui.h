

#ifndef ZTUI_H
#define ZTUI_H

#define _POSIX_C_SOURCE 200809L

#ifndef Z_COMMON_BUNDLED
#define Z_COMMON_BUNDLED

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define Z_OK 0
#define Z_ERR -1
#define Z_ENOMEM -2
#define Z_EOOB -3
#define Z_EEMPTY -4
#define Z_ENOTFOUND -5
#define Z_EINVAL -6
#define Z_EEXIST -7

#ifndef Z_MALLOC
#include <stdlib.h>
#define Z_MALLOC(sz) malloc(sz)
#define Z_CALLOC(n, sz) calloc(n, sz)
#define Z_REALLOC(p, sz) realloc(p, sz)
#define Z_FREE(p) free(p)
#endif

#ifndef Z_GROWTH_FACTOR
#define Z_GROWTH_FACTOR(cap) ((cap) == 0 ? 32 : (cap) * 2)
#endif

#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef int ztui_color;

#define ztui_rgb(R, G, B) ((ztui_color)(0x1000000u | ((R) << 16) | ((G) << 8) | (B)))

typedef enum
{
    ZTUI_BLACK,
    ZTUI_RED,
    ZTUI_GREEN,
    ZTUI_YELLOW,
    ZTUI_BLUE,
    ZTUI_MAGENTA,
    ZTUI_CYAN,
    ZTUI_WHITE,
    ZTUI_DEFAULT = 9
} ztui_color_name;

typedef enum
{
    ZTUI_STYLE_BOLD = 1,
    ZTUI_STYLE_DIM = 2,
    ZTUI_STYLE_ITALIC = 4,
    ZTUI_STYLE_UNDERLINE = 8
} ztui_style_bit;

typedef struct
{
    ztui_color fg, bg;
    uint8_t bits;
} ztui_style;

typedef enum
{
    ZTUI_BOX_ASCII,
    ZTUI_BOX_SINGLE,
    ZTUI_BOX_DOUBLE
} ztui_box_style;
typedef enum
{
    ZTUI_LAYOUT_VERTICAL,
    ZTUI_LAYOUT_HORIZONTAL
} ztui_layout_dir;
typedef enum
{
    ZTUI_JUSTIFY_START,
    ZTUI_JUSTIFY_CENTER,
    ZTUI_JUSTIFY_END,
    ZTUI_JUSTIFY_SPACE_BETWEEN,
    ZTUI_JUSTIFY_SPACE_EVENLY
} ztui_justify;
typedef enum
{
    ZTUI_ALIGN_STRETCH,
    ZTUI_ALIGN_START,
    ZTUI_ALIGN_CENTER,
    ZTUI_ALIGN_END
} ztui_align;
typedef enum
{
    ZTUI_TEXT_LEFT,
    ZTUI_TEXT_CENTER,
    ZTUI_TEXT_RIGHT
} ztui_text_align;
typedef enum
{
    ZTUI_TEXTAREA_LINE_NUMBERS = 1,
    ZTUI_TEXTAREA_NO_UNDO = 2
} ztui_textarea_flag;

typedef struct
{
    int x, y;
} ztui_point;
typedef struct
{
    int x, y, w, h;
} ztui_rect;

#ifdef __cplusplus
static inline ztui_point ztui_pt(int x, int y)
{
    ztui_point p;
    p.x = x;
    p.y = y;
    return p;
}
static inline ztui_rect ztui_rc(int x, int y, int w, int h)
{
    ztui_rect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    return r;
}
#else
#define ztui_pt(x, y) ((ztui_point){(x), (y)})
#define ztui_rc(x, y, w, h) ((ztui_rect){(x), (y), (w), (h)})
#endif

typedef enum
{
    ZTUI_EV_NONE,
    ZTUI_EV_ESCAPE,
    ZTUI_EV_ENTER,
    ZTUI_EV_TAB,
    ZTUI_EV_BACKSPACE,
    ZTUI_EV_DELETE,
    ZTUI_EV_HOME,
    ZTUI_EV_END,
    ZTUI_EV_PAGE_UP,
    ZTUI_EV_PAGE_DOWN,
    ZTUI_EV_LEFT,
    ZTUI_EV_RIGHT,
    ZTUI_EV_UP,
    ZTUI_EV_DOWN,
    ZTUI_EV_CTRL_LEFT,
    ZTUI_EV_CTRL_RIGHT,
    ZTUI_EV_CTRL_HOME,
    ZTUI_EV_CTRL_END,
    ZTUI_EV_MOUSE_DOWN,
    ZTUI_EV_MOUSE_UP,
    ZTUI_EV_MOUSE_MOVE,
    ZTUI_EV_WHEEL_UP,
    ZTUI_EV_WHEEL_DOWN,
    ZTUI_EV_KEY,
} ztui_ev_type;

typedef struct
{
    ztui_ev_type type;
    int key;

    char utf8[5];

    ztui_point mouse;
    int mods;

} ztui_event;

typedef struct
{
    ztui_style text, muted, accent, accent_focus;
    ztui_style danger, danger_focus, panel, border;
    ztui_style field, field_focus, selection, scrollbar;
} ztui_theme;

typedef struct
{
    int min_h, max_h;

    int min_w, max_w;

    int weight;

    int fill;

    ztui_align align;

} ztui_layout_opts;

#define ZTUI_OPTS_FIXED(h) ((ztui_layout_opts){h, h, 0, 0, 0, 0, ZTUI_ALIGN_STRETCH})
#define ZTUI_OPTS_FILL() ((ztui_layout_opts){1, -1, 0, 0, 1, 1, ZTUI_ALIGN_STRETCH})
#define ZTUI_OPTS_WEIGHT(w) ((ztui_layout_opts){1, -1, 0, 0, w, 0, ZTUI_ALIGN_STRETCH})

typedef struct
{
    float current, target;
    float speed;

    float epsilon;

} ztui_anim;

typedef struct
{
    int x, y;
    int w;
    int selected;
    int count;
    int active;
} ztui_popup;

typedef struct
{
    int pos;

    int min_a, min_b;

    int drag_ofs;

    int is_vert;

} ztui_split;

typedef struct
{
    int active;
    int count;
    int hovered;
} ztui_tab_bar;

typedef struct
{
    char msg[256];
    int ttl;

} ztui_status;

typedef struct
{
    char msg[128];
    int ttl;
} ztui_toast;

typedef struct
{
    char* buffer;
    int capacity;
    int length;
    int cursor;
    int preferred_col;

} ztui_text_state;

typedef struct
{
    int offset;
    int viewport_x, viewport_y;
    int viewport_w, viewport_h;
    int content_height;
    int active;
} ztui_scroll_state;

typedef struct
{
    int width;

    ztui_text_align align;
} ztui_column;

typedef enum
{
    ZTUI_OK = Z_OK,
    ZTUI_ERR_OOM = Z_ENOMEM,
    ZTUI_ERR_TERMINAL = -8,
    ZTUI_ERR_IO = -9
} ztui_error;

ztui_error ztui_init(void);
void ztui_cleanup(void);
ztui_error ztui_get_error(void);
int ztui_has_truecolor(void);

void ztui_frame_begin(ztui_color bg);
void ztui_frame_end(void);
void ztui_get_size(int* w, int* h);
int ztui_was_resized(void);

int ztui_poll(ztui_event* ev);

int ztui_poll_timeout(ztui_event* ev, int ms);

int ztui_add_fd(int fd, void (*cb)(int fd, void* arg), void* arg);

void ztui_set_cell(ztui_point p, const char* glyph, ztui_style s);
void ztui_draw_str(ztui_point p, const char* str, ztui_style s);
void ztui_fill_rect(ztui_rect r, const char* glyph, ztui_style s);
void ztui_draw_box(ztui_rect r, ztui_box_style bs, ztui_style s);

void ztui_clip_set(ztui_rect r);
void ztui_clip_push(ztui_rect r);
void ztui_clip_pop(void);

void ztui_layout_begin(ztui_rect area, ztui_layout_dir dir, int gap);
void ztui_layout_begin_ex(ztui_rect area, ztui_layout_dir dir, int gap, ztui_justify justify,
                          ztui_align align, int wrap);
void ztui_layout_end(void);
ztui_rect ztui_layout_next(void);
ztui_rect ztui_layout_next_h(int h);
ztui_rect ztui_layout_next_wh(int w, int h);
ztui_rect ztui_layout_next_fill(void);
int ztui_layout_cursor(void);

int ztui_layout_remaining(void);

void ztui_panel_begin(ztui_rect r, const char* title, ztui_box_style bs, int gap);
void ztui_panel_end(void);

void ztui_scroll_begin(ztui_scroll_state* s, ztui_rect r, ztui_event* ev);
void ztui_scroll_end(ztui_scroll_state* s);

void ztui_dialog_begin(int w, int h, const char* title);
void ztui_dialog_end(void);
int ztui_dialog_confirm(const char* title, const char* msg, const char* ok, const char* cancel,
                        ztui_event* ev);

#define ZTUI_AUTO_ID (-1)

int ztui_focus_get(void);
void ztui_focus_set(int id);
void ztui_focus_next(void);
void ztui_focus_max(int max);

void ztui_text_state_init(ztui_text_state* s, char* buf, int cap);
void ztui_scroll_state_init(ztui_scroll_state* s);

void ztui_push_id(int id);
void ztui_push_ptr(const void* p);
void ztui_push_str(const char* s);
void ztui_pop_id(void);

void ztui_input_auto(ztui_rect r, const char* hint, ztui_event* ev, ztui_style n, ztui_style f);
const char* ztui_input_get(ztui_rect r);

void ztui_textarea_auto(ztui_rect r, ztui_event* ev, ztui_style n, ztui_style f);
const char* ztui_textarea_get(ztui_rect r);

int ztui_slider_auto(ztui_rect r, float* val, ztui_event* ev, ztui_style n, ztui_style f);
int ztui_checkbox_auto(ztui_rect r, const char* label, int* val, ztui_event* ev, ztui_style n,
                       ztui_style f);

void ztui_label(ztui_rect r, const char* text, ztui_style s);
void ztui_separator(ztui_rect r, ztui_style s);

int ztui_button(ztui_rect r, int id, const char* label, ztui_event* ev, ztui_style n, ztui_style f);

int ztui_checkbox(ztui_rect r, int id, const char* label, int* val, ztui_event* ev, ztui_style n,
                  ztui_style f);

int ztui_slider(ztui_rect r, int id, float* val, ztui_event* ev, ztui_style n, ztui_style f);

void ztui_progress(ztui_rect r, int id, float prog, ztui_style fill, ztui_style empty,
                   ztui_style focus);

int ztui_input(ztui_rect r, int id, ztui_text_state* state, ztui_event* ev, ztui_style n,
               ztui_style f);

int ztui_textarea(ztui_rect r, int id, ztui_text_state* state, ztui_scroll_state* scroll,
                  ztui_event* ev, ztui_style n, ztui_style f, int flags);

int ztui_list(ztui_rect r, int id, const char* const* items, int n, int* sel,
              ztui_scroll_state* scroll, ztui_event* ev, ztui_style nrm, ztui_style selsty,
              ztui_style f);

int ztui_table(ztui_rect r, int id, const char* const* headers, const ztui_column* cols, int nc,
               const char* const* cells, int nr, int* sel, ztui_scroll_state* scroll,
               ztui_event* ev, ztui_style hdr, ztui_style cell, ztui_style selsty, ztui_style f);

int ztui_text_measure(const char* text, int width);
void ztui_text_block(ztui_rect r, const char* text, ztui_style s);
void ztui_text_block_aligned(ztui_rect r, const char* text, ztui_text_align a, ztui_style s);

int ztui_dropdown(ztui_rect r, int id, const char* const* items, int n, int* sel, ztui_event* ev,
                  ztui_style nrm, ztui_style f, ztui_style list);

int ztui_radio(ztui_rect r, int id, const char* label, int group_id, int* val, int this_val,
               ztui_event* ev, ztui_style nrm, ztui_style f);

void ztui_spinner(ztui_rect r, int id, int frame, ztui_style s);

int ztui_autocomplete(ztui_rect r, int id, ztui_text_state* state, const char* const* items, int n,
                      ztui_event* ev, ztui_style nrm, ztui_style foc, ztui_style list);

int ztui_tree(ztui_rect r, int id, const char* const* labels, const int* indents, int nn,
              int* expanded, int* selected, ztui_scroll_state* scroll, ztui_event* ev,
              ztui_style nrm, ztui_style sel, ztui_style foc, ztui_style sb);

ztui_rect ztui_layout_next_opts(ztui_layout_opts o);

void ztui_split_draw(ztui_rect area, ztui_split* s, ztui_event* ev, ztui_rect* a, ztui_rect* b);

int ztui_tab_bar_draw(ztui_rect r, ztui_tab_bar* s, ztui_event* ev, const char* const* labels,
                      int n, ztui_style tab, ztui_style active);

void ztui_status_draw(ztui_status* s, ztui_rect r, ztui_style style);
void ztui_status_set(ztui_status* s, const char* msg, int frames);

void ztui_toast_show(ztui_toast* t, const char* msg, int frames);
void ztui_toast_draw(ztui_toast* t);

void ztui_anim_init(ztui_anim* a, float val);
void ztui_anim_set(ztui_anim* a, float target);
void ztui_anim_tick(ztui_anim* a);
int ztui_anim_done(ztui_anim* a);
float ztui_anim_val(ztui_anim* a);
void ztui_progress_anim(ztui_rect r, int id, ztui_anim* a, ztui_style fill, ztui_style empty,
                        ztui_style focus);

int ztui_popup_run(ztui_popup* p, const char* const* items, int n, int x, int y, int w,
                   ztui_event* ev);

ztui_style ztui_style_make(ztui_color fg, ztui_color bg, uint8_t bits);
ztui_theme ztui_default_theme(void);
void ztui_set_theme(const ztui_theme* t);
const ztui_theme* ztui_get_theme(void);

static inline void ztui_label_next(const char* t)
{
    ztui_label(ztui_layout_next(), t, ztui_get_theme()->text);
}
static inline void ztui_label_muted(const char* t)
{
    ztui_label(ztui_layout_next(), t, ztui_get_theme()->muted);
}
static inline int ztui_button_next(const char* l, ztui_event* ev)
{
    return ztui_button(ztui_layout_next(), ZTUI_AUTO_ID, l, ev, ztui_get_theme()->accent,
                       ztui_get_theme()->accent_focus);
}
static inline int ztui_button_danger(const char* l, ztui_event* ev)
{
    return ztui_button(ztui_layout_next(), ZTUI_AUTO_ID, l, ev, ztui_get_theme()->danger,
                       ztui_get_theme()->danger_focus);
}
static inline int ztui_checkbox_next(const char* l, int* v, ztui_event* ev)
{
    return ztui_checkbox(ztui_layout_next(), ZTUI_AUTO_ID, l, v, ev, ztui_get_theme()->text,
                         ztui_get_theme()->field_focus);
}
static inline int ztui_slider_next(float* v, ztui_event* ev)
{
    return ztui_slider(ztui_layout_next(), ZTUI_AUTO_ID, v, ev, ztui_get_theme()->text,
                       ztui_get_theme()->field_focus);
}
static inline void ztui_progress_next(float p)
{
    ztui_progress(ztui_layout_next(), ZTUI_AUTO_ID, p, ztui_get_theme()->accent,
                  ztui_get_theme()->muted, ztui_get_theme()->field_focus);
}
static inline int ztui_input_next(ztui_text_state* s, ztui_event* ev)
{
    return ztui_input(ztui_layout_next(), ZTUI_AUTO_ID, s, ev, ztui_get_theme()->field,
                      ztui_get_theme()->field_focus);
}
static inline int ztui_list_next(int w, int h, const char* const* items, int n, int* sel,
                                 ztui_scroll_state* scroll, ztui_event* ev)
{
    return ztui_list(ztui_rc(0, 0, w, h), ZTUI_AUTO_ID, items, n, sel, scroll, ev,
                     ztui_get_theme()->text, ztui_get_theme()->selection,
                     ztui_get_theme()->field_focus);
}

#define ZTUI_LABEL(t) ztui_label(ztui_layout_next(), t, ztui_get_theme()->text)
#define ZTUI_LABEL_MUTED(t) ztui_label(ztui_layout_next(), t, ztui_get_theme()->muted)
#define ZTUI_BUTTON(l, ev) ztui_button_next(l, ev)
#define ZTUI_BUTTON_DANGER(l, ev) ztui_button_danger(l, ev)
#define ZTUI_CHECKBOX(l, v, ev) ztui_checkbox_next(l, v, ev)
#define ZTUI_SLIDER(v, ev) ztui_slider_next(v, ev)
#define ZTUI_PROGRESS(v) ztui_progress_next(v)
#define ZTUI_INPUT(s, ev) ztui_input_next(s, ev)
#define ZTUI_SEPARATOR() ztui_separator(ztui_layout_next(), ztui_get_theme()->muted)

#define ZTUI_FIELD(label, width)                                                                   \
    (void)(ztui_layout_begin(ztui_layout_next_h(1), ZTUI_LAYOUT_HORIZONTAL, 1),                    \
           ztui_label(ztui_layout_next_wh(width, 1), label, ztui_get_theme()->muted))
#define ZTUI_FIELD_END ztui_layout_end()

#ifdef ZTUI_IMPLEMENTATION

typedef struct
{
    char glyph[5];
    ztui_color fg;
    ztui_color bg;
    uint8_t bits;
    uint8_t width;

} ztui_cell;

#define ZTUI_STACK_MAX 32

static ztui_cell* _ztui_front = NULL;
static ztui_cell* _ztui_back = NULL;
static int _ztui_width = 0;
static int _ztui_height = 0;
static int _ztui_mouse_x = -1;
static int _ztui_mouse_y = -1;
static int _ztui_resized = 0;
static int _ztui_full_repaint = 1;
static ztui_error _ztui_last_err = ZTUI_OK;

static int _ztui_clip_x0 = 0, _ztui_clip_y0 = 0;
static int _ztui_clip_x1 = 0, _ztui_clip_y1 = 0;
static int _ztui_clip_stack_x0[ZTUI_STACK_MAX], _ztui_clip_stack_y0[ZTUI_STACK_MAX];
static int _ztui_clip_stack_x1[ZTUI_STACK_MAX], _ztui_clip_stack_y1[ZTUI_STACK_MAX];
static int _ztui_clip_top = 0;

static int _focus_id = 1, _max_focus_id = 1;
static int _ztui_auto_counter = 0, _ztui_auto_max = 0;

static ztui_theme _ztui_theme;
static int _ztui_theme_ready = 0;
static int _ztui_truecolor_ok = 0;

static ztui_scroll_state* _ztui_drag_scroll = NULL;
static int _ztui_drag_scroll_y0 = 0, _ztui_drag_scroll_track_h = 0;
static int _ztui_drag_scroll_thumb_h = 0, _ztui_drag_scroll_grab = 0;

#define ZTUI_MAX_EXTRA_FD 8

typedef struct
{
    int fd;
    void (*cb)(int, void*);
    void* arg;
} _ztui_fd_entry;

static _ztui_fd_entry _ztui_extra_fds[ZTUI_MAX_EXTRA_FD];
static int _ztui_extra_fd_count = 0;

int ztui_add_fd(int fd, void (*cb)(int, void*), void* arg)
{
    if (_ztui_extra_fd_count >= ZTUI_MAX_EXTRA_FD)
        return -1;
    _ztui_extra_fds[_ztui_extra_fd_count].fd = fd;
    _ztui_extra_fds[_ztui_extra_fd_count].cb = cb;
    _ztui_extra_fds[_ztui_extra_fd_count].arg = arg;
    _ztui_extra_fd_count++;
    return 0;
}

#define ZTUI_UNDO_MAX 64
typedef struct
{
    int pos, dl, il;
    char data[48];
} _ztui_undo;
static _ztui_undo _ztui_undo_buf[ZTUI_UNDO_MAX];
static int _ztui_undo_n = 0, _ztui_redo_n = 0;

static void _ztui_undo_push(int pos, int dl, int il, const char* txt)
{
    if (_ztui_undo_n >= ZTUI_UNDO_MAX)
    {
        memmove(&_ztui_undo_buf[0], &_ztui_undo_buf[1], sizeof(_ztui_undo) * (ZTUI_UNDO_MAX - 1));
        _ztui_undo_n--;
    }
    _ztui_undo* e = &_ztui_undo_buf[_ztui_undo_n++];
    e->pos = pos;
    e->dl = dl;
    e->il = il;
    int cp = il > 0 ? il : dl;
    if (cp > 48)
        cp = 48;
    memcpy(e->data, txt, (size_t)cp);
    _ztui_redo_n = 0;
}

static int _ztui_undo_apply(ztui_text_state* s)
{
    if (_ztui_undo_n <= 0)
        return 0;
    _ztui_undo* e = &_ztui_undo_buf[--_ztui_undo_n];

    if (_ztui_redo_n < ZTUI_UNDO_MAX)
    {
        _ztui_undo* r = &_ztui_undo_buf[ZTUI_UNDO_MAX - 1 - _ztui_redo_n];
        *r = *e;
        _ztui_redo_n++;
    }
    s->cursor = e->pos;
    if (e->il > 0)
    {

        int len = e->il;
        if (e->pos + len > s->length)
            len = s->length - e->pos;
        memmove(&s->buffer[e->pos], &s->buffer[e->pos + len],
                (size_t)(s->length - e->pos - len + 1));
        s->length -= len;
    }
    else
    {

        int len = e->dl;
        if (s->length + len > s->capacity - 1)
            len = s->capacity - 1 - s->length;
        memmove(&s->buffer[e->pos + len], &s->buffer[e->pos], (size_t)(s->length - e->pos + 1));
        memcpy(&s->buffer[e->pos], e->data, (size_t)len);
        s->length += len;
    }
    return 1;
}

static int _ztui_redo_apply(ztui_text_state* s)
{
    if (_ztui_redo_n <= 0)
        return 0;
    _ztui_undo* r = &_ztui_undo_buf[ZTUI_UNDO_MAX - _ztui_redo_n];
    _ztui_redo_n--;

    if (_ztui_undo_n < ZTUI_UNDO_MAX)
        _ztui_undo_buf[_ztui_undo_n++] = *r;
    s->cursor = r->pos;
    if (r->il > 0)
    {
        int len = r->il;
        if (r->pos + len > s->length)
            len = s->length - r->pos;
        memmove(&s->buffer[r->pos], &s->buffer[r->pos + len],
                (size_t)(s->length - r->pos - len + 1));
        s->length -= len;
    }
    else
    {
        int len = r->dl;
        if (s->length + len > s->capacity - 1)
            len = s->capacity - 1 - s->length;
        memmove(&s->buffer[r->pos + len], &s->buffer[r->pos], (size_t)(s->length - r->pos + 1));
        memcpy(&s->buffer[r->pos], r->data, (size_t)len);
        s->length += len;
    }
    return 1;
}

typedef struct
{
    ztui_rect area;
    ztui_layout_dir dir;
    int gap;
    int cursor;

    int fill_reserved;
    ztui_justify justify;
    ztui_align align;
    int wrap;
    int max_cross;

    int first_item_pos;

    int used_advance;

} ztui_layout_internal;

static ztui_layout_internal _ly;
static ztui_layout_internal _ly_stack[ZTUI_STACK_MAX];
static int _ly_top = 0;

static int _ztui_panel_layout_mark[ZTUI_STACK_MAX];
static int _ztui_panel_clip_mark[ZTUI_STACK_MAX];
static int _ztui_panel_top = 0;

ztui_error ztui_get_error(void)
{
    ztui_error e = _ztui_last_err;
    _ztui_last_err = ZTUI_OK;
    return e;
}

static void _ztui_set_err(ztui_error e)
{
    if (_ztui_last_err == ZTUI_OK)
        _ztui_last_err = e;
}

#define ZTUI_OUT_BUF_SIZE (1 << 20)
static char _ztui_out_buf[ZTUI_OUT_BUF_SIZE];
static int _ztui_out_pos = 0;

static void _ztui_out_flush(void)
{
    if (_ztui_out_pos > 0)
    {
        fwrite(_ztui_out_buf, 1, (size_t)_ztui_out_pos, stdout);
        fflush(stdout);
        _ztui_out_pos = 0;
    }
}

static void _ztui_out_char(char c)
{
    if (_ztui_out_pos >= ZTUI_OUT_BUF_SIZE - 64)
        _ztui_out_flush();
    _ztui_out_buf[_ztui_out_pos++] = c;
}

static void _ztui_out_str(const char* s)
{
    while (*s)
        _ztui_out_char(*s++);
}

static void _ztui_out_int(int n)
{
    if (n < 0)
    {
        _ztui_out_char('-');
        n = -n;
    }
    char tmp[12];
    int len = 0;
    if (n == 0)
    {
        _ztui_out_char('0');
        return;
    }
    while (n > 0)
    {
        tmp[len++] = (char)('0' + n % 10);
        n /= 10;
    }
    for (int i = len - 1; i >= 0; i--)
        _ztui_out_char(tmp[i]);
}

/* ── UTF-8 helpers ────────────────────────────────────────────────────────── */

static int _ztui_min(int a, int b)
{
    return a < b ? a : b;
}
static int _ztui_max(int a, int b)
{
    return a > b ? a : b;
}

static int _ztui_utf8_len(unsigned char c)
{
    if ((c & 0x80u) == 0x00u)
        return 1;
    if ((c & 0xE0u) == 0xC0u)
        return 2;
    if ((c & 0xF0u) == 0xE0u)
        return 3;
    if ((c & 0xF8u) == 0xF0u)
        return 4;
    return 1;
}

static uint32_t _ztui_utf8_decode(const char* s, int* out_bytes)
{
    unsigned char c = (unsigned char)s[0];
    uint32_t cp;
    int n;
    if (c < 0x80u)
    {
        *out_bytes = 1;
        return c;
    }
    if ((c & 0xE0u) == 0xC0u)
    {
        n = 2;
        cp = c & 0x1Fu;
    }
    else if ((c & 0xF0u) == 0xE0u)
    {
        n = 3;
        cp = c & 0x0Fu;
    }
    else if ((c & 0xF8u) == 0xF0u)
    {
        n = 4;
        cp = c & 0x07u;
    }
    else
    {
        *out_bytes = 1;
        return 0xFFFDu;
    }
    for (int i = 1; i < n; i++)
    {
        if (((unsigned char)s[i] & 0xC0u) != 0x80u)
        {
            *out_bytes = i;
            return 0xFFFDu;
        }
        cp = (cp << 6) | ((unsigned char)s[i] & 0x3Fu);
    }
    *out_bytes = n;
    return cp;
}

static int _ztui_is_wide(uint32_t cp)
{
    return (cp >= 0x1100u && cp <= 0x115Fu) || (cp == 0x2329u || cp == 0x232Au) ||
           (cp >= 0x2E80u && cp <= 0x303Eu) || (cp >= 0x3041u && cp <= 0x33BFu) ||
           (cp >= 0x3400u && cp <= 0x4DBFu) || (cp >= 0x4E00u && cp <= 0xA4CFu) ||
           (cp >= 0xA960u && cp <= 0xA97Fu) || (cp >= 0xAC00u && cp <= 0xD7AFu) ||
           (cp >= 0xF900u && cp <= 0xFAFFu) || (cp >= 0xFE10u && cp <= 0xFE1Fu) ||
           (cp >= 0xFE30u && cp <= 0xFE4Fu) || (cp >= 0xFF01u && cp <= 0xFF60u) ||
           (cp >= 0xFFE0u && cp <= 0xFFE6u) || (cp >= 0x1B000u && cp <= 0x1B0FFu) ||
           (cp >= 0x1F300u && cp <= 0x1F64Fu) || (cp >= 0x1F900u && cp <= 0x1F9FFu) ||
           (cp >= 0x20000u && cp <= 0x2FFFDu) || (cp >= 0x30000u && cp <= 0x3FFFDu);
}

static int _ztui_glyph_w(const char* g)
{
    int b;
    return _ztui_is_wide(_ztui_utf8_decode(g, &b)) ? 2 : 1;
}

static int _ztui_str_w(const char* s)
{
    int cols = 0;
    for (int i = 0; s[i];)
    {
        int b = _ztui_utf8_len((unsigned char)s[i]);
        char buf[5] = {0};
        for (int j = 0; j < b && s[i + j] && j < 4; j++)
            buf[j] = s[i + j];
        cols += _ztui_glyph_w(buf);
        i += b;
    }
    return cols;
}

static int _ztui_str_w_n(const char* s, int blen)
{
    int cols = 0;
    for (int i = 0; i < blen && s[i];)
    {
        int b = _ztui_utf8_len((unsigned char)s[i]);
        char buf[5] = {0};
        for (int j = 0; j < b && (i + j) < blen && s[i + j] && j < 4; j++)
            buf[j] = s[i + j];
        cols += _ztui_glyph_w(buf);
        i += b;
    }
    return cols;
}

static int _ztui_byte_at_col(const char* buf, int blen, int tcol)
{
    int col = 0, i = 0;
    while (i < blen && buf[i] && buf[i] != '\n')
    {
        int b = _ztui_utf8_len((unsigned char)buf[i]);
        char tmp[5] = {0};
        for (int j = 0; j < b && (i + j) < blen && buf[i + j] && j < 4; j++)
            tmp[j] = buf[i + j];
        int cw = _ztui_glyph_w(tmp);
        if (col + cw > tcol)
            break;
        col += cw;
        i += b;
    }
    return i;
}

/* ── theme ────────────────────────────────────────────────────────────────── */

static void _ztui_sync_theme(void)
{
    if (_ztui_theme_ready)
        return;
    _ztui_theme = ztui_default_theme();
    _ztui_theme_ready = 1;
}

ztui_style ztui_style_make(ztui_color fg, ztui_color bg, uint8_t bits)
{
    ztui_style s = {fg, bg, bits};
    return s;
}

ztui_theme ztui_default_theme(void)
{
    ztui_theme t;
    t.text = ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0);
    t.muted = ztui_style_make(ZTUI_CYAN, ZTUI_BLACK, ZTUI_STYLE_DIM);
    t.accent = ztui_style_make(ZTUI_WHITE, ZTUI_BLUE, 0);
    t.accent_focus = ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD);
    t.danger = ztui_style_make(ZTUI_RED, ZTUI_BLACK, 0);
    t.danger_focus = ztui_style_make(ZTUI_BLACK, ZTUI_RED, ZTUI_STYLE_BOLD);
    t.panel = ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0);
    t.border = ztui_style_make(ZTUI_YELLOW, ZTUI_BLACK, 0);
    t.field = ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0);
    t.field_focus = ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD);
    t.selection = ztui_style_make(ZTUI_BLACK, ZTUI_CYAN, ZTUI_STYLE_BOLD);
    t.scrollbar = ztui_style_make(ZTUI_YELLOW, ZTUI_BLACK, 0);
    return t;
}

void ztui_set_theme(const ztui_theme* t)
{
    _ztui_theme = *t;
    _ztui_theme_ready = 1;
}
const ztui_theme* ztui_get_theme(void)
{
    _ztui_sync_theme();
    return &_ztui_theme;
}

/* ── buffer management ────────────────────────────────────────────────────── */

static void _ztui_grow_buffers(int w, int h)
{
    size_t count = (size_t)w * (size_t)h;
    ztui_cell* nf = (ztui_cell*)Z_CALLOC(count, sizeof(ztui_cell));
    ztui_cell* nb = (ztui_cell*)Z_CALLOC(count, sizeof(ztui_cell));
    if (!nf || !nb)
    {
        Z_FREE(nf);
        Z_FREE(nb);
        _ztui_set_err(ZTUI_ERR_OOM);
        return;
    }
    Z_FREE(_ztui_front);
    Z_FREE(_ztui_back);
    _ztui_front = nf;
    _ztui_back = nb;
    _ztui_width = w;
    _ztui_height = h;
    _ztui_clip_x0 = 0;
    _ztui_clip_y0 = 0;
    _ztui_clip_x1 = w;
    _ztui_clip_y1 = h;
    _ztui_clip_top = 0;
    _ly_top = 0;
    _ztui_full_repaint = 1;
}

/* ── focus helpers ────────────────────────────────────────────────────────── */

static int _ztui_hit(ztui_event* ev, ztui_rect r)
{
    return ev->mouse.x >= r.x && ev->mouse.x < r.x + r.w && ev->mouse.y >= r.y &&
           ev->mouse.y < r.y + r.h;
}

static void _ztui_focus_advance(void)
{
    if (_max_focus_id < 1)
    {
        _focus_id = 1;
        return;
    }
    _focus_id++;
    if (_focus_id > _max_focus_id)
        _focus_id = 1;
}

static int _ztui_resolve_id(int id)
{
    if (id == ZTUI_AUTO_ID)
        id = _ztui_auto_counter + 1;
    if (id > _ztui_auto_counter)
        _ztui_auto_counter = id;
    if (id > 0 && id > _ztui_auto_max)
        _ztui_auto_max = id;
    return id;
}

static int _ztui_focused(int id, int hovered, ztui_event* ev)
{
    if (id <= 0)
        return 0;
    if (hovered && (ev->type == ZTUI_EV_MOUSE_MOVE || ev->type == ZTUI_EV_MOUSE_DOWN ||
                    ev->type == ZTUI_EV_MOUSE_UP))
        _focus_id = id;
    return _focus_id == id;
}

void ztui_focus_set(int id)
{
    _focus_id = id > 0 ? id : 1;
}
int ztui_focus_get(void)
{
    return _focus_id;
}
void ztui_focus_next(void)
{
    _ztui_focus_advance();
}
void ztui_focus_max(int m)
{
    _max_focus_id = m < 1 ? 1 : m;
    if (_focus_id > _max_focus_id)
        _focus_id = 1;
}

/* ── truecolor detection ──────────────────────────────────────────────────── */

int ztui_has_truecolor(void)
{
    const char* ct = getenv("COLORTERM");
    if (ct && (strstr(ct, "truecolor") || strstr(ct, "24bit")))
        return 1;
    const char* t = getenv("TERM");
    if (t && (strstr(t, "truecolor") || strstr(t, "24bit") || strstr(t, "kitty") ||
              strstr(t, "alacritty")))
        return 1;
    return 0;
}

/* ── color output helpers ────────────────────────────────────────────────────
 * Emit color prefix (without final 'm') so the caller can append ';N;Nm'
 * in one combined SGR sequence. */

static void _ztui_emit_fg(ztui_color c)
{
    unsigned int uc = (unsigned int)c;
    if (uc >= 0x1000000u && _ztui_truecolor_ok)
    {
        int r = (int)((uc >> 16) & 0xFF), g = (int)((uc >> 8) & 0xFF), b = (int)(uc & 0xFF);
        _ztui_out_str("38;2;");
        _ztui_out_int(r);
        _ztui_out_char(';');
        _ztui_out_int(g);
        _ztui_out_char(';');
        _ztui_out_int(b);
    }
    else
    {
        if (uc >= 0x1000000u)
            uc = 15;
        if ((int)uc < 0 || (int)uc > 255)
            uc = 9;
        _ztui_out_str("38;5;");
        _ztui_out_int((int)uc);
    }
}

static void _ztui_emit_bg(ztui_color c)
{
    unsigned int uc = (unsigned int)c;
    if (uc >= 0x1000000u && _ztui_truecolor_ok)
    {
        int r = (int)((uc >> 16) & 0xFF), g = (int)((uc >> 8) & 0xFF), b = (int)(uc & 0xFF);
        _ztui_out_str("48;2;");
        _ztui_out_int(r);
        _ztui_out_char(';');
        _ztui_out_int(g);
        _ztui_out_char(';');
        _ztui_out_int(b);
    }
    else
    {
        if (uc >= 0x1000000u)
            uc = 15;
        if ((int)uc < 0 || (int)uc > 255)
            uc = 9;
        _ztui_out_str("48;5;");
        _ztui_out_int((int)uc);
    }
}

/* ── layout implementation ────────────────────────────────────────────────── */

#ifdef ZTUI_LAYOUT_DEBUG
static void _ztui_layout_debug_rect(ztui_rect r)
{
    static const ztui_color colors[] = {ZTUI_RED,     ZTUI_GREEN, ZTUI_YELLOW, ZTUI_BLUE,
                                        ZTUI_MAGENTA, ZTUI_CYAN,  ZTUI_WHITE};
    ztui_color c = colors[_ly_top % (sizeof(colors) / sizeof(colors[0]))];
    ztui_style s = {c, ZTUI_BLACK, ZTUI_STYLE_DIM};
    if (r.w < 1 || r.h < 1)
        return;
    ztui_set_cell(ztui_pt(r.x, r.y), "+", s);
    if (r.w > 1)
        ztui_set_cell(ztui_pt(r.x + r.w - 1, r.y), "+", s);
    if (r.h > 1)
    {
        ztui_set_cell(ztui_pt(r.x, r.y + r.h - 1), "+", s);
        if (r.w > 1)
            ztui_set_cell(ztui_pt(r.x + r.w - 1, r.y + r.h - 1), "+", s);
    }
    for (int i = 1; i < r.w - 1; i++)
    {
        ztui_set_cell(ztui_pt(r.x + i, r.y), "-", s);
        if (r.h > 1)
            ztui_set_cell(ztui_pt(r.x + i, r.y + r.h - 1), "-", s);
    }
    for (int j = 1; j < r.h - 1; j++)
    {
        ztui_set_cell(ztui_pt(r.x, r.y + j), "|", s);
        if (r.w > 1)
            ztui_set_cell(ztui_pt(r.x + r.w - 1, r.y + j), "|", s);
    }
}
#endif

static int _ztui_advance_avail(void)
{
    return (_ly.dir == ZTUI_LAYOUT_VERTICAL) ? _ly.area.y + _ly.area.h - _ly.cursor
                                             : _ly.area.x + _ly.area.w - _ly.cursor;
}

static ztui_rect _ztui_layout_alloc(int advance_sz, int cross_sz)
{
    ztui_rect r;
    int a_sz = advance_sz > 0 ? advance_sz : 1;
    int c_sz = cross_sz;

    /* wrap: not enough room for this item? */
    if (_ly.wrap && a_sz > _ztui_advance_avail())
    {
        /* reset cursor to next "line" */
        if (_ly.dir == ZTUI_LAYOUT_VERTICAL)
        {
            _ly.cursor = _ly.area.y;
            _ly.area.x += _ly.max_cross > 0 ? _ly.max_cross + _ly.gap : 1;
            _ly.area.w = _ly.area.w - (_ly.area.x - _ly.area.x);
            _ly.max_cross = 0;
        }
        else
        {
            _ly.cursor = _ly.area.x;
            _ly.area.y += _ly.max_cross > 0 ? _ly.max_cross + _ly.gap : 1;
            _ly.area.h = _ly.area.h - (_ly.area.y - _ly.area.y);
            _ly.max_cross = 0;
        }
    }

    /* allocate along advance axis */
    if (_ly.dir == ZTUI_LAYOUT_VERTICAL)
    {
        r.x = _ly.area.x;
        r.y = _ly.cursor;
        r.w = (c_sz > 0) ? _ztui_min(c_sz, _ly.area.w) : _ly.area.w;
        r.h = a_sz;
        _ly.cursor = r.y + r.h + _ly.gap;
        if (_ly.align != ZTUI_ALIGN_STRETCH && c_sz <= 0)
        {
            if (_ly.align == ZTUI_ALIGN_CENTER)
                r.x = _ly.area.x + (_ly.area.w - r.w) / 2;
            else if (_ly.align == ZTUI_ALIGN_END)
                r.x = _ly.area.x + _ly.area.w - r.w;
        }
        if (r.h > _ly.max_cross)
            _ly.max_cross = r.h;
        _ly.used_advance += r.h + _ly.gap;
    }
    else
    {
        r.x = _ly.cursor;
        r.y = _ly.area.y;
        r.w = a_sz;
        r.h = (c_sz > 0) ? _ztui_min(c_sz, _ly.area.h) : _ly.area.h;
        _ly.cursor = r.x + r.w + _ly.gap;
        if (_ly.align != ZTUI_ALIGN_STRETCH && c_sz <= 0)
        {
            if (_ly.align == ZTUI_ALIGN_CENTER)
                r.y = _ly.area.y + (_ly.area.h - r.h) / 2;
            else if (_ly.align == ZTUI_ALIGN_END)
                r.y = _ly.area.y + _ly.area.h - r.h;
        }
        if (r.w > _ly.max_cross)
            _ly.max_cross = r.w;
        _ly.used_advance += r.w + _ly.gap;
    }
#ifdef ZTUI_LAYOUT_DEBUG
    _ztui_layout_debug_rect(r);
#endif
    return r;
}

void ztui_layout_begin(ztui_rect area, ztui_layout_dir dir, int gap)
{
    ztui_layout_begin_ex(area, dir, gap, ZTUI_JUSTIFY_START, ZTUI_ALIGN_STRETCH, 0);
}

void ztui_layout_begin_ex(ztui_rect area, ztui_layout_dir dir, int gap, ztui_justify justify,
                          ztui_align align, int wrap)
{
    if (_ly_top < ZTUI_STACK_MAX)
        _ly_stack[_ly_top++] = _ly;
    _ly.area = area;
    _ly.dir = dir;
    _ly.gap = gap < 1 ? 1 : gap;
    _ly.cursor = (dir == ZTUI_LAYOUT_VERTICAL) ? area.y : area.x;
    _ly.fill_reserved = 0;
    _ly.justify = justify;
    _ly.align = align;
    _ly.wrap = wrap;
    _ly.max_cross = 0;
    _ly.first_item_pos = _ly.cursor;
    _ly.used_advance = 0;
}

void ztui_layout_end(void)
{
    if (_ly_top > 0)
        _ly = _ly_stack[--_ly_top];
}

ztui_rect ztui_layout_next(void)
{
    return _ztui_layout_alloc(1, 0);
}
ztui_rect ztui_layout_next_h(int h)
{
    return _ztui_layout_alloc(h, 0);
}
ztui_rect ztui_layout_next_wh(int w, int h)
{
    if (_ly.dir == ZTUI_LAYOUT_VERTICAL)
        return _ztui_layout_alloc(h, w);
    else
        return _ztui_layout_alloc(w, h);
}

int ztui_layout_cursor(void)
{
    return _ly.cursor;
}

int ztui_layout_remaining(void)
{
    if (_ly.dir == ZTUI_LAYOUT_VERTICAL)
        return _ly.area.y + _ly.area.h - _ly.cursor;
    else
        return _ly.area.x + _ly.area.w - _ly.cursor;
}

ztui_rect ztui_layout_next_fill(void)
{
    ztui_rect r;
    if (_ly.dir == ZTUI_LAYOUT_VERTICAL)
    {
        r.x = _ly.area.x;
        r.y = _ly.cursor;
        r.w = _ly.area.w;
        r.h = _ly.area.y + _ly.area.h - _ly.cursor;
        if (r.h < 1)
            r.h = 1;
        _ly.cursor = _ly.area.y + _ly.area.h;
    }
    else
    {
        r.x = _ly.cursor;
        r.y = _ly.area.y;
        r.h = _ly.area.h;
        r.w = _ly.area.x + _ly.area.w - _ly.cursor;
        if (r.w < 1)
            r.w = 1;
        _ly.cursor = _ly.area.x + _ly.area.w;
    }
    _ly.fill_reserved = 1;
    return r;
}

ztui_rect ztui_layout_next_opts(ztui_layout_opts o)
{
    if (o.fill)
        return ztui_layout_next_fill();
    if (o.weight > 0)
    {
        int sz = o.weight;
        if (o.max_h > 0 && sz > o.max_h)
            sz = o.max_h;
        if (o.min_h > 0 && sz < o.min_h)
            sz = o.min_h;
        return _ztui_layout_alloc(sz, 0);
    }
    int h = o.min_h;
    if (o.min_h < 1)
        h = 1;
    int w = o.min_w;
    if (o.align != ZTUI_ALIGN_STRETCH)
    {
        ztui_align saved = _ly.align;
        _ly.align = o.align;
        ztui_rect r;
        if (_ly.dir == ZTUI_LAYOUT_VERTICAL && o.min_w > 0)
            r = _ztui_layout_alloc(h, w);
        else
            r = _ztui_layout_alloc(h, 0);
        _ly.align = saved;
        return r;
    }
    if (_ly.dir == ZTUI_LAYOUT_VERTICAL && o.min_w > 0)
        return _ztui_layout_alloc(h, w);
    return _ztui_layout_alloc(h, 0);
}

/* ── clipping ─────────────────────────────────────────────────────────────── */

void ztui_clip_set(ztui_rect r)
{
    _ztui_clip_x0 = r.x;
    _ztui_clip_y0 = r.y;
    _ztui_clip_x1 = r.x + r.w;
    _ztui_clip_y1 = r.y + r.h;
}

void ztui_clip_push(ztui_rect r)
{
    if (_ztui_clip_top < ZTUI_STACK_MAX)
    {
        _ztui_clip_stack_x0[_ztui_clip_top] = _ztui_clip_x0;
        _ztui_clip_stack_y0[_ztui_clip_top] = _ztui_clip_y0;
        _ztui_clip_stack_x1[_ztui_clip_top] = _ztui_clip_x1;
        _ztui_clip_stack_y1[_ztui_clip_top] = _ztui_clip_y1;
    }
    _ztui_clip_top++;
    int x0 = _ztui_max(_ztui_clip_x0, r.x);
    int y0 = _ztui_max(_ztui_clip_y0, r.y);
    int x1 = _ztui_min(_ztui_clip_x1, r.x + r.w);
    int y1 = _ztui_min(_ztui_clip_y1, r.y + r.h);
    _ztui_clip_x0 = x0;
    _ztui_clip_y0 = y0;
    _ztui_clip_x1 = x1;
    _ztui_clip_y1 = y1;
}

void ztui_clip_pop(void)
{
    if (_ztui_clip_top <= 0)
        return;
    _ztui_clip_top--;
    if (_ztui_clip_top < ZTUI_STACK_MAX)
    {
        _ztui_clip_x0 = _ztui_clip_stack_x0[_ztui_clip_top];
        _ztui_clip_y0 = _ztui_clip_stack_y0[_ztui_clip_top];
        _ztui_clip_x1 = _ztui_clip_stack_x1[_ztui_clip_top];
        _ztui_clip_y1 = _ztui_clip_stack_y1[_ztui_clip_top];
    }
}

/* ── drawing ──────────────────────────────────────────────────────────────── */

void ztui_set_cell(ztui_point p, const char* glyph, ztui_style s)
{
    if (p.x < _ztui_clip_x0 || p.x >= _ztui_clip_x1 || p.y < _ztui_clip_y0 || p.y >= _ztui_clip_y1)
        return;
    if (p.x < 0 || p.x >= _ztui_width || p.y < 0 || p.y >= _ztui_height)
        return;
    int dw = _ztui_glyph_w(glyph);
    if (dw == 2 && p.x + 1 >= _ztui_width)
        return;
    int i = p.y * _ztui_width + p.x;
    int j = 0;
    while (j < 4 && glyph[j])
    {
        _ztui_back[i].glyph[j] = glyph[j];
        j++;
    }
    _ztui_back[i].glyph[j] = '\0';
    _ztui_back[i].fg = s.fg;
    _ztui_back[i].bg = s.bg;
    _ztui_back[i].bits = s.bits;
    _ztui_back[i].width = (uint8_t)dw;
    if (dw == 2)
    {
        _ztui_back[i + 1].glyph[0] = ' ';
        _ztui_back[i + 1].glyph[1] = '\0';
        _ztui_back[i + 1].fg = s.fg;
        _ztui_back[i + 1].bg = s.bg;
        _ztui_back[i + 1].bits = s.bits;
        _ztui_back[i + 1].width = 0;
    }
}

void ztui_draw_str(ztui_point p, const char* str, ztui_style s)
{
    int col = 0;
    for (int i = 0; str[i];)
    {
        int b = _ztui_utf8_len((unsigned char)str[i]);
        char buf[5] = {0};
        for (int j = 0; j < b && str[i + j] && j < 4; j++)
            buf[j] = str[i + j];
        int dw = _ztui_glyph_w(buf);
        ztui_set_cell(ztui_pt(p.x + col, p.y), buf, s);
        i += b;
        col += dw;
    }
}

void ztui_fill_rect(ztui_rect r, const char* glyph, ztui_style s)
{
    for (int j = 0; j < r.h; j++)
        for (int i = 0; i < r.w; i++)
            ztui_set_cell(ztui_pt(r.x + i, r.y + j), glyph, s);
}

void ztui_draw_box(ztui_rect r, ztui_box_style bs, ztui_style s)
{
    const char *tl, *tr, *bl, *br, *hl, *vl;
    if (r.w < 2 || r.h < 2)
        return;
    if (bs == ZTUI_BOX_SINGLE)
    {
        tl = "╭";
        tr = "╮";
        bl = "╰";
        br = "╯";
        hl = "─";
        vl = "│";
    }
    else if (bs == ZTUI_BOX_DOUBLE)
    {
        tl = "╔";
        tr = "╗";
        bl = "╚";
        br = "╝";
        hl = "═";
        vl = "║";
    }
    else
    {
        tl = "+";
        tr = "+";
        bl = "+";
        br = "+";
        hl = "-";
        vl = "|";
    }
    for (int i = 1; i < r.w - 1; i++)
    {
        ztui_set_cell(ztui_pt(r.x + i, r.y), hl, s);
        ztui_set_cell(ztui_pt(r.x + i, r.y + r.h - 1), hl, s);
    }
    for (int j = 1; j < r.h - 1; j++)
    {
        ztui_set_cell(ztui_pt(r.x, r.y + j), vl, s);
        ztui_set_cell(ztui_pt(r.x + r.w - 1, r.y + j), vl, s);
    }
    ztui_set_cell(ztui_pt(r.x, r.y), tl, s);
    ztui_set_cell(ztui_pt(r.x + r.w - 1, r.y), tr, s);
    ztui_set_cell(ztui_pt(r.x, r.y + r.h - 1), bl, s);
    ztui_set_cell(ztui_pt(r.x + r.w - 1, r.y + r.h - 1), br, s);
}

/* ── clipped draw helpers ─────────────────────────────────────────────────── */

static void _ztui_draw_str_clipped(ztui_point p, int w, const char* str, ztui_style s)
{
    int col = 0;
    for (int i = 0; str[i];)
    {
        int b = _ztui_utf8_len((unsigned char)str[i]);
        char buf[5] = {0};
        for (int j = 0; j < b && str[i + j] && j < 4; j++)
            buf[j] = str[i + j];
        int dw = _ztui_glyph_w(buf);
        if (col + dw > w)
            break;
        ztui_set_cell(ztui_pt(p.x + col, p.y), buf, s);
        i += b;
        col += dw;
    }
}

static void _ztui_draw_str_aligned(ztui_point p, int w, const char* str, ztui_text_align a,
                                   ztui_style s)
{
    int dw = _ztui_str_w(str);
    int dx = p.x;
    if (dw > w)
        dw = w;
    if (a == ZTUI_TEXT_CENTER && dw < w)
        dx = p.x + (w - dw) / 2;
    else if (a == ZTUI_TEXT_RIGHT && dw < w)
        dx = p.x + (w - dw);
    _ztui_draw_str_clipped(ztui_pt(dx, p.y), w, str, s);
}

/* ── text state helpers ───────────────────────────────────────────────────── */

void ztui_text_state_init(ztui_text_state* s, char* buf, int cap)
{
    s->buffer = buf;
    s->capacity = cap;
    s->cursor = 0;
    s->preferred_col = -1;
    if (cap > 0)
    {
        buf[cap - 1] = '\0';
        s->length = (int)strlen(buf);
    }
    else
        s->length = 0;
    s->cursor = s->length;
}

void ztui_scroll_state_init(ztui_scroll_state* s)
{
    s->offset = 0;
    s->viewport_x = s->viewport_y = 0;
    s->viewport_w = s->viewport_h = 0;
    s->content_height = 0;
    s->active = 0;
}

static void _ztui_sync_text(ztui_text_state* s)
{
    if (s->capacity <= 0)
    {
        s->length = 0;
        s->cursor = 0;
        return;
    }
    s->buffer[s->capacity - 1] = '\0';
    s->length = (int)strlen(s->buffer);
    if (s->length >= s->capacity)
        s->length = s->capacity - 1;
    s->cursor = _ztui_max(0, _ztui_min(s->cursor, s->length));
}

static int _ztui_text_insert(ztui_text_state* s, char c)
{
    if (s->length >= s->capacity - 1)
        return 0;
    memmove(&s->buffer[s->cursor + 1], &s->buffer[s->cursor], (size_t)(s->length - s->cursor + 1));
    s->buffer[s->cursor] = c;
    s->length++;
    s->cursor++;
    return 1;
}

static int _ztui_text_insert_utf8(ztui_text_state* s, const char* u)
{
    int n = 0;
    while (n < 4 && u[n])
        n++;
    if (s->length + n > s->capacity - 1)
        return 0;
    memmove(&s->buffer[s->cursor + n], &s->buffer[s->cursor], (size_t)(s->length - s->cursor + 1));
    memcpy(&s->buffer[s->cursor], u, (size_t)n);
    s->cursor += n;
    s->length += n;
    return 1;
}

static int _ztui_text_bksp(ztui_text_state* s)
{
    if (s->cursor <= 0 || s->length <= 0)
        return 0;
    int del_start = s->cursor - 1;
    while (del_start > 0 && ((unsigned char)s->buffer[del_start] & 0xC0u) == 0x80u)
        del_start--;
    int del_len = s->cursor - del_start;
    memmove(&s->buffer[del_start], &s->buffer[s->cursor], (size_t)(s->length - s->cursor + 1));
    s->cursor -= del_len;
    s->length -= del_len;
    return 1;
}

static int _ztui_text_del(ztui_text_state* s)
{
    if (s->cursor >= s->length)
        return 0;
    int b = _ztui_utf8_len((unsigned char)s->buffer[s->cursor]);
    if (b > s->length - s->cursor)
        b = s->length - s->cursor;
    memmove(&s->buffer[s->cursor], &s->buffer[s->cursor + b],
            (size_t)(s->length - s->cursor - b + 1));
    s->length -= b;
    return 1;
}

static int _ztui_word_left(const ztui_text_state* s)
{
    int p = s->cursor;
    while (p > 0 && s->buffer[p - 1] == ' ')
        p--;
    while (p > 0 && s->buffer[p - 1] != ' ')
        p--;
    return p;
}

static int _ztui_word_right(const ztui_text_state* s)
{
    int p = s->cursor;
    while (p < s->length && s->buffer[p] == ' ')
        p++;
    while (p < s->length && s->buffer[p] != ' ')
        p++;
    return p;
}

static int _ztui_linestart(const ztui_text_state* s, int pos)
{
    while (pos > 0 && s->buffer[pos - 1] != '\n')
        pos--;
    return pos;
}

static int _ztui_lineend(const ztui_text_state* s, int pos)
{
    while (pos < s->length && s->buffer[pos] != '\n')
        pos++;
    return pos;
}

static int _ztui_line_dcol(const ztui_text_state* s, int pos)
{
    int ls = _ztui_linestart(s, pos);
    return _ztui_str_w_n(s->buffer + ls, pos - ls);
}

static void _ztui_move_cursor_v(ztui_text_state* s, int dir)
{
    int cur_start = _ztui_linestart(s, s->cursor);
    int cur_col = _ztui_line_dcol(s, s->cursor);
    int tgt_start, tgt_end;
    if (s->preferred_col < 0)
        s->preferred_col = cur_col;
    if (dir < 0)
    {
        if (cur_start <= 0)
            return;
        tgt_end = cur_start - 1;
        tgt_start = _ztui_linestart(s, tgt_end);
    }
    else
    {
        tgt_start = _ztui_lineend(s, s->cursor);
        if (tgt_start >= s->length)
            return;
        tgt_start++;
        tgt_end = _ztui_lineend(s, tgt_start);
    }
    int line_len = tgt_end - tgt_start;
    int byte_off = _ztui_byte_at_col(s->buffer + tgt_start, line_len, s->preferred_col);
    s->cursor = tgt_start + byte_off;
}

static int _ztui_count_lines(const ztui_text_state* s)
{
    int n = 1;
    for (int i = 0; i < s->length; i++)
        if (s->buffer[i] == '\n')
            n++;
    return n;
}

static int _ztui_cursor_line(const ztui_text_state* s)
{
    int n = 0;
    for (int i = 0; i < s->cursor && i < s->length; i++)
        if (s->buffer[i] == '\n')
            n++;
    return n;
}

static int _ztui_line_pos_at(const ztui_text_state* s, int tl, int tc)
{
    int line = 0, pos = 0, start = 0;
    while (line < tl && pos < s->length)
    {
        if (s->buffer[pos] == '\n')
        {
            line++;
            start = pos + 1;
        }
        pos++;
    }
    pos = start;
    while (pos < s->length && s->buffer[pos] != '\n' && tc > 0)
    {
        pos++;
        tc--;
    }
    return pos;
}

/* ── ID stack / state arena ───────────────────────────────────────────────── */

#define _ZTUI_SLOTS 64
#define _ZTUI_BUF_SZ 256

typedef struct
{
    uint32_t hash;
    char buffer[_ZTUI_BUF_SZ];
    int length, cursor;
    int scroll_offset;
    int int_val;
    float float_val;
} _ztui_slot;

static _ztui_slot _ztui_slots[_ZTUI_SLOTS];
static int _ztui_slot_count = 0;
static uint32_t _ztui_id_stack[32];
static int _ztui_id_top = 0;

static uint32_t _ztui_id_hash(void)
{
    uint32_t h = 0;
    for (int i = 0; i < _ztui_id_top; i++)
        h = h * 31 + _ztui_id_stack[i];
    return h ? h : 1;
}

static _ztui_slot* _ztui_slot_find(uint32_t hash)
{
    for (int i = 0; i < _ztui_slot_count; i++)
        if (_ztui_slots[i].hash == hash)
            return &_ztui_slots[i];
    return NULL;
}

static _ztui_slot* _ztui_slot_get_or_create(uint32_t hash)
{
    _ztui_slot* s = _ztui_slot_find(hash);
    if (s)
        return s;
    if (_ztui_slot_count >= _ZTUI_SLOTS)
        return NULL;
    s = &_ztui_slots[_ztui_slot_count++];
    memset(s, 0, sizeof(*s));
    s->hash = hash;
    return s;
}

void ztui_push_id(int id)
{
    if (_ztui_id_top < 32)
        _ztui_id_stack[_ztui_id_top++] = (uint32_t)id;
}

void ztui_push_ptr(const void* p)
{
    if (_ztui_id_top < 32)
        _ztui_id_stack[_ztui_id_top++] = (uint32_t)(uintptr_t)p;
}

void ztui_push_str(const char* s)
{
    uint32_t h = 0;
    while (*s)
        h = h * 31 + (unsigned char)*s++;
    if (_ztui_id_top < 32)
        _ztui_id_stack[_ztui_id_top++] = h;
}

void ztui_pop_id(void)
{
    if (_ztui_id_top > 0)
        _ztui_id_top--;
}

/* ── auto-widgets ────────────────────────────────────────────────────────── */

static _ztui_slot* _ztui_auto_slot(void)
{
    return _ztui_slot_get_or_create(_ztui_id_hash());
}

void ztui_input_auto(ztui_rect r, const char* hint, ztui_event* ev, ztui_style n, ztui_style f)
{
    _ztui_slot* s = _ztui_auto_slot();
    if (!s)
    {
        ztui_input(r, ZTUI_AUTO_ID, NULL, ev, n, f);
        return;
    }
    ztui_text_state ts;
    ts.buffer = s->buffer;
    ts.capacity = _ZTUI_BUF_SZ;
    ts.length = s->length;
    ts.cursor = s->cursor;
    ts.preferred_col = -1;
    ztui_input(r, ZTUI_AUTO_ID, &ts, ev, n, f);
    s->length = ts.length;
    s->cursor = ts.cursor;
    (void)hint;
}

const char* ztui_input_get(ztui_rect r)
{
    (void)r;
    _ztui_slot* s = _ztui_auto_slot();
    return s ? s->buffer : "";
}

void ztui_textarea_auto(ztui_rect r, ztui_event* ev, ztui_style n, ztui_style f)
{
    _ztui_slot* s = _ztui_auto_slot();
    if (!s)
        return;
    /* build a temporary text_state */
    ztui_text_state ts;
    ts.buffer = s->buffer;
    ts.capacity = _ZTUI_BUF_SZ;
    ts.length = s->length;
    ts.cursor = s->cursor;
    ts.preferred_col = -1;
    ztui_scroll_state scroll;
    ztui_scroll_state_init(&scroll);
    scroll.offset = s->scroll_offset;
    ztui_textarea(r, ZTUI_AUTO_ID, &ts, &scroll, ev, n, f, 0);
    s->length = ts.length;
    s->cursor = ts.cursor;
    s->scroll_offset = scroll.offset;
}

const char* ztui_textarea_get(ztui_rect r)
{
    (void)r;
    _ztui_slot* s = _ztui_auto_slot();
    return s ? s->buffer : "";
}

int ztui_slider_auto(ztui_rect r, float* val, ztui_event* ev, ztui_style n, ztui_style f)
{
    _ztui_slot* s = _ztui_auto_slot();
    if (s)
    {
        s->float_val = *val;
        int changed = ztui_slider(r, ZTUI_AUTO_ID, &s->float_val, ev, n, f);
        *val = s->float_val;
        return changed;
    }
    return ztui_slider(r, ZTUI_AUTO_ID, val, ev, n, f);
}

int ztui_checkbox_auto(ztui_rect r, const char* label, int* val, ztui_event* ev, ztui_style n,
                       ztui_style f)
{
    _ztui_slot* s = _ztui_auto_slot();
    if (s)
    {
        s->int_val = *val;
        int changed = ztui_checkbox(r, ZTUI_AUTO_ID, label, &s->int_val, ev, n, f);
        *val = s->int_val;
        return changed;
    }
    return ztui_checkbox(r, ZTUI_AUTO_ID, label, val, ev, n, f);
}

/* ── scroll helpers ───────────────────────────────────────────────────────── */

static void _ztui_scroll_ensure(ztui_scroll_state* s, int item_y, int item_h)
{
    if (item_y < s->offset)
        s->offset = item_y;
    if (item_y + item_h > s->offset + s->viewport_h)
        s->offset = item_y + item_h - s->viewport_h;
    if (s->offset < 0)
        s->offset = 0;
}

/* ── word-wrap ────────────────────────────────────────────────────────────── */

static int _ztui_wrap_line(const char* text, int start, int width, int* next_start, int* out_len)
{
    int i = start, last_space = -1, last_next = -1, col = 0;
    if (width < 1)
        width = 1;
    if (!text[i])
    {
        *next_start = i;
        *out_len = 0;
        return 0;
    }
    while (text[i] == ' ')
        i++;
    start = i;
    while (text[i] && text[i] != '\n')
    {
        int b = _ztui_utf8_len((unsigned char)text[i]);
        char buf[5] = {0};
        for (int j = 0; j < b && text[i + j] && j < 4; j++)
            buf[j] = text[i + j];
        int cw = _ztui_glyph_w(buf);
        if (text[i] == ' ')
        {
            last_space = i;
            last_next = i + 1;
        }
        if (col + cw > width)
            break;
        col += cw;
        i += b;
    }
    if (text[i] == '\n')
    {
        *out_len = i - start;
        *next_start = i + 1;
        return 1;
    }
    if (!text[i])
    {
        *out_len = i - start;
        *next_start = i;
        return 1;
    }
    if (last_space >= start)
    {
        *out_len = last_space - start;
        *next_start = last_next;
        while (text[*next_start] == ' ')
            (*next_start)++;
        return 1;
    }
    *out_len = i - start;
    *next_start = i;
    return 1;
}

int ztui_text_measure(const char* text, int width)
{
    int n = 0, start = 0, next = 0, len = 0;
    while (_ztui_wrap_line(text, start, width, &next, &len))
    {
        n++;
        start = next;
        if (!text[start])
            break;
    }
    return n == 0 ? 1 : n;
}

/* ── containers ───────────────────────────────────────────────────────────── */

void ztui_panel_begin(ztui_rect r, const char* title, ztui_box_style bs, int gap)
{
    const ztui_theme* t = ztui_get_theme();
    if (_ztui_panel_top >= ZTUI_STACK_MAX)
        return;
    ztui_draw_box(r, bs, t->border);
    ztui_fill_rect(ztui_rc(r.x + 1, r.y + 1, r.w - 2, r.h - 2), " ", t->panel);
    if (title)
        ztui_draw_str(ztui_pt(r.x + 2, r.y), title, t->muted);
    _ztui_panel_layout_mark[_ztui_panel_top] = _ly_top;
    _ztui_panel_clip_mark[_ztui_panel_top] = _ztui_clip_top;
    _ztui_panel_top++;
    int inner_w = r.w - 4 < 1 ? 1 : r.w - 4;
    int inner_h = r.h - 4 < 1 ? 1 : r.h - 4;
    ztui_clip_push(ztui_rc(r.x + 1, r.y + 1, r.w - 2, r.h - 2));
    ztui_layout_begin(ztui_rc(r.x + 2, r.y + 2, inner_w, inner_h), ZTUI_LAYOUT_VERTICAL, gap);
}

void ztui_panel_end(void)
{
    if (_ztui_panel_top <= 0)
        return;
    _ztui_panel_top--;
    while (_ly_top > _ztui_panel_layout_mark[_ztui_panel_top])
        ztui_layout_end();
    while (_ztui_clip_top > _ztui_panel_clip_mark[_ztui_panel_top])
        ztui_clip_pop();
}

void ztui_scroll_begin(ztui_scroll_state* s, ztui_rect r, ztui_event* ev)
{
    s->viewport_x = r.x;
    s->viewport_y = r.y;
    s->viewport_w = r.w;
    s->viewport_h = r.h;
    s->active = 1;
    int track_x = r.x + r.w - 1;
    if (_ztui_hit(ev, r))
    {
        if (ev->type == ZTUI_EV_UP && s->offset > 0)
            s->offset--;
        if (ev->type == ZTUI_EV_DOWN)
            s->offset++;
        if (ev->type == ZTUI_EV_WHEEL_UP && s->offset > 0)
            s->offset--;
        if (ev->type == ZTUI_EV_WHEEL_DOWN)
            s->offset++;
    }
    if (ev->type == ZTUI_EV_MOUSE_DOWN && ev->mouse.x == track_x && ev->mouse.y >= r.y &&
        ev->mouse.y < r.y + r.h && s->content_height > s->viewport_h)
    {
        int thumb_h = (r.h * r.h) / s->content_height;
        if (thumb_h < 1)
            thumb_h = 1;
        int thumb_y = r.y + (s->offset * (r.h - thumb_h)) / (s->content_height - s->viewport_h);
        _ztui_drag_scroll = s;
        _ztui_drag_scroll_y0 = r.y;
        _ztui_drag_scroll_track_h = r.h;
        _ztui_drag_scroll_thumb_h = thumb_h;
        _ztui_drag_scroll_grab = ev->mouse.y - thumb_y;
    }
    if (ev->type == ZTUI_EV_MOUSE_MOVE && _ztui_drag_scroll == s &&
        s->content_height > s->viewport_h)
    {
        int thumb_y = ev->mouse.y - _ztui_drag_scroll_grab;
        int max_y = _ztui_drag_scroll_y0 + _ztui_drag_scroll_track_h - _ztui_drag_scroll_thumb_h;
        if (thumb_y < _ztui_drag_scroll_y0)
            thumb_y = _ztui_drag_scroll_y0;
        if (thumb_y > max_y)
            thumb_y = max_y;
        s->offset = ((thumb_y - _ztui_drag_scroll_y0) * (s->content_height - s->viewport_h)) /
                    (_ztui_drag_scroll_track_h - _ztui_drag_scroll_thumb_h);
    }
    if (ev->type == ZTUI_EV_MOUSE_UP && _ztui_drag_scroll == s)
        _ztui_drag_scroll = NULL;
    if (s->offset < 0)
        s->offset = 0;
    ztui_clip_push(r);
    ztui_layout_begin(ztui_rc(r.x, r.y - s->offset, r.w, 9999), ZTUI_LAYOUT_VERTICAL, 1);
}

void ztui_scroll_end(ztui_scroll_state* s)
{
    /* capture the area cursor from the scroll's layout before popping */
    int content_end = _ly.cursor;
    ztui_layout_end();
    ztui_clip_pop();
    s->active = 0;
    int content_h = (content_end - (s->viewport_y - s->offset));
    if (content_h < s->viewport_h)
        content_h = s->viewport_h;
    s->content_height = content_h;
    if (s->offset > s->content_height - s->viewport_h)
        s->offset = s->content_height - s->viewport_h;
    if (s->offset < 0)
        s->offset = 0;
    if (s->content_height > s->viewport_h && s->viewport_w > 0)
    {
        int th = s->viewport_h;
        int thumb_h = (th * s->viewport_h) / s->content_height;
        if (thumb_h < 1)
            thumb_h = 1;
        int thumb_y =
            s->viewport_y + (s->offset * (th - thumb_h)) / (s->content_height - s->viewport_h);
        for (int i = 0; i < th; i++)
        {
            int in = (i >= thumb_y - s->viewport_y && i < thumb_y - s->viewport_y + thumb_h);
            const char* g = in ? ((_ztui_drag_scroll == s) ? "▓" : "█") : "│";
            ztui_set_cell(ztui_pt(s->viewport_x + s->viewport_w - 1, s->viewport_y + i), g,
                          ztui_get_theme()->scrollbar);
        }
    }
}

void ztui_dialog_begin(int w, int h, const char* title)
{
    int sw, sh;
    ztui_get_size(&sw, &sh);
    if (w > sw - 2)
        w = sw - 2;
    if (h > sh - 2)
        h = sh - 2;
    if (w < 8)
        w = 8;
    if (h < 6)
        h = 6;
    ztui_style dim = ztui_style_make(ZTUI_BLACK, ZTUI_BLACK, ZTUI_STYLE_DIM);
    ztui_fill_rect(ztui_rc(0, 0, sw, sh), " ", dim);
    ztui_panel_begin(ztui_rc((sw - w) / 2, (sh - h) / 2, w, h), title, ZTUI_BOX_DOUBLE, 1);
}

void ztui_dialog_end(void)
{
    ztui_panel_end();
}

int ztui_dialog_confirm(const char* title, const char* msg, const char* ok, const char* cancel,
                        ztui_event* ev)
{
    int result = 0;
    ztui_dialog_begin(34, 8, title);
    ztui_text_block_aligned(ztui_rc(_ly.area.x, _ly.area.y, _ly.area.w, 3), msg, ZTUI_TEXT_CENTER,
                            ztui_get_theme()->text);
    int btns_y = _ly.area.y + _ly.area.h - 2;
    ztui_layout_begin(ztui_rc(_ly.area.x, btns_y, _ly.area.w, 1), ZTUI_LAYOUT_HORIZONTAL, 2);
    if (ztui_button(ztui_layout_next_wh(14, 1), ZTUI_AUTO_ID, ok, ev, ztui_get_theme()->danger,
                    ztui_get_theme()->danger_focus))
        result = 1;
    if (ztui_button(ztui_layout_next_wh(14, 1), ZTUI_AUTO_ID, cancel, ev, ztui_get_theme()->text,
                    ztui_get_theme()->field_focus))
        result = -1;
    ztui_layout_end();
    ztui_dialog_end();
    if (ev->type == ZTUI_EV_ESCAPE)
        result = -1;
    return result;
}

/* ── split pane ──────────────────────────────────────────────────────────── */

void ztui_split_draw(ztui_rect area, ztui_split* s, ztui_event* ev, ztui_rect* a, ztui_rect* b)
{
    int divider = 1;
    int pos = s->pos;
    if (pos < s->min_a + divider)
        pos = s->min_a + divider;
    int max_pos = (s->is_vert ? area.h : area.w) - s->min_b - divider;
    if (pos > max_pos)
        pos = max_pos;
    s->pos = pos;

    if (s->is_vert)
    {
        a->x = area.x;
        a->y = area.y;
        a->w = area.w;
        a->h = pos;
        b->x = area.x;
        b->y = area.y + pos + divider;
        b->w = area.w;
        b->h = area.h - pos - divider;
        /* draw divider */
        ztui_style div_style = ztui_style_make(ZTUI_CYAN, ZTUI_BLACK, ZTUI_STYLE_DIM);
        for (int i = 0; i < area.w; i++)
            ztui_set_cell(ztui_pt(area.x + i, area.y + pos), "─", div_style);
    }
    else
    {
        a->x = area.x;
        a->y = area.y;
        a->w = pos;
        a->h = area.h;
        b->x = area.x + pos + divider;
        b->y = area.y;
        b->w = area.w - pos - divider;
        b->h = area.h;
        ztui_style div_style = ztui_style_make(ZTUI_CYAN, ZTUI_BLACK, ZTUI_STYLE_DIM);
        for (int i = 0; i < area.h; i++)
            ztui_set_cell(ztui_pt(area.x + pos, area.y + i), "┃", div_style);
        /* divider handle */
        int hx = area.x + pos;
        int hy = area.y + area.h / 2;
        ztui_set_cell(ztui_pt(hx, hy), "╋", div_style);
    }

    /* drag logic */
    if (ev->type == ZTUI_EV_MOUSE_DOWN)
    {
        int d = s->is_vert ? ev->mouse.y : ev->mouse.x;
        int div_pos = s->is_vert ? area.y + pos : area.x + pos;
        if (d >= div_pos - 1 && d <= div_pos + 1)
            s->drag_ofs = ev->mouse.x - (area.x + pos);
    }
    if (ev->type == ZTUI_EV_MOUSE_MOVE && s->drag_ofs >= 0)
    {
        int new_pos = (s->is_vert ? ev->mouse.y : ev->mouse.x) - (s->is_vert ? area.y : area.x);
        if (new_pos < s->min_a + divider)
            new_pos = s->min_a + divider;
        if (new_pos > max_pos)
            new_pos = max_pos;
        s->pos = new_pos;
    }
    if (ev->type == ZTUI_EV_MOUSE_UP)
        s->drag_ofs = -1;
}

/* ── tab bar ──────────────────────────────────────────────────────────────── */

int ztui_tab_bar_draw(ztui_rect r, ztui_tab_bar* s, ztui_event* ev, const char* const* labels,
                      int n, ztui_style tab, ztui_style active)
{
    (void)s;
    int x = r.x;
    for (int i = 0; i < n; i++)
    {
        int w = _ztui_str_w(labels[i]) + 3; /* " %s " */
        int selected = (i == s->active);
        ztui_style st = selected ? active : tab;
        if (selected)
        {
            ztui_set_cell(ztui_pt(x, r.y), " ", st);
            ztui_draw_str(ztui_pt(x + 1, r.y), labels[i], st);
            ztui_set_cell(ztui_pt(x + w - 2, r.y), " ", st);
        }
        else
        {
            /* bracket style */
            ztui_set_cell(ztui_pt(x, r.y), " ", st);
            ztui_set_cell(ztui_pt(x + 1, r.y), "[", st);
            ztui_draw_str(ztui_pt(x + 2, r.y), labels[i], st);
            ztui_set_cell(ztui_pt(x + w - 2, r.y), "]", st);
            ztui_set_cell(ztui_pt(x + w - 1, r.y), " ", st);
        }
        if (_ztui_hit(ev, ztui_rc(x, r.y, w, 1)) && ev->type == ZTUI_EV_MOUSE_DOWN)
            s->active = i;
        x += w;
    }
    /* fill rest of row with background */
    ztui_style bg = ztui_style_make(ZTUI_BLACK, ZTUI_BLACK, 0);
    while (x < r.x + r.w)
    {
        ztui_set_cell(ztui_pt(x, r.y), " ", bg);
        x++;
    }
    return s->active;
}

/* ── status bar ───────────────────────────────────────────────────────────── */

void ztui_status_set(ztui_status* s, const char* msg, int frames)
{
    int len = (int)strlen(msg);
    if (len > 255)
        len = 255;
    memcpy(s->msg, msg, (size_t)len);
    s->msg[len] = '\0';
    s->ttl = frames;
}

void ztui_status_draw(ztui_status* s, ztui_rect r, ztui_style style)
{
    /* separator line */
    for (int i = 0; i < r.w; i++)
        ztui_set_cell(ztui_pt(r.x + i, r.y), "▔", style);
    /* message */
    if (s->ttl != 0 && s->msg[0])
    {
        _ztui_draw_str_clipped(ztui_pt(r.x, r.y + 1), r.w, s->msg, style);
        if (s->ttl > 0)
            s->ttl--;
    }
}

/* ── toast ────────────────────────────────────────────────────────────────── */

void ztui_toast_show(ztui_toast* t, const char* msg, int frames)
{
    int len = (int)strlen(msg);
    if (len > 127)
        len = 127;
    memcpy(t->msg, msg, (size_t)len);
    t->msg[len] = '\0';
    t->ttl = frames;
}

void ztui_toast_draw(ztui_toast* t)
{
    if (t->ttl == 0)
        return;
    int sw, sh;
    ztui_get_size(&sw, &sh);
    int len = _ztui_str_w(t->msg);
    int x = (sw - len - 4) / 2;
    if (x < 0)
        x = 0;
    int y = 1;
    ztui_style bg = ztui_style_make(ZTUI_WHITE, ZTUI_BLUE, ZTUI_STYLE_BOLD);
    ztui_fill_rect(ztui_rc(x, y, len + 4, 1), " ", bg);
    ztui_draw_str(ztui_pt(x + 2, y), t->msg, bg);
    if (t->ttl > 0)
        t->ttl--;
}

/* ── animation ────────────────────────────────────────────────────────────── */

void ztui_anim_init(ztui_anim* a, float val)
{
    a->current = a->target = val;
    a->speed = 0.15f;
    a->epsilon = 0.5f;
}

void ztui_anim_set(ztui_anim* a, float target)
{
    a->target = target;
}

void ztui_anim_tick(ztui_anim* a)
{
    float diff = a->target - a->current;
    if (diff > -a->epsilon && diff < a->epsilon)
    {
        a->current = a->target;
        return;
    }
    a->current += diff * a->speed;
}

int ztui_anim_done(ztui_anim* a)
{
    return (a->current > a->target - a->epsilon && a->current < a->target + a->epsilon);
}

float ztui_anim_val(ztui_anim* a)
{
    return a->current;
}

void ztui_progress_anim(ztui_rect r, int id, ztui_anim* a, ztui_style fill, ztui_style empty,
                        ztui_style focus)
{
    ztui_anim_tick(a);
    ztui_progress(r, id, a->current, fill, empty, focus);
}

/* ── popup ────────────────────────────────────────────────────────────────── */

int ztui_popup_run(ztui_popup* p, const char* const* items, int n, int x, int y, int w,
                   ztui_event* ev)
{
    if (!p->active)
        return -1;
    p->x = x;
    p->y = y;
    p->w = w;
    p->count = n;
    if (p->selected < 0)
        p->selected = 0;
    if (p->selected >= n)
        p->selected = n - 1;

    /* handle input */
    if (ev->type == ZTUI_EV_UP && p->selected > 0)
        p->selected--;
    if (ev->type == ZTUI_EV_DOWN && p->selected < n - 1)
        p->selected++;
    if (ev->type == ZTUI_EV_ENTER)
    {
        p->active = 0;
        return p->selected;
    }
    if (ev->type == ZTUI_EV_ESCAPE)
    {
        p->active = 0;
        return -1;
    }
    if (ev->type == ZTUI_EV_MOUSE_DOWN)
    {
        int row = ev->mouse.y - y - 1;
        if (row >= 0 && row < n)
        {
            p->selected = row;
            p->active = 0;
            return row;
        }
        if (ev->mouse.y < y || ev->mouse.y >= y + n + 2 || ev->mouse.x < x || ev->mouse.x >= x + w)
            p->active = 0;
    }

    /* draw */
    int popup_h = n + 2;
    ztui_draw_box(ztui_rc(x, y, w, popup_h), ZTUI_BOX_SINGLE, ztui_get_theme()->border);
    ztui_fill_rect(ztui_rc(x + 1, y + 1, w - 2, n), " ", ztui_get_theme()->panel);
    for (int i = 0; i < n; i++)
    {
        int sel = (i == p->selected);
        ztui_style s = sel ? ztui_get_theme()->selection : ztui_get_theme()->text;
        _ztui_draw_str_clipped(ztui_pt(x + 1, y + 1 + i), w - 2, items[i], s);
    }
    return -1; /* still active */
}

/* ── widgets ──────────────────────────────────────────────────────────────── */

void ztui_label(ztui_rect r, const char* text, ztui_style s)
{
    ztui_draw_str(ztui_pt(r.x, r.y), text, s);
}

void ztui_separator(ztui_rect r, ztui_style s)
{
    for (int i = 0; i < r.w; i++)
        ztui_set_cell(ztui_pt(r.x + i, r.y), "─", s);
}

int ztui_button(ztui_rect r, int id, const char* label, ztui_event* ev, ztui_style n, ztui_style f)
{
    id = _ztui_resolve_id(id);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int clicked =
        focused && (ev->type == ZTUI_EV_ENTER || (hovered && ev->type == ZTUI_EV_MOUSE_DOWN));
    ztui_draw_str(ztui_pt(r.x, r.y), label, focused ? f : n);
    return clicked;
}

int ztui_checkbox(ztui_rect r, int id, const char* label, int* val, ztui_event* ev, ztui_style n,
                  ztui_style f)
{
    char text[256];
    snprintf(text, sizeof(text), "[%c] %s", *val ? 'x' : ' ', label);
    id = _ztui_resolve_id(id);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int changed = 0;
    if (focused && (ev->type == ZTUI_EV_ENTER || (hovered && ev->type == ZTUI_EV_MOUSE_DOWN)))
    {
        *val = !*val;
        changed = 1;
        snprintf(text, sizeof(text), "[%c] %s", *val ? 'x' : ' ', label);
    }
    ztui_draw_str(ztui_pt(r.x, r.y), text, focused ? f : n);
    return changed;
}

int ztui_slider(ztui_rect r, int id, float* val, ztui_event* ev, ztui_style n, ztui_style f)
{
    id = _ztui_resolve_id(id);
    if (*val < 0.0f)
        *val = 0.0f;
    if (*val > 1.0f)
        *val = 1.0f;
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int changed = 0;
    if (focused && ev->type == ZTUI_EV_LEFT && *val > 0.0f)
    {
        *val -= 0.05f;
        changed = 1;
    }
    if (focused && ev->type == ZTUI_EV_RIGHT && *val < 1.0f)
    {
        *val += 0.05f;
        changed = 1;
    }
    if (hovered && ev->type == ZTUI_EV_MOUSE_DOWN && r.w > 1)
    {
        *val = (float)(ev->mouse.x - r.x) / (float)(r.w - 1);
        changed = 1;
    }
    *val = *val < 0.0f ? 0.0f : (*val > 1.0f ? 1.0f : *val);
    int handle = (int)(*val * (float)(r.w - 1));
    for (int i = 0; i < r.w; i++)
        ztui_set_cell(ztui_pt(r.x + i, r.y), i == handle ? "●" : "─", focused ? f : n);
    return changed;
}

void ztui_progress(ztui_rect r, int id, float prog, ztui_style fill, ztui_style empty,
                   ztui_style focus)
{
    (void)id;
    (void)focus;
    if (prog < 0.0f)
        prog = 0.0f;
    if (prog > 1.0f)
        prog = 1.0f;
    int n = (int)(r.w * prog);
    for (int i = 0; i < r.w; i++)
        ztui_set_cell(ztui_pt(r.x + i, r.y), i < n ? "█" : "░", i < n ? fill : empty);
}

/* ── single-line input ────────────────────────────────────────────────────── */

static int _ztui_input_scroll_start(ztui_text_state* s, int inner_w)
{
    int ccol = _ztui_str_w_n(s->buffer, s->cursor);
    if (ccol < inner_w)
        return 0;
    int target = ccol - inner_w + 1;
    int col = 0, i = 0;
    while (i < s->length)
    {
        int b = _ztui_utf8_len((unsigned char)s->buffer[i]);
        char buf[5] = {0};
        for (int j = 0; j < b && s->buffer[i + j] && j < 4; j++)
            buf[j] = s->buffer[i + j];
        int cw = _ztui_glyph_w(buf);
        if (col + cw > target)
            break;
        col += cw;
        i += b;
    }
    return i;
}

int ztui_input(ztui_rect r, int id, ztui_text_state* state, ztui_event* ev, ztui_style n,
               ztui_style f)
{
    if (r.w < 3)
        r.w = 3;
    id = _ztui_resolve_id(id);
    _ztui_sync_text(state);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int changed = 0;
    int inner_w = r.w - 2;
    if (hovered && ev->type == ZTUI_EV_MOUSE_DOWN)
    {
        int click_col = ev->mouse.x - r.x - 1;
        int start = _ztui_input_scroll_start(state, inner_w);
        state->cursor =
            start + _ztui_byte_at_col(state->buffer + start, state->length - start, click_col);
        state->preferred_col = -1;
    }
    if (focused)
    {
        if (ev->type == ZTUI_EV_LEFT && state->cursor > 0)
        {
            state->cursor--;
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_RIGHT && state->cursor < state->length)
        {
            state->cursor++;
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_CTRL_LEFT)
        {
            state->cursor = _ztui_word_left(state);
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_CTRL_RIGHT)
        {
            state->cursor = _ztui_word_right(state);
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_HOME)
        {
            state->cursor = 0;
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_END)
        {
            state->cursor = state->length;
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_BACKSPACE)
            changed = _ztui_text_bksp(state);
        else if (ev->type == ZTUI_EV_DELETE)
            changed = _ztui_text_del(state);
        else if (ev->key >= 32 && ev->key != 127 && ev->utf8[0])
            changed = _ztui_text_insert_utf8(state, ev->utf8);
    }
    int start_byte = _ztui_input_scroll_start(state, inner_w);
    ztui_set_cell(ztui_pt(r.x, r.y), "[", focused ? f : n);
    ztui_set_cell(ztui_pt(r.x + r.w - 1, r.y), "]", focused ? f : n);
    int col = 0, i = start_byte;
    while (i < state->length && col < inner_w)
    {
        int b = _ztui_utf8_len((unsigned char)state->buffer[i]);
        char buf[5] = {0};
        for (int j = 0; j < b && state->buffer[i + j] && j < 4; j++)
            buf[j] = state->buffer[i + j];
        int dw = _ztui_glyph_w(buf);
        if (col + dw > inner_w)
            break;
        ztui_set_cell(ztui_pt(r.x + 1 + col, r.y), buf, focused ? f : n);
        col += dw;
        i += b;
    }
    while (col < inner_w)
    {
        ztui_set_cell(ztui_pt(r.x + 1 + col, r.y), " ", focused ? f : n);
        col++;
    }
    if (focused)
    {
        int cd = _ztui_str_w_n(state->buffer, state->cursor);
        int sd = _ztui_str_w_n(state->buffer, start_byte);
        int cc = cd - sd;
        if (cc >= 0 && cc < inner_w)
        {
            char caret[2] = {' ', '\0'};
            if (state->cursor < state->length)
                caret[0] = state->buffer[state->cursor];
            ztui_set_cell(ztui_pt(r.x + 1 + cc, r.y), caret,
                          ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD));
        }
    }
    return changed;
}

/* ── text area ────────────────────────────────────────────────────────────── */

int ztui_textarea(ztui_rect r, int id, ztui_text_state* state, ztui_scroll_state* scroll,
                  ztui_event* ev, ztui_style n, ztui_style f, int flags)
{
    if (r.w < 3)
        r.w = 3;
    if (r.h < 2)
        r.h = 2;
    int no_undo = (flags & ZTUI_TEXTAREA_NO_UNDO) != 0;
    int line_nums = (flags & ZTUI_TEXTAREA_LINE_NUMBERS) != 0;
    int gutter = line_nums ? 4 : 0;
    int text_w = r.w - gutter;
    if (text_w < 1)
        text_w = 1;

    id = _ztui_resolve_id(id);
    _ztui_sync_text(state);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int changed = 0;
    if (focused && ev->type != ZTUI_EV_UP && ev->type != ZTUI_EV_DOWN)
        state->preferred_col = -1;
    if (hovered && ev->type == ZTUI_EV_MOUSE_DOWN)
    {
        int click_line = scroll->offset + (ev->mouse.y - r.y);
        int click_col = ev->mouse.x - (r.x + gutter);
        if (click_col < 0)
            click_col = 0;
        state->cursor = _ztui_line_pos_at(state, click_line, click_col);
        state->preferred_col = -1;
    }
    if (focused)
    {
        if (!no_undo && ev->type == ZTUI_EV_KEY && (ev->key == 26 || ev->key == 25))
        {
            /* Ctrl+Z (26) = undo, Ctrl+Y (25) = redo */
            if (ev->key == 26)
                changed = _ztui_undo_apply(state);
            else
                changed = _ztui_redo_apply(state);
        }
        else
        {
            if (ev->type == ZTUI_EV_LEFT && state->cursor > 0)
            {
                state->cursor--;
            }
            else if (ev->type == ZTUI_EV_RIGHT && state->cursor < state->length)
            {
                state->cursor++;
            }
            else if (ev->type == ZTUI_EV_UP)
                _ztui_move_cursor_v(state, -1);
            else if (ev->type == ZTUI_EV_DOWN)
                _ztui_move_cursor_v(state, 1);
            else if (ev->type == ZTUI_EV_PAGE_UP)
            {
                int tl = _ztui_cursor_line(state) - r.h;
                if (tl < 0)
                    tl = 0;
                if (state->preferred_col < 0)
                    state->preferred_col = _ztui_line_dcol(state, state->cursor);
                state->cursor = _ztui_line_pos_at(state, tl, state->preferred_col);
            }
            else if (ev->type == ZTUI_EV_PAGE_DOWN)
            {
                int tl = _ztui_cursor_line(state) + r.h;
                int ml = _ztui_count_lines(state) - 1;
                if (tl > ml)
                    tl = ml;
                if (state->preferred_col < 0)
                    state->preferred_col = _ztui_line_dcol(state, state->cursor);
                state->cursor = _ztui_line_pos_at(state, tl, state->preferred_col);
            }
            else if (ev->type == ZTUI_EV_CTRL_LEFT)
            {
                state->cursor = _ztui_word_left(state);
                state->preferred_col = -1;
            }
            else if (ev->type == ZTUI_EV_CTRL_RIGHT)
            {
                state->cursor = _ztui_word_right(state);
                state->preferred_col = -1;
            }
            else if (ev->type == ZTUI_EV_CTRL_HOME)
            {
                state->cursor = 0;
                state->preferred_col = -1;
            }
            else if (ev->type == ZTUI_EV_CTRL_END)
            {
                state->cursor = state->length;
                state->preferred_col = -1;
            }
            else if (ev->type == ZTUI_EV_HOME)
            {
                state->cursor = _ztui_linestart(state, state->cursor);
                state->preferred_col = -1;
            }
            else if (ev->type == ZTUI_EV_END)
            {
                state->cursor = _ztui_lineend(state, state->cursor);
                state->preferred_col = -1;
            }
            else if (ev->type == ZTUI_EV_BACKSPACE)
            {
                if (!no_undo)
                {
                    char del[8] = {0};
                    int dlen = 0;
                    if (state->cursor > 0)
                    {
                        dlen = 1;
                        if (state->cursor > 1 &&
                            ((unsigned char)state->buffer[state->cursor - 1] & 0xC0u) == 0x80u)
                        {
                            dlen = 2;
                            if (state->cursor > 2 &&
                                ((unsigned char)state->buffer[state->cursor - 2] & 0xC0u) == 0x80u)
                                dlen = 3;
                        }
                        memcpy(del, &state->buffer[state->cursor - dlen], (size_t)dlen);
                    }
                    int old_cursor = state->cursor;
                    changed = _ztui_text_bksp(state);
                    if (changed)
                        _ztui_undo_push(old_cursor - dlen, dlen, 0, del);
                }
                else
                {
                    changed = _ztui_text_bksp(state);
                }
            }
            else if (ev->type == ZTUI_EV_DELETE)
            {
                if (!no_undo)
                {
                    char del[8] = {0};
                    int dlen = state->cursor < state->length
                                   ? _ztui_utf8_len((unsigned char)state->buffer[state->cursor])
                                   : 0;
                    if (dlen > 0)
                        memcpy(del, &state->buffer[state->cursor], (size_t)dlen);
                    changed = _ztui_text_del(state);
                    if (changed)
                        _ztui_undo_push(state->cursor, dlen, 0, del);
                }
                else
                {
                    changed = _ztui_text_del(state);
                }
            }
            else if (ev->type == ZTUI_EV_ENTER)
            {
                if (!no_undo)
                    _ztui_undo_push(state->cursor, 0, 1, "\n");
                changed = _ztui_text_insert(state, '\n');
            }
            else if (ev->key >= 32 && ev->key != 127 && ev->utf8[0])
            {
                if (!no_undo)
                    _ztui_undo_push(state->cursor, 0, (int)strlen(ev->utf8), ev->utf8);
                changed = _ztui_text_insert_utf8(state, ev->utf8);
            }
            if (changed && !no_undo)
            {
                /* for backspace/delete we already pushed above */
            }
        }
    }
    int cursor_line = _ztui_cursor_line(state);
    int line_count = _ztui_count_lines(state);
    scroll->viewport_h = r.h;
    _ztui_scroll_ensure(scroll, cursor_line, 1);
    ztui_scroll_begin(scroll, r, ev);
    int pos = 0;
    for (int line = 0; line < line_count; line++)
    {
        int ls = pos, le = _ztui_lineend(state, pos);
        int len = le - ls;
        char buf[512];
        if (len > (int)sizeof(buf) - 1)
            len = (int)sizeof(buf) - 1;
        memcpy(buf, state->buffer + ls, (size_t)len);
        buf[len] = '\0';

        /* line number gutter */
        if (line_nums)
        {
            int draw_line = line + 1;
            char num[16];
            snprintf(num, sizeof(num), "%3d", draw_line);
            ztui_style gs = (line == cursor_line)
                                ? ztui_style_make(ZTUI_YELLOW, ZTUI_BLACK, ZTUI_STYLE_BOLD)
                                : ztui_style_make(ZTUI_CYAN, ZTUI_BLACK, ZTUI_STYLE_DIM);
            ztui_set_cell(ztui_pt(r.x, r.y - scroll->offset + line), " ", gs);
            ztui_draw_str(ztui_pt(r.x + 1, r.y - scroll->offset + line), num, gs);
        }

        int draw_x = r.x + gutter;
        _ztui_draw_str_clipped(ztui_pt(draw_x, r.y - scroll->offset + line), text_w - 1, buf,
                               focused ? f : n);
        if (focused && line == cursor_line)
        {
            int cc = _ztui_str_w_n(state->buffer + ls, state->cursor - ls);
            if (cc >= 0 && cc < text_w - 1)
            {
                char caret[2] = {' ', '\0'};
                if (state->cursor < le)
                    caret[0] = state->buffer[state->cursor];
                ztui_set_cell(ztui_pt(draw_x + cc, r.y - scroll->offset + line), caret,
                              ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD));
            }
        }
        pos = le;
        if (pos < state->length && state->buffer[pos] == '\n')
            pos++;
    }
    ztui_scroll_end(scroll);
    return changed;
}

/* ── list ─────────────────────────────────────────────────────────────────── */

int ztui_list(ztui_rect r, int id, const char* const* items, int n, int* sel,
              ztui_scroll_state* scroll, ztui_event* ev, ztui_style nrm, ztui_style selsty,
              ztui_style f)
{
    id = _ztui_resolve_id(id);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int activated = 0;
    if (n <= 0)
        return 0;
    if (*sel < 0)
        *sel = 0;
    if (*sel >= n)
        *sel = n - 1;
    if (focused && ev->type == ZTUI_EV_UP && *sel > 0)
        (*sel)--;
    if (focused && ev->type == ZTUI_EV_DOWN && *sel < n - 1)
        (*sel)++;
    if (focused && ev->type == ZTUI_EV_ENTER)
        activated = 1;
    if (hovered && ev->type == ZTUI_EV_MOUSE_DOWN)
    {
        int row = scroll->offset + (ev->mouse.y - r.y);
        if (row >= 0 && row < n)
        {
            *sel = row;
            activated = 1;
        }
    }
    scroll->viewport_h = r.h;
    _ztui_scroll_ensure(scroll, *sel, 1);
    ztui_scroll_begin(scroll, r, ev);
    for (int i = 0; i < n; i++)
    {
        ztui_style s = (i == *sel) ? (focused ? f : selsty) : nrm;
        ztui_draw_str(ztui_pt(r.x, r.y - scroll->offset + i), items[i], s);
    }
    ztui_scroll_end(scroll);
    return activated;
}

/* ── table ────────────────────────────────────────────────────────────────── */

static void _ztui_col_bounds(const ztui_column* cols, int nc, int tw, int ci, int* ox, int* ow)
{
    int fixed = 0, flex = 0, cur = 0;
    for (int i = 0; i < nc; i++)
    {
        if (cols && cols[i].width > 0)
            fixed += cols[i].width;
        else
            flex++;
    }
    if (fixed > tw)
        fixed = tw;
    int fw = flex > 0 ? (tw - fixed) / flex : 0;
    for (int i = 0; i < nc; i++)
    {
        int cw = (cols && cols[i].width > 0) ? cols[i].width : fw;
        if (i == nc - 1)
            cw = tw - cur;
        if (i == ci)
        {
            *ox = cur;
            *ow = cw;
            return;
        }
        cur += cw;
    }
    *ox = 0;
    *ow = tw;
}

int ztui_table(ztui_rect r, int id, const char* const* headers, const ztui_column* cols, int nc,
               const char* const* cells, int nr, int* sel, ztui_scroll_state* scroll,
               ztui_event* ev, ztui_style hdr, ztui_style cell, ztui_style selsty, ztui_style f)
{
    int row_h = r.h - 1 < 1 ? 1 : r.h - 1;
    if (nc < 1)
        return 0;
    id = _ztui_resolve_id(id);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int activated = 0;
    if (nr <= 0)
        return 0;
    if (*sel < 0)
        *sel = 0;
    if (*sel >= nr)
        *sel = nr - 1;
    if (focused && ev->type == ZTUI_EV_UP && *sel > 0)
        (*sel)--;
    if (focused && ev->type == ZTUI_EV_DOWN && *sel < nr - 1)
        (*sel)++;
    if (focused && ev->type == ZTUI_EV_ENTER)
        activated = 1;
    if (hovered && ev->type == ZTUI_EV_MOUSE_DOWN && ev->mouse.y > r.y)
    {
        int row = scroll->offset + (ev->mouse.y - r.y - 1);
        if (row >= 0 && row < nr)
        {
            *sel = row;
            activated = 1;
        }
    }
    for (int c = 0; c < nc; c++)
    {
        int co = 0, cw = 0;
        ztui_text_align a = cols ? cols[c].align : ZTUI_TEXT_LEFT;
        _ztui_col_bounds(cols, nc, r.w - 1, c, &co, &cw);
        _ztui_draw_str_aligned(ztui_pt(r.x + co, r.y), cw, headers[c], a, hdr);
    }
    _ztui_scroll_ensure(scroll, *sel, 1);
    ztui_scroll_begin(scroll, ztui_rc(r.x, r.y + 1, r.w, row_h), ev);
    for (int row = 0; row < nr; row++)
    {
        ztui_style rs = (row == *sel) ? (focused ? f : selsty) : cell;
        for (int c = 0; c < nc; c++)
        {
            int co = 0, cw = 0;
            ztui_text_align a = cols ? cols[c].align : ZTUI_TEXT_LEFT;
            _ztui_col_bounds(cols, nc, r.w - 1, c, &co, &cw);
            _ztui_draw_str_aligned(ztui_pt(r.x + co, r.y + 1 - scroll->offset + row), cw,
                                   cells[row * nc + c], a, rs);
        }
    }
    ztui_scroll_end(scroll);
    return activated;
}

/* ── text block ───────────────────────────────────────────────────────────── */

void ztui_text_block_aligned(ztui_rect r, const char* text, ztui_text_align a, ztui_style s)
{
    int start = 0, next = 0, line_len = 0, line = 0;
    if (r.w < 1)
        r.w = 1;
    while (_ztui_wrap_line(text, start, r.w, &next, &line_len))
    {
        char buf[512];
        if (line_len > (int)sizeof(buf) - 1)
            line_len = (int)sizeof(buf) - 1;
        memcpy(buf, text + start, (size_t)line_len);
        buf[line_len] = '\0';
        int dw = _ztui_str_w(buf);
        int dx = r.x;
        if (a == ZTUI_TEXT_CENTER && dw < r.w)
            dx = r.x + (r.w - dw) / 2;
        else if (a == ZTUI_TEXT_RIGHT && dw < r.w)
            dx = r.x + (r.w - dw);
        ztui_draw_str(ztui_pt(dx, r.y + line), buf, s);
        line++;
        start = next;
        if (!text[start])
            break;
    }
}

void ztui_text_block(ztui_rect r, const char* text, ztui_style s)
{
    ztui_text_block_aligned(r, text, ZTUI_TEXT_LEFT, s);
}

/* ── dropdown ─────────────────────────────────────────────────────────────── */

int ztui_dropdown(ztui_rect r, int id, const char* const* items, int n, int* sel, ztui_event* ev,
                  ztui_style nrm, ztui_style f, ztui_style list)
{
    id = _ztui_resolve_id(id);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int changed = 0;
    if (*sel < 0)
        *sel = 0;
    if (*sel >= n)
        *sel = n - 1;
    char buf[256];
    snprintf(buf, sizeof(buf), "[ %s \u25BC", items[*sel]);
    ztui_draw_str(ztui_pt(r.x, r.y), buf, focused ? f : nrm);
    if (focused && (ev->type == ZTUI_EV_ENTER || (hovered && ev->type == ZTUI_EV_MOUSE_DOWN)))
    {
        int list_h = n < 8 ? n : 8;
        ztui_fill_rect(ztui_rc(r.x, r.y + 1, r.w, list_h), " ", focused ? f : list);
        for (int i = 0; i < n && i < list_h; i++)
        {
            ztui_style is = (i == *sel) ? f : list;
            _ztui_draw_str_clipped(ztui_pt(r.x, r.y + 1 + i), r.w, items[i], is);
        }
        if (ev->type == ZTUI_EV_UP && *sel > 0)
        {
            (*sel)--;
            changed = 1;
        }
        if (ev->type == ZTUI_EV_DOWN && *sel < n - 1)
        {
            (*sel)++;
            changed = 1;
        }
        if (ev->type == ZTUI_EV_ENTER)
            changed = 1;
    }
    return changed;
}

/* ── radio ────────────────────────────────────────────────────────────────── */

int ztui_radio(ztui_rect r, int id, const char* label, int group_id, int* val, int this_val,
               ztui_event* ev, ztui_style nrm, ztui_style f)
{
    (void)group_id;
    char buf[256];
    snprintf(buf, sizeof(buf), "(%c) %s", *val == this_val ? '*' : ' ', label);
    id = _ztui_resolve_id(id);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int changed = 0;
    if (focused && (ev->type == ZTUI_EV_ENTER || (hovered && ev->type == ZTUI_EV_MOUSE_DOWN)))
    {
        *val = this_val;
        changed = 1;
    }
    ztui_draw_str(ztui_pt(r.x, r.y), buf, focused ? f : nrm);
    return changed;
}

/* ── spinner ──────────────────────────────────────────────────────────────── */

void ztui_spinner(ztui_rect r, int id, int frame, ztui_style s)
{
    (void)id;
    static const char* frames = "|/-\\";
    char buf[2] = {frames[frame & 3], '\0'};
    ztui_draw_str(ztui_pt(r.x, r.y), buf, s);
}

/* ── autocomplete ─────────────────────────────────────────────────────────── */

int ztui_autocomplete(ztui_rect r, int id, ztui_text_state* state, const char* const* items, int n,
                      ztui_event* ev, ztui_style nrm, ztui_style foc, ztui_style list)
{
    if (r.w < 3)
        r.w = 3;
    id = _ztui_resolve_id(id);
    _ztui_sync_text(state);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int changed = 0;
    int inner_w = r.w - 2;

    /* input handling */
    if (focused)
    {
        if (ev->type == ZTUI_EV_LEFT && state->cursor > 0)
        {
            state->cursor--;
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_RIGHT && state->cursor < state->length)
        {
            state->cursor++;
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_HOME)
        {
            state->cursor = 0;
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_END)
        {
            state->cursor = state->length;
            state->preferred_col = -1;
        }
        else if (ev->type == ZTUI_EV_BACKSPACE)
            changed = _ztui_text_bksp(state);
        else if (ev->type == ZTUI_EV_DELETE)
            changed = _ztui_text_del(state);
        else if (ev->key >= 32 && ev->key != 127 && ev->utf8[0])
            changed = _ztui_text_insert_utf8(state, ev->utf8);
    }

    /* draw input */
    int start_byte = 0;
    if (state->length > 0)
    {
        int ccol = _ztui_str_w_n(state->buffer, state->cursor);
        if (ccol >= inner_w)
        {
            int target = ccol - inner_w + 1, col = 0, i = 0;
            while (i < state->length)
            {
                int b = _ztui_utf8_len((unsigned char)state->buffer[i]);
                char buf[5] = {0};
                for (int j = 0; j < b && state->buffer[i + j] && j < 4; j++)
                    buf[j] = state->buffer[i + j];
                int cw = _ztui_glyph_w(buf);
                if (col + cw > target)
                    break;
                col += cw;
                i += b;
            }
            start_byte = i;
        }
    }

    ztui_set_cell(ztui_pt(r.x, r.y), "[", focused ? foc : nrm);
    ztui_set_cell(ztui_pt(r.x + r.w - 1, r.y), "]", focused ? foc : nrm);

    int col = 0, i = start_byte;
    while (i < state->length && col < inner_w)
    {
        int b = _ztui_utf8_len((unsigned char)state->buffer[i]);
        char buf[5] = {0};
        for (int j = 0; j < b && state->buffer[i + j] && j < 4; j++)
            buf[j] = state->buffer[i + j];
        int dw = _ztui_glyph_w(buf);
        if (col + dw > inner_w)
            break;
        ztui_set_cell(ztui_pt(r.x + 1 + col, r.y), buf, focused ? foc : nrm);
        col += dw;
        i += b;
    }
    while (col < inner_w)
    {
        ztui_set_cell(ztui_pt(r.x + 1 + col, r.y), " ", focused ? foc : nrm);
        col++;
    }

    /* cursor */
    if (focused)
    {
        int cc = _ztui_str_w_n(state->buffer, state->cursor);
        int sc = _ztui_str_w_n(state->buffer, start_byte);
        int cx = cc - sc;
        if (cx >= 0 && cx < inner_w)
        {
            char caret[2] = {' ', '\0'};
            if (state->cursor < state->length)
                caret[0] = state->buffer[state->cursor];
            ztui_set_cell(ztui_pt(r.x + 1 + cx, r.y), caret,
                          ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD));
        }
    }

    /* dropdown list */
    if (focused && state->length > 0)
    {
        int match = 0, first = -1;
        for (int j = 0; j < n; j++)
        {
            if (strncmp(items[j], state->buffer, (size_t)state->length) == 0)
            {
                if (first < 0)
                    first = j;
                match++;
            }
        }
        if (match > 0)
        {
            int dh = match < 8 ? match : 8;
            int dy = r.y + 1;
            ztui_draw_box(ztui_rc(r.x, dy, r.w, dh + 2), ZTUI_BOX_SINGLE, list);
            ztui_fill_rect(ztui_rc(r.x + 1, dy + 1, r.w - 2, dh), " ", list);
            int drawn = 0;
            for (int j = 0; j < n && drawn < dh; j++)
            {
                if (strncmp(items[j], state->buffer, (size_t)state->length) == 0)
                {
                    _ztui_draw_str_clipped(ztui_pt(r.x + 1, dy + 1 + drawn), r.w - 2, items[j],
                                           list);
                    drawn++;
                }
            }
            /* click to select */
            if (ev->type == ZTUI_EV_MOUSE_DOWN && ev->mouse.y > r.y && ev->mouse.y < r.y + 1 + dh &&
                ev->mouse.x >= r.x && ev->mouse.x < r.x + r.w)
            {
                int row = ev->mouse.y - dy - 1;
                int drawn2 = 0;
                for (int j = 0; j < n; j++)
                {
                    if (strncmp(items[j], state->buffer, (size_t)state->length) == 0)
                    {
                        if (drawn2 == row)
                        {
                            state->cursor = (int)strlen(items[j]);
                            if (state->cursor >= state->capacity)
                                state->cursor = state->capacity - 1;
                            memcpy(state->buffer, items[j], (size_t)state->cursor);
                            state->buffer[state->cursor] = '\0';
                            state->length = state->cursor;
                            changed = 1;
                            break;
                        }
                        drawn2++;
                    }
                }
            }
        }
    }

    return changed;
}

/* ── tree ──────────────────────────────────────────────────────────────────── */

int ztui_tree(ztui_rect r, int id, const char* const* labels, const int* indents, int nn,
              int* expanded, int* selected, ztui_scroll_state* scroll, ztui_event* ev,
              ztui_style nrm, ztui_style sel, ztui_style foc, ztui_style sb)
{
    (void)sb;
    id = _ztui_resolve_id(id);
    int hovered = _ztui_hit(ev, r);
    int focused = _ztui_focused(id, hovered, ev);
    int activated = 0;
    if (nn <= 0)
        return 0;
    if (*selected < 0)
        *selected = 0;
    if (*selected >= nn)
        *selected = nn - 1;

    if (focused)
    {
        if (ev->type == ZTUI_EV_UP && *selected > 0)
            (*selected)--;
        if (ev->type == ZTUI_EV_DOWN && *selected < nn - 1)
            (*selected)++;
        if (ev->type == ZTUI_EV_RIGHT && expanded[*selected])
            expanded[*selected] = 1;
        if (ev->type == ZTUI_EV_LEFT)
            expanded[*selected] = 0;
        if (ev->type == ZTUI_EV_ENTER)
            activated = 1;
    }
    if (hovered && ev->type == ZTUI_EV_MOUSE_DOWN)
    {
        int row = scroll->offset + (ev->mouse.y - r.y);
        if (row >= 0 && row < nn)
        {
            *selected = row;
            activated = 1;
        }
    }

    scroll->viewport_h = r.h;
    _ztui_scroll_ensure(scroll, *selected, 1);
    ztui_scroll_begin(scroll, r, ev);

    for (int i = 0; i < nn; i++)
    {
        int indent = indents ? indents[i] : 0;
        ztui_point pt = {r.x + indent * 2, r.y - scroll->offset + i};
        int has_children = (i + 1 < nn && indents && indents[i + 1] > indents[i]);

        /* expand/collapse icon */
        ztui_style icon_s = (i == *selected && focused) ? foc : nrm;
        if (has_children)
            ztui_draw_str(pt, expanded && expanded[i] ? "▾ " : "▸ ", icon_s);
        else
            ztui_draw_str(pt, "  ", icon_s);

        /* label */
        ztui_style label_s = (i == *selected) ? (focused ? foc : sel) : nrm;
        ztui_draw_str(ztui_pt(pt.x + 2, pt.y), labels[i], label_s);
    }

    ztui_scroll_end(scroll);
    return activated;
}

/* ── frame flush (buffered, diff-based) ───────────────────────────────────── */

static void _ztui_frame_flush(void)
{
    ztui_color cur_fg = 0;
    ztui_color cur_bg = 0;
    int cur_bits = 0;
    int total = _ztui_width * _ztui_height;

    for (int i = 0; i < total; i++)
    {
        if (_ztui_back[i].width == 0)
        {
            _ztui_front[i] = _ztui_back[i];
            continue;
        }
        int dirty = _ztui_full_repaint || strcmp(_ztui_front[i].glyph, _ztui_back[i].glyph) != 0 ||
                    _ztui_front[i].fg != _ztui_back[i].fg ||
                    _ztui_front[i].bg != _ztui_back[i].bg ||
                    _ztui_front[i].bits != _ztui_back[i].bits ||
                    _ztui_front[i].width != _ztui_back[i].width;
        if (!dirty)
            continue;

        int y = i / _ztui_width;
        int x = i % _ztui_width;

        /* cursor position */
        _ztui_out_str("\x1b[");
        _ztui_out_int(y + 1);
        _ztui_out_char(';');
        _ztui_out_int(x + 1);
        _ztui_out_char('H');

        /* style and colour (single SGR sequence) */
        if (cur_fg != _ztui_back[i].fg || cur_bg != _ztui_back[i].bg ||
            cur_bits != (int)_ztui_back[i].bits)
        {
            _ztui_out_str("\x1b[0");
            if (_ztui_back[i].bits & ZTUI_STYLE_BOLD)
                _ztui_out_str(";1");
            if (_ztui_back[i].bits & ZTUI_STYLE_DIM)
                _ztui_out_str(";2");
            if (_ztui_back[i].bits & ZTUI_STYLE_ITALIC)
                _ztui_out_str(";3");
            if (_ztui_back[i].bits & ZTUI_STYLE_UNDERLINE)
                _ztui_out_str(";4");
            _ztui_out_char(';');
            _ztui_emit_fg(_ztui_back[i].fg);
            _ztui_out_char(';');
            _ztui_emit_bg(_ztui_back[i].bg);
            _ztui_out_char('m');
            cur_fg = _ztui_back[i].fg;
            cur_bg = _ztui_back[i].bg;
            cur_bits = (int)_ztui_back[i].bits;
        }

        _ztui_out_str(_ztui_back[i].glyph);
        _ztui_front[i] = _ztui_back[i];

        if (_ztui_back[i].width == 2 && i + 1 < total)
            _ztui_front[i + 1] = _ztui_back[i + 1];
    }
    _ztui_out_flush();
    _ztui_full_repaint = 0;
}

/* ════════════════════════════════════════════════════════════════════════════ */
/* PLATFORM LAYER                                                             */
/* ════════════════════════════════════════════════════════════════════════════ */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
static HANDLE _hIn, _hOut;
static DWORD _origIn, _origOut;
static UINT _origCP;

void ztui_get_size(int* w, int* h)
{
    CONSOLE_SCREEN_BUFFER_INFO c;
    GetConsoleScreenBufferInfo(_hOut, &c);
    *w = c.srWindow.Right - c.srWindow.Left + 1;
    *h = c.srWindow.Bottom - c.srWindow.Top + 1;
}

void ztui_cleanup(void)
{
    _ztui_out_str("\x1b[0m\x1b[?25h\x1b[?1049l\x1b[?1003l\x1b[?1006l");
    _ztui_out_flush();
    SetConsoleMode(_hIn, _origIn);
    SetConsoleMode(_hOut, _origOut);
    SetConsoleOutputCP(_origCP);
}

ztui_error ztui_init(void)
{
    _hIn = GetStdHandle(STD_INPUT_HANDLE);
    _hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(_hIn, &_origIn);
    GetConsoleMode(_hOut, &_origOut);
    _origCP = GetConsoleOutputCP();
    atexit(ztui_cleanup);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleMode(_hOut, _origOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleMode(_hIn, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
    _ztui_truecolor_ok = ztui_has_truecolor();
    _ztui_out_str("\x1b[?1049h\x1b[?25l\x1b[?1003h\x1b[?1006h");
    _ztui_out_flush();
    ztui_get_size(&_ztui_width, &_ztui_height);
    _ztui_grow_buffers(_ztui_width, _ztui_height);
    _ztui_full_repaint = 1;
    _ztui_sync_theme();
    return _ztui_last_err;
}

static int _ztui_cp_utf8(uint32_t cp, char* out)
{
    if (cp < 0x80u)
    {
        out[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800u)
    {
        out[0] = (char)(0xC0u | (cp >> 6));
        out[1] = (char)(0x80u | (cp & 0x3Fu));
        return 2;
    }
    if (cp < 0x10000u)
    {
        out[0] = (char)(0xE0u | (cp >> 12));
        out[1] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
        out[2] = (char)(0x80u | (cp & 0x3Fu));
        return 3;
    }
    out[0] = (char)(0xF0u | (cp >> 18));
    out[1] = (char)(0x80u | ((cp >> 12) & 0x3Fu));
    out[2] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
    out[3] = (char)(0x80u | (cp & 0x3Fu));
    return 4;
}

int ztui_poll(ztui_event* ev)
{
    memset(ev, 0, sizeof(*ev));
    ev->mouse.x = _ztui_mouse_x;
    ev->mouse.y = _ztui_mouse_y;
    DWORD evts;
    INPUT_RECORD ir;
    GetNumberOfConsoleInputEvents(_hIn, &evts);
    if (evts == 0)
        return 0;
    ReadConsoleInput(_hIn, &ir, 1, &evts);
    if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown)
    {
        WORD vk = ir.Event.KeyEvent.wVirtualKeyCode;
        DWORD mods = ir.Event.KeyEvent.dwControlKeyState;
        if (vk == VK_ESCAPE)
            ev->type = ZTUI_EV_ESCAPE;
        else if (vk == VK_UP)
            ev->type = ZTUI_EV_UP;
        else if (vk == VK_DOWN)
            ev->type = ZTUI_EV_DOWN;
        else if (vk == VK_LEFT && (mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)))
            ev->type = ZTUI_EV_CTRL_LEFT;
        else if (vk == VK_RIGHT && (mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)))
            ev->type = ZTUI_EV_CTRL_RIGHT;
        else if (vk == VK_HOME && (mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)))
            ev->type = ZTUI_EV_CTRL_HOME;
        else if (vk == VK_END && (mods & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)))
            ev->type = ZTUI_EV_CTRL_END;
        else if (vk == VK_LEFT)
            ev->type = ZTUI_EV_LEFT;
        else if (vk == VK_RIGHT)
            ev->type = ZTUI_EV_RIGHT;
        else if (vk == VK_HOME)
            ev->type = ZTUI_EV_HOME;
        else if (vk == VK_END)
            ev->type = ZTUI_EV_END;
        else if (vk == VK_PRIOR)
            ev->type = ZTUI_EV_PAGE_UP;
        else if (vk == VK_NEXT)
            ev->type = ZTUI_EV_PAGE_DOWN;
        else if (vk == VK_DELETE)
            ev->type = ZTUI_EV_DELETE;
        else if (vk == VK_RETURN)
            ev->type = ZTUI_EV_ENTER;
        else if (vk == VK_TAB)
        {
            ev->type = ZTUI_EV_TAB;
            _ztui_focus_advance();
        }
        else if (vk == VK_BACK)
            ev->type = ZTUI_EV_BACKSPACE;
        else
        {
            WCHAR wc = ir.Event.KeyEvent.uChar.UnicodeChar;
            if (wc >= 32)
            {
                ev->type = ZTUI_EV_KEY;
                uint32_t cp = (uint32_t)wc;
                int n = _ztui_cp_utf8(cp, ev->utf8);
                ev->utf8[n] = '\0';
                ev->key = (int)cp;
            }
        }
    }
    else if (ir.EventType == MOUSE_EVENT)
    {
        _ztui_mouse_x = ir.Event.MouseEvent.dwMousePosition.X;
        _ztui_mouse_y = ir.Event.MouseEvent.dwMousePosition.Y;
        ev->mouse.x = _ztui_mouse_x;
        ev->mouse.y = _ztui_mouse_y;
        if (ir.Event.MouseEvent.dwEventFlags == MOUSE_MOVED)
            ev->type = ZTUI_EV_MOUSE_MOVE;
        else if (ir.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED)
        {
            short delta = (short)HIWORD(ir.Event.MouseEvent.dwButtonState);
            ev->type = (delta > 0) ? ZTUI_EV_WHEEL_UP : ZTUI_EV_WHEEL_DOWN;
        }
        else if (ir.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
            ev->type = ZTUI_EV_MOUSE_DOWN;
        else
            ev->type = ZTUI_EV_MOUSE_UP;
    }
    return 1;
}

int ztui_poll_timeout(ztui_event* ev, int ms)
{
    if (ms < 0)
    {
        /* block: wait for input */
        while (ztui_poll(ev) == 0)
        {
            Sleep(1);
        }
        return 1;
    }
    if (ms == 0)
        return ztui_poll(ev);
    /* timeout: poll with sleep */
    for (int i = 0; i < ms; i++)
    {
        if (ztui_poll(ev))
            return 1;
        Sleep(1);
    }
    return 0;
}

#else /* Unix */

#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

static struct termios _orig_tio;

static void _ztui_sig_winch(int sig)
{
    (void)sig;
    _ztui_resized = 1;
}

static void _ztui_sig_cont(int sig)
{
    (void)sig;
    _ztui_full_repaint = 1;
}

static void _ztui_sig_term(int sig)
{
    ztui_cleanup();
    signal(sig, SIG_DFL);
    raise(sig);
}

void ztui_get_size(int* w, int* h)
{
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    *w = ws.ws_col;
    *h = ws.ws_row;
}

void ztui_cleanup(void)
{
    _ztui_out_str("\x1b[0m\x1b[?25h\x1b[?1049l\x1b[?1003l\x1b[?1006l");
    _ztui_out_flush();
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &_orig_tio);
}

ztui_error ztui_init(void)
{
    if (!isatty(STDOUT_FILENO) || !isatty(STDIN_FILENO))
    {
        _ztui_set_err(ZTUI_ERR_TERMINAL);
        return _ztui_last_err;
    }
    struct termios raw;
    tcgetattr(STDIN_FILENO, &_orig_tio);
    atexit(ztui_cleanup);
    raw = _orig_tio;
    raw.c_lflag &= (tcflag_t) ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = _ztui_sig_winch;
    sigaction(SIGWINCH, &sa, NULL);
    sa.sa_handler = _ztui_sig_cont;
    sigaction(SIGCONT, &sa, NULL);
    sa.sa_handler = _ztui_sig_term;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    _ztui_truecolor_ok = ztui_has_truecolor();
    _ztui_out_str("\x1b[?1049h\x1b[?25l\x1b[?1003h\x1b[?1006h");
    _ztui_out_flush();
    ztui_get_size(&_ztui_width, &_ztui_height);
    _ztui_grow_buffers(_ztui_width, _ztui_height);
    _ztui_full_repaint = 1;
    _ztui_sync_theme();
    return _ztui_last_err;
}

static int _ztui_kbhit(void)
{
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

static int _ztui_select_timeout(int ms)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    int maxfd = STDIN_FILENO;
    for (int i = 0; i < _ztui_extra_fd_count; i++)
    {
        FD_SET(_ztui_extra_fds[i].fd, &fds);
        if (_ztui_extra_fds[i].fd > maxfd)
            maxfd = _ztui_extra_fds[i].fd;
    }
    if (ms < 0)
        return select(maxfd + 1, &fds, NULL, NULL, NULL);
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    return select(maxfd + 1, &fds, NULL, NULL, &tv);
}

static void _ztui_fire_extra_fds(void)
{
    for (int i = 0; i < _ztui_extra_fd_count; i++)
    {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(_ztui_extra_fds[i].fd, &fds);
        struct timeval tv = {0, 0};
        if (select(_ztui_extra_fds[i].fd + 1, &fds, NULL, NULL, &tv) > 0 &&
            FD_ISSET(_ztui_extra_fds[i].fd, &fds))
            _ztui_extra_fds[i].cb(_ztui_extra_fds[i].fd, _ztui_extra_fds[i].arg);
    }
}

static int _ztui_stdin_ready(void)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    struct timeval tv = {0, 0};
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

static int _ztui_parse_escape(ztui_event* ev)
{
    char seq[32] = {0};
    int len = 0;
    if (!_ztui_kbhit())
    {
        ev->type = ZTUI_EV_ESCAPE;
        return 1;
    }
    if (read(STDIN_FILENO, &seq[len], 1) != 1)
        return 0;
    len++;
    if (seq[0] == 'O')
    {
        if (!_ztui_kbhit() || read(STDIN_FILENO, &seq[len], 1) != 1)
            return 0;
        if (seq[len] == 'H')
            ev->type = ZTUI_EV_HOME;
        else if (seq[len] == 'F')
            ev->type = ZTUI_EV_END;
        return 1;
    }
    if (seq[0] != '[')
    {
        ev->type = ZTUI_EV_ESCAPE;
        return 1;
    }
    while (len < 31 && _ztui_kbhit())
    {
        if (read(STDIN_FILENO, &seq[len], 1) != 1)
            break;
        if ((seq[len] >= 'A' && seq[len] <= 'Z') || seq[len] == '~' || seq[len] == 'm' ||
            seq[len] == 'M')
        {
            len++;
            break;
        }
        len++;
    }
    seq[len] = '\0';
    if (strcmp(seq, "[A") == 0)
        ev->type = ZTUI_EV_UP;
    else if (strcmp(seq, "[B") == 0)
        ev->type = ZTUI_EV_DOWN;
    else if (strcmp(seq, "[C") == 0)
        ev->type = ZTUI_EV_RIGHT;
    else if (strcmp(seq, "[D") == 0)
        ev->type = ZTUI_EV_LEFT;
    else if (strcmp(seq, "[H") == 0 || strcmp(seq, "[1~") == 0 || strcmp(seq, "[7~") == 0)
        ev->type = ZTUI_EV_HOME;
    else if (strcmp(seq, "[F") == 0 || strcmp(seq, "[4~") == 0 || strcmp(seq, "[8~") == 0)
        ev->type = ZTUI_EV_END;
    else if (strcmp(seq, "[5~") == 0)
        ev->type = ZTUI_EV_PAGE_UP;
    else if (strcmp(seq, "[6~") == 0)
        ev->type = ZTUI_EV_PAGE_DOWN;
    else if (strcmp(seq, "[3~") == 0)
        ev->type = ZTUI_EV_DELETE;
    else if (strcmp(seq, "[1;5C") == 0 || strcmp(seq, "[5C") == 0)
        ev->type = ZTUI_EV_CTRL_RIGHT;
    else if (strcmp(seq, "[1;5D") == 0 || strcmp(seq, "[5D") == 0)
        ev->type = ZTUI_EV_CTRL_LEFT;
    else if (strcmp(seq, "[1;5H") == 0)
        ev->type = ZTUI_EV_CTRL_HOME;
    else if (strcmp(seq, "[1;5F") == 0)
        ev->type = ZTUI_EV_CTRL_END;
    else if (seq[1] == '<')
    {
        int btn = 0, mx = 0, my = 0;
        sscanf(&seq[2], "%d;%d;%d", &btn, &mx, &my);
        _ztui_mouse_x = mx - 1;
        _ztui_mouse_y = my - 1;
        ev->mouse.x = _ztui_mouse_x;
        ev->mouse.y = _ztui_mouse_y;
        if (seq[len - 1] == 'm')
            ev->type = ZTUI_EV_MOUSE_UP;
        else if (btn == 64)
            ev->type = ZTUI_EV_WHEEL_UP;
        else if (btn == 65)
            ev->type = ZTUI_EV_WHEEL_DOWN;
        else if (btn == 35)
            ev->type = ZTUI_EV_MOUSE_MOVE;
        else if (btn == 0)
            ev->type = ZTUI_EV_MOUSE_DOWN;
        else
            ev->type = ZTUI_EV_MOUSE_MOVE;
    }
    else
        ev->type = ZTUI_EV_ESCAPE;
    return 1;
}

int ztui_poll(ztui_event* ev)
{
    return ztui_poll_timeout(ev, 0);
}

int ztui_poll_timeout(ztui_event* ev, int ms)
{
    memset(ev, 0, sizeof(*ev));
    ev->mouse.x = _ztui_mouse_x;
    ev->mouse.y = _ztui_mouse_y;

    int n = _ztui_select_timeout(ms);
    if (n <= 0)
        return 0;

    /* fire extra FD callbacks */
    _ztui_fire_extra_fds();

    /* check stdin */
    if (!_ztui_stdin_ready())
        return 1; /* event from FD, not stdin */

    char c;
    if (read(STDIN_FILENO, &c, 1) != 1)
        return 0;
    if (c == '\n' || c == '\r')
        ev->type = ZTUI_EV_ENTER;
    else if (c == '\t')
    {
        ev->type = ZTUI_EV_TAB;
        _ztui_focus_advance();
    }
    else if ((unsigned char)c == 127u || c == '\b')
        ev->type = ZTUI_EV_BACKSPACE;
    else if (c == '\x1b')
        _ztui_parse_escape(ev);
    else if ((unsigned char)c >= 0x80u)
    {
        ev->type = ZTUI_EV_KEY;
        int n = _ztui_utf8_len((unsigned char)c);
        ev->utf8[0] = c;
        int got = 1;
        while (got < n && got < 4 && _ztui_kbhit())
        {
            if (read(STDIN_FILENO, &ev->utf8[got], 1) != 1)
                break;
            got++;
        }
        ev->utf8[got] = '\0';
        int bytes;
        ev->key = (int)_ztui_utf8_decode(ev->utf8, &bytes);
    }
    else
    {
        ev->type = ZTUI_EV_KEY;
        ev->key = (unsigned char)c;
        ev->utf8[0] = c;
        ev->utf8[1] = '\0';
    }
    return 1;
}

#endif /* Unix */

/* ── resize ───────────────────────────────────────────────────────────────── */

int ztui_was_resized(void)
{
    int r = _ztui_resized;
    _ztui_resized = 0;
    return r;
}

/* ── frame begin / end ────────────────────────────────────────────────────── */

void ztui_frame_begin(ztui_color bg)
{
    int w = 0, h = 0;
    _ztui_sync_theme();
    ztui_get_size(&w, &h);
    if (w != _ztui_width || h != _ztui_height)
    {
        _ztui_grow_buffers(w, h);
        _ztui_full_repaint = 1;
        _ztui_resized = 1;
    }
    for (int i = 0; i < _ztui_width * _ztui_height; i++)
    {
        _ztui_back[i].glyph[0] = ' ';
        _ztui_back[i].glyph[1] = '\0';
        _ztui_back[i].fg = ZTUI_WHITE;
        _ztui_back[i].bg = bg;
        _ztui_back[i].bits = 0;
        _ztui_back[i].width = 1;
    }
    _ztui_auto_counter = 0;
    _ztui_auto_max = 0;
    _ztui_slot_count = 0;
    _ztui_id_top = 0;
}

void ztui_frame_end(void)
{
    if (_ztui_auto_max > 0)
        ztui_focus_max(_ztui_auto_max);
    _ztui_frame_flush();
}

#endif /* ZTUI_IMPLEMENTATION */

/* ── C++ wrapper ──────────────────────────────────────────────────────────── */

#ifdef __cplusplus

namespace ztui
{

// RAII wrapper: calls ztui_init() in constructor, ztui_cleanup() in destructor.
struct app
{
    app()
    {
        ztui_init();
    }
    ~app()
    {
        ztui_cleanup();
    }
    app(const app&) = delete;
    app& operator=(const app&) = delete;
};

} // namespace ztui

#endif /* __cplusplus */

#endif /* ZTUI_H */
