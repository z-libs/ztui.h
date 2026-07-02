#define ZTUI_IMPLEMENTATION
#include "../ztui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUF_CAP (4 * 1048576)

typedef enum
{
    NORMAL,
    INSERT,
    VISUAL,
    VLINE,
    COMMAND
} Mode;

typedef struct
{
    char* buf;
    int cap, len, cur, col, top;
    Mode mode;
    int sel_start, sel_active;
    char reg[8192];
    int reg_len;
    int dirty;
    char file[256];
    int cmd_pos;
    char cmd_buf[256];
    int cmd_len;
    char search[256];
    int search_len;
    int search_dir;
    int op;
    int cnt;
    int ins_start;
    char ins_text[512];
    int ins_len;

} Editor;

static Editor e;
static ztui_status e_status;

static int bol(int p)
{
    while (p > 0 && e.buf[p - 1] != '\n')
        p--;
    return p;
}
static int eol(int p)
{
    while (p < e.len && e.buf[p] != '\n')
        p++;
    return p;
}
static int line_of(int p)
{
    int n = 0;
    for (int i = 0; i < p && i < e.len; i++)
        if (e.buf[i] == '\n')
            n++;
    return n;
}
static int byte_of(int line)
{
    int n = 0, p = 0;
    while (n < line && p < e.len)
    {
        if (e.buf[p] == '\n')
            n++;
        p++;
    }
    return p;
}
static int line_count(void)
{
    if (e.len == 0)
        return 1;
    int n = 1;
    for (int i = 0; i < e.len; i++)
        if (e.buf[i] == '\n')
            n++;
    return n;
}
static int col_of(int p)
{
    int s = bol(p);
    return _ztui_str_w_n(e.buf + s, p - s);
}

static void cur_to(int p)
{
    if (p < 0)
        p = 0;
    if (p > e.len)
        p = e.len;
    e.cur = p;
    e.col = col_of(p);
}

/* ── text operations ───────────────────────────────────────────────────── */

static void insert_at(int pos, const char* s, int n)
{
    if (e.len + n >= e.cap)
        return;
    memmove(e.buf + pos + n, e.buf + pos, (size_t)(e.len - pos + 1));
    memcpy(e.buf + pos, s, (size_t)n);
    e.len += n;
}

static void delete_at(int pos, int n)
{
    if (pos + n > e.len)
        n = e.len - pos;
    memmove(e.buf + pos, e.buf + pos + n, (size_t)(e.len - pos - n + 1));
    e.len -= n;
}

static void push_undo(int pos, int del, int ins, const char* txt)
{
    _ztui_undo_push(pos, del, ins, txt);
}

static void undo(void)
{
    ztui_text_state ts = {e.buf, e.cap, e.len, e.cur, -1};
    if (_ztui_undo_apply(&ts))
    {
        e.len = ts.length;
        cur_to(ts.cursor);
        e.dirty = 1;
    }
}

static void redo(void)
{
    ztui_text_state ts = {e.buf, e.cap, e.len, e.cur, -1};
    if (_ztui_redo_apply(&ts))
    {
        e.len = ts.length;
        cur_to(ts.cursor);
        e.dirty = 1;
    }
}

/* ── utf8-safe char classification ────────────────────────────────────── */

static int is_word_char(unsigned char c)
{
    return isalnum(c) || c == '_';
}

/* ── motions ────────────────────────────────────────────────────────────── */

static int m_h(int cnt)
{
    int p = e.cur;
    while (cnt-- > 0 && p > 0)
    {
        int b = 1;
        while (p - b > 0 && ((unsigned char)e.buf[p - b] & 0xC0u) == 0x80u)
            b++;
        p -= b;
    }
    return p;
}

static int m_l(int cnt)
{
    int p = e.cur;
    while (cnt-- > 0 && p < e.len)
    {
        int b = _ztui_utf8_len((unsigned char)e.buf[p]);
        p += b;
    }
    return p;
}

static int m_j(int cnt)
{
    int c = e.col;
    int l = line_of(e.cur);
    int target = l + cnt;
    if (target >= line_count())
        target = line_count() - 1;
    int bp = byte_of(target);
    int be = eol(bp);
    int bc = 0;
    while (bc < c && bp + bc < be)
    {
        int b = _ztui_utf8_len((unsigned char)e.buf[bp + bc]);
        if (bp + bc + b > be)
            break;
        bc += b;
    }
    return bp + bc;
}

static int m_k(int cnt)
{
    int c = e.col;
    int l = line_of(e.cur);
    int target = l - cnt;
    if (target < 0)
        target = 0;
    int bp = byte_of(target);
    int be = eol(bp);
    int bc = 0;
    while (bc < c && bp + bc < be)
    {
        int b = _ztui_utf8_len((unsigned char)e.buf[bp + bc]);
        if (bp + bc + b > be)
            break;
        bc += b;
    }
    return bp + bc;
}

static int m_0(void)
{
    return bol(e.cur);
}
static int m_dollar(void)
{
    return eol(e.cur);
}
static int m_hat(void)
{
    int p = bol(e.cur);
    while (p < e.len && (e.buf[p] == ' ' || e.buf[p] == '\t'))
        p++;
    return p;
}

static int m_G(int cnt)
{
    if (cnt <= 0)
        return line_count() > 0 ? eol(e.len - 1) : 0;
    cnt--;
    if (cnt >= line_count())
        cnt = line_count() - 1;
    return byte_of(cnt);
}

static int m_gg(int cnt)
{
    if (cnt <= 0)
        cnt = 1;
    cnt--;
    if (cnt >= line_count())
        cnt = line_count() - 1;
    return byte_of(cnt);
}

static int m_w(int cnt)
{
    int p = e.cur;
    while (cnt-- > 0)
    {
        if (p >= e.len)
            break;
        while (p < e.len && is_word_char((unsigned char)e.buf[p]))
            p += _ztui_utf8_len((unsigned char)e.buf[p]);
        while (p < e.len && !is_word_char((unsigned char)e.buf[p]) && e.buf[p] != ' ' &&
               e.buf[p] != '\t' && e.buf[p] != '\n')
            p += _ztui_utf8_len((unsigned char)e.buf[p]);
        while (p < e.len && (e.buf[p] == ' ' || e.buf[p] == '\t'))
            p += _ztui_utf8_len((unsigned char)e.buf[p]);
    }
    return p;
}

