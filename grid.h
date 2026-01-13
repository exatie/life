#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct Grid {
    bool   *cells;
    bool   *front_cells;
    bool   *back_cells;
    size_t  rows;
    size_t  cols;
} Grid;

int  wrap(int n, int ceiling);
Grid grid_init(size_t rows, size_t cols);
void grid_free(Grid *grid);
void grid_resize(Grid *grid, size_t new_rows, size_t new_cols);
void grid_evolve(Grid *grid);
void grid_clear(Grid *grid);
void grid_randomize(Grid *grid);
void grid_toggle_cell(Grid *grid, int y, int x);
void grid_print(const Grid *grid);

#endif // GRID_H
