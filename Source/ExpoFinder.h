#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillytExpo.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytEnemy.h"
#include "WillytState.h"
#include "ExpoUtilities.h"
#include "MapUtilities.h"

//2018-06-23: search bases for expansions

struct ExpoFinder
{
public:
	void check();

private:
	void check_additional_expansion();
	void check_additional_refinery();
	void build(BWAPI::UnitType my_type, BWAPI::TilePosition my_tile);
	BWAPI::TilePosition calc_enemy_center();
	bool allow_min_only_expansion();
	bool any_geyser_available();
	int calc_theo_income_min();
	int calc_theo_income_gas();
	void update_unclaimed_expo();
	void add_suspected_expo(int imax);
	bool allow_additional_wierd_expansion_condition();
	void check_fast_island_expansion();
	void check_build_wallin();
	void define_sneaky_tile();
	BWAPI::TilePosition find_sneaky_tile(int x0, int y0);
	void count_highground_defense();
};