static int m_b(int cnt)
{
    int p = e.cur;
    while (cnt-- > 0 && p > 0)
    {
        int step = 1;
        while (p - step > 0 && ((unsigned char)e.buf[p - step] & 0xC0u) == 0x80u)
            step++;
        p -= step;
        if (p <= 0)
        {
            p = 0;
            break;
        }
        while (p > 0 && (e.buf[p] == ' ' || e.buf[p] == '\t' || e.buf[p] == '\n'))
        {
            step = 1;
            while (p - step > 0 && ((unsigned char)e.buf[p - step] & 0xC0u) == 0x80u)
                step++;
            p -= step;
            if (p <= 0)
            {
                p = 0;
                break;
            }
        }
        if (p <= 0)
            break;
        while (p > 0 && !is_word_char((unsigned char)e.buf[p]) && e.buf[p] != ' ' &&
               e.buf[p] != '\t' && e.buf[p] != '\n')
        {
            step = 1;
            while (p - step > 0 && ((unsigned char)e.buf[p - step] & 0xC0u) == 0x80u)
                step++;
            p -= step;
            if (p <= 0)
            {
                p = 0;
                break;
            }
        }
        if (p <= 0)
            break;
        while (p > 0 && is_word_char((unsigned char)e.buf[p]))
        {
            step = 1;
            while (p - step > 0 && ((unsigned char)e.buf[p - step] & 0xC0u) == 0x80u)
                step++;
            p -= step;
            if (p <= 0)
            {
                p = 0;
                break;
            }
        }
        if (p > 0 && !is_word_char((unsigned char)e.buf[p]))
        {
            step = 1;
            while (p - step > 0 && ((unsigned char)e.buf[p - step] & 0xC0u) == 0x80u)
                step++;
            p += step;
        }
    }
    if (p < 0)
        p = 0;
    return p;
}

static int m_e(int cnt)
{
    int p = e.cur;
    if (p >= e.len)
        return p;
    while (cnt-- > 0)
    {
        if (p >= e.len)
            break;
        int b = _ztui_utf8_len((unsigned char)e.buf[p]);
        p += b;
        if (p >= e.len)
            break;
        if (is_word_char((unsigned char)e.buf[p]))
        {
            while (p < e.len && is_word_char((unsigned char)e.buf[p]))
                p += _ztui_utf8_len((unsigned char)e.buf[p]);
            if (p > 0)
            {
                int step = 1;
                while (p - step > 0 && ((unsigned char)e.buf[p - step] & 0xC0u) == 0x80u)
                    step++;
                p -= step;
            }
        }
        else
        {
            while (p < e.len && !is_word_char((unsigned char)e.buf[p]) && e.buf[p] != ' ' &&
                   e.buf[p] != '\t' && e.buf[p] != '\n')
                p += _ztui_utf8_len((unsigned char)e.buf[p]);
            if (p > 0)
            {
                int step = 1;
                while (p - step > 0 && ((unsigned char)e.buf[p - step] & 0xC0u) == 0x80u)
                    step++;
                p -= step;
            }
        }
    }
    return p;
}

static int m_percent(void)
{
    int p = e.cur;
    if (p >= e.len)
        return p;
    char c = e.buf[p];
    char match;
    if (c == '(')
        match = ')';
    else if (c == ')')
        match = '(';
    else if (c == '[')
        match = ']';
    else if (c == ']')
        match = '[';
    else if (c == '{')
        match = '}';
    else if (c == '}')
        match = '{';
    else
        return p;
    int dir = (c == '(' || c == '[' || c == '{') ? 1 : -1;
    int depth = 1;
    while (p >= 0 && p < e.len)
    {
        p += dir;
        if (p < 0 || p >= e.len)
            break;
        if (e.buf[p] == c)
            depth++;
        if (e.buf[p] == match)
        {
            depth--;
            if (depth == 0)
                return p;
        }
    }
    return e.cur;
}

/* ── operator + motion ──────────────────────────────────────────────────── */

static void apply_motion_and_op(char key, int do_op)
{
    int cnt = e.cnt > 0 ? e.cnt : 1;
    int new_cur = e.cur, start = e.cur;
    int is_line_op = 0;

    if (key == 'j')
    {
        new_cur = m_j(cnt);
        is_line_op = 1;
    }
    else if (key == 'k')
    {
        new_cur = m_k(cnt);
        is_line_op = 1;
    }
    else if (key == 'h')
    {
        new_cur = m_h(cnt);
    }
    else if (key == 'l')
    {
        new_cur = m_l(cnt);
    }
    else if (key == 'w')
    {
        new_cur = m_w(cnt);
    }
    else if (key == 'b')
    {
        new_cur = m_b(cnt);
    }
    else if (key == 'e')
    {
        new_cur = m_e(cnt);
    }
    else if (key == '0')
    {
        new_cur = m_0();
    }
    else if (key == '$')
    {
        new_cur = m_dollar();
        is_line_op = 1;
    }
    else if (key == '^')
    {
        new_cur = m_hat();
    }
    else if (key == 'G')
    {
        new_cur = m_G(cnt);
        is_line_op = 1;
    }
    else if (key == '%')
    {
        new_cur = m_percent();
    }
    else if (key == 'g')
    {
        new_cur = m_gg(cnt);
        is_line_op = 1;
    }

    if (do_op && do_op != 'y' && do_op != 'Y')
    {
        push_undo(start, 0, 0, NULL);
        char del_buf[4096];
        int del_len = 0;
        if (is_line_op)
        {
            int s = bol(start), nw = new_cur;
            int ee = eol(nw);
            if (ee < e.len)
                ee++;
            del_len = ee - s;
            if (del_len > 4096)
                del_len = 4096;
            memcpy(del_buf, e.buf + s, (size_t)del_len);
            if (do_op == 'd' || do_op == 'D')
            {
                int ylen = ee - s;
                if (ylen > 8192)
                    ylen = 8192;
                memcpy(e.reg, e.buf + s, (size_t)ylen);
                e.reg_len = ylen;
                delete_at(s, ee - s);
                cur_to(s);
            }
        }
        else
        {
            if (new_cur < start)
            {
                int t = start;
                start = new_cur;
                new_cur = t;
            }
            else if (new_cur == start)
                new_cur = start + 1;
            if (new_cur > e.len)
                new_cur = e.len;
            del_len = new_cur - start;
            if (del_len > 4096)
                del_len = 4096;
            memcpy(del_buf, e.buf + start, (size_t)del_len);
            if (do_op == 'd' || do_op == 'D')
            {
                int ylen = new_cur - start;
                if (ylen > 8192)
                    ylen = 8192;
                memcpy(e.reg, e.buf + start, (size_t)ylen);
                e.reg_len = ylen;
                delete_at(start, new_cur - start);
                cur_to(start);
            }
        }
        if (do_op == 'd' || do_op == 'D')
        {
            e.dirty = 1;
            _ztui_undo_buf[_ztui_undo_n - 1].dl = del_len;
            memcpy(_ztui_undo_buf[_ztui_undo_n - 1].data, del_buf,
                   (size_t)(del_len < 48 ? del_len : 48));
        }
        else
        {
            _ztui_undo_n--;
        }
    }

    if (do_op == 'y' || do_op == 'Y')
    {
        int s = bol(start), e2 = eol(new_cur);
        if (e2 < e.len)
            e2++;
        int ylen = e2 - s;
        if (ylen > 8192)
            ylen = 8192;
        memcpy(e.reg, e.buf + s, (size_t)ylen);
        e.reg_len = ylen;
    }

    if (do_op != 'y' && do_op != 'Y')
        cur_to(new_cur);
    e.op = 0;
    e.cnt = 0;
}

