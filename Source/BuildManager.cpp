#include "BuildManager.h"

void BuildManager::init() {
	use_buildplan = true;
	wilplanner::create_buildplan();
	if (willyt::strategy == 1) { wilbuild::locking_time = 450; }
	if (willyt::strategy == 4) { wilbuild::locking_time = 720; }
}



void BuildManager::build() {
	using namespace wilbuild;
	using namespace wilmap;
	using namespace BWAPI::UnitTypes;
	if (building_queue > 0) {
		building_queue--;
		return;
	}

	min = BWAPI::Broodwar->self()->minerals();
	gas = BWAPI::Broodwar->self()->gas();
	my_tile = BWAPI::TilePositions::None;
	my_type = BWAPI::UnitTypes::None;

	check_proxy_building();

	if (my_type == None) my_type = check_depot();
	if (my_type == None) my_type = check_turrets();
	if (my_type == Terran_Missile_Turret &&
		(BWAPI::Broodwar->elapsedTime() - 4) % 16 == 0) my_type = None;
	//dismiss every second turret to avoid blocking buildorder

	if (my_type == None && willyt::strategy == 1) my_type = check_order_1();
	if (my_type == None && willyt::strategy == 2) my_type = check_order_2();
	if (my_type == None && willyt::strategy == 3) my_type = check_order_3();
	if (my_type == None && willyt::strategy == 4) my_type = check_order_4();
	if (my_type == None && willyt::strategy == 6) my_type = check_order_6();
	if (my_type == None && willyt::strategy <= 3 &&
		BWAPI::Broodwar->self()->supplyUsed() > 120) my_type = check_advanced();

	if (my_type == None || vector_holds(planned_builds, my_type) ||
		min < my_type.mineralPrice() || gas < my_type.gasPrice()) {
		return;
	}

	if (true) {
		switch (my_type) {
		case Terran_Bunker:			my_tile = buildgenerator.vary_build_tile(my_type, choose_bunker_tile()); break;
		case Terran_Missile_Turret:	my_tile = buildgenerator.vary_build_tile(my_type, choose_turret_tile()); break;
		case Terran_Refinery:		my_tile = choose_refinery_tile(); break;
		case Terran_Command_Center:	my_tile = choose_commandcenter_tile(); break;
		}
	}
	if (use_buildplan) {
		switch (my_type) {
		case Terran_Supply_Depot:		my_tile = wilplanner::choose_tile(plan_small, 3, 2); break;
		case Terran_Academy:			my_tile = wilplanner::choose_tech(plan_small, plan_small_tech, 3, 2); break;
		case Terran_Armory:				my_tile = wilplanner::choose_tech(plan_small, plan_small_tech, 3, 2); break;
		case Terran_Barracks:			my_tile = wilplanner::choose_tile(plan_large, 4, 3); break;
		case Terran_Factory:			my_tile = wilplanner::choose_tile(plan_large, 6, 3); break;
		case Terran_Engineering_Bay:	my_tile = wilplanner::choose_tile(plan_large, 6, 3); break;
		case Terran_Starport:			my_tile = wilplanner::choose_tech(plan_large, plan_large_tech, 6, 3); break;
		case Terran_Science_Facility:	my_tile = wilplanner::choose_tech(plan_large, plan_large_tech, 6, 3); break;
		}
	}
	if (!use_buildplan) {
		switch (my_type) {
		case Terran_Supply_Depot:		my_tile = buildgenerator.generate_pair_tile(smallbs); break;
		case Terran_Academy:			my_tile = buildgenerator.generate_pair_tile(smallbs); break;
		case Terran_Armory:				my_tile = buildgenerator.generate_pair_tile(smallbs); break;
		//case Terran_Starport:			my_tile = sas_tile; break;
		//case Terran_Science_Facility:	my_tile = sas_tile; break;
		}
	}

	if (my_tile == BWAPI::TilePositions::None) {
		my_tile = buildgenerator.generate_random_tile(my_type, wilmap::my_main_tile, 1);
	}
	if (my_tile != BWAPI::TilePositions::None) {
		auto it = choose_scv(my_tile);
		if (it != wilunits::scvs.end()) {
			it->set_build_mission(my_type, my_tile);
			if (my_type == Terran_Missile_Turret) {
				building_queue = 8;
			} else {
				building_queue = 16;
			}
			//BWAPI::Broodwar->printf("building %s", my_type.c_str());
		}
	}
}

