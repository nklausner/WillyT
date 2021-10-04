#include "WillytBuild.h"

//Building::Building(BWAPI::Unit my_unit, bool my_island) {
//	unit = my_unit;
//	id = my_unit->getID();
//	is_island = my_island;
//	std::vector<int> miners = {};
//}


namespace wilbuild
{

	std::vector<BWAPI::Unit> commandcenters;
	std::vector<BWAPI::Unit> refineries;

	std::vector<BWAPI::Unit> depots;
	std::vector<BWAPI::Unit> academies;
	std::vector<BWAPI::Unit> armories;
	std::vector<BWAPI::Unit> bunkers;
	std::vector<BWAPI::Unit> turrets;

	std::vector<BWAPI::Unit> barracks;
	std::vector<BWAPI::Unit> factories;
	std::vector<BWAPI::Unit> starports;
	std::vector<BWAPI::Unit> engineerbays;
	std::vector<BWAPI::Unit> sciencefacs;

	std::vector<BWAPI::Unit> comsatstations;
	std::vector<BWAPI::Unit> nuclearsilos;
	std::vector<BWAPI::Unit> machineshops;
	std::vector<BWAPI::Unit> controltowers;
	std::vector<BWAPI::Unit> covertops;
	std::vector<BWAPI::Unit> physicslabs;

	std::vector<BWAPI::Unit> buildings;
	std::vector<BWAPI::Unit> smallbs;
	std::vector<BWAPI::UnitType> planned_builds;
	unsigned planned_build_queue = 0;
	unsigned locking_time = 300;
	BWAPI::TilePosition blocked_tile = BWAPI::TilePositions::None;
	bool scanned_blocked_tile = false;

	std::vector<Resource> minerals;
	std::vector<Resource> geysers;
	//MINERAL_LOCK_UPDATE
	//std::vector<BWAPI::Unit> minerals;
	//std::vector<BWAPI::Unit> geysers;
	//std::vector<BWAPI::Unit> mainland_minerals;
	//std::vector<BWAPI::Unit> mainland_geysers;
	//std::vector<BWAPI::Unit> all_minerals;
	//std::vector<BWAPI::Unit> all_geysers;
	//std::vector<BWAPI::TilePosition> all_mintiles;
	//std::vector<BWAPI::TilePosition> all_gastiles;



	void append_building(BWAPI::Unit unit) {
		if (!unit->getType().isBuilding()) return;
		using namespace wilbuild;
		using namespace BWAPI::UnitTypes;
		
		switch (unit->getType())
		{
		case Terran_Command_Center:		commandcenters.push_back(unit); break;
		case Terran_Refinery:			refineries.push_back(unit); break;

		case Terran_Supply_Depot:		depots.push_back(unit); smallbs.push_back(unit); break;
		case Terran_Academy:			academies.push_back(unit); smallbs.push_back(unit); break;
		case Terran_Armory:				armories.push_back(unit); smallbs.push_back(unit); break; //
		case Terran_Bunker:				bunkers.push_back(unit); break;
		case Terran_Missile_Turret:		turrets.push_back(unit); break;

		case Terran_Barracks:			barracks.push_back(unit); break;
		case Terran_Factory:			factories.push_back(unit); break;
		case Terran_Starport:			starports.push_back(unit); break;
		case Terran_Engineering_Bay:	engineerbays.push_back(unit); break;
		case Terran_Science_Facility:	sciencefacs.push_back(unit); break;

		case Terran_Comsat_Station:		comsatstations.push_back(unit); break;
		case Terran_Nuclear_Silo:		nuclearsilos.push_back(unit); break;
		case Terran_Machine_Shop:		machineshops.push_back(unit); break;
		case Terran_Control_Tower:		controltowers.push_back(unit); break;
		case Terran_Covert_Ops:			covertops.push_back(unit); break;
		case Terran_Physics_Lab:		physicslabs.push_back(unit); break;
		}
		if (is_none(wilmap::en_start) ||
			sqdist(wilmap::en_start, unit->getTilePosition()) > 1024) {
			buildings.push_back(unit); //excluding proxy buildings
		}

		if (!unit->isLifted()) {
			if (unit->getType().canBuildAddon()) {
				BWAPI::TilePosition t = unit->getTilePosition();
				set_map_area(wilmap::build_map_var, t.x + 4, t.y + 1, 2, 2, false);
			}
			set_map_area(wilmap::build_map_var, unit, false);
		}
		if (!unit->getType().isAddon() &&
			!unit->isLifted()) {
			change_influence(unit, +1);
		}

	}

