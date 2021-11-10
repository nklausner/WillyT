#pragma once
#include <BWAPI.h>
#include "BuildGenerator.h"
#include "BuildPlanner.h"
#include "WillytMap.h"
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytEnemy.h"
#include "BuildDefense.h"
#include "Utilities.h"
#include "MapUtilities.h"

//2018-03-08: building manager
//2018-09-28: can add 3 missile turrets in main base
//2019-04-15:
//2019-08-13: using 6x6-grid build plan

struct BuildManager
{
public:
	//check for building:
	//--- check for building queue
	//--- check for building type
	//--- check for resources
	//--- check for building tile
	//--- choose scv and build
	void init();
	void build();
	void build_consecutive(BWAPI::Unit my_last);

private:
	BuildGenerator buildgenerator;
	int building_queue = 0;
	bool use_buildplan;

	BWAPI::TilePosition my_tile;
	BWAPI::UnitType my_type;

	BWAPI::UnitType check_order_1();
	BWAPI::UnitType check_order_2();
	BWAPI::UnitType check_order_3();
	BWAPI::UnitType check_order_4();
	BWAPI::UnitType check_order_6();
	BWAPI::UnitType check_depot();
	BWAPI::UnitType check_turrets();
	BWAPI::UnitType check_advanced();

	BWAPI::TilePosition choose_bunker_tile();
	BWAPI::TilePosition choose_turret_tile();
	BWAPI::TilePosition choose_commandcenter_tile();
	BWAPI::TilePosition choose_refinery_tile();
	std::vector<BWAPI::TilePosition> turret_tiles;

	BWAPI::TilePosition sas_tile;
	bool need_one_bunker();
	bool soon_completed(std::vector<BWAPI::Unit> &my_vec);

	void check_proxy_building();

	int gas;
	int min;
};