BWAPI::UnitType BuildManager::check_order_1() {
	int s = BWAPI::Broodwar->self()->supplyUsed() / 2;
	using namespace wilbuild;
	using namespace BWAPI::UnitTypes;

	if (s >=  1 && commandcenters.size() < 1) return Terran_Command_Center;
	if (willyt::eight_rax) {
		if (s >= 8 && barracks.size() < 1)	return Terran_Barracks;
		if (s >= 8 && depots.size() < 1)	return Terran_Supply_Depot;
		if (s >= 10 && need_one_bunker())	return Terran_Bunker;
	}
	if (s >=  9 && depots.size() < 1)		return Terran_Supply_Depot;
	if (s >= 10 && barracks.size() < 2)		return Terran_Barracks;
	if (s >= 12 && bunkers.size() < 1)		return Terran_Bunker;
	if (s >= 15 && depots.size() < 2)		return Terran_Supply_Depot;
	if (s >= 21 && academies.size() < 1)	return Terran_Academy;
	if (s >= 21 && refineries.size() < 1)	return Terran_Refinery;

	if (s >= 32 && factories.size() < 1)	return Terran_Factory;
	if (s >= 44 && engineerbays.size() < 1) return Terran_Engineering_Bay;
	if (s >= 60 && armories.size() < 1)		return Terran_Armory;
	return my_type;
}

BWAPI::UnitType BuildManager::check_order_2() {
	int s = BWAPI::Broodwar->self()->supplyUsed() / 2;
	using namespace wilbuild;
	using namespace BWAPI::UnitTypes;

	if (s >=  1 && commandcenters.size() < 1) return Terran_Command_Center;
	if (s >=  9 && depots.size() < 1)		return Terran_Supply_Depot;
	if (s >= 11 && barracks.size() < 1)		return Terran_Barracks;
	if (s >= 15 && commandcenters.size() < 2) return Terran_Command_Center;

	if (s >= 16 && depots.size() < 2)		return Terran_Supply_Depot;
	if (s >= 18 && need_one_bunker())		return Terran_Bunker;

	if (s >= 20 && barracks.size() < 2)		return Terran_Barracks;
	if (s >= 22 && refineries.size() < 1)	return Terran_Refinery;
	if (s >= 24 && academies.size() < 1)	return Terran_Academy;

	if (s >= 36 && factories.size() < 1)	return Terran_Factory;
	if (s >= 42 && engineerbays.size() < 1) return Terran_Engineering_Bay;
	if (s >= 60 && armories.size() < 1)		return Terran_Armory;
	return my_type;
}

BWAPI::UnitType BuildManager::check_order_3() {
	int s = BWAPI::Broodwar->self()->supplyUsed() / 2;
	using namespace wilbuild;
	using namespace BWAPI::UnitTypes;

	if (s >=  1 && commandcenters.size() < 1) return Terran_Command_Center;
	if (s >=  9 && depots.size() < 1)		return Terran_Supply_Depot;
	if (s >= 11 && barracks.size() < 1)		return Terran_Barracks;

	if (willyt::fast_expand) {
		if (s >= 15 && commandcenters.size() < 2) return Terran_Command_Center;

		if (wilenemy::race == BWAPI::Races::Zerg) {
			if (s >= 16 && need_one_bunker())		return Terran_Bunker;
			if (s >= 16 && depots.size() < 2)		return Terran_Supply_Depot;
			if (s >= 16 && refineries.size() < 1)	return Terran_Refinery;
		} else {
			if (s >= 16 && refineries.size() < 1)	return Terran_Refinery;
			if (s >= 16 && depots.size() < 2)		return Terran_Supply_Depot;
			if (s >= 18 && need_one_bunker())		return Terran_Bunker;
		}
		if (s >= 18 && factories.size() < 1)	return Terran_Factory;
		if (wilenemy::race != BWAPI::Races::Zerg &&
			!willyt::siege_started)				return my_type;

		if (s >= 27 && factories.size() < 2)	return Terran_Factory;
		if (s >= 35 && refineries.size() < 2)	return Terran_Refinery;
		if (s >= 40 && armories.size() < 1)		return Terran_Armory;
		if (s >= 45 && factories.size() < 3)	return Terran_Factory;
		if (s >= 50 && academies.size() < 1)	return Terran_Academy;
	} else {
		if (s >= 12 && refineries.size() < 1)	return Terran_Refinery;
		if (s >= 16 && depots.size() < 2)		return Terran_Supply_Depot;
		if (s >= 18 && need_one_bunker())		return Terran_Bunker;
		if (s >= 18 && factories.size() < 1)	return Terran_Factory;

		if (s >= 25 && factories.size() < 2)	return Terran_Factory;
		if (s >= 40 && armories.size() < 1)		return Terran_Armory;
		if (s >= 50 && academies.size() < 1)	return Terran_Academy;
	}

	return my_type;
}

