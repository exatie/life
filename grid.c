#include "grid.h"
#include <ncurses.h>

Grid grid_init(const size_t rows, const size_t cols) {
    bool *cells = calloc(2 * rows * cols, sizeof cells);
    if (!cells) abort();

    return (Grid){
        .cells       = cells,
        .front_cells = cells,
        .back_cells  = cells + rows * cols,
        .rows  = rows,
        .cols  = cols,
    };
}

void grid_free(Grid *grid) {
    free(grid->cells);

    grid->cells       = NULL;
    grid->front_cells = NULL;
    grid->back_cells  = NULL;
}

void grid_print(const Grid *grid) {
    for (size_t y = 0; y < grid->rows; y++) {
        for (size_t x = 0; x < grid->cols; x++) {
            int ch = grid->front_cells[y * grid->cols + x] ? '#' : '.';
            mvaddch(y, x, ch);
        }
    }
}

void grid_toggle_cell(Grid *grid, int y, int x) {
    y = abs(y % (signed)grid->rows);
    x = abs(x % (signed)grid->cols);

    const bool is_alive = grid->front_cells[y * grid->cols + x];
    grid->front_cells[y * grid->cols + x] = !is_alive;
}
