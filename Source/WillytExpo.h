#pragma once
#include <BWAPI.h>
#include "SCV.h"

//2018-05-21: expo/base object created
//2019-04-06: added namespace to contain vector

struct Expo
{
	BWAPI::TilePosition tile;
	BWAPI::Position posi;
	std::vector<BWAPI::Unit> minerals;
	std::vector<BWAPI::Unit> geysers;
	std::vector<BWAPI::TilePosition> min_tiles;
	std::vector<BWAPI::TilePosition> gas_tiles;
	bool is_owned = false;
	bool is_enemy = false;
	bool is_main = false;
	bool is_natu = false;
	bool is_island = false;
	bool is_continent = false;
	bool is_constructing = false;
	unsigned min_miner_count = 0;
	unsigned gas_miner_count = 0;
	bool is_oversaturated = false;
	bool is_undrsaturated = false;
	BWAPI::Position tpos_min;
	BWAPI::Position tpos_gas;
	std::vector<BWAPI::TilePosition> def_tiles;
	int failed_build_attempts = 0;
	BWAPI::Position corresponding_main = BWAPI::Positions::None;
};

namespace wilexpo
{
	extern std::vector<Expo> all;
}