BWAPI::UnitType BuildManager::check_order_4() {
	int s = BWAPI::Broodwar->self()->supplyUsed() / 2;
	using namespace wilbuild;
	using namespace BWAPI::UnitTypes;

	if (s >= 1 && commandcenters.size() < 1) return Terran_Command_Center;
	if (s >= 9 && depots.size() < 1)		return Terran_Supply_Depot;
	if (s >= 11 && barracks.size() < 1)		return Terran_Barracks;

	if (s >= 12 && refineries.size() < 1)	return Terran_Refinery;
	if (s >= 16 && depots.size() < 2)		return Terran_Supply_Depot;
	if (s >= 18 && factories.size() < 1)	return Terran_Factory;
	if (s >= 18 && need_one_bunker())		return Terran_Bunker;

	if (s >= 28 && engineerbays.size() < 1) return Terran_Engineering_Bay;
	if (s >= 28 && factories.size() < 2)	return Terran_Factory;
	if (s >= 40 && armories.size() < 1)		return Terran_Armory;
	if (s >= 40 && academies.size() < 1)	return Terran_Academy;
	if (s >= 40 && factories.size() < 3)	return Terran_Factory;
	if (s >= 60 && starports.size() < 1)	return Terran_Starport;

	if (refineries.size() > 2 && min > 600 && gas > 400) {
		if (s >= 120 && sciencefacs.size() < 1)	return Terran_Science_Facility;
		if (s >= 120 && factories.size() < 6)	return Terran_Factory;
	}
	return my_type;
}

BWAPI::UnitType BuildManager::check_order_6() {
	int s = BWAPI::Broodwar->self()->supplyUsed() / 2;
	using namespace wilbuild;
	using namespace BWAPI::UnitTypes;

	if (s >= 1 && commandcenters.size() < 1) return Terran_Command_Center;
	if (s >= 9 && depots.size() < 1)		return Terran_Supply_Depot;
	if (s >= 11 && barracks.size() < 1)		return Terran_Barracks;

	if (s >= 12 && refineries.size() < 1)	return Terran_Refinery;
	if (s >= 14 && bunkers.size() < 1)		return Terran_Bunker;
	if (s >= 16 && depots.size() < 2)		return Terran_Supply_Depot;

	if (s >= 18 && factories.size() < 1)	return Terran_Factory;
	if (s >= 22 && starports.size() < 2)	return Terran_Starport;

	if (s >= 40 && academies.size() < 1)	return Terran_Academy;
	if (s >= 40 && armories.size() < 1)		return Terran_Armory;
	if (willyt::has_natural && bunkers.size() < 2) return Terran_Bunker;

	if (BWAPI::Broodwar->self()->gas() > 400 &&
		s >= 60 && starports.size() < 6)	return Terran_Starport;
	if (s >= 60 && engineerbays.size() < 1) return Terran_Engineering_Bay;
	if (s >= 80 && sciencefacs.size() < 1)	return Terran_Science_Facility;

	//if (s >= 15 && commandcenters.size() < 2) return Terran_Command_Center;
	//if (s >= 18 && depots.size() < 2)		return Terran_Supply_Depot;
	//if (s >= 18 && bunkers.size() < 1)		return Terran_Bunker;
	//if (s >= 21 && refineries.size() < 1)	return Terran_Refinery;
	//if (s >= 24 && bunkers.size() < 2)		return Terran_Bunker;
	//if (s >= 27 && academies.size() < 1)	return Terran_Academy;

	//if (s >= 30 && factories.size() < 1)	return Terran_Factory;
	//if (s >= 33 && bunkers.size() < 3)		return Terran_Bunker;
	//if (s >= 39 && starports.size() < 1)	return Terran_Starport;
	//if (s >= 48 && sciencefacs.size() < 1)	return Terran_Science_Facility;
	//if (s >= 51 && engineerbays.size() < 1) return Terran_Engineering_Bay;

	//if (s >= 54 && starports.size() < 2)	return Terran_Starport;
	//if (s >= 60 && armories.size() < 1)		return Terran_Armory;
	//if (s >= 72 && starports.size() < 3)	return Terran_Starport;
	//if (BWAPI::Broodwar->self()->gas() > 600 &&
	//	s >= 90 && starports.size() < 6)	return Terran_Starport;
	return my_type;
}




