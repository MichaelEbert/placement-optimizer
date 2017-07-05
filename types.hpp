#pragma once
const short NUM_COMPONENT_TYPES = 4;
const short NUM_RESOURCE_TYPES = 2;
const short GRID_X_SIZE = 4;
const short GRID_Y_SIZE = 3;

const int GRID_SIZE = GRID_X_SIZE*GRID_Y_SIZE;
typedef unsigned char cell;
typedef cell* grid;
typedef unsigned int gsize_t;
typedef signed long goffset_t;//max offset = entire grid.
typedef signed int result_t;//type of result used in genetic algorithm to evaluate candidates


//GRID TYPEDEFS
//basically instead of doing (instance.field), we want to write field[instance]
//grids: adjacency special grid, resource grids, property grid 
typedef uint8_t adjacency_t;//type of adjacency special grid
typedef signed short res_cell;
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


//FUNCTION TYPES
//this struct contains thread local vars
typedef struct{
	gsize_t thisCell;
	grid typegrid;
	adjacency_t* adjacency_sg;
	cell_properties* properties_g;
	res_cell* energy_g;
	res_cell* heat_g;
} function_args;

//most functions should take in all variables for use... huh. think about inlining more maybe?
//function example declaration: func(gsize_t thisCell, grid typegrid, adjacency_t* adjacency_sg)
typedef const void (*component_func_t)(function_args&);



#define COUNT_SIMS true
const unsigned long ONE_BILLION = 1000000000;
unsigned long count_sims_low = 0;
unsigned long count_sims_high = 0;
