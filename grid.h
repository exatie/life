#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stdlib.h>

int wrap(int n, int ceiling);

typedef struct Grid {
    bool     *cells;
    bool     *front_cells;
    bool     *back_cells;
    size_t    rows;
    size_t    cols;
    unsigned  generation;
} Grid;

Grid grid_init(size_t rows, size_t cols);
void grid_free(Grid *grid);
void grid_resize(Grid *grid, size_t new_rows, size_t new_cols);
void grid_evolve(Grid *grid);
void grid_clear(Grid *grid);
void grid_randomize(Grid *grid);
void grid_toggle_cell(Grid *grid, int y, int x);
void grid_print(const Grid *grid);

typedef struct GridSave {
    bool     *cells;
    size_t    rows;
    size_t    cols;
    unsigned  generation;
} GridSave;

GridSave grid_save_init(void);
void     grid_save_free(GridSave *save);
void     grid_save(const Grid *grid, GridSave *save);
void     grid_load(Grid *grid, const GridSave *save);

#endif // GRID_H
