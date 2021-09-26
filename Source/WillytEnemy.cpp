#include "WillytEnemy.h"

namespace wilenemy
{
	BWAPI::Race race = BWAPI::Races::Unknown;
	std::string name = "enemies";
	std::string score = "0-0";
	int result[8] = { 0 };
	std::vector<std::string> history;

	std::vector<BWAPI::Position> positions;
	std::vector<BWAPI::Position> main_pos;
	int sem = -1;
	std::vector<BWAPI::TilePosition> incomplete_def;

	int supply_work = 0;
	int supply_army = 0;
	int supply_air = 0;
	int supply_small = 0;
	int supply_large = 0;
	int supply_cloak = 0;
	int air_percentage = 0;
	int small_percentage = 0;
	int cloak_percentage = 0;

	std::vector<BWAPI::Unit> intruders;
	int intruder_totstr = 0;
	int intruder_airstr = 0;
	int intruder_lurkstr = 0;
	std::vector<BWAPI::Unit> near_bunkers;
	int supply_near_bunkers = 0;

	int grddef_count = 0;
	int airdef_count = 0;
	int produc_count = 0;
	int airprod_count = 0;
	int mainbuild_count = 0;

	bool has_covertops = false;
	bool has_physiclab = false;
	bool has_nukesilo = false;
	bool has_pool = false;

	bool has_hydraden = false;
	bool has_lair = false;
	bool has_hive = false;
	bool has_defmound = false;
	bool has_ultracav = false;
	bool has_lurkers = false;

	bool has_forge = false;
	bool has_robo = false;
	bool has_archives = false;
	bool has_tribunal = false;
	bool has_fleetbea = false;
	unsigned time_lair_reaction = 7200;

	bool goes_megamacro = false;

	std::vector<BWAPI::Unit> tlg;
	std::vector<BWAPI::Unit> tlgg;
	std::vector<BWAPI::Unit> tlga;
	std::vector<BWAPI::Unit> tlgs;
	std::vector<BWAPI::Unit> tlgc;
	std::vector<BWAPI::Unit> tlgw;
	std::vector<BWAPI::Unit> tlgm;
	std::vector<BWAPI::Unit> tlgl;
	std::vector<BWAPI::Unit> tla;
	std::vector<BWAPI::Unit> tlag;
	std::vector<BWAPI::Unit> tlaa;
	std::vector<BWAPI::Unit> tlas;
	std::vector<BWAPI::Unit> tlac;

	std::vector<BWAPI::Unit> tleggla;
	std::vector<BWAPI::Unit> tlflare;
	std::vector<BWAPI::Unit> tllockd;
	std::vector<BWAPI::Unit> tlyama;
	std::vector<BWAPI::Unit> tldefg;
	std::vector<BWAPI::Unit> tldefa;
	std::vector<BWAPI::Unit> tlself;

	int target_count = 0;

	std::vector<BWAPI::Position> unclaimed_expo_pos;
}

EnemyManager::EnemyManager() {
	if (BWAPI::Broodwar->enemies().size() == 1) {
		BWAPI::Race r = BWAPI::Broodwar->enemy()->getRace();
		if (r == BWAPI::Races::Terran ||
			r == BWAPI::Races::Protoss ||
			r == BWAPI::Races::Zerg)
			wilenemy::race = r;
		else
			need_race = true;
		wilenemy::name = BWAPI::Broodwar->enemy()->getName();
	}
	else {
		wilenemy::name = "%d enemies", BWAPI::Broodwar->enemies().size();
	}
}

void EnemyManager::append_unit(BWAPI::Unit unit) {
	using namespace wilenemy;

	if (!unit->exists()) { return; }
	if (need_race) { determine_race(unit); }
	if (std::find(ids.begin(), ids.end(), unit->getID()) != ids.end()) { return; }

	ids.push_back(unit->getID());
	BWAPI::UnitType t = unit->getType();
	BWAPI::Position p = unit->getPosition();
	if (t.supplyRequired() > 0) {
		change_supply(t, t.supplyRequired());
		check_tech_unit(t);
	}
	if (t.isBuilding()) {
		if (!vector_holds(positions, p)) { positions.push_back(p); }
		if (t.isResourceDepot()) { determine_main(unit); }
		if (!unit->isLifted()) { set_map_area(wilmap::build_map_var, unit, false); }
		check_tech(t, +1);
		check_defense(unit, +1);
		check_time_lair(unit, false);
	}
	return;
}