/* ── mode switching ────────────────────────────────────────────────────── */

static void enter_insert(int pos)
{
    cur_to(pos);
    e.mode = INSERT;
    e.ins_start = e.cur;
}

/* ── command execution ─────────────────────────────────────────────────── */

static void execute_command(void)
{
    char* cmd = e.cmd_buf;
    while (*cmd == ' ')
        cmd++;
    if (strcmp(cmd, "q") == 0 || strcmp(cmd, "q!") == 0)
        exit(0);
    if (strncmp(cmd, "w", 1) == 0 || strncmp(cmd, "wq", 2) == 0)
    {
        if (e.file[0])
        {
            FILE* f = fopen(e.file, "w");
            if (f)
            {
                fwrite(e.buf, 1, (size_t)e.len, f);
                fclose(f);
                e.dirty = 0;
            }
        }
        if (cmd[0] == 'w' && cmd[1] == 'q')
            exit(0);
    }
    if (strncmp(cmd, "e ", 2) == 0)
    {
        char* fname = cmd + 2;
        while (*fname == ' ')
            fname++;
        FILE* f = fopen(fname, "r");
        if (f)
        {
            strncpy(e.file, fname, sizeof(e.file) - 1);
            e.len = (int)fread(e.buf, 1, (size_t)(e.cap - 1), f);
            e.buf[e.len] = '\0';
            fclose(f);
            e.dirty = 0;
            e.top = 0;
            cur_to(0);
            _ztui_undo_n = 0;
            _ztui_redo_n = 0;
        }
    }
    if (strncmp(cmd, "/", 1) == 0)
    {
        int slen = (int)strlen(cmd) - 1;
        if (slen > 255)
            slen = 255;
        memcpy(e.search, cmd + 1, (size_t)slen);
        e.search[slen] = '\0';
        e.search_len = slen;
        e.search_dir = 1;
        if (e.search_len > 0)
        {
            char* found = strstr(e.buf + e.cur + 1, e.search);
            if (!found)
                found = strstr(e.buf, e.search);
            if (found)
                cur_to((int)(found - e.buf));
        }
    }
    e.mode = NORMAL;
    e.cmd_len = 0;
}

/* ── normal mode key handling ──────────────────────────────────────────── */