BWAPI::UnitType BuildManager::check_depot() {
	int depotn = wilbuild::depots.size();
	int supply_full = 8 * depotn;
	for (BWAPI::Unit cc : wilbuild::commandcenters)
		if (cc->isCompleted() || cc->getRemainingBuildTime() < 960)
			supply_full += 10;
	int supply_pred = 2 + (int)(0.55 * BWAPI::Broodwar->self()->supplyUsed());
	if (depotn < 2 || depotn >= 18) return my_type;
	if (supply_pred >= supply_full)	return BWAPI::UnitTypes::Terran_Supply_Depot;
	return my_type;
}

BWAPI::TilePosition BuildManager::choose_commandcenter_tile() {
	BWAPI::TilePosition t0 = wilmap::main_tiles[wilmap::mm];
	BWAPI::TilePosition t1 = wilmap::natu_tiles[wilmap::mm];
	int n = wilbuild::commandcenters.size();
	if (n == 0) return BWAPI::TilePosition{ t0.x - 1 , t0.y - 1 };
	if (n == 1) return BWAPI::TilePosition{ t1.x - 1 , t1.y - 1 };
	else return BWAPI::TilePositions::None;
}

BWAPI::TilePosition BuildManager::choose_refinery_tile() {
	for (Resource r : wilbuild::geysers) {
		if (r.is_owned &&
			r.unit->exists() &&
			r.unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser &&
			r.unit->getResources() >= 400 &&
			r.tile.isValid()) {
			return r.tile;
		}
	}
	return BWAPI::TilePositions::None;
}

BWAPI::TilePosition BuildManager::choose_bunker_tile() {
	if (willyt::rush_alert || willyt::proxy_prod_alert || willyt::cannon_rush_alert)
		return wilmap::rush_def_tile[wilmap::mm];
	if (willyt::has_natural)
		return wilmap::natu_def_tile[wilmap::mm][0];
	return wilmap::main_def_tile[wilmap::mm][0];
}

BWAPI::UnitType BuildManager::check_turrets() {
	using namespace wilmap;
	if (!willyt::has_natural)
		define_turret_tiles(turret_tiles, main_def_tile[mm][1], willyt::need_turrets_2);
	if (willyt::has_natural)
		define_turret_tiles(turret_tiles, natu_def_tile[mm][1], willyt::need_turrets_2);
	if (wilunits::scvs.size() < 6)
		return my_type;

	if (willyt::need_detection &&
		wilbuild::academies.empty())
		return BWAPI::UnitTypes::Terran_Academy;
	if (willyt::need_turrets &&
		wilbuild::engineerbays.empty())
		return BWAPI::UnitTypes::Terran_Engineering_Bay;
	if (willyt::need_turrets &&
		wilbuild::turrets.size() < turret_tiles.size() &&
		soon_completed(wilbuild::engineerbays))
		return BWAPI::UnitTypes::Terran_Missile_Turret;
	return my_type;
}

BWAPI::TilePosition BuildManager::choose_turret_tile() {
	return get_max_turret_tile(turret_tiles);
}





