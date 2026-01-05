#include "grid.h"
#include <ncurses.h>
#include <time.h>

typedef enum { EDIT, PLAY, QUIT } State;

static void init(void);
static void draw(const Grid *grid);
static void handle_input(Grid *grid);
static void handle_input_edit(Grid *grid, const int ch);
static void toggle_edit_play(void);

static const int DEFAULT_TIMEOUT_MS = 250;

static State state  = EDIT;
static int cursor_y = 0;
static int cursor_x = 0;

int main(void) {
    init();
    Grid grid = grid_init(getmaxy(stdscr), getmaxx(stdscr));

    while (state != QUIT) {
        draw(&grid);
        handle_input(&grid);
    }

    grid_free(&grid);
    endwin();
    return EXIT_SUCCESS;
}

static void init(void) {
    srand(time(NULL));
    initscr();
    noecho();
    keypad(stdscr, true);
}

static void draw(const Grid *grid) {
    erase();
    grid_draw(grid);
    move(cursor_y, cursor_x);
    refresh();
}

static void handle_input(Grid *grid) {
    const int ch = getch();

    if (state == EDIT) handle_input_edit(grid, ch);

    switch (ch) {
        case 'q':
            state = QUIT;
            break;
        case 'c':
            grid_clear(grid);
            break;
        case 'r':
            grid_randomize(grid);
            break;
        case ' ':
            toggle_edit_play();
            break;
        case 'n':
        case ERR:
            grid_simulate(grid);
            break;
    }
}

static void handle_input_edit(Grid *grid, const int ch) {
    switch (ch) {
        case '\n':
        case 's':
            grid_toggle_cell(grid, cursor_y, cursor_x);
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

    cursor_y = wrap(cursor_y, grid->rows);
    cursor_x = wrap(cursor_x, grid->cols);
}

static void toggle_edit_play(void) {
    state = state == EDIT ? PLAY : EDIT;
    curs_set(state == EDIT ? 1 : 0);
    timeout(state == EDIT ? -1 : DEFAULT_TIMEOUT_MS);
}
