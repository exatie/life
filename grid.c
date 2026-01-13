#include "grid.h"
#include <ncurses.h>
#include <string.h>

static int grid_count_neighbors(const Grid *grid, int cy, int cx);

int wrap(int n, const int ceiling) {
    static const int FLOOR = 0;

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

void grid_resize(Grid *grid, const size_t new_rows, const size_t new_cols) {
    if (new_rows == grid->rows && new_cols == grid->cols) return;

    bool *new_cells = calloc(2 * new_rows * new_cols, sizeof *new_cells);
    if (!new_cells) abort();

    const size_t copy_rows = new_rows < grid->rows ? new_rows : grid->rows;
    const size_t copy_cols = new_cols < grid->cols ? new_cols : grid->cols;

    for (size_t y = 0; y < copy_rows; y++) {
        for (size_t x = 0; x < copy_cols; x++) {
            new_cells[y * new_cols + x] = grid->front_cells[y * grid->cols + x];
        }
    }

    free(grid->cells);

    grid->cells       = new_cells;
    grid->front_cells = new_cells;
    grid->back_cells  = new_cells + new_rows * new_cols;
    grid->rows        = new_rows;
    grid->cols        = new_cols;
}

void grid_evolve(Grid *grid) {
    for (size_t y = 0; y < grid->rows; y++) {
        for (size_t x = 0; x < grid->cols; x++) {
            const int  neighbors = grid_count_neighbors(grid, y, x);
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

void grid_toggle_cell(Grid *grid, const int y, const int x) {
    const bool is_alive = grid->front_cells[y * grid->cols + x];
    grid->front_cells[y * grid->cols + x] = !is_alive;
}

void grid_print(const Grid *grid) {
    for (size_t y = 0; y < grid->rows; y++) {
        for (size_t x = 0; x < grid->cols; x++) {
            if (grid->front_cells[y * grid->cols + x]) mvaddch(y, x, '#');
        }
    }
}