BWAPI::UnitType BuildManager::check_advanced() {
	using namespace wilbuild;
	using namespace BWAPI::UnitTypes;
	if (willyt::strategy <= 2 && min >= 200) {
		if (wilenemy::race == BWAPI::Races::Zerg && barracks.size() < 4) {
			return Terran_Barracks;
		}
		if (wilenemy::race == BWAPI::Races::Protoss && barracks.size() < 3) {
			return Terran_Barracks;
		}
	}
	if (min > 300 && willyt::mineral_count >= 12 &&
		gas > 200 && willyt::geyser_count >= 2)
	{
		int bfsc = barracks.size() + factories.size() + starports.size();
		if (starports.size() < 1 && bfsc >= 5) {
			return Terran_Starport;
		}
		if (sciencefacs.size() < 1 && bfsc >= 7) {
			return Terran_Science_Facility;
		}
		if (bfsc < 9) {
			return Terran_Factory;
		}
	}
	return my_type;
}

bool BuildManager::need_one_bunker() {
	if (wilbuild::bunkers.size() < 1 &&
		BWAPI::Broodwar->getFrameCount() < 21600 &&
		soon_completed(wilbuild::barracks))
		return true;
	return false;
}

bool BuildManager::soon_completed(std::vector<BWAPI::Unit> &my_vec) {
	if (my_vec.empty() ||
		my_vec.front()->getRemainingBuildTime() > 320)
		return false;
	return true;
}

void BuildManager::build_consecutive(BWAPI::Unit my_last) {
	using namespace wilbuild;
	using namespace wilmap;
	using namespace BWAPI::UnitTypes;
	BWAPI::UnitType t = my_last->getType();
	my_tile = BWAPI::TilePositions::None;
	my_type = BWAPI::UnitTypes::None;
	if (t == Terran_Supply_Depot && factories.size() < 1 &&
		willyt::strategy == 3 && wilenemy::race != BWAPI::Races::Zerg)	{ my_type = Terran_Factory; }
	if (t == Terran_Barracks && commandcenters.size() < 2)	{ my_type = Terran_Command_Center; }
	if (t == Terran_Supply_Depot && barracks.size() < 1)	{ my_type = Terran_Barracks; }

	if (vector_holds(wilbuild::planned_builds, my_type)) { return; }
	switch (my_type)
	{
	case Terran_Barracks: my_tile = wilplanner::choose_tile_near(plan_large, 6, 3, BWAPI::TilePosition(my_main_def)); break;
	case Terran_Command_Center:	my_tile = choose_commandcenter_tile(); break;
	case Terran_Factory: my_tile = wilplanner::choose_tile_near(plan_large, 6, 3, BWAPI::TilePosition(my_main_def)); break;
	default: return;
	}
	if (my_tile != BWAPI::TilePositions::None) {
		auto it = choose_scv(my_tile);
		if (it != wilunits::scvs.end()) {
			it->set_build_mission(my_type, my_tile);
		}
		//BWAPI::Broodwar->printf("consecutive building %s", my_type.c_str());
	}
	return;
}

