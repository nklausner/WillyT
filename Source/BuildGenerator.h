#pragma once
#include <BWAPI.h>
#include "MapUtilities.h"
#include "WillyThreat.h"

//2018-09-02: need a random build tile generator

struct BuildGenerator
{
public:
	BWAPI::TilePosition generate_random_tile(BWAPI::UnitType my_type, BWAPI::TilePosition my_start, int my_space);
	BWAPI::TilePosition vary_build_tile(BWAPI::UnitType my_type, BWAPI::TilePosition my_tile);
	BWAPI::TilePosition generate_pair_tile(std::vector<BWAPI::Unit> &my_vector);
	BWAPI::TilePosition generate_proxy_tile(BWAPI::UnitType my_type, BWAPI::TilePosition my_start, std::vector<BWAPI::Position> &my_vec, BWAPI::Position my_choke, int(&my_dist_map)[256][256]);

private:
	BWAPI::TilePosition t;
	int r;
	int s;
	int x;
	int y;
	int w;
	int h;
	int n;
	int i;

	BWAPI::TilePosition check_line_top(int x0, int y0);
	BWAPI::TilePosition check_line_bot(int x0, int y0);
	BWAPI::TilePosition check_line_lef(int x0, int y0);
	BWAPI::TilePosition check_line_rig(int x0, int y0);
	BWAPI::TilePosition check_line_ver(int x0, int y0);
	BWAPI::TilePosition check_line_hor(int x0, int y0);

	bool check_pair_tile(std::vector<BWAPI::Unit> &my_vector);
};