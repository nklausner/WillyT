#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillytState.h"
#include "MapUtilities.h"
#include "Utilities.h"

// 2018-10-18: generate main/natural choke defense positions
// 2021-02-04: generate (uncomplete) walls at naturals

void calculate_army_positions();
BWAPI::Position find_def_area(BWAPI::Position cp, BWAPI::Position bp, int i);
BWAPI::Position fill_def_area(BWAPI::Position dp, BWAPI::Position cp, BWAPI::TilePosition(&dt)[2]);
BWAPI::Position sort_def_area(BWAPI::Position dp, BWAPI::Position cp, BWAPI::TilePosition(&dt)[2]);
BWAPI::Position calculate_retreat_pos(BWAPI::Position my_pos);
BWAPI::Position find_gathering_pos(BWAPI::Position p1, BWAPI::Position p2);

int create_natu_wallin(int i);
void get_wall_space(int& s, bool& can_use, int x0, int y0, int x1, int y1, int a);
void rearrange_natu_def_area(int i, int nd);

int get_map_specific_wallin_angle(BWAPI::TilePosition my_tile, int my_angle);
int get_map_specific_minimum(BWAPI::TilePosition my_tile, int my_min);
void set_map_specific_compare_pos(BWAPI::TilePosition& my_tile, int i);
bool get_map_specific_use_walkable_border();
bool check_hardcoded_wallin();
	
	//BWAPI::Position get_center(std::vector<int> &v);
	//BWAPI::TilePosition find_path_min(int d0, int d1);
	//BWAPI::TilePosition path_fore(BWAPI::TilePosition t);
	//void flood_fill_choke(int x, int y, int z, std::vector<int> &v);

	//void create_width_map();
	//int measure_width(BWAPI::TilePosition t, int dx, int dy);
	//int measure_side(BWAPI::TilePosition t, int dx, int dy, int di);