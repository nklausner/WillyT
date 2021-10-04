#include "ArmyManager.h"

void ArmyManager::update() {

	if (BWAPI::Broodwar->getGameType() == BWAPI::GameTypes::Use_Map_Settings && willyt::ums_to_pratice_micro) {
		attack_to_pratice_micro();
		return;
	}

	int n = willyt::sup_bio + willyt::sup_mech;
	int d = willyt::sup_bio + willyt::sup_mech + willyt::sup_mil + 2 * wilbuild::bunkers.size();
	bool should_attack = decide_attack(n);
	bool under_attack = check_base_under_attack();

	//update_siegetank_influence();
	//willyt::siege_pos = calc_tank_center();
	//willyt::guard_siege = check_siege_guarding();
	willyt::stim_allowed = check_stim_medics();

	willyt::gather_pos = get_gathering_position();
	willyt::def_target_pos = get_defending_target_position(wilenemy::intruders);
	willyt::att_target_pos = get_attacking_target_position(should_attack);
	willyt::leader_pos = calc_leading_pos(willyt::att_target_pos);
	willyt::def_target_pos = hold_back_defense_at_natural_choke(willyt::def_target_pos);

	willyt::is_swarming = check_swarming(should_attack);
	willyt::is_choke_def = check_main_or_natu_choke_defense();
	wilmap::my_bunker_def_pos = check_bunker_def_pos(should_attack);
	check_militia(under_attack, d);
	check_cohesion(n);
	check_flyers(wilunits::wraiths, 4);
	check_flyers(wilunits::cruisers, 3);

	if (willyt::def_target_pos == BWAPI::Positions::None &&
		willyt::att_target_pos == BWAPI::Positions::None &&
		willyt::is_swarming == false) {
		army_attack(willyt::gather_pos, false);
	}
	if (willyt::def_target_pos != BWAPI::Positions::None &&
		willyt::att_target_pos == BWAPI::Positions::None) {
		army_split(willyt::def_target_pos, willyt::gather_pos, wilenemy::intruder_totstr, wilenemy::intruder_airstr, false);
	}
	if (willyt::def_target_pos == BWAPI::Positions::None &&
		willyt::att_target_pos != BWAPI::Positions::None) {
		army_attack(willyt::att_target_pos, true);
	}
	if (willyt::def_target_pos != BWAPI::Positions::None &&
		willyt::att_target_pos != BWAPI::Positions::None) {
		army_split(willyt::def_target_pos, willyt::att_target_pos, wilenemy::intruder_totstr, wilenemy::intruder_airstr, true);
	}
	return;
}





bool ArmyManager::decide_attack(int n) {
	if ((willyt::att_target_pos != BWAPI::Positions::None && n >= willyt::attack_supply - 6) ||
		(willyt::att_target_pos == BWAPI::Positions::None && n >= willyt::attack_supply))
		return true;
	return false;
}

BWAPI::Position ArmyManager::get_gathering_position() {
	if (willyt::is_rushing) {
		if (wilenemy::goes_megamacro) {
			return wilmap::natu_def_pos[wilenemy::sem];
		} else {
			return wilmap::main_pos[wilenemy::sem];
		}
	}
	if (wilbuild::bunkers.size() > 0 &&
		!willyt::cannon_rush_alert &&
		(willyt::rush_alert || willyt::proxy_alert)) {
		return wilbuild::bunkers.front()->getPosition();
	}
	if (wilbuild::commandcenters.size() >= 2) {
		return wilmap::my_natu_def;
	}
	return wilmap::my_main_def;
}

BWAPI::Position ArmyManager::get_defending_target_position(std::vector<BWAPI::Unit>& my_vec) {
	if (hold_back_siegetank()) {
		return wilmap::my_main_def;
	}
	BWAPI::Unit my_enemy = choose_enemy_from(my_vec);
	if (my_enemy != NULL && my_enemy->getPosition().isValid())
	{
		BWAPI::Position p = estimate_next_pos(my_enemy, 24);
		if (!willyt::is_rushing) { return p; }
		if (can_attack_safe_r4(p)) { return p; }
		if (!wilunits::siegetanks.empty()) { return p; }
	}
	return BWAPI::Positions::None;
}

BWAPI::Position ArmyManager::get_attacking_target_position(bool my_cond) {
	if (my_cond && !wilenemy::positions.empty())
	{
		if (willyt::strategy == 4 && wilenemy::sem >= 0) {
			return choose_enemy_building_near(wilenemy::positions, wilmap::en_main);
		} else {
			return choose_enemy_building_away(wilenemy::positions, wilenemy::main_pos);
		}
	}
	if (my_cond) {
		return choose_enemy_unit();
	}
	//if (willyt::strategy == 4 && willyt::my_time > 600 && wilbuild::commandcenters.size() < 2) {
	//	return wilmap::my_natu_def;
	//}
	return BWAPI::Positions::None;
}

bool ArmyManager::check_swarming(bool my_cond) {
	if (my_cond && willyt::att_target_pos == BWAPI::Positions::None) {
		army_swarm(!willyt::is_swarming);
		return true;
	}
	return false;
}

