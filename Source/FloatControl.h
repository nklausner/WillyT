#pragma once
#include <BWAPI.h>
#include "BuildPlanner.h"
#include "ExpoManager.h"
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytMap.h"
#include "WillytEnemy.h"
#include "WillyThreat.h"
#include "Grouper.h"
#include "Utilities.h"

struct FloatControl
{
public:
	void update(ExpoManager& expomanager);

private:
	void lift_all(std::vector<BWAPI::Unit>& my_vec);
	void check_relocation(BWAPI::Unit my_unit);
	void check_relocation_wallin(BWAPI::Unit my_unit, BWAPI::TilePosition my_tile);
	void check_open_close_wallin(BWAPI::Unit my_unit, BWAPI::TilePosition my_tile);
	void relocate_to(BWAPI::Unit my_unit, BWAPI::TilePosition my_tile);
	void check_lift_base_when_cannon_rushed(BWAPI::Unit my_unit, ExpoManager& expomanager);
	void check_land_base_when_cannon_rushed(BWAPI::Unit my_unit, ExpoManager& expomanager);

	bool need_occupying = false;
	std::vector<BWAPI::Unit> floaters;

	void float_scout_enemy_siegetanks();
	void update_float_scouts();
	BWAPI::Position get_enemy_siegetank_pos();
};