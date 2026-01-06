#include "grid.h"
#include <ncurses.h>
#include <string.h>

static int grid_count_neighbors(const Grid *grid, const int cy, const int cx);

int wrap(int n, const int ceiling) {
    const int FLOOR = 0;

    n %= ceiling;
    if (n < FLOOR) n += ceiling;
    return n;
}

Grid grid_init(const size_t rows, const size_t cols) {
    bool *cells = calloc(2 * rows * cols, sizeof *cells);
    if (!cells) abort();

    return (Grid){
        .cells       = cells,
        .front_cells = cells,
        .back_cells  = cells + rows * cols,
        .rows        = rows,
        .cols        = cols,
    };
}

void grid_free(Grid *grid) {
    free(grid->cells);

    grid->cells       = NULL;
    grid->front_cells = NULL;
    grid->back_cells  = NULL;
}

void grid_draw(const Grid *grid) {
    const char ALIVE = '#';

    for (size_t y = 0; y < grid->rows; y++) {
        for (size_t x = 0; x < grid->cols; x++) {
            const bool is_alive = grid->front_cells[y * grid->cols + x];
            if (is_alive) mvaddch(y, x, ALIVE);
        }
    }
}

void grid_toggle_cell(Grid *grid, const int y, const int x) {
    const bool is_alive = grid->front_cells[y * grid->cols + x];
    grid->front_cells[y * grid->cols + x] = !is_alive;
}

void grid_clear(Grid *grid) {
    memset(
        grid->front_cells,
        false,
        grid->rows * grid->cols * sizeof *grid->front_cells
    );
}

void grid_randomize(Grid *grid) {
    for (size_t y = 0; y < grid->rows; y++) {
        for (size_t x = 0; x < grid->cols; x++) {
            grid->front_cells[y * grid->cols + x] = rand() % 2;
        }
    }
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