void EnemyManager::remove_unit(BWAPI::Unit unit) {
	using namespace wilenemy;

	ids.erase(std::remove(ids.begin(), ids.end(), unit->getID()), ids.end());
	BWAPI::UnitType t = unit->getType();
	if (t.supplyRequired() > 0) { change_supply(t, -1 * t.supplyRequired()); }

	if (t.isBuilding()) {
		vector_remove(positions, unit->getPosition());
		if (!unit->isLifted()) { clear_build_area(wilmap::build_map_var, wilmap::build_map_fix, unit); }
		check_tech(t, -1);
		check_defense(unit, -1);
	}
	return;
}

void EnemyManager::change_unit(BWAPI::Unit unit) {
	using namespace BWAPI::UnitTypes;
	using namespace wilenemy;
	if (!unit->exists()) return;
	BWAPI::UnitType t = unit->getType();
	BWAPI::Position p = unit->getPosition();

	if (t.isBuilding()) {
		if (t == Resource_Vespene_Geyser) {
			vector_remove(positions, p);
			return;
		}
		if (t == Terran_Refinery ||
			t == Protoss_Assimilator) {
			positions.push_back(p);
			return;
		}
		if (t == Zerg_Sunken_Colony ||
			t == Zerg_Spore_Colony) {
			check_defense(unit, +1);
			if (!vector_holds(positions, p)) {
				positions.push_back(p);
				set_map_area(wilmap::build_map_var, unit, false);
				//BWAPI::Broodwar->printf("added position of morphed %s", t.c_str());
			}
			return;
		}
		if (t == Zerg_Lair ||
			t == Zerg_Hive ||
			t == Zerg_Greater_Spire) {
			check_tech(t, +1);
			check_time_lair(unit, true);
			if (!vector_holds(positions, p)) {
				positions.push_back(p);
				set_map_area(wilmap::build_map_var, unit, false);
				//BWAPI::Broodwar->printf("added position of morphed %s", t.c_str());
			}
			return;
		}
		if (true) { //other zerg buildings
			check_tech(t, +1);
			positions.push_back(p);
			set_map_area(wilmap::build_map_var, unit, false);
			safe_sum(supply_work, -2);
			return;
		}
	}
	if (t == Terran_Siege_Tank_Siege_Mode ||
		t == Terran_Siege_Tank_Tank_Mode ||
		t == Zerg_Guardian ||
		t == Zerg_Devourer ||
		t == Zerg_Lurker_Egg) {
		return;
	}
	if (t == Zerg_Lurker) {
		supply_army += 2;
		supply_large += 2;
		supply_cloak += 4;
		return;
	}
	if (t.supplyRequired() > 0) {
		change_supply(t, t.supplyRequired()); //larva to unit
	}
}

void EnemyManager::update() {
	using namespace wilenemy;
	if (supply_army > 0) {
		air_percentage   = 100 * supply_air / supply_army;
		cloak_percentage = 100 * supply_cloak / supply_army;
		small_percentage = 100 * supply_small / supply_army;
	} else {
		air_percentage   = 0;
		cloak_percentage = 0;
		small_percentage = 0;
	}

	collect_intruders(wilenemy::intruders, wilmap::defense_map);
	intruder_totstr = evaluate_strength(intruders);
	intruder_airstr = evaluate_airstrength(intruders);

	collect_intruders(wilenemy::near_bunkers, wilmap::bunker_map);
	supply_near_bunkers = count_supply(near_bunkers);
	check_incomplete_defense();
	return;
}

void EnemyManager::determine_race(BWAPI::Unit unit) {
	BWAPI::Race r = unit->getType().getRace();
	if (r == BWAPI::Races::Terran ||
		r == BWAPI::Races::Protoss ||
		r == BWAPI::Races::Zerg) {
		wilenemy::race = r;
		need_race = false;
		//BWAPI::Broodwar->sendText("so you are %s", r.c_str());
	}
	return;
}

void EnemyManager::correct() {
	for (std::size_t i = 0; i < wilenemy::positions.size(); ++i) {
		BWAPI::TilePosition my_tile = BWAPI::TilePosition(wilenemy::positions[i]);
		if (BWAPI::Broodwar->isVisible(my_tile)) {
			bool found = false;
			for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsOnTile(my_tile)) {
				if (u->getType().isBuilding() &&
					u->getPlayer()->isEnemy(BWAPI::Broodwar->self())) {
					found = true;
				}
			}
			if (!found) {
				clear_deducted_build_tiles(wilenemy::positions.at(i));
				wilenemy::positions.erase(wilenemy::positions.begin() + i);
				//BWAPI::Broodwar->printf("removing vacant enemy position");
			}
		}
	}
	return;
}