static void normal_key(int key)
{
    /* 'r' (replace) consumes next key as replacement character */
    if (e.op == 'r')
    {
        if (e.cur < e.len && key >= 32)
        {
            int b = _ztui_utf8_len((unsigned char)e.buf[e.cur]);
            char newc[5] = {(char)key, 0, 0, 0, 0};
            push_undo(e.cur, b, 1, newc);
            delete_at(e.cur, b);
            insert_at(e.cur, newc, 1);
            e.dirty = 1;
        }
        e.op = 0;
        e.cnt = 0;
        return;
    }

    /* digit prefix (except 0 which is a motion) */
    if (key >= '1' && key <= '9')
    {
        e.cnt = e.cnt * 10 + (key - '0');
        return;
    }

    /* search: n / N */
    if (key == 'n' && e.search_len > 0)
    {
        char* found = strstr(e.buf + e.cur + 1, e.search);
        if (!found)
            found = strstr(e.buf, e.search);
        if (found)
            cur_to((int)(found - e.buf));
        return;
    }
    if (key == 'N' && e.search_len > 0)
    {
        char* found = NULL;
        for (char* p = e.buf; p < e.buf + e.cur;)
        {
            char* next = strstr(p, e.search);
            if (!next || next >= e.buf + e.cur)
                break;
            found = next;
            p = next + 1;
        }
        if (!found)
        {
            char* p = e.buf;
            while (1)
            {
                char* next = strstr(p, e.search);
                if (!next)
                    break;
                found = next;
                p = next + 1;
            }
        }
        if (found)
            cur_to((int)(found - e.buf));
        return;
    }

    /* operator keys (double-tap for line ops) */
    if (key == 'd' || key == 'y' || key == 'c' || key == 'Y')
    {
        if (key == 'd' && e.op == 'd')
        {
            int s = bol(e.cur), ee = eol(e.cur);
            if (ee < e.len)
                ee++;
            push_undo(s, 0, 0, NULL);
            int ylen = ee - s;
            if (ylen > 8192)
                ylen = 8192;
            memcpy(e.reg, e.buf + s, (size_t)ylen);
            e.reg_len = ylen;
            char del_buf[4096];
            int dlen = ee - s;
            if (dlen > 4096)
                dlen = 4096;
            memcpy(del_buf, e.buf + s, (size_t)dlen);
            delete_at(s, ee - s);
            cur_to(s);
            if (e.cur > e.len)
                e.cur = e.len;
            e.dirty = 1;
            _ztui_undo_buf[_ztui_undo_n - 1].dl = dlen;
            memcpy(_ztui_undo_buf[_ztui_undo_n - 1].data, del_buf, (size_t)(dlen < 48 ? dlen : 48));
            e.op = 0;
            e.cnt = 0;
            return;
        }
        if (key == 'y' && e.op == 'y')
        {
            int s = bol(e.cur), ee = eol(e.cur);
            if (ee < e.len)
                ee++;
            int ylen = ee - s;
            if (ylen > 8192)
                ylen = 8192;
            memcpy(e.reg, e.buf + s, (size_t)ylen);
            e.reg_len = ylen;
            e.op = 0;
            e.cnt = 0;
            return;
        }
        e.op = key;
        return;
    }

    /* paste */
    if (key == 'p' && e.reg_len > 0)
    {
        int pos = eol(e.cur);
        if (pos < e.len)
            pos++;
        push_undo(pos, 0, e.reg_len, e.reg);
        insert_at(pos, e.reg, e.reg_len);
        cur_to(pos + e.reg_len);
        e.dirty = 1;
        return;
    }
    if (key == 'P' && e.reg_len > 0)
    {
        int pos = bol(e.cur);
        push_undo(pos, 0, e.reg_len, e.reg);
        insert_at(pos, e.reg, e.reg_len);
        cur_to(pos + e.reg_len);
        e.dirty = 1;
        return;
    }

    /* undo / redo */
    if (key == 'u')
    {
        undo();
        return;
    }
    if (key == 18)
    {
        redo();
        return;
    }

    /* repeat last insertion */
    if (key == '.' && e.ins_len > 0)
    {
        push_undo(e.cur, 0, e.ins_len, e.ins_text);
        insert_at(e.cur, e.ins_text, e.ins_len);
        cur_to(e.cur + e.ins_len);
        e.dirty = 1;
        return;
    }

    /* enter insert mode */
    if (key == 'i')
    {
        enter_insert(e.cur);
        return;
    }
    if (key == 'I')
    {
        enter_insert(m_hat());
        return;
    }
    if (key == 'a')
    {
        enter_insert(m_l(1));
        return;
    }
    if (key == 'A')
    {
        enter_insert(m_dollar());
        return;
    }
    if (key == 'o')
    {
        int nl = eol(e.cur);
        push_undo(nl, 0, 1, "\n");
        insert_at(nl, "\n", 1);
        cur_to(nl + 1);
        e.dirty = 1;
        e.ins_start = e.cur;
        e.mode = INSERT;
        return;
    }
    if (key == 'O')
    {
        int sl = bol(e.cur);
        push_undo(sl, 0, 1, "\n");
        insert_at(sl, "\n", 1);
        cur_to(sl);
        e.dirty = 1;
        e.ins_start = e.cur;
        e.mode = INSERT;
        return;
    }

    /* delete: x = del char, X = backspace, D = delete to eol, dd handled above */
    if (key == 'x' && e.cur < e.len)
    {
        int b = _ztui_utf8_len((unsigned char)e.buf[e.cur]);
        char del[8] = {0};
        memcpy(del, e.buf + e.cur, (size_t)b);
        push_undo(e.cur, b, 0, del);
        int ylen = b;
        if (ylen > 8192)
            ylen = 8192;
        memcpy(e.reg, e.buf + e.cur, (size_t)ylen);
        e.reg_len = ylen;
        delete_at(e.cur, b);
        e.dirty = 1;
        e.cnt = 0;
        return;
    }
    if (key == 'X' && e.cur > 0)
    {
        int b = 1;
        int nc = e.cur - 1;
        if (e.cur > 1 && ((unsigned char)e.buf[e.cur - 1] & 0xC0u) == 0x80u)
        {
            b = 2;
            if (e.cur > 2 && ((unsigned char)e.buf[e.cur - 2] & 0xC0u) == 0x80u)
                b = 3;
            nc = e.cur - b;
        }
        char del[8] = {0};
        memcpy(del, e.buf + nc, (size_t)b);
        push_undo(nc, b, 0, del);
        delete_at(nc, b);
        cur_to(nc);
        e.dirty = 1;
        e.cnt = 0;
        return;
    }
    if (key == 'D')
    {
        int ee = eol(e.cur);
        if (ee > e.cur)
        {
            char del[4096];
            int dlen = ee - e.cur;
            if (dlen > 4096)
                dlen = 4096;
            memcpy(del, e.buf + e.cur, (size_t)dlen);
            push_undo(e.cur, dlen, 0, del);
            int ylen = ee - e.cur;
            if (ylen > 8192)
                ylen = 8192;
            memcpy(e.reg, e.buf + e.cur, (size_t)ylen);
            e.reg_len = ylen;
            delete_at(e.cur, ee - e.cur);
            e.dirty = 1;
        }
        e.cnt = 0;
        return;
    }

    /* substitute: s = cl (delete char, insert), S = cc (delete line, insert) */
    if (key == 's')
    {
        if (e.cur < e.len)
        {
            int b = _ztui_utf8_len((unsigned char)e.buf[e.cur]);
            char del[8] = {0};
            memcpy(del, e.buf + e.cur, (size_t)b);
            push_undo(e.cur, b, 0, del);
            delete_at(e.cur, b);
        }
        e.dirty = 1;
        e.ins_start = e.cur;
        e.mode = INSERT;
        e.cnt = 0;
        return;
    }

    /* change: C = change to eol, c{motion} handled by operator engine */
    if (key == 'C')
    {
        int ee = eol(e.cur);
        if (ee > e.cur)
        {
            char del[4096];
            int dlen = ee - e.cur;
            if (dlen > 4096)
                dlen = 4096;
            memcpy(del, e.buf + e.cur, (size_t)dlen);
            push_undo(e.cur, dlen, 0, del);
            delete_at(e.cur, ee - e.cur);
        }
        e.dirty = 1;
        e.ins_start = e.cur;
        e.mode = INSERT;
        e.cnt = 0;
        return;
    }

    /* replace: r = replace single char */
    if (key == 'r')
    {
        e.op = 'r';
        return; /* next key replaces char under cursor */
    }

    /* visual mode */
    if (key == 'v')
    {
        e.mode = VISUAL;
        e.sel_start = e.cur;
        e.sel_active = 1;
        return;
    }
    if (key == 'V')
    {
        e.mode = VLINE;
        e.sel_start = e.cur;
        e.sel_active = 1;
        return;
    }

    /* command line */
    if (key == ':')
    {
        e.mode = COMMAND;
        e.cmd_len = 0;
        e.cmd_pos = 0;
        return;
    }
    if (key == '/')
    {
        e.mode = COMMAND;
        e.cmd_len = 1;
        e.cmd_buf[0] = '/';
        e.cmd_pos = 1;
        return;
    }

    /* motions (either with pending operator or direct) */
    int do_op = e.op;
    if (key == '0')
    {
        apply_motion_and_op('0', do_op);
        return;
    }
    if (key == '$')
    {
        apply_motion_and_op('$', do_op);
        return;
    }
    if (key == '^')
    {
        apply_motion_and_op('^', do_op);
        return;
    }
    if (key == '%')
    {
        apply_motion_and_op('%', do_op);
        return;
    }
    if (key == 'G')
    {
        apply_motion_and_op('G', do_op);
        return;
    }
    if (key == 'g' && e.cnt == 0)
    {
        e.cnt = -1;
        return;
    } /* prefix for gg */
    if (key == 'g' && e.cnt == -1)
    {
        e.cnt = 0;
        cur_to(m_gg(do_op ? 0 : e.cnt));
        e.op = 0;
        return;
    }
    if (key == 'h' || key == 'j' || key == 'k' || key == 'l')
    {
        if (!do_op && (key == 'j' || key == 'k'))
        {
            int cnt = e.cnt > 0 ? e.cnt : 1;
            if (key == 'j')
            {
                int cl = line_of(e.cur) + cnt;
                if (cl >= line_count())
                    cl = line_count() - 1;
                cur_to(byte_of(cl));
            }
            else
            {
                int cl = line_of(e.cur) - cnt;
                if (cl < 0)
                    cl = 0;
                cur_to(byte_of(cl));
            }
            e.cnt = 0;
            return;
        }
        apply_motion_and_op(key, do_op);
        return;
    }
    if (key == 'w' || key == 'b' || key == 'e')
    {
        apply_motion_and_op(key, do_op);
        return;
    }

    e.op = 0;
    e.cnt = 0;
}

