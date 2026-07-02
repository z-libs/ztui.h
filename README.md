# ztui.h

`ztui.h` is a modern, single-header TUI library for C projects. Designed for immediate-mode UI development, it offers layout management, widgets, containers, and terminal abstraction while remaining pure C.

It includes a minimal **C++11/17 wrapper** for mixed codebases, and a complete **vim-like editor demo** as a reference implementation.

## Features

* **Immediate-Mode API**: No widget tree, no callbacks, no event loop to own. Draw and respond to input in a single loop.
* **Flex Layout System**: Stack-based single-pass allocator with vertical/horizontal direction, fill, weight, justify-content, and align-items.
* **Double-Buffered Rendering**: Diff-based flush sends only changed cells to the terminal, minimizing output bandwidth.
* **Truecolor Support**: Automatic detection from `$COLORTERM` with fallback to 256-color ANSI palette.
* **Mouse Support**: SGR extended mode, scrollbar dragging, click-to-position in text areas.
* **Signal-Safe**: SIGWINCH (resize), SIGCONT (full repaint after Ctrl+Z), SIGINT/SIGTERM (clean terminal restore).
* **UTF-8 Aware**: Multi-byte decoding, wide-character (CJK/emoji) display width, cursor navigation by display column.
* **ID-Based State Arena**: Auto-managed widget state via PushID/PopID stack (ImGui-style), eliminating user-managed state.
* **Animation**: Simple float interpolation for smooth progress bars and transitions.
* **Async Input**: Non-blocking event polling with `ztui_poll_timeout()` and FD registration for background I/O.
* **C++ Support**: RAII wrapper with `ztui::app` for automatic init/cleanup.
* **Zero Dependencies**: Only standard C headers and POSIX termios (Unix) or Win32 API (Windows).

## Installation

### C/C++ (Header Only)

`ztui` uses the standard implementation pattern.

1.  Copy `ztui.h` (and `zcommon.h` if separated) into your project's include directory.
2.  Define `ZTUI_IMPLEMENTATION` in exactly one `.c` file before including.
3.  Include it in your code.

```bash
cc -Wall -Wextra -std=c11 -o myapp myapp.c
```

## Usage: C

```c
#define ZTUI_IMPLEMENTATION
#include "ztui.h"

int main(void)
{
    if (ztui_init() != ZTUI_OK) return 1;

    int running = 1;
    float value = 0.4f;

    while (running) {
        ztui_event ev;
        ztui_frame_begin(ZTUI_BLACK);
        ztui_poll(&ev);

        if (ev.type == ZTUI_EV_ESCAPE) running = 0;

        ztui_panel_begin(ztui_rc(0, 0, 32, 10), " Demo ", ZTUI_BOX_DOUBLE, 1);
        ztui_label_next("Hello from ztui");
        ztui_progress_next(value);
        ztui_slider_next(&value, &ev);
        ztui_panel_end();

        ztui_frame_end();
    }

    ztui_cleanup();
    return 0;
}
```

## Usage: C++

The library detects C++ compilers automatically. RAII is provided via the `ztui::app` class.

```cpp
#define ZTUI_IMPLEMENTATION
#include "ztui.h"

int main()
{
    ztui::app tui;

    int running = 1;
    float value = 0.4f;

    while (running) {
        ztui_event ev;
        ztui_frame_begin(ZTUI_BLACK);
        ztui_poll(&ev);

        if (ev.type == ZTUI_EV_ESCAPE) running = 0;

        ztui_panel_begin(ztui_rc(0, 0, 32, 10), " Demo ", ZTUI_BOX_DOUBLE, 1);
        ztui_label_next("Hello from ztui");
        ztui_progress_next(value);
        ztui_slider_next(&value, &ev);
        ztui_panel_end();

        ztui_frame_end();
    }

    return 0;
}
```

## API Reference (C)

**Lifecycle**

| Function | Description |
| :--- | :--- |
| `ztui_init()` | Initialise terminal (alternate screen, raw mode, signals). Returns `ztui_error` |
| `ztui_cleanup()` | Restore terminal to original state |
| `ztui_get_error()` | Return and clear last error code |

**Frame**

| Function | Description |
| :--- | :--- |
| `ztui_frame_begin(bg)` | Clear back buffer, detect resize, reset state arena |
| `ztui_frame_end()` | Flush dirty cells to terminal, advance focus |
| `ztui_get_size(w, h)` | Get terminal dimensions |
| `ztui_was_resized()` | Check if terminal was resized last frame |
| `ztui_poll(ev)` | Read input event (blocking). Returns 1 if event consumed |
| `ztui_poll_timeout(ev, ms)` | Read input with timeout (0 = non-blocking, -1 = block) |
| `ztui_add_fd(fd, cb, arg)` | Register FD for background polling |

**Drawing Primitives**

