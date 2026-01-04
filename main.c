#include "grid.h"
#include <ncurses.h>

typedef enum { PAUSE, PLAY, QUIT } State;

static void handle_input(void);
static void toggle_pause(void);

State state = PAUSE;

int main(void) {
    initscr();
    noecho();

    Grid grid = grid_init(10, 10);

    // glider
    grid_toggle_cell(&grid, 0, 1);
    grid_toggle_cell(&grid, 1, 2);
    grid_toggle_cell(&grid, 2, 0);
    grid_toggle_cell(&grid, 2, 1);
    grid_toggle_cell(&grid, 2, 2);

    while (state != QUIT) {
        grid_print(&grid);
        refresh();

        handle_input();

        grid_simulate(&grid);
        if (state == PLAY) napms(500);
    }

    grid_free(&grid);
    endwin();
    return EXIT_SUCCESS;
}

static void handle_input(void) {
    int ch;

    do {
        switch (ch = getch()) {
            case 'q':
                state = QUIT;
                break;
            case ' ':
                toggle_pause();
                break;
        }
    } while (state == PLAY && ch != ERR);
}

static void toggle_pause(void) {
    state = state == PAUSE ? PLAY : PAUSE;
    curs_set(state == PAUSE ? 1 : 0);
    nodelay(stdscr, state != PAUSE);
}