/* ── syntax highlighting ────────────────────────────────────────────────── */

static int highlight_keyword(const char* word, ztui_color* fg, uint8_t* bits)
{
    static const struct
    {
        const char* w;
        ztui_color c;
        int b;
    } kw[] = {
        {"int", 3, 0},    {"void", 3, 0},     {"char", 3, 0},     {"long", 3, 0},
        {"short", 3, 0},  {"unsigned", 3, 0}, {"signed", 3, 0},   {"float", 3, 0},
        {"double", 3, 0}, {"if", 5, 0},       {"else", 5, 0},     {"for", 5, 0},
        {"while", 5, 0},  {"do", 5, 0},       {"switch", 5, 0},   {"case", 5, 0},
        {"return", 5, 0}, {"break", 5, 0},    {"continue", 5, 0}, {"default", 5, 0},
        {"struct", 3, 1}, {"union", 3, 1},    {"typedef", 3, 1},  {"enum", 3, 1},
        {"static", 2, 0}, {"const", 2, 0},    {"sizeof", 2, 0},   {"include", 2, 1},
        {"define", 2, 1}, {"ifdef", 2, 1},    {"endif", 2, 1},    {"pragma", 2, 1},
        {"NULL", 6, 0},   {"size_t", 6, 0},
    };
    for (int k = 0; k < (int)(sizeof(kw) / sizeof(kw[0])); k++)
    {
        if (strcmp(word, kw[k].w) == 0)
        {
            *fg = kw[k].c;
            if (kw[k].b)
                *bits |= ZTUI_STYLE_BOLD;
            return 1;
        }
    }
    return 0;
}

/* ── drawing ────────────────────────────────────────────────────────────── */

static void draw_status(void)
{
    int w, h;
    ztui_get_size(&w, &h);
    char buf[512];
    const char* mode_str = "";
    switch (e.mode)
    {
        case NORMAL:
            mode_str = "NORMAL";
            break;
        case INSERT:
            mode_str = "INSERT";
            break;
        case VISUAL:
            mode_str = "VISUAL";
            break;
        case VLINE:
            mode_str = "V-LINE";
            break;
        case COMMAND:
            mode_str = "COMMAND";
            break;
    }
    int ln = line_of(e.cur) + 1;
    int cl = col_of(e.cur) + 1;
    snprintf(buf, sizeof(buf), "%s%s  %s  line %d, col %d", e.file[0] ? e.file : "[No Name]",
             e.dirty ? " [+]" : "", mode_str, ln, cl);
    ztui_status_set(&e_status, buf, -1);
    ztui_status_draw(&e_status, (ztui_rect){0, h - 2, w, 2}, ztui_get_theme()->muted);
}

