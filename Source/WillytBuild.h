#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillytState.h"
#include "Utilities.h"
#include "MapUtilities.h"
#include "MapInfluence.h"

//2018-03-07: building list
//2018-09-02: remove struct
//2019-03-17: add namespace

//struct Building
//{
//	Building(BWAPI::Unit my_unit, bool my_island);
//	BWAPI::Unit unit;
//	int id;
//	bool is_island;
//	std::vector<int> miners;
//};


struct Resource
{
	Resource(BWAPI::Unit my_unit);

	BWAPI::Unit unit;
	BWAPI::TilePosition tile;
	int id;
	bool is_island;
	bool is_owned;
	bool is_enemy;
	int scv_count;
};


namespace wilbuild
{

	//building lists
	extern std::vector<BWAPI::Unit> commandcenters;
	extern std::vector<BWAPI::Unit> refineries;

	extern std::vector<BWAPI::Unit> depots;
	extern std::vector<BWAPI::Unit> academies;
	extern std::vector<BWAPI::Unit> armories;
	extern std::vector<BWAPI::Unit> bunkers;
	extern std::vector<BWAPI::Unit> turrets;

	extern std::vector<BWAPI::Unit> barracks;
	extern std::vector<BWAPI::Unit> factories;
	extern std::vector<BWAPI::Unit> starports;
	extern std::vector<BWAPI::Unit> engineerbays;
	extern std::vector<BWAPI::Unit> sciencefacs;

	extern std::vector<BWAPI::Unit> comsatstations;
	extern std::vector<BWAPI::Unit> nuclearsilos;
	extern std::vector<BWAPI::Unit> machineshops;
	extern std::vector<BWAPI::Unit> controltowers;
	extern std::vector<BWAPI::Unit> covertops;
	extern std::vector<BWAPI::Unit> physicslabs;

	//building lists
	extern std::vector<BWAPI::Unit> buildings;
	extern std::vector<BWAPI::Unit> smallbs;
	extern std::vector<BWAPI::UnitType> planned_builds;
	extern unsigned planned_build_queue;
	extern unsigned locking_time;
	extern BWAPI::TilePosition blocked_tile;
	extern bool scanned_blocked_tile;

	//resource lists
	extern std::vector<Resource> minerals;
	extern std::vector<Resource> geysers;

	//MINERAL_LOCK_UPDATE
	//extern std::vector<BWAPI::Unit> minerals;
	//extern std::vector<BWAPI::Unit> geysers;
	//extern std::vector<BWAPI::Unit> mainland_minerals;
	//extern std::vector<BWAPI::Unit> mainland_geysers;
	//extern std::vector<BWAPI::Unit> all_minerals;
	//extern std::vector<BWAPI::Unit> all_geysers;
	//extern std::vector<BWAPI::TilePosition> all_mintiles;
	//extern std::vector<BWAPI::TilePosition> all_gastiles;

	void append_building(BWAPI::Unit unit);
	void remove_building(BWAPI::Unit unit);
	void change_influence(BWAPI::Unit unit, int dz);
	void change_inf_bunker(BWAPI::Unit unit, int dz);
	void change_inf_turret(BWAPI::Unit unit, int dz);

	void lift(BWAPI::Unit unit);
	void land(BWAPI::Unit unit, BWAPI::TilePosition tile);

}
