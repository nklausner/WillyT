#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillyThreat.h"
#include "MapUtilities.h"
#include "Utilities.h"

//2019-08-09: initialize build planner

namespace wilplanner
{

	//public
	void create_buildplan();
	BWAPI::TilePosition choose_tile(std::vector<BWAPI::TilePosition>& v, int w, int h);
	BWAPI::TilePosition choose_tech(std::vector<BWAPI::TilePosition>& v, std::vector<BWAPI::TilePosition>& vt, int w, int h);
	BWAPI::TilePosition choose_tile_near(std::vector<BWAPI::TilePosition>& v, int w, int h, BWAPI::TilePosition t);


	//private
	void fill_main_map(int x, int y);
	void fill_mining_space(int x, int y);
	void fill_center_roundabout(int x, int y);
	void fill_main_corridors();

	void create_buildplan_position(int x0, int y0);
	void create_buildplan_columns(int x0, int y0);
	void balance_buildplan();
	void avoid_refinery_trap();
	void avoid_depot_trap(); //lex andromeda
	void sort_vector_tile(std::vector<BWAPI::TilePosition>& v, BWAPI::TilePosition t0);
	void fill_vector_tech(std::vector<BWAPI::TilePosition>& v, std::vector<BWAPI::TilePosition>& vt, int w, int h);
	bool is_on_map_edge(int x, int y, int w, int h);
	//void create_buildplan_large();

}