#pragma once
#include <BWAPI.h>
#include "BuildPlanner.h"
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytMap.h"
#include "WillytEnemy.h"
#include "WillyThreat.h"
#include "Utilities.h"

struct FloatControl
{
public:
	void update();

private:
	void check_relocation(BWAPI::Unit my_unit);
	void check_relocation_wallin(BWAPI::Unit my_unit, BWAPI::TilePosition my_tile);
	void check_open_close_wallin(BWAPI::Unit my_unit, BWAPI::TilePosition my_tile);
	void relocate_to(BWAPI::Unit my_unit, BWAPI::TilePosition my_tile);
};