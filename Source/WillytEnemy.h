#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillyThreat.h"
#include "Utilities.h"
#include "MapUtilities.h"
#include "MapInfluence.h"
#include "EnemyUtilities.h"


//2018-03-11: list for storing enemy information
//2019-08-17: dividing namespace and struct


namespace wilenemy
{
	//enemy basics

	extern BWAPI::Race race;
	extern std::string name;
	extern std::string score;
	extern int result[8];
	extern std::vector<std::string> history;

	//enemy building positions

	extern std::vector<BWAPI::Position> positions;
	extern std::vector<BWAPI::Position> main_pos;
	extern int sem;								//single enemy main (number)
	extern std::vector<BWAPI::TilePosition> incomplete_def;

	//enemy supply of various kinds

	extern int supply_work;
	extern int supply_army;
	extern int supply_small;
	extern int supply_large;
	extern int supply_air;
	extern int supply_cloak;
	extern int air_percentage;
	extern int small_percentage;
	extern int cloak_percentage;

	//enemy units in own base

	extern std::vector<BWAPI::Unit> intruders;	//enemy units in defense range
	extern int intruder_totstr;					//enemy unit evaluated strength
	extern int intruder_airstr;					//enemy unit evaluated strength
	extern std::vector<BWAPI::Unit> near_bunkers;
	extern int supply_near_bunkers;

	//enemy defense and production building counts

	extern int grddef_count;
	extern int airdef_count;
	extern int produc_count;				//all prodction buildings
	extern int airprod_count;				//spaceport, stargate, spires
	extern int mainbuild_count;				//hatcheries, comcenters, nexi

	//enemy tech

	extern bool has_covertops;
	extern bool has_physiclab;
	extern bool has_nukesilo;

	extern bool has_pool;
	extern bool has_hydraden;
	extern bool has_lair;
	extern bool has_hive;
	extern bool has_defmound;
	extern bool has_ultracav;
	extern bool has_lurkers;

	extern bool has_forge;
	extern bool has_robo;
	extern bool has_archives;
	extern bool has_tribunal;
	extern bool has_fleetbea;
	extern unsigned time_lair_reaction;

	//suspected or recognized enemy strategies

	extern bool goes_megamacro;

	//target lists

	extern std::vector<BWAPI::Unit> tlg;	//target list ground
	extern std::vector<BWAPI::Unit> tlgg;	//target list ground to ground
	extern std::vector<BWAPI::Unit> tlga;	//target list ground to air
	extern std::vector<BWAPI::Unit> tlgs;	//target list ground special
	extern std::vector<BWAPI::Unit> tlgc;	//target list ground cloaked
	extern std::vector<BWAPI::Unit> tlgw;	//target list ground worker
	extern std::vector<BWAPI::Unit> tlgm;	//target list ground small/medium
	extern std::vector<BWAPI::Unit> tlgl;	//target list ground large
	extern std::vector<BWAPI::Unit> tla;	//target list air
	extern std::vector<BWAPI::Unit> tlag;	//target list air to ground
	extern std::vector<BWAPI::Unit> tlaa;	//target list air to air
	extern std::vector<BWAPI::Unit> tlas;	//target list air special
	extern std::vector<BWAPI::Unit> tlac;	//target list air cloaked

	extern std::vector<BWAPI::Unit> tleggla;	//target list egg/larva
	extern std::vector<BWAPI::Unit> tlflare;	//target list flare
	extern std::vector<BWAPI::Unit> tllockd;	//target list lock down
	extern std::vector<BWAPI::Unit> tlyama;		//target list yamato gun
	extern std::vector<BWAPI::Unit> tldefg;		//target list ground defense buildings
	extern std::vector<BWAPI::Unit> tldefa;		//target list air defense buildings
	extern std::vector<BWAPI::Unit> tlself;		//target list to force attacking own units

	extern int target_count;

	extern std::vector<BWAPI::Position> unclaimed_expo_pos;
}

struct EnemyManager
{
public:
	EnemyManager();

	void append_unit(BWAPI::Unit unit);
	void remove_unit(BWAPI::Unit unit);
	void change_unit(BWAPI::Unit unit);
	void update();
	void correct();

	void check_time_lair(BWAPI::Unit u, bool just_changed);

private:
	bool need_race = false;
	std::vector<int> ids;

	void determine_race(BWAPI::Unit unit);
	void determine_main(BWAPI::Unit unit);
	void check_defense(BWAPI::Unit unit, int dz);
	void handle_defense(BWAPI::Unit u, int dz, int size, bool is_grd, bool is_air, bool is_det);
	void check_incomplete_defense();
	void check_tech(BWAPI::UnitType type, int dz);
	void check_tech_unit(BWAPI::UnitType type);

	void change_supply(BWAPI::UnitType t, int ds);
	bool is_small(BWAPI::UnitType t);
	bool can_cloak(BWAPI::UnitType t);
	void clear_deducted_build_tiles(BWAPI::Position p);

};

int get_intruder_count(BWAPI::UnitType my_type);