void EnemyManager::determine_main(BWAPI::Unit unit) {
	using namespace wilenemy;
	//if (!main_pos.empty() &&
		//std::find(main_pos.begin(), main_pos.end(), unit->getPosition()) != main_pos.end()) {
		//BWAPI::Broodwar->printf("enemy main already listed");
		//return;
	//}
	for (int i = 0; i < wilmap::mn; i++) {
		if (sqdist(unit->getPosition(), wilmap::main_pos[i]) <= 256) {
			if (BWAPI::Broodwar->enemies().size() == 1 && sem == -1) {
				sem = i;
				wilmap::unscouted.clear();
				wilmap::en_start = wilmap::main_tiles[i];
				wilmap::en_main = wilmap::main_pos[i];
				wilmap::en_natu = wilmap::natu_pos[i];
				wilmap::entrance_circle_increment = get_direction_8(wilmap::main_pos[i], wilmap::main_choke_pos[i]) * 3;
				//BWAPI::Broodwar->printf("found 1v1 enemy main");
			}
			if (BWAPI::Broodwar->enemies().size() >= 2 || main_pos.empty()) {
				main_pos.push_back(unit->getPosition());
				//BWAPI::Broodwar->printf("found any enemy main");
			}
			return;
		}
	}
	return;
}

void EnemyManager::change_supply(BWAPI::UnitType t, int ds) {
	using namespace wilenemy;
	if (t.isWorker()) {
		safe_sum(supply_work, ds);
	} else {
		safe_sum(supply_army, ds);
		if (t.isFlyer()) safe_sum(supply_air, ds);
		else if (is_small(t)) safe_sum(supply_small, ds);
		else safe_sum(supply_large, ds);
		if (can_cloak(t)) safe_sum(supply_cloak, ds);
	}
	return;
}

bool EnemyManager::is_small(BWAPI::UnitType t) {
	if (t.size() == BWAPI::UnitSizeTypes::Small) { return true; }
	return false;
}

bool EnemyManager::can_cloak(BWAPI::UnitType t) {
	if (t.isBuilding())
		return false;
	if (t == BWAPI::UnitTypes::Protoss_Dark_Templar ||
		t == BWAPI::UnitTypes::Protoss_Arbiter ||
		t == BWAPI::UnitTypes::Terran_Ghost ||
		t == BWAPI::UnitTypes::Terran_Wraith ||
		t == BWAPI::UnitTypes::Zerg_Lurker)
		return true;
	return false;
}

void EnemyManager::clear_deducted_build_tiles(BWAPI::Position p) {
	if (p == BWAPI::Positions::None) {
		return;
	}
	if (p.x % 32 == 0 && p.y % 32 == 16) {
		clear_build_area(wilmap::build_map_var, wilmap::build_map_fix, p.x/32 - 2, p.y/32 - 1, 4, 3);
		return;
	}
	if (p.x % 32 == 16 && p.y % 32 == 0) {
		clear_build_area(wilmap::build_map_var, wilmap::build_map_fix, p.x/32 - 1, p.y/32 - 1, 3, 2);
		return;
	}
	if (p.x % 32 == 0 && p.y % 32 == 0) {
		clear_build_area(wilmap::build_map_var, wilmap::build_map_fix, p.x/32 - 1, p.y/32 - 1, 2, 2);
		return;
	}
	return;
}



void EnemyManager::check_defense(BWAPI::Unit unit, int dz) {
	using namespace BWAPI::UnitTypes;
	switch (unit->getType())
	{
	case Terran_Bunker:			handle_defense(unit, dz, 60, true, true, false); break;
	case Terran_Missile_Turret: handle_defense(unit, dz, 68, false, true, true); break;
	case Zerg_Sunken_Colony:	handle_defense(unit, dz, 68, true, false, false); break;
	case Zerg_Spore_Colony:		handle_defense(unit, dz, 68, false, true, true); break;
	case Protoss_Photon_Cannon:	handle_defense(unit, dz, 68, true, true, true); break;
	}
	return;
}

void EnemyManager::handle_defense(BWAPI::Unit u, int dz, int size, bool is_grd, bool is_air, bool is_det) {
	BWAPI::Position p = u->getPosition();
	BWAPI::TilePosition t = u->getTilePosition();

	if (u->isCompleted()) {
		if (is_grd && p.isValid()) {
			set_influence(wilthr::grddef, p.x, p.y, size, dz);
			safe_sum(wilenemy::grddef_count, dz);
		}
		if (is_air && p.isValid()) {
			set_influence(wilthr::airdef, p.x, p.y, size, dz);
			safe_sum(wilenemy::airdef_count, dz);
		}
		if (is_det && p.isValid()) {
			set_influence(wilthr::stadet, p.x, p.y, size, dz);
		}
	} else {
		if (dz > 0 && t.isValid()) {
			wilenemy::incomplete_def.push_back(t);
		}
	}
	return;
}