| Function | Description |
| :--- | :--- |
| `ztui_set_cell(p, glyph, style)` | Set a single cell at point p |
| `ztui_draw_str(p, str, style)` | Draw a UTF-8 string at point p |
| `ztui_fill_rect(r, glyph, style)` | Fill a rectangle with a glyph |
| `ztui_draw_box(r, bs, style)` | Draw a box border |

**Layout**

| Function | Description |
| :--- | :--- |
| `ztui_layout_begin(area, dir, gap)` | Push a layout context (V or H) |
| `ztui_layout_begin_ex(area, dir, gap, justify, align, wrap)` | Full flex layout control |
| `ztui_layout_end()` | Pop layout context |
| `ztui_layout_next()` | Allocate 1-row slot, fill cross-axis |
| `ztui_layout_next_h(h)` | Allocate h-row slot |
| `ztui_layout_next_wh(w, h)` | Allocate exact w x h slot |
| `ztui_layout_next_fill()` | Allocate remaining space |
| `ztui_layout_next_opts(opts)` | Allocate with constraints (fill, weight, min/max) |
| `ztui_layout_cursor()` | Current position along advance axis |
| `ztui_layout_remaining()` | Space remaining along advance axis |

**Containers**

| Function | Description |
| :--- | :--- |
| `ztui_panel_begin(r, title, bs, gap)` | Draw box + title, begin layout on inner area |
| `ztui_panel_end()` | End panel, restore layout/clip |
| `ztui_scroll_begin(s, r, ev)` | Begin scrollable viewport with wheel/drag support |
| `ztui_scroll_end(s)` | End scroll area, draw scrollbar |
| `ztui_dialog_begin(w, h, title)` | Dim screen, centre box, begin layout |
| `ztui_dialog_end()` | End dialog layout |
| `ztui_dialog_confirm(title, msg, ok, cancel, ev)` | Convenience confirm/cancel dialog |

**High-Level Containers**

| Function | Description |
| :--- | :--- |
| `ztui_split_draw(area, s, ev, a, b)` | Draw draggable H/V split pane, return child rects |
| `ztui_tab_bar_draw(r, s, ev, labels, n, tab, active)` | Draw clickable tab bar |
| `ztui_status_set(s, msg, ttl)` | Set status bar message with auto-clear |
| `ztui_status_draw(s, r, style)` | Draw status bar with separator |
| `ztui_toast_show(t, msg, ttl)` | Show floating notification overlay |
| `ztui_toast_draw(t)` | Draw toast, decrement TTL |

**Widgets**

| Function | Description |
| :--- | :--- |
| `ztui_label(r, text, style)` | Draw a label |
| `ztui_separator(r, style)` | Draw a horizontal separator |
| `ztui_button(r, id, label, ev, n, f)` | Clickable button. Returns 1 on activation |
| `ztui_checkbox(r, id, label, val, ev, n, f)` | Toggle checkbox. Returns 1 on change |
| `ztui_slider(r, id, val, ev, n, f)` | Adjustable slider. Returns 1 on change |
| `ztui_progress(r, id, pct, fill, empty, focus)` | Static progress bar |
| `ztui_input(r, id, state, ev, n, f)` | Single-line text input. Returns 1 on change |
| `ztui_textarea(r, id, state, scroll, ev, n, f, flags)` | Multi-line text editor with undo/redo and line numbers |
| `ztui_list(r, id, items, n, sel, scroll, ev, ...)` | Selectable list. Returns 1 on activation |
| `ztui_table(r, id, headers, cols, nc, cells, nr, sel, scroll, ev, ...)` | Scrollable table with selection |
| `ztui_text_block(r, text, style)` | Wrapped text block |
| `ztui_dropdown(r, id, items, n, sel, ev, ...)` | Dropdown selection |
| `ztui_radio(r, id, label, group, val, this, ev, ...)` | Radio button. Returns 1 on change |
| `ztui_spinner(r, id, frame, style)` | Animated spinner character |
| `ztui_autocomplete(r, id, state, items, n, ev, ...)` | Input with filtered dropdown |
| `ztui_tree(r, id, labels, indents, n, expanded, sel, scroll, ev, ...)` | Collapsible tree |

**ID Stack / Auto-State**

| Function | Description |
| :--- | :--- |
| `ztui_push_id(id)` | Push integer onto ID stack |
| `ztui_push_ptr(p)` | Push pointer onto ID stack |
| `ztui_push_str(s)` | Push string hash onto ID stack |
| `ztui_pop_id()` | Pop from ID stack |
| `ztui_input_auto(r, hint, ev, n, f)` | Input with auto-managed state |
| `ztui_input_get(r)` | Get current text from auto-input |
| `ztui_textarea_auto(r, ev, n, f)` | Textarea with auto-managed state |
| `ztui_slider_auto(r, val, ev, n, f)` | Slider with auto-managed state |
| `ztui_checkbox_auto(r, label, val, ev, n, f)` | Checkbox with auto-managed state |

**Convenience Macros**