static void draw_cursor_line(int line, int draw_y, int w)
{
    int start = byte_of(line);
    int end = eol(start);
    int len = end - start;
    char tmp[4096];
    if (len > 4095)
        len = 4095;
    memcpy(tmp, e.buf + start, (size_t)len);
    tmp[len] = '\0';

    /* line number */
    char num[16];
    snprintf(num, sizeof(num), " %3d ", line + 1);
    ztui_style ns = (line == line_of(e.cur))
                        ? ztui_style_make(ZTUI_YELLOW, ZTUI_BLACK, ZTUI_STYLE_BOLD)
                        : ztui_style_make(ZTUI_CYAN, ZTUI_BLACK, ZTUI_STYLE_DIM);
    ztui_draw_str((ztui_point){0, draw_y}, num, ns);

    /* visual selection */
    int is_sel = 0;
    if ((e.mode == VISUAL || e.mode == VLINE) && e.sel_active)
    {
        int s = e.sel_start, es = e.cur;
        if (s > es)
        {
            int t = s;
            s = es;
            es = t;
        }
        int sl = line_of(s), el = line_of(es);
        is_sel = (line >= sl && line <= el);
    }

    int gutter = 5;
    int x = gutter;

    for (int i = 0; i < len && x < w;)
    {
        int b = _ztui_utf8_len((unsigned char)tmp[i]);
        char ch[5] = {0};
        for (int j = 0; j < b && i + j < len; j++)
            ch[j] = tmp[i + j];
        int dw = _ztui_glyph_w(ch);
        if (x + dw > w)
            break;

        ztui_color fg = ZTUI_WHITE;
        ztui_color bg = ZTUI_BLACK;
        uint8_t bits = 0;

        /* character-precise selection */
        if (is_sel)
        {
            int byte_pos = start + i;
            int ss = e.sel_start, se = e.cur;
            if (e.mode == VLINE)
            {
                bg = ZTUI_BLUE;
                fg = ZTUI_WHITE;
            }
            else if (e.mode == VISUAL)
            {
                if (ss > se)
                {
                    int t = ss;
                    ss = se;
                    se = t;
                }
                if (byte_pos >= ss && byte_pos < se)
                {
                    bg = ZTUI_BLUE;
                    fg = ZTUI_WHITE;
                }
            }
        }

        /* syntax highlighting (check types in order: comment > string > keyword > preproc) */
        /* line comment */
        if (tmp[i] == '/' && i + 1 < len && tmp[i + 1] == '/')
        {
            fg = ZTUI_GREEN;
            bits = ZTUI_STYLE_DIM;
            while (i < len && x < w)
            {
                int cb = _ztui_utf8_len((unsigned char)tmp[i]);
                char cc[5] = {0};
                for (int jj = 0; jj < cb && i + jj < len; jj++)
                    cc[jj] = tmp[i + jj];
                int cdw = _ztui_glyph_w(cc);
                if (x + cdw > w)
                    break;
                ztui_set_cell((ztui_point){x, draw_y}, cc, ztui_style_make(fg, bg, bits));
                x += cdw;
                i += cb;
            }
            break; /* rest of line = comment */
        }

        /* block comment */
        if (tmp[i] == '/' && i + 1 < len && tmp[i + 1] == '*')
        {
            fg = ZTUI_GREEN;
            bits = ZTUI_STYLE_DIM;
            while (i < len && x < w)
            {
                int cb = _ztui_utf8_len((unsigned char)tmp[i]);
                char cc[5] = {0};
                for (int jj = 0; jj < cb && i + jj < len; jj++)
                    cc[jj] = tmp[i + jj];
                int cdw = _ztui_glyph_w(cc);
                if (x + cdw > w)
                    break;
                ztui_set_cell((ztui_point){x, draw_y}, cc, ztui_style_make(fg, bg, bits));
                x += cdw;
                i += cb;
                /* check for */
                if (i > 1 && tmp[i - 1] == '/' && tmp[i - 2] == '*')
                    break;
            }
            continue; /* process rest of line */
        }

        /* string or char literal */
        if (tmp[i] == '"' || tmp[i] == '\'')
        {
            char quote = tmp[i];
            fg = ZTUI_GREEN;
            while (i < len && x < w)
            {
                int cb = _ztui_utf8_len((unsigned char)tmp[i]);
                char cc[5] = {0};
                for (int jj = 0; jj < cb && i + jj < len; jj++)
                    cc[jj] = tmp[i + jj];
                int cdw = _ztui_glyph_w(cc);
                if (x + cdw > w)
                    break;
                ztui_set_cell((ztui_point){x, draw_y}, cc, ztui_style_make(fg, bg, bits));
                x += cdw;
                i += cb;
                if (cc[0] == quote && !(i > 1 && tmp[i - 2] == '\\'))
                    break;
            }
            continue; /* process rest of line */
        }

        /* keyword check */
        if (is_word_char((unsigned char)tmp[i]))
        {
            char word[64];
            int wi = 0, si = i;
            while (si < len && is_word_char((unsigned char)tmp[si]) && wi < 63)
                word[wi++] = tmp[si++];
            word[wi] = '\0';
            if (highlight_keyword(word, &fg, &bits))
            {
                /* draw entire keyword */
                for (int ki = 0; ki < wi;)
                {
                    int kb = _ztui_utf8_len((unsigned char)word[ki]);
                    char kc[5] = {0};
                    for (int jj = 0; jj < kb && ki + jj < wi; jj++)
                        kc[jj] = word[ki + jj];
                    int kdw = _ztui_glyph_w(kc);
                    if (x + kdw > w)
                        break;
                    ztui_set_cell((ztui_point){x, draw_y}, kc, ztui_style_make(fg, bg, bits));
                    x += kdw;
                    ki += kb;
                    i += kb;
                }
                continue;
            }
        }

        /* preprocessor directive */
        if (tmp[i] == '#' && i == 0)
        {
            fg = ZTUI_GREEN;
            bits = ZTUI_STYLE_BOLD;
        }

        /* cursor */
        int is_cursor = (line == line_of(e.cur) && e.cur >= start && e.cur < start + i + b &&
                         e.cur >= start && e.mode != INSERT);

        if (is_cursor)
        {
            ztui_set_cell((ztui_point){x, draw_y}, ch,
                          ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD));
        }
        else
        {
            ztui_set_cell((ztui_point){x, draw_y}, ch, ztui_style_make(fg, bg, bits));
        }
        x += dw;
        i += b;
    }

    /* cursor on empty line */
    if (len == 0 && line == line_of(e.cur) && e.mode != INSERT)
    {
        ztui_set_cell((ztui_point){x, draw_y}, " ",
                      ztui_style_make(ZTUI_BLACK, ZTUI_WHITE, ZTUI_STYLE_BOLD));
    }

    /* clear rest of line */
    ztui_fill_rect((ztui_rect){x, draw_y, w - x, 1}, " ",
                   ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0));
}

/* ── main ────────────────────────────────────────────────────────────────── */

