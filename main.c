#include "grid.h"
#include <ncurses.h>
#include <time.h>

typedef enum { RESET, DECREASE, INCREASE } Change;

static void init(void);
static void clean_up(void);
static void draw(void);
static void draw_footer(void);
static void handle_input(void);
static void change_timeout_ms(const Change change);
static void toggle_mode(void);

static const int DEFAULT_TIMEOUT_MS = 250;
static const int FOOTER_HEIGHT      = 1;

static bool     running    = true;
static bool     edit_mode  = true;
static int      timeout_ms = DEFAULT_TIMEOUT_MS;
static unsigned generation = 0;
static int      cursor_y   = 0;
static int      cursor_x   = 0;
static int      max_y;
static int      max_x;
static Grid     grid;

int main(void) {
    init();

    while (running) {
        draw();
        handle_input();
    }

    clean_up();
    return EXIT_SUCCESS;
}

static void init(void) {
    srand(time(NULL));
    initscr();
    noecho();
    keypad(stdscr, true);
    getmaxyx(stdscr, max_y, max_x);
    grid = grid_init(max_y - FOOTER_HEIGHT, max_x);
}

static void clean_up(void) {
    grid_free(&grid);
    endwin();
}

static void draw(void) {
    erase();
    grid_draw(&grid);
    draw_footer();
    move(cursor_y, cursor_x);
    refresh();
}

static void draw_footer(void) {
    const int footer_y     = max_y - FOOTER_HEIGHT;
    const int mode_x       = 0;
    const int generation_x = 17;
    const int cursor_pos_x = max_x - 18;
    const int timeout_x    = max_x - 6;

    mvaddstr(footer_y, mode_x,       edit_mode ? "-- EDIT --" : "-- PLAY --");
    mvprintw(footer_y, generation_x, "GEN %u", generation);
    mvprintw(footer_y, cursor_pos_x, "%d,%d",  cursor_y, cursor_x);
    mvprintw(footer_y, timeout_x,    "%4dms",   timeout_ms);

    mvchgat(footer_y, 0, max_x, A_REVERSE, 0, NULL);
}

static void handle_input(void) {
    const int ch = getch();

    switch (ch) {
        case 'q':
            running = false;
            break;
        case 'c':
            generation = 0;
            grid_clear(&grid);
            break;
        case 'r':
            generation = 0;
            grid_randomize(&grid);
            break;
        case '0':
            change_timeout_ms(RESET);
            break;
        case '-':
            change_timeout_ms(DECREASE);
            break;
        case '=':
            change_timeout_ms(INCREASE);
            break;
        case ' ':
            toggle_mode();
            if (edit_mode) break;
            [[fallthrough]];
        case 'n':
        case ERR:
            generation++;
            grid_simulate(&grid);
            break;
    }

    if (edit_mode) {
        switch (ch) {
            case '\n':
            case 's':
                grid_toggle_cell(&grid, cursor_y, cursor_x);
                break;
            case KEY_LEFT:
            case 'h':
                cursor_x--;
                break;
            case KEY_DOWN:
            case 'j':
                cursor_y++;
                break;
            case KEY_UP:
            case 'k':
                cursor_y--;
                break;
            case KEY_RIGHT:
            case 'l':
                cursor_x++;
                break;
        }

        cursor_y = wrap(cursor_y, grid.rows);
        cursor_x = wrap(cursor_x, grid.cols);
    }
}

static void change_timeout_ms(const Change change) {
    const int MIN_TIMEOUT_MS   = 50;
    const int MAX_TIMEOUT_MS   = 1000;
    const int TIMEOUT_MS_DELTA = 50;

    switch (change) {
        case RESET:
            timeout_ms = DEFAULT_TIMEOUT_MS;
            break;
        case DECREASE:
            timeout_ms -= TIMEOUT_MS_DELTA;
            if (timeout_ms < MIN_TIMEOUT_MS) timeout_ms = MIN_TIMEOUT_MS;
            break;
        case INCREASE:
            timeout_ms += TIMEOUT_MS_DELTA;
            if (timeout_ms > MAX_TIMEOUT_MS) timeout_ms = MAX_TIMEOUT_MS;
            break;
    }

    if (!edit_mode) timeout(timeout_ms);
}

static void toggle_mode(void) {
    edit_mode = !edit_mode;
    curs_set(edit_mode);
    timeout(edit_mode ? -1 : timeout_ms);
}
