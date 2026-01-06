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

int  wrap(int n, const int ceiling);
Grid grid_init(const size_t rows, const size_t cols);
void grid_free(Grid *grid);
void grid_draw(const Grid *grid);
void grid_toggle_cell(Grid *grid, const int y, const int x);
void grid_clear(Grid *grid);
void grid_randomize(Grid *grid);
void grid_simulate(Grid *grid);

#endif // GRID_H
