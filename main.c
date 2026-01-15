#include "grid.h"
#include <ncurses.h>
#include <time.h>

typedef enum {
    SET_DEFAULT,
    SET_MIN,
    SET_MAX,
    DECREASE,
    INCREASE,
} TimeoutAdjustment;

static void init(void);
static void clean_up(void);
static void handle_input_global(int ch);
static void handle_input_edit(int ch);
static void handle_resize(void);
static void reset_grid(void);
static void toggle_mode(void);
static void adjust_timeout_ms(TimeoutAdjustment adjustment);
static void print(void);
static void print_status_bar(void);

static const int TIMEOUT_MS_DEFAULT = 250;
static const int STATUS_BAR_HEIGHT  = 1;

static bool running    = true;
static bool edit_mode  = true;
static int  timeout_ms = TIMEOUT_MS_DEFAULT;
static int  cursor_y   = 0;
static int  cursor_x   = 0;

static Grid     grid;
static GridSave save;

int main(void) {
    init();

    while (running) {
        print();

        const int ch = getch();
        handle_input_global(ch);
        if (edit_mode) handle_input_edit(ch);
    }

    clean_up();
    return EXIT_SUCCESS;
}

static void init(void) {
    srand(time(NULL));
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);

    grid = grid_init(LINES - STATUS_BAR_HEIGHT, COLS);
    save = grid_save_init();
}

static void clean_up(void) {
    grid_save_free(&save);
    grid_free(&grid);

    endwin();
}

static void handle_input_global(const int ch) {
    switch (ch) {
        case KEY_RESIZE:
            handle_resize();
            break;
        case ERR:
        case 'n':
            grid_evolve(&grid);
            break;
        case ' ':
            toggle_mode();
            break;
        case 'c':
            reset_grid();
            break;
        case 'r':
            grid_randomize(&grid);
            break;
        case 'y':
            grid_save(&grid, &save);
            break;
        case 'p':
            grid_load(&grid, &save);
            break;
        case '0':
            adjust_timeout_ms(SET_DEFAULT);
            break;
        case '_':
            adjust_timeout_ms(SET_MIN);
            break;
        case '+':
            adjust_timeout_ms(SET_MAX);
            break;
        case '-':
            adjust_timeout_ms(DECREASE);
            break;
        case '=':
            adjust_timeout_ms(INCREASE);
            break;
        case 'q':
            running = false;
            break;
    }
}

static void handle_input_edit(const int ch) {
    switch (ch) {
        case 's':
        case '\n':
            grid_toggle_cell(&grid, cursor_y, cursor_x);
            break;
        case 'h':
        case KEY_LEFT:
            cursor_x--;
            break;
        case 'j':
        case KEY_DOWN:
            cursor_y++;
            break;
        case 'k':
        case KEY_UP:
            cursor_y--;
            break;
        case 'l':
        case KEY_RIGHT:
            cursor_x++;
            break;
    }

    cursor_y = wrap(cursor_y, grid.rows);
    cursor_x = wrap(cursor_x, grid.cols);
}

static void handle_resize(void) {
    grid_resize(&grid, LINES - STATUS_BAR_HEIGHT, COLS);

    if (cursor_y >= (int)grid.rows) cursor_y = grid.rows - 1;
    if (cursor_x >= (int)grid.cols) cursor_x = grid.cols - 1;
}

static void reset_grid(void) {
    if (!edit_mode) toggle_mode();
    grid_clear(&grid);
}

static void toggle_mode(void) {
    edit_mode = !edit_mode;
    curs_set(edit_mode ? 1 : 0);
    timeout(edit_mode ? -1 : timeout_ms);

    if (!edit_mode) grid_evolve(&grid);
}

static void adjust_timeout_ms(const TimeoutAdjustment adjustment) {
    static const int TIMEOUT_MS_MIN = 25;
    static const int TIMEOUT_MS_MAX = 1000;

    switch (adjustment) {
        case SET_DEFAULT:
            timeout_ms = TIMEOUT_MS_DEFAULT;
            break;
        case SET_MIN:
            timeout_ms = TIMEOUT_MS_MIN;
            break;
        case SET_MAX:
            timeout_ms = TIMEOUT_MS_MAX;
            break;
        case DECREASE:
            timeout_ms -= TIMEOUT_MS_MIN;
            if (timeout_ms < TIMEOUT_MS_MIN) timeout_ms = TIMEOUT_MS_MIN;
            break;
        case INCREASE:
            timeout_ms += TIMEOUT_MS_MIN;
            if (timeout_ms > TIMEOUT_MS_MAX) timeout_ms = TIMEOUT_MS_MAX;
            break;
    }

    if (!edit_mode) timeout(timeout_ms);
}

static void print(void) {
    erase();
    grid_print(&grid);
    print_status_bar();
    move(cursor_y, cursor_x);
    refresh();
}

static void print_status_bar(void) {
    const int bar_y        = LINES - STATUS_BAR_HEIGHT;
    const int mode_x       = 0;
    const int generation_x = 13;
    const int cursor_pos_x = COLS - 18;
    const int timeout_x    = COLS - 6;

    mvaddstr(bar_y, mode_x,       edit_mode ? "-- EDIT --" : "-- PLAY --");
    mvprintw(bar_y, generation_x, "GEN %u", grid.generation);
    mvprintw(bar_y, cursor_pos_x, "%d,%d",  cursor_y, cursor_x);
    mvprintw(bar_y, timeout_x,    "%4dms",  timeout_ms);

    mvchgat(bar_y, 0, COLS, A_REVERSE, 0, NULL);
}