| Macro | Expands To |
| :--- | :--- |
| `ZTUI_LABEL(t)` | `ztui_label(ztui_layout_next(), t, theme->text)` |
| `ZTUI_BUTTON(l, ev)` | `ztui_button_next(l, ev)` |
| `ZTUI_SLIDER(v, ev)` | `ztui_slider_next(v, ev)` |
| `ZTUI_PROGRESS(v)` | `ztui_progress_next(v)` |
| `ZTUI_CHECKBOX(l, v, ev)` | `ztui_checkbox_next(l, v, ev)` |
| `ZTUI_INPUT(s, ev)` | `ztui_input_next(s, ev)` |
| `ZTUI_FIELD(label, w)` | Opens horizontal row with right-aligned label |
| `ZTUI_FIELD_END` | Closes field row |

**Animation**

| Function | Description |
| :--- | :--- |
| `ztui_anim_init(a, val)` | Initialise animation at value |
| `ztui_anim_set(a, target)` | Set animation target |
| `ztui_anim_tick(a)` | Advance animation one frame |
| `ztui_anim_done(a)` | Returns 1 if current ~= target |
| `ztui_anim_val(a)` | Returns current animated value |
| `ztui_progress_anim(r, id, a, fill, empty, focus)` | Animated progress bar |

**Popup / Context Menu**

| Function | Description |
| :--- | :--- |
| `ztui_popup_run(p, items, n, x, y, w, ev)` | Run popup menu. Returns selected index or -1 |

**Theme**

| Function | Description |
| :--- | :--- |
| `ztui_style_make(fg, bg, bits)` | Construct a style from foreground, background, and style bits |
| `ztui_default_theme()` | Returns the default theme |
| `ztui_set_theme(t)` | Set the active theme |
| `ztui_get_theme()` | Returns pointer to active theme |

## Events

```
ZTUI_EV_NONE, ZTUI_EV_ESCAPE, ZTUI_EV_ENTER, ZTUI_EV_TAB, ZTUI_EV_BACKSPACE,
ZTUI_EV_DELETE, ZTUI_EV_HOME, ZTUI_EV_END,
ZTUI_EV_PAGE_UP, ZTUI_EV_PAGE_DOWN,
ZTUI_EV_LEFT, ZTUI_EV_RIGHT, ZTUI_EV_UP, ZTUI_EV_DOWN,
ZTUI_EV_CTRL_LEFT, ZTUI_EV_CTRL_RIGHT,
ZTUI_EV_CTRL_HOME, ZTUI_EV_CTRL_END,
ZTUI_EV_MOUSE_DOWN, ZTUI_EV_MOUSE_UP, ZTUI_EV_MOUSE_MOVE,
ZTUI_EV_WHEEL_UP, ZTUI_EV_WHEEL_DOWN,
ZTUI_EV_KEY
```

Printable characters are reported as `ZTUI_EV_KEY` with the Unicode codepoint in `ev.key` and raw UTF-8 bytes in `ev.utf8`. Mouse coordinates are in `ev.mouse`.

## Memory Management

By default, `ztui.h` uses `Z_MALLOC`, `Z_REALLOC`, `Z_CALLOC`, and `Z_FREE` (which default to the standard C library functions).

You can override these macros **before** including `ztui.h` to use custom allocators:

```c
#define Z_MALLOC(sz)      my_pool_alloc(sz)
#define Z_FREE(p)         my_pool_free(p)

#include "ztui.h"
```

## Notes

### Implementation Pattern

`ztui.h` uses the single-header implementation pattern. Define `ZTUI_IMPLEMENTATION` in exactly one `.c` or `.cpp` file before including the header. Other files can include the header normally to access the API.

### Terminal Requirements

A modern terminal emulator with ANSI escape code support is required. This includes most terminals in use today: Alacritty, Kitty, GNOME Terminal, Konsole, iTerm2, Windows Terminal, and Visual Studio Code integrated terminal. Legacy terminals (vt100, xterm without xterm-256color) are not supported.

### Signal Handling

`ztui_init()` installs handlers for SIGWINCH (terminal resize), SIGCONT (full repaint after Ctrl+Z), and SIGINT/SIGTERM (clean terminal restore). This behaviour can be disabled by defining `ZTUI_NO_SIGNALS` before including the header.

### Layout Debugging

Define `ZTUI_LAYOUT_DEBUG` before including the header to draw coloured outlines around every allocated layout rect. Each nesting level uses a different colour, making layout composition visible.

### Examples

The `examples/` directory contains working programs:

- `minimal.c` -- smallest usable event loop
- `forms.c` -- form layout with input, slider, checkbox
- `table_dialog.c` -- scrollable table with confirm dialog
- `dashboard.c` -- live system monitor with async polling, split panes, tab bar
- `editor.c` -- simple text editor with file I/O and line numbers
- `tinyvim.c` -- full vim-like editor (927 lines) with modal editing, syntax highlighting, search, undo/redo