void EnemyManager::check_incomplete_defense() {
	for (BWAPI::TilePosition t : wilenemy::incomplete_def) {
		if (BWAPI::Broodwar->isVisible(t)) {
			BWAPI::Unitset my_set = BWAPI::Broodwar->getUnitsOnTile(t);
			for (BWAPI::Unit u : my_set) {
				if (u->isCompleted() &&
					u->getType().isBuilding() &&
					u->isFlying() == false)
				{
					check_defense(u, +1);
					vector_remove(wilenemy::incomplete_def, t);
					//BWAPI::Broodwar->printf("enemy defense now completed");
					return;
				}
			}
			if (my_set.empty()) {
				vector_remove(wilenemy::incomplete_def, t);
				//BWAPI::Broodwar->printf("enemy defense mysteriously dissappeared");
				return;
			}
		}
	}
	return;
}

void EnemyManager::check_tech(BWAPI::UnitType type, int dz) {
	using namespace wilenemy;
	using namespace BWAPI::UnitTypes;
	if (type.canProduce()) {
		safe_sum(produc_count, dz);
	}
	if (type.isResourceDepot()) {
		safe_sum(mainbuild_count, dz);
	}
	switch (type)
	{
	case(Zerg_Spire):			safe_sum(airprod_count, dz); break;
	case(Zerg_Greater_Spire):	safe_sum(airprod_count, dz); break;
	case(Terran_Starport):		safe_sum(airprod_count, dz); break;
	case(Protoss_Stargate):		safe_sum(airprod_count, dz); break;
	}
	if (dz < 0) { return; }

	switch (type)
	{
	case(Terran_Covert_Ops):			has_covertops = true; break;
	case(Terran_Physics_Lab):			has_physiclab = true; break;
	case(Terran_Nuclear_Silo):			has_nukesilo = true; has_covertops = true; break;

	case(Zerg_Spawning_Pool):		has_pool = true; break;
	case(Zerg_Hydralisk_Den):		has_hydraden = true; break;
	case(Zerg_Lair):				has_lair = true; break;
	case(Zerg_Hive):				has_lair = true; has_hive = true; break;
	case(Zerg_Defiler_Mound):		has_defmound = true; has_hive = true; break;
	case(Zerg_Ultralisk_Cavern):	has_ultracav = true; has_hive = true; break;

	case(Protoss_Forge):				has_forge = true; break;
	case(Protoss_Photon_Cannon):		has_forge = true; break;
	case(Protoss_Robotics_Facility):	has_robo = true; break;
	case(Protoss_Robotics_Support_Bay):	has_robo = true; break;
	case(Protoss_Observatory):			has_robo = true; break;
	case(Protoss_Templar_Archives):		has_archives = true; break;
	case(Protoss_Arbiter_Tribunal):		has_tribunal = true; has_archives = true; break;
	case(Protoss_Fleet_Beacon):			has_fleetbea = true; break;
	}
	return;
}

void EnemyManager::check_tech_unit(BWAPI::UnitType type) {
	using namespace wilenemy;
	using namespace BWAPI::UnitTypes;

	if (race == BWAPI::Races::Zerg) {
		if (has_lurkers == false &&
			(type == Zerg_Lurker ||
			type == Zerg_Lurker_Egg)) {
			has_lurkers = true;
			has_hydraden = true;
			has_lair = true;
		}
	}
	return;
}

void EnemyManager::check_time_lair(BWAPI::Unit u, bool just_changed) {
	if (u->getType() == BWAPI::UnitTypes::Zerg_Lair)
	{
		unsigned t = BWAPI::Broodwar->getFrameCount() * 42 / 1000;
		if (just_changed && !u->isCompleted()) { t += 63 + 40; }
		if (just_changed && u->isCompleted()) { t += 40; }
		if (!just_changed && !u->isCompleted()) { t += 30 + 40; }
		if (just_changed && !u->isCompleted()) { t += 0; }
		wilenemy::time_lair_reaction = t;
	}
	//t_spire + t_muta = 100 s
	//t_aspect + t_lurker = 100 s
	//t_engybay + t_turret = 57 s
	//traveltimes might be similar
	//reaction should be at around: t_lair + 40 s
	return;
}

int get_intruder_count(BWAPI::UnitType my_type) {
	int i = 0;
	for (BWAPI::Unit u : wilenemy::intruders) {
		if (u->getType() == my_type) { i++; }
	}
	return i;
}