void BuildManager::check_proxy_building() {
	if (willyt::is_rushing && willyt::cannon_rush_alert && wilenemy::sem != -1)
	{
		std::vector<SCV>::iterator my_scv = get_scout();
		BWAPI::UnitType my_type = BWAPI::UnitTypes::Terran_Barracks;
		BWAPI::Unit my_rax = get_in_range(wilbuild::barracks, wilmap::en_start, 1024);

		if (my_rax == NULL &&
			my_scv != wilunits::scvs.end() &&
			BWAPI::Broodwar->self()->minerals() >= 150 &&
			vector_holds(wilbuild::planned_builds, my_type) == false)
		{
			BWAPI::TilePosition my_tile = buildgenerator.generate_proxy_tile(my_type, wilmap::en_start, wilenemy::positions, wilmap::main_choke_pos[wilenemy::sem], wilmap::maindistarray[wilenemy::sem]);
			if (my_tile != BWAPI::TilePositions::None) {
				my_scv->set_build_mission(my_type, my_tile);
			}
		}
		if (my_rax != NULL && my_scv != wilunits::scvs.end()) {
			my_scv->is_scout = false;
			my_scv->is_militia = true;
		}
	}
	if (willyt::is_rushing && wilenemy::goes_megamacro && wilenemy::sem != -1)
	{
		std::vector<SCV>::iterator my_scv = get_scout();
		BWAPI::UnitType my_type = BWAPI::UnitTypes::Terran_Bunker;
		BWAPI::Unit my_bunker = get_in_range(wilbuild::bunkers, wilmap::en_natu, 1024);

		if (my_bunker == NULL &&
			my_scv != wilunits::scvs.end() &&
			BWAPI::Broodwar->self()->minerals() >= 100 &&
			vector_holds(wilbuild::planned_builds, my_type) == false)
		{
			BWAPI::TilePosition my_tile = wilmap::natu_def_tile[wilenemy::sem][0];
			if (my_tile != BWAPI::TilePositions::None) {
				my_scv->set_build_mission(my_type, my_tile);
			}
		}
		if (my_bunker != NULL && my_scv != wilunits::scvs.end()) {
			my_scv->is_scout = false;
			my_scv->is_militia = true;
		}
	}
	return;
}



//std::vector<BWAPI::Unit> BuildManager::get_combined_vector() {
	//using namespace wilbuild;
	//std::vector<BWAPI::Unit> my_vector;
	//my_vector.insert(my_vector.end(), depots.begin(), depots.end());
	//my_vector.insert(my_vector.end(), academies.begin(), academies.end());
	//my_vector.insert(my_vector.end(), armories.begin(), armories.end());
	//return my_vector;
//}

//BWAPI::UnitType BuildManager::check_build() {
//	int supply_full = 20 * willyt::commandcenters.size() + 16 * willyt::depots.size();
//	int supply_pred = 5 * BWAPI::Broodwar->self()->supplyUsed() / 4;
//	int my_minerals = BWAPI::Broodwar->self()->minerals();
//
//	if (supply_full < supply_pred && supply_full < 400 &&
//		willyt::barracks.size() >= 1 && my_minerals >= 100)
//		return BWAPI::UnitTypes::Terran_Supply_Depot;
//	if (willyt::academies.size() < 1 &&
//		willyt::barracks.size() >= 2 && my_minerals >= 150)
//		return BWAPI::UnitTypes::Terran_Academy;
//	if (willyt::refineries.size() < 1 &&
//		willyt::academies.size() >= 1 && my_minerals >= 100)
//		return BWAPI::UnitTypes::Terran_Refinery;
//	if (willyt::barracks.size() < 4 &&
//		willyt::commandcenters.size() >= 1 && my_minerals >= 200)
//		return BWAPI::UnitTypes::Terran_Barracks;
//
//	return BWAPI::UnitTypes::None;
//}

//void BuildManager::update_defense(BWAPI::Unit my_scout) {
//	if (def_scouted || !my_scout) { return; }
//	if (my_scout->getDistance(willyt::start_pos) >= 256) {
//		def_tile = my_scout->getTilePosition();
//		def_scouted = true;
//		BWAPI::Broodwar->sendText("determine bunker placement");
//	}
//}

//if (s >= 22 && depots.size() < 3)		return Terran_Supply_Depot;
//if (s >= 28 && depots.size() < 4)		return Terran_Supply_Depot;
//if (s >= 24 && depots.size() < 3)		return Terran_Supply_Depot;
//if (s >= 30 && depots.size() < 4)		return Terran_Supply_Depot;

	//sas_tile = determine_sas_tile();
	//wilmap::sas_tile = sas_tile;
	//set_map_area(wilmap::build_map, sas_tile.x, sas_tile.y + 0, 4, 1, false);
	//set_map_area(wilmap::build_map, sas_tile.x, sas_tile.y + 1, 6, 2, false);
	//BWAPI::Broodwar->printf("initialize defense placement");

	//bun_tile = wilmap::main_def_tile[wilmap::mm][0];
	//tur_tile = wilmap::main_def_tile[wilmap::mm][1];
	//bun_tile = wilmap::natu_def_tile[wilmap::mm][0];
	//tur_tile = wilmap::natu_def_tile[wilmap::mm][1];