int main(int argc, char** argv)
{
    e.buf = (char*)malloc(BUF_CAP);
    if (!e.buf)
        return 1;
    e.cap = BUF_CAP;
    e.len = 0;
    e.buf[0] = '\0';
    e.mode = NORMAL;
    e.dirty = 0;
    e.top = 0;
    e.cur = 0;
    e.col = 0;
    e.op = 0;
    e.cnt = 0;
    e.reg_len = 0;
    e.search_len = 0;
    e.sel_active = 0;
    e.ins_len = 0;
    _ztui_undo_n = 0;
    _ztui_redo_n = 0;

    if (argc > 1)
    {
        strncpy(e.file, argv[1], sizeof(e.file) - 1);
        FILE* f = fopen(argv[1], "r");
        if (f)
        {
            e.len = (int)fread(e.buf, 1, (size_t)(e.cap - 1), f);
            e.buf[e.len] = '\0';
            fclose(f);
        }
    }

    if (ztui_init() != ZTUI_OK)
        return 1;

    int running = 1;
    while (running)
    {
        ztui_event ev;
        int w, h;
        ztui_get_size(&w, &h);
        ztui_frame_begin(ZTUI_BLACK);

        int vis = h - 2;
        ztui_clip_push((ztui_rect){0, 0, w, vis});

        /* draw visible lines */
        int total = line_count();
        for (int i = 0; i < vis && e.top + i < total; i++)
            draw_cursor_line(e.top + i, i, w);

        /* fill remaining */
        for (int i = total - e.top; i < vis; i++)
            ztui_fill_rect((ztui_rect){0, i, w, 1}, " ",
                           ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0));

        ztui_clip_pop();

        /* status */
        draw_status();

        /* poll */
        ztui_poll(&ev);

        /* global shortcuts */
        if (ev.type == ZTUI_EV_KEY && ev.key == 17)
        {
            running = 0;
        } /* Ctrl+Q */

        /* mode dispatch */
        if (e.mode == INSERT)
        {
            if (ev.type == ZTUI_EV_ESCAPE)
            {
                e.mode = NORMAL;
                e.cnt = 0;
                /* save inserted text for . repeat */
                e.ins_len = e.cur - e.ins_start;
                if (e.ins_len > 512)
                    e.ins_len = 512;
                if (e.ins_len > 0)
                    memcpy(e.ins_text, e.buf + e.ins_start, (size_t)e.ins_len);
            }
            else if (ev.type == ZTUI_EV_ENTER)
            {
                push_undo(e.cur, 0, 1, "\n");
                insert_at(e.cur, "\n", 1);
                cur_to(e.cur + 1);
                e.dirty = 1;
            }
            else if (ev.type == ZTUI_EV_BACKSPACE)
            {
                if (e.cur == 0)
                { /* no-op at start of buffer */
                }
                else if (e.buf[e.cur - 1] == '\n')
                {
                    /* join lines: delete the newline */
                    push_undo(e.cur - 1, 1, 0, "\n");
                    delete_at(e.cur - 1, 1);
                    cur_to(e.cur - 1);
                    e.dirty = 1;
                }
                else
                {
                    int b = 1;
                    if (e.cur > 1 && ((unsigned char)e.buf[e.cur - 1] & 0xC0u) == 0x80u)
                    {
                        b = 2;
                        if (e.cur > 2 && ((unsigned char)e.buf[e.cur - 2] & 0xC0u) == 0x80u)
                            b = 3;
                    }
                    int new_cur = e.cur - b;
                    char del[8] = {0};
                    memcpy(del, e.buf + new_cur, (size_t)b);
                    push_undo(new_cur, b, 0, del);
                    delete_at(new_cur, b);
                    cur_to(new_cur);
                    e.dirty = 1;
                }
            }
            else if (ev.type == ZTUI_EV_DELETE && e.cur < e.len)
            {
                int b = _ztui_utf8_len((unsigned char)e.buf[e.cur]);
                char del[8] = {0};
                memcpy(del, e.buf + e.cur, (size_t)b);
                push_undo(e.cur, b, 0, del);
                delete_at(e.cur, b);
                e.dirty = 1;
            }
            else if (ev.type == ZTUI_EV_LEFT && e.cur > 0)
            {
                int b = 1;
                if (e.cur > 1 && ((unsigned char)e.buf[e.cur - 1] & 0xC0u) == 0x80u)
                {
                    b = 2;
                    if (e.cur > 2 && ((unsigned char)e.buf[e.cur - 2] & 0xC0u) == 0x80u)
                        b = 3;
                }
                cur_to(e.cur - b);
            }
            else if (ev.type == ZTUI_EV_RIGHT && e.cur < e.len)
            {
                cur_to(e.cur + _ztui_utf8_len((unsigned char)e.buf[e.cur]));
            }
            else if (ev.type == ZTUI_EV_HOME)
            {
                cur_to(bol(e.cur));
            }
            else if (ev.type == ZTUI_EV_END)
            {
                cur_to(eol(e.cur));
            }
            else if (ev.type == ZTUI_EV_KEY && ev.key >= 32 && ev.key != 127 && ev.utf8[0])
            {
                int nl = (int)strlen(ev.utf8);
                push_undo(e.cur, 0, nl, ev.utf8);
                insert_at(e.cur, ev.utf8, nl);
                cur_to(e.cur + nl);
                e.dirty = 1;
            }
        }
        else if (e.mode == NORMAL)
        {
            if (ev.type == ZTUI_EV_ESCAPE)
            {
                e.op = 0;
                e.cnt = 0;
            }
            if (ev.type == ZTUI_EV_KEY)
            {
                int key = ev.key;
                if (key > 0)
                    normal_key(key);
            }
            if (ev.type == ZTUI_EV_LEFT)
            {
                if (!e.op)
                    cur_to(m_h(1));
                else
                    apply_motion_and_op('h', e.op);
            }
            if (ev.type == ZTUI_EV_RIGHT)
            {
                if (!e.op)
                    cur_to(m_l(1));
                else
                    apply_motion_and_op('l', e.op);
            }
            if (ev.type == ZTUI_EV_UP)
            {
                if (!e.op)
                {
                    int l = line_of(e.cur) - 1;
                    if (l < 0)
                        l = 0;
                    cur_to(byte_of(l));
                }
                else
                    apply_motion_and_op('k', e.op);
            }
            if (ev.type == ZTUI_EV_DOWN)
            {
                if (!e.op)
                {
                    int l = line_of(e.cur) + 1;
                    if (l >= line_count())
                        l = line_count() - 1;
                    cur_to(byte_of(l));
                }
                else
                    apply_motion_and_op('j', e.op);
            }
            /* Ctrl+d / Ctrl+u: scroll half page */
            if (ev.type == ZTUI_EV_KEY && ev.key == 4)
            {
                int half = vis / 2;
                int l = line_of(e.cur) + half;
                if (l >= line_count())
                    l = line_count() - 1;
                cur_to(byte_of(l));
                e.top += half;
                if (e.top + vis > line_count())
                    e.top = line_count() - vis;
                if (e.top < 0)
                    e.top = 0;
            }
            if (ev.type == ZTUI_EV_KEY && ev.key == 21)
            {
                int half = vis / 2;
                int l = line_of(e.cur) - half;
                if (l < 0)
                    l = 0;
                cur_to(byte_of(l));
                e.top -= half;
                if (e.top < 0)
                    e.top = 0;
            }
            if (ev.type == ZTUI_EV_MOUSE_DOWN)
            {
                cur_to(byte_of(ev.mouse.y + e.top) + (ev.mouse.x > 5 ? ev.mouse.x - 5 : 0));
                if (e.cur > e.len)
                    e.cur = e.len;
            }
        }
        else if (e.mode == COMMAND)
        {
            if (ev.type == ZTUI_EV_ESCAPE)
            {
                e.mode = NORMAL;
                e.cmd_len = 0;
            }
            else if (ev.type == ZTUI_EV_ENTER)
            {
                execute_command();
            }
            else if (ev.type == ZTUI_EV_BACKSPACE && e.cmd_len > 0)
            {
                e.cmd_len--;
            }
            else if (ev.type == ZTUI_EV_KEY && ev.key >= 32 && ev.utf8[0] && e.cmd_len < 255)
                e.cmd_buf[e.cmd_len++] = ev.utf8[0];
        }
        else if (e.mode == VISUAL || e.mode == VLINE)
        {
            if (ev.type == ZTUI_EV_KEY)
            {
                int key = ev.key;
                if (key == 27)
                {
                    e.mode = NORMAL;
                    e.sel_active = 0;
                }
                else if (key == 'v' || key == 'V')
                {
                    e.mode = NORMAL;
                    e.sel_active = 0;
                }
                else if (key == 'h')
                {
                    cur_to(m_h(1));
                }
                else if (key == 'l')
                {
                    cur_to(m_l(1));
                }
                else if (key == 'j')
                {
                    int l = line_of(e.cur) + 1;
                    if (l >= line_count())
                        l = line_count() - 1;
                    cur_to(byte_of(l));
                }
                else if (key == 'k')
                {
                    int l = line_of(e.cur) - 1;
                    if (l < 0)
                        l = 0;
                    cur_to(byte_of(l));
                }
                else if (key == 'w')
                {
                    cur_to(m_w(1));
                }
                else if (key == 'b')
                {
                    cur_to(m_b(1));
                }
                else if (key == 'e')
                {
                    cur_to(m_e(1));
                }
                else if (key == '0')
                {
                    cur_to(m_0());
                }
                else if (key == '$')
                {
                    cur_to(m_dollar());
                }
                else if (key == '^')
                {
                    cur_to(m_hat());
                }
                else if (key == 'd' || key == 'D')
                {
                    int s = e.sel_start, en = e.cur;
                    if (s > en)
                    {
                        int t = s;
                        s = en;
                        en = t;
                    }
                    if (e.mode == VLINE)
                    {
                        s = bol(s);
                        en = eol(en);
                        if (en < e.len)
                            en++;
                    }
                    push_undo(s, 0, 0, NULL);
                    char del[4096];
                    int dlen = en - s;
                    if (dlen > 4096)
                        dlen = 4096;
                    memcpy(del, e.buf + s, (size_t)dlen);
                    int ylen = en - s;
                    if (ylen > 8192)
                        ylen = 8192;
                    memcpy(e.reg, e.buf + s, (size_t)ylen);
                    e.reg_len = ylen;
                    delete_at(s, en - s);
                    cur_to(s);
                    e.dirty = 1;
                    e.mode = NORMAL;
                    e.sel_active = 0;
                }
                else if (key == 'y' || key == 'Y')
                {
                    int s = e.sel_start, en = e.cur;
                    if (s > en)
                    {
                        int t = s;
                        s = en;
                        en = t;
                    }
                    if (e.mode == VLINE)
                    {
                        s = bol(s);
                        en = eol(en);
                        if (en < e.len)
                            en++;
                    }
                    int ylen = en - s;
                    if (ylen > 8192)
                        ylen = 8192;
                    memcpy(e.reg, e.buf + s, (size_t)ylen);
                    e.reg_len = ylen;
                    e.mode = NORMAL;
                    e.sel_active = 0;
                }
            }
            if (ev.type == ZTUI_EV_LEFT)
            {
                cur_to(m_h(1));
            }
            if (ev.type == ZTUI_EV_RIGHT)
            {
                cur_to(m_l(1));
            }
            if (ev.type == ZTUI_EV_UP)
            {
                int l = line_of(e.cur) - 1;
                if (l < 0)
                    l = 0;
                cur_to(byte_of(l));
            }
            if (ev.type == ZTUI_EV_DOWN)
            {
                int l = line_of(e.cur) + 1;
                if (l >= line_count())
                    l = line_count() - 1;
                cur_to(byte_of(l));
            }
        }

        /* scroll */
        {
            int cl = line_of(e.cur);
            if (cl < e.top)
                e.top = cl;
            if (cl >= e.top + vis)
                e.top = cl - vis + 1;
            if (e.top < 0)
                e.top = 0;
        }

        /* command line */
        if (e.mode == COMMAND)
        {
            char cmdline[256];
            int cmdlen = e.cmd_len;
            memcpy(cmdline, e.cmd_buf, (size_t)cmdlen);
            cmdline[cmdlen] = '\0';

            ztui_fill_rect((ztui_rect){0, h - 1, w, 1}, " ",
                           ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0));
            ztui_style cmds = ztui_style_make(ZTUI_WHITE, ZTUI_BLACK, 0);

            if (cmdlen > 0 && cmdline[0] == '/')
            {
                ztui_draw_str((ztui_point){0, h - 1}, cmdline, cmds);
                if (cmdlen > 1 && e.buf[0])
                {
                    int count = 0;
                    char* p = e.buf;
                    while ((p = strstr(p, cmdline + 1)))
                    {
                        count++;
                        p++;
                    }
                    char cnts[32];
                    snprintf(cnts, sizeof(cnts), "  (%d matches)", count);
                    ztui_draw_str((ztui_point){cmdlen, h - 1}, cnts,
                                  ztui_style_make(ZTUI_CYAN, ZTUI_BLACK, ZTUI_STYLE_DIM));
                }
            }
            else
            {
                ztui_draw_str((ztui_point){0, h - 1}, cmdline, cmds);
            }
        }

        ztui_frame_end();
    }

    ztui_cleanup();
    free(e.buf);
    return 0;
}