	void remove_building(BWAPI::Unit unit) {
		if (!unit->getType().isBuilding()) return;
		using namespace wilbuild;
		using namespace BWAPI::UnitTypes;

		switch (unit->getType())
		{
		case Terran_Command_Center:		vector_remove(commandcenters, unit); break;
		case Terran_Refinery:			vector_remove(refineries, unit); break;

		case Terran_Supply_Depot:		vector_remove(depots, unit); vector_remove(smallbs, unit); break;
		case Terran_Academy:			vector_remove(academies, unit); vector_remove(smallbs, unit); break;
		case Terran_Armory:				vector_remove(armories, unit); vector_remove(smallbs, unit); break;
		case Terran_Bunker:				vector_remove(bunkers, unit); change_inf_bunker(unit, -1); break;
		case Terran_Missile_Turret:		vector_remove(turrets, unit); change_inf_turret(unit, -1); break;

		case Terran_Barracks:			vector_remove(barracks, unit); break;
		case Terran_Factory:			vector_remove(factories, unit); break;
		case Terran_Starport:			vector_remove(starports, unit); break;
		case Terran_Engineering_Bay:	vector_remove(engineerbays, unit); break;
		case Terran_Science_Facility:	vector_remove(sciencefacs, unit); break;

		case Terran_Comsat_Station:		vector_remove(comsatstations, unit); break;
		case Terran_Nuclear_Silo:		vector_remove(nuclearsilos, unit); break;
		case Terran_Machine_Shop:		vector_remove(machineshops, unit); break;
		case Terran_Control_Tower:		vector_remove(controltowers, unit); break;
		case Terran_Covert_Ops:			vector_remove(covertops, unit); break;
		case Terran_Physics_Lab:		vector_remove(physicslabs, unit); break;
		}
		vector_remove(buildings, unit);

		if (!unit->isLifted()) {
			if (unit->getType().canBuildAddon()) {
				BWAPI::TilePosition t = unit->getTilePosition();
				set_map_area(wilmap::build_map_var, t.x + 4, t.y + 1, 2, 2, true);
			}
			clear_build_area(wilmap::build_map_var, wilmap::build_map_fix, unit);
		}
		if (!unit->getType().isAddon() &&
			!unit->isLifted()) {
			change_influence(unit, -1);
		}
	}

	void change_influence(BWAPI::Unit unit, int dz) {
		if (unit->getType().tileWidth() == 4) {
			// Terran_Command_Center
			// Terran_Refinery
			// Terran_Barracks
			// Terran_Factory
			// Terran_Starport
			// Terran_Engineering_Bay
			// Terran_Science_Facility
			set_influence_l(wilmap::defense_map, unit->getTilePosition(), dz);
		}
		else {
			// Terran_Supply_Depot ||
			// Terran_Academy ||
			// Terran_Armory ||
			// Terran_Bunker ||
			// Terran_Missile_Turret) {
			set_influence_m(wilmap::defense_map, unit->getTilePosition(), dz);
		}
		return;
	}


	void change_inf_bunker(BWAPI::Unit unit, int dz) {
		set_influence_m(wilmap::bunker_map, unit->getTilePosition(), dz);
		return;
	}
	void change_inf_turret(BWAPI::Unit unit, int dz) {
		set_influence_m(wilmap::turret_map, unit->getTilePosition(), dz);
		return;
	}


	void lift(BWAPI::Unit unit)
	{
		set_map_area(wilmap::build_map_var, unit, true);
		set_influence_l(wilmap::defense_map, unit->getTilePosition(), -1);
		unit->lift();
		return;
	}
	void land(BWAPI::Unit unit, BWAPI::TilePosition tile)
	{
		set_map_area(wilmap::build_map_var, tile.x, tile.y, 4, 3, false);
		set_influence_l(wilmap::defense_map, tile, +1);
		unit->land(tile);
		return;
	}

}

Resource::Resource(BWAPI::Unit my_unit)
{
	unit = my_unit;
	tile = my_unit->getTilePosition();
	id = my_unit->getID();
	is_island = false;
	is_owned = false;
	is_enemy = false;
	scv_count = 0;
	return;
}