#include "grid.h"
#include <ncurses.h>
#include <time.h>

typedef enum { EDIT, PLAY, QUIT } Mode;

static void init(void);
static void clean_up(void);
static void draw(void);
static void draw_footer(void);
static void handle_input(void);
static void toggle_edit_play(void);

static const int FOOTER_HEIGHT = 1;
static const int DEFAULT_TIMEOUT_MS = 250;

static Mode mode = EDIT;
static Grid grid;
static int max_y;
static int max_x;
static int cursor_y = 0;
static int cursor_x = 0;
static unsigned generation = 0;

int main(void) {
    init();

    while (mode != QUIT) {
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
    const int timeout_x    = max_x - 5;

    mvaddstr(footer_y, mode_x, mode == EDIT ? "-- EDIT --" : "-- PLAY --");
    mvprintw(footer_y, generation_x, "GEN %u", generation);
    mvprintw(footer_y, cursor_pos_x, "%d,%d", cursor_y, cursor_x);
    mvprintw(footer_y, timeout_x, "%dms", DEFAULT_TIMEOUT_MS);

    mvchgat(footer_y, 0, max_x, A_REVERSE, 0, NULL);
}

static void handle_input(void) {
    const int ch = getch();

    switch (ch) {
        case 'q':
            mode = QUIT;
            break;
        case 'c':
            generation = 0;
            grid_clear(&grid);
            break;
        case 'r':
            generation = 0;
            grid_randomize(&grid);
            break;
        case ' ':
            toggle_edit_play();
            if (mode != PLAY) break;
            [[fallthrough]];
        case 'n':
        case ERR:
            generation++;
            grid_simulate(&grid);
            break;
    }

    if (mode == EDIT) {
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

static void toggle_edit_play(void) {
    mode = mode == EDIT ? PLAY : EDIT;
    curs_set(mode == EDIT ? 1 : 0);
    timeout(mode == EDIT ? -1 : DEFAULT_TIMEOUT_MS);
}
