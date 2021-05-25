#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillytExpo.h"
#include "WillytBuild.h"
#include "WillytState.h"
#include "Utilities.h"
#include "MapUtilities.h"
#include "ReadWrite.h"

//2019-04-15: generate defense building tiles

//determine expo turret tiles
void determine_defs_all();
int get_defense_orientation(BWAPI::Position r, BWAPI::Position c);

//check expo turet cover
void define_turret_tiles(std::vector<BWAPI::TilePosition>& my_vector, BWAPI::TilePosition t0, bool need_more);
BWAPI::TilePosition get_max_turret_tile(std::vector<BWAPI::TilePosition> &my_vector);
int get_min_sqdist(BWAPI::TilePosition my_tile, std::vector<BWAPI::Unit> &my_vector);
//bool is_turret_tile(BWAPI::TilePosition my_tile);
//bool is_turret_near(BWAPI::TilePosition my_tile);

//determine starport_and_sciencefacility_tile
//BWAPI::TilePosition determine_sas_tile();
//BWAPI::TilePosition get_sas_tile_line(int x0, int y0, int dx, int dy);