#include "grid.h"
#include <ncurses.h>

int main(void) {
    initscr();
    noecho();
    curs_set(0);

    Grid grid = grid_init(10, 10);

    // glider
    grid_toggle_cell(&grid, 0, 1);
    grid_toggle_cell(&grid, 1, 2);
    grid_toggle_cell(&grid, 2, 0);
    grid_toggle_cell(&grid, 2, 1);
    grid_toggle_cell(&grid, 2, 2);

    do {
        grid_print(&grid);
        refresh();
        grid_simulate(&grid);
    } while (getch() != 'q');

    grid_free(&grid);
    endwin();
    return EXIT_SUCCESS;
}
