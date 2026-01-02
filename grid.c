#include "grid.h"
#include <ncurses.h>

Grid grid_init(const size_t rows, const size_t cols) {
    bool *cells = calloc(2 * rows * cols, sizeof cells);
    if (!cells) abort();

    return (Grid){
        .cells       = cells,
        .front_cells = cells,
        .back_cells  = cells + rows * cols,
        .rows = rows,
        .cols = cols,
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

static int wrap(int n, const int ceiling) {
    n %= ceiling;
    if (n < 0) n += ceiling;
    return n;
}

static int grid_count_neighbors(const Grid *grid, const int cy, const int cx) {
    int result = 0;

    for (int dy = -1; dy <= 1; dy++) {
        const int y = wrap(cy + dy, grid->rows);

        for (int dx = -1; dx <= 1; dx++) {
            const int x = wrap(cx + dx, grid->cols);
            if (y == cy && x == cx) continue;

            if (grid->front_cells[y * grid->cols + x]) result++;
        }
    }

    return result;
}

void grid_simulate(Grid *grid) {
    for (size_t y = 0; y < grid->rows; y++) {
        for (size_t x = 0; x < grid->cols; x++) {
            const int neighbors = grid_count_neighbors(grid, y, x);

            const bool is_alive  = grid->front_cells[y * grid->cols + x];
            const bool will_live = neighbors==3 || (neighbors==2 && is_alive);

            grid->back_cells[y * grid->cols + x] = will_live;
        }
    }

    bool *tmp         = grid->front_cells;
    grid->front_cells = grid->back_cells;
    grid->back_cells  = tmp;
}

void grid_toggle_cell(Grid *grid, int y, int x) {
    const bool is_alive = grid->front_cells[y * grid->cols + x];
    grid->front_cells[y * grid->cols + x] = !is_alive;
}
