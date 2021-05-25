#pragma once
#include <BWAPI.h>
#include "BuildDefense.h"
#include "SCV.h"
#include "WillytBuild.h"
#include "WillytExpo.h"
#include "WillytState.h"
#include "Utilities.h"
#include "MapUtilities.h"
#include "ExpoUtilities.h"

//2018-05-21: expo/base list created
//2019-04-06: removed expo vector

struct ExpoManager
{
public:
	void init();
	void update(std::vector<SCV>& scvs);
	void correct();

	void occupy_expo(BWAPI::Unit my_unit);
	void desert_expo(BWAPI::TilePosition &tile);
	void destroy_mineral(BWAPI::Unit my_min);
	void destroy_geyser(BWAPI::Unit my_gas);
	void rebuild_geyser(BWAPI::Unit my_gas);
	void set_enemy_expo(BWAPI::TilePosition tile, bool status);

private:
	void set_expo_resources(Expo& my_expo, bool my_value);
	void check_construction(Expo& my_expo);
	void update_miners(Expo& my_expo, std::vector<SCV>& scvs);
	bool all_visible(std::vector<BWAPI::TilePosition>& my_vec);
	bool all_existing(std::vector<BWAPI::Unit>& my_vec);
	void init_all_resources();
	bool decide_islands();
	void set_resource_owned(std::vector<Resource>& my_vec, int id, bool my_value);
	void remove_resource(std::vector<Resource>& my_vec, BWAPI::Unit my_unit);

};