void ArmyManager::check_militia(bool is_under_attack, int def_strength) {

	if (willyt::rush_alert) {
		if (willyt::sup_mil < 2 &&
			willyt::my_time >= 120) {
			call_militia(2);
		}
	}
	if (willyt::rush_alert ||
		willyt::is_rushing) {
		if (!is_under_attack &&
			willyt::sup_mil > 2) {
			stop_militia();
		}
	}
	int relevant_strength = wilenemy::intruder_totstr - wilenemy::intruder_airstr - 6 * get_intruder_count(BWAPI::UnitTypes::Zerg_Lurker);

	if (!willyt::hold_bunker &&
		is_under_attack &&
		def_strength < 12 &&
		def_strength < relevant_strength) {
		call_militia(relevant_strength - def_strength);
	}
	if (willyt::strategy == 1 &&
		!willyt::first_attack &&
		!wilenemy::goes_megamacro &&
		willyt::att_target_pos != BWAPI::Positions::None) {
		willyt::first_attack = true;
		call_militia(7);
		create_repulsive_field_at_enemy_natural();
		//BWAPI::Broodwar->sendText("It's go time!");
	}
	if (willyt::carrier_rush_alert &&
		!willyt::first_attack) {
		willyt::first_attack = true;
		call_militia(6);
	}
	if (willyt::sup_mil > 0 &&
		!willyt::rush_alert &&
		!willyt::is_rushing &&
		!is_under_attack &&
		(willyt::my_time > 600 ||
		willyt::avoid_grddef ||
		willyt::att_target_pos == BWAPI::Positions::None)) {
		stop_militia();
		//BWAPI::Broodwar->sendText("stop militia");
	}
}

//void ArmyManager::check_defend(int d, int a, BWAPI::Unit enemy_attacker) {
//	using namespace willyt;
//
//	//defend if units in base
//	if (enemy_attacker && enemy_attacker->exists()) {
//		def_target_pos = estimate_next_pos(enemy_attacker, 24);
//	} else {
//		def_target_pos = BWAPI::Positions::None;
//	}
//
//	//high alert when attack on base
//	//call militia for help if needed
//	//under_attack = base_under_attack();
//	if (under_attack && d < 12 && d < a) {
//		call_militia(a - d);
//	}
//}

//void ArmyManager::check_attack(int n) {
//	using namespace willyt;
//
//	//determine attack state based on army supply
//	//choose attack target
//	//buildings, units, swarming
//	if (n >= attack_supply || 
//		(n + 6 >= attack_supply &&
//		att_target_pos != BWAPI::Positions::None)) {
//		if (true) {
//			att_target_pos = choose_enemy_building(wilenemy::positions, wilenemy::main_pos);
//		}
//		if (att_target_pos == BWAPI::Positions::None) {
//			att_target_pos = choose_enemy_unit();
//		}
//		if (att_target_pos == BWAPI::Positions::None) {
//			army_swarm(!is_swarming);
//			is_swarming = true;
//		} else {
//			is_swarming = false;
//		}
//		army_queue = 32;
//	} else {
//		att_target_pos = BWAPI::Positions::None;
//	}
//}

void ArmyManager::check_cohesion(int &n) {
	using namespace willyt;

	if (!is_none(att_target_pos) &&
		is_none(def_target_pos) &&
		!is_swarming)
	{
		if (!is_none(leader_pos) &&
			get_ground_dist(leader_pos) > 0)
		{
			if (willyt::strategy == 3) {
				leader_dist = get_ground_dist(leader_pos) + 6;
			}
			if (willyt::strategy == 4) {
				leader_dist = get_ground_dist(leader_pos);
			}
		}
	} else {
		leader_pos = BWAPI::Positions::None;
		leader_dist = 0;
	}
	if (!is_none(leader_pos)) {
		int a = count_around(leader_pos, 320);
		if (a < 20 && 2 * a < n) {
			att_target_pos = leader_pos;
			//army_queue = 8;
			//Broodwar->printf("Get together!");
		}
	}
	clear_repulsive_field_at_enemy_natural();
	return;
}

void ArmyManager::check_flyers(std::vector<Flyer>& my_vec, int gs) {
	int dc = ((rand() % 2) * 2) - 1;
	int i = 0;
	for (auto &c : my_vec) {
		if (c.get_circle_diff() == 0) { ++i; }
	}
	if (i >= gs) {
		for (auto &c : my_vec) {
			if (c.get_circle_diff() == 0) {
				c.set_circle_diff(dc);
			}
		}
		BWAPI::Broodwar->printf("new flyer attack group %d going %d", gs, dc);
	}
	return;
}

void ArmyManager::attack_to_pratice_micro()
{
	BWAPI::TilePosition t = BWAPI::Broodwar->getStartLocations()[1];
	willyt::att_target_pos = BWAPI::Position(32*t.x + 64, 32*t.y + 48);
	willyt::gather_pos = get_gathering_position();
	willyt::stim_allowed = check_stim_medics();
	army_attack(willyt::att_target_pos, true);
	//BWAPI::Broodwar->printf("UMS attack");
	return;
}



//void ArmyManager::update_positions() {
//	int x = 0;
//	int y = 0;
//	int i = 0;
//	for (BWAPI::Unit &unit : willyt::commandcenters) {
//		x += unit->getPosition().x;
//		y += unit->getPosition().y;
//		++i;
//	}
//	for (BWAPI::Unit &unit : willyt::bunkers) {
//		x += unit->getPosition().x;
//		y += unit->getPosition().y;
//		++i;
//	}
//	if (i > 0) { gather_pos = BWAPI::Position{ x / i, y / i }; }
//	else { gather_pos = willyt::start_pos; }
//	//BWAPI::Broodwar->printf("Updating defense position.");
//}