#include "WillytState.h"

namespace willyt
{
	int strategy = 1;
	int test_strategy = 0;
	int orig_strategy = 1;
	bool show_info = true;
	bool is_vs_human = false;
	bool avoid_weak_strategies = false;
	bool use_hardcoded_strategies = false;
	bool ums_to_pratice_micro = false;

	unsigned my_time = 0;

	int sup_scv = 0;
	int sup_mil = 0;
	int sup_bio = 0;
	int sup_mech = 0;
	int sup_air = 0;

	int attack_supply = 15;
	int mining_supply = 20;
	int attack_supply_modifier = 0;

	int mineral_count = 8;
	int geyser_count = 1;
	int min_scv_count_min = 0;
	int min_scv_count_gas = 0;

	bool is_swarming = false;
	bool do_bio_scv_rush = false;
	bool is_choke_def = false;
	bool is_rushing = false;

	bool go_bio = true;
	int max_bio_upgrade = 3;
	bool fast_expand = true;
	bool siege_started = false;

	bool has_natural = false;
	bool has_transport = false;
	bool go_islands = false;
	bool go_raiding = false;
	bool blocked_mainchoke = false;
	unsigned available_scans = 0;

	bool rush_alert = false;
	bool eight_rax = false;
	bool need_turrets = false;
	bool need_turrets_2 = false;
	bool need_detection = false;
	bool stim_allowed = false;
	bool avoid_grddef = false;
	bool proxy_alert = false;
	bool proxy_prod_alert = false;
	bool cannon_rush_alert = false;
	bool carrier_rush_alert = false;
	bool lurker_rush_alert = false;
	bool hold_bunker = false;
	//bool guard_siege = false;
	bool flyer_attack_airdef = false;
	bool need_scout_natu = false;

	BWAPI::Position retreat_pos = BWAPI::Positions::None;
	BWAPI::Position gather_pos = BWAPI::Positions::None;
	BWAPI::Position def_target_pos = BWAPI::Positions::None;
	BWAPI::Position att_target_pos = BWAPI::Positions::None;
	BWAPI::Position leader_pos = BWAPI::Positions::None;
	//BWAPI::Position siege_pos = BWAPI::Positions::None;
	BWAPI::Position enemy_natural_pos = BWAPI::Positions::None;
	int leader_dist = 0;

	std::vector<BWAPI::Unit> target_vector;

	int count_trapped = 0;
	unsigned highground_defense_tanks = 0;
	unsigned planned_highground_defense = 0;
}


void StateManager::init()
{
	using namespace willyt;

	//strategy dependent adaptations
	switch (strategy) {
	case 1:
		do_bio_scv_rush = true;
		go_bio = true;
		fast_expand = false;
		attack_supply_modifier = 0;
		break;
	case 2:
		go_bio = true;
		fast_expand = true;
		attack_supply_modifier = 6;
		break;
	case 3:
		go_bio = false;
		fast_expand = true;
		attack_supply_modifier = 12;
		break;
	case 4:
		go_bio = false;
		fast_expand = false;
		attack_supply_modifier = 12;
		break;
	case 6:
		go_bio = true;
		fast_expand = false;
		attack_supply_modifier = 6;
		break;
	}
	return;
}


void StateManager::update(int n_ds)
{
	using namespace willyt;

	//mining supply treshold
	mining_supply = 3 + 2 * mineral_count + 3 * geyser_count;
	if (mining_supply > 60) { mining_supply = 60; }

	//attack supply treshold
	attack_supply = (my_time / 20) + (15 * BWAPI::Broodwar->enemies().size()) - 15;
	if (attack_supply < 15) { attack_supply = 15; }
	if (attack_supply > 60) { attack_supply = 60; }
	attack_supply += attack_supply_modifier;

	if (strategy == 4) {
		attack_supply = 90;
		if (mining_supply > 40) { mining_supply = 40; }
		if (mining_supply > 30 && my_time < 900) { mining_supply = 30; }
	}

	//other stuff
	if (carrier_rush_alert) { attack_supply = 12; }
	if (is_rushing && my_time > 600) { is_rushing = false; }
	target_vector.clear();
	has_transport = (n_ds > 0) ? true : false;
	return;
}