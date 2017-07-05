#pragma once
const short NUM_COMPONENT_TYPES = 4;
const short NUM_RESOURCE_TYPES = 2;
const short GRID_X_SIZE = 5;
const short GRID_Y_SIZE = 3;

const int GRID_SIZE = GRID_X_SIZE*GRID_Y_SIZE;
typedef unsigned char cell;
typedef cell* grid;
typedef unsigned int gsize_t;
typedef signed long goffset_t;//max offset = entire grid.
typedef signed int result_t;//type of result used in genetic algorithm to evaluate candidates

//o shit resource grids fuck shitup
//resources have different sizes? or no?
typedef signed short res_cell;

//setup function type: cell* thisCell,gsize_t thisX,gsize_t thisY -> void
typedef const void (*component_func_t)(gsize_t,grid);

//basically instead of doing (instance.field), we want to write field[instance]

typedef struct {
	uint8_t acceptsHeat:1;
	uint8_t pad:7;
} cell_properties;


//localA is for variables that represent something for every type. localB is for whatever scrap you want

typedef struct {
	uint8_t adjComponents[NUM_COMPONENT_TYPES];
	uint8_t Prop1:1;
	uint8_t localA;
	uint8_t localB;
} localVars;

#define COUNT_SIMS true
const unsigned long ONE_BILLION = 1000000000;
thread_local unsigned long count_sims_low = 0;
thread_local unsigned long count_sims_high = 0;
