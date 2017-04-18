//gridManip.h - contains grid manipulation methods
#pragma once
#include <cstring>
#include "types.hpp"
//functions for detecting if the given index is on an edge of the grid.
inline bool is_left_edge(gsize_t index) noexcept{
	return (index%GRID_X_SIZE == 0);
}
inline bool is_right_edge(gsize_t index) noexcept{
	return ((index+1)%GRID_X_SIZE == 0);
}
inline bool is_top_edge(gsize_t index) noexcept{
	return (index < GRID_X_SIZE);
}
inline bool is_bottom_edge(gsize_t index) noexcept{
	return (index >= ((GRID_Y_SIZE-1)*GRID_X_SIZE));
}

//returns linear offset given an x offset - X can be negative!
inline goffset_t cell_XOffset(goffset_t deltaX) noexcept{
	return deltaX;//*std::static_cast<goffset_t>(sizeof cell)
}
//returns linear offset given a y offset - y can be negative!
inline goffset_t cell_YOffset(goffset_t deltaY, gsize_t xSize) noexcept{
	return deltaY* (static_cast<goffset_t>(xSize));//*std::static_cast<goffset_t>(sizeof cell)
}

inline gsize_t cell_YPos(gsize_t cellIndex) noexcept{
	return cellIndex/GRID_X_SIZE;
}

inline gsize_t cell_XPos(gsize_t cellIndex) noexcept{
	return cellIndex%GRID_X_SIZE;
}

//turns (x,y) coords into a linear offset - x,y CANNOT be negative!
inline gsize_t cell_linear_offset(gsize_t x,gsize_t y) noexcept{
	return x+(y*GRID_X_SIZE);
}

//get the address of the neighboring cell.
inline cell* cell_neighbor(cell* thisCell, goffset_t deltaX, goffset_t deltaY) noexcept{
	return thisCell + cell_XOffset(deltaX) + cell_YOffset(deltaY, GRID_X_SIZE);
}
	

int sum_grid(grid grid, gsize_t gridSize,  unsigned int resource) noexcept;
int setup_grid(grid typegrid, gsize_t gridSize) noexcept;
int run_grid(grid typegrid, gsize_t gridSize) noexcept;
void increment_grid(grid grid, gsize_t gridSize) noexcept;
void sumAdjacentComponents(gsize_t thisCell) noexcept;
void clearNonTypeGrids() noexcept;


