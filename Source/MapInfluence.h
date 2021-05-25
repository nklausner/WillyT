#pragma once
#include <BWAPI.h>
#include "WillytMap.h"

//2019-02-16: set areas on int maps

//(a)ttack, (m)edium, (l)arge, siege(t)tank
void set_influence_a(int(&my_map)[256][256], BWAPI::TilePosition my_tile, int dz);
void set_influence_m(int(&my_map)[256][256], BWAPI::TilePosition my_tile, int dz);
void set_influence_l(int(&my_map)[256][256], BWAPI::TilePosition my_tile, int dz);
void set_influence_t(bool(&my_map)[256][256], BWAPI::TilePosition my_tile);

void set_influence_line_even(int(&my_map)[256][256], int y, int x0, int dx, int dz);
void set_influence_line_odd(bool(&my_map)[256][256], int y, int x0, int dx, bool b);

//change variable circle area on a map, size 16, 56/58, 72, 104, 60, 68
void set_influence(int(&my_map)[1024][1024], int x0, int y0, int size, int dz);
void set_influence(int(&my_map)[1024][1024], int x0, int y0, int size, int my_arr[], int dz);
void set_influence(int(&my_map)[1024][1024], BWAPI::Position p, int size, int my_arr[], int dz);

void set_influence_line_even(int(&my_map)[1024][1024], int y, int x0, int dx, int dz);
void set_influence_line_odd(int(&my_map)[1024][1024], int y, int x0, int dx, int dz);
