#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct Grid {
    bool *cells;
    bool *front_cells;
    bool *back_cells;
    const size_t rows;
    const size_t cols;
} Grid;

Grid grid_init(const size_t rows, const size_t cols);
void grid_free(Grid *grid);
void grid_print(const Grid *grid);
void grid_toggle_cell(Grid *grid, int y, int x);

#endif // GRID_H
