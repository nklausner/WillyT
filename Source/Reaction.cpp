#include "Reaction.h"

void check_all_reactions() {
	check_need_turrets();
	willyt::rush_alert = check_zergling_rush();
	willyt::avoid_grddef = check_defense_focus();
	willyt::proxy_alert = check_proxy_buildings();
	willyt::proxy_prod_alert = check_proxy_prod_buildings();
	willyt::hold_bunker = check_pressure_on_bunker();
	check_mega_macro();
	check_carrier_rush();
	change_strategies();
	check_resign();
	return;
}

void check_need_turrets() {
	int sc = wilenemy::supply_cloak;
	int sa = wilenemy::supply_air;
	willyt::need_turrets_2 = false;
	if (sc + sa > 0) willyt::need_turrets = true;
	if (sc + sa > 18) willyt::need_turrets_2 = true;
	if (sc > 0) willyt::need_detection = true;
	if (wilenemy::airprod_count > 0 ||
		wilenemy::has_archives ||
		wilenemy::has_robo ||
		wilenemy::has_lurkers ||
		willyt::my_time > wilenemy::time_lair_reaction)
		willyt::need_turrets = true;
	if (!willyt::need_detection &&
		willyt::my_time > 330 &&
		wilenemy::race == BWAPI::Races::Protoss &&
		(wilenemy::sem == -1 ||											//scout denied, e.g. BananaBrain
		(willyt::cannon_rush_alert && wilenemy::grddef_count <= 4)))	//anti krasi0P
		willyt::need_detection = true;
	return;
}

bool check_zergling_rush() {
	int n = BWAPI::Broodwar->getFrameCount();
	if (n >= 5400) return false;
	if (n >= 4000 && !willyt::rush_alert) return false;
	if (n <= 5400 && willyt::rush_alert) return true;
	int i = 0;
	int dmin = 16384;
	for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
		for (BWAPI::Unit u : p->getUnits()) {
			if (u->getType() == BWAPI::UnitTypes::Zerg_Zergling) {
				i++;
				int d = 3 * get_ground_dist(u);
				if (d > 0 && d < dmin) { dmin = d; }
			}
		}
	}
	if (i >= 4 && n + dmin < 4000) { return true; }
	return false;
}

bool check_defense_focus() {
	int an = willyt::sup_bio + willyt::sup_mech;
	int gn = wilenemy::grddef_count;
	if (an > 96) return false;
	if (gn > 0 && 4 * gn >= an) return true;
	return false;
}

bool check_proxy_buildings() {
	if (willyt::my_time < 600)
		for (BWAPI::Position p : wilenemy::positions)
			if (get_ground_dist(p) < 128)
				return true;
	return false;
}

bool check_proxy_prod_buildings() {
	if (willyt::my_time < 600)
		for (BWAPI::Player p : BWAPI::Broodwar->enemies())
			for (BWAPI::Unit u : p->getUnits())
				if (u->getType().isBuilding() &&
					u->getType().canProduce() &&
					!u->isFlying() &&
					get_ground_dist(u) < 128)
					return true;
	return false;
}

bool check_pressure_on_bunker() {
	int my_supply = 2 * willyt::sup_bio + 2 * willyt::sup_mech;
	if (BWAPI::Broodwar->getFrameCount() > 10080 ||
		my_supply > wilenemy::supply_army ||
		wilbuild::bunkers.empty())
		return false;
	if (willyt::hold_bunker)
		return true;
		//wilenemy::supply_near_bunker > 0 &&
		//wilenemy::supply_near_bunker >= my_supply
	if (willyt::fast_expand &&
		wilenemy::supply_army > my_supply &&
		!wilbuild::bunkers.empty() &&
		BWAPI::Broodwar->getFrameCount() > 6480)
		return true;
	return false;
}

void check_mega_macro() {
	if (willyt::my_time == 210 &&
		BWAPI::Broodwar->enemies().size() == 1 &&
		wilenemy::main_pos.size() == 1 &&
		scout_near(wilenemy::main_pos.front()) &&
		wilenemy::produc_count == wilenemy::mainbuild_count &&
		wilenemy::supply_army == 0 &&
		!wilenemy::has_pool &&
		!wilenemy::has_forge &&
		!willyt::proxy_alert)
	{
		wilenemy::goes_megamacro = true;
		willyt::is_rushing = true;
		BWAPI::Broodwar->printf("detect enemy strategy: MEGA-MACRO");
	}
	if (willyt::my_time == 225 &&
		wilenemy::goes_megamacro &&
		(wilenemy::produc_count != wilenemy::mainbuild_count ||
		wilenemy::supply_army > 0 ||
		wilenemy::has_pool ||
		wilenemy::has_forge ||
		willyt::proxy_alert))
	{
		wilenemy::goes_megamacro = false;
		willyt::is_rushing = false;
		BWAPI::Broodwar->printf("reject enemy strategy: MEGA-MACRO");
	}
	return;
}

void check_carrier_rush() {
	if (willyt::my_time == 300 &&
		wilenemy::sem != -1 &&
		wilenemy::race == BWAPI::Races::Protoss &&
		wilenemy::airprod_count > 0 &&
		wilenemy::produc_count - wilenemy::airprod_count - wilenemy::mainbuild_count == 1) {
		willyt::carrier_rush_alert = true;
	}
	if (willyt::my_time == 600) {
		willyt::carrier_rush_alert = false;
	}
	return;
}

void change_strategies() {
	using namespace willyt;
	if (rush_alert &&
		strategy != 1) {
		strategy = 1;
		go_bio = true;
		BWAPI::Broodwar->printf("change strategy to 1 to defend rush");
	}
	if (proxy_prod_alert &&
		!avoid_grddef &&
		BWAPI::Broodwar->getFrameCount() < 7200 &&
		strategy != 1) {
		strategy = 1;
		go_bio = true;
		BWAPI::Broodwar->printf("change strategy to 1 to defend proxy");
	}
	if (proxy_alert && !cannon_rush_alert &&
		(wilenemy::has_forge || avoid_grddef)) {
		strategy = 3;
		fast_expand = false;
		go_bio = false;
		is_rushing = true;
		cannon_rush_alert = true;
		stop_all_build_missions();
		BWAPI::Broodwar->printf("change strategy to 3 to defend cannon rush");
	}
	return;
}

void check_resign() {
	if (BWAPI::Broodwar->getFrameCount() >= 10000 &&
		BWAPI::Broodwar->self()->supplyUsed() <= 36 &&
		!wilbuild::buildings.empty() &&
		has_only_island_buildings()) {
		BWAPI::Broodwar->sendText("island lockdown");
		BWAPI::Broodwar->leaveGame();
	}
	if (willyt::is_vs_human &&
		BWAPI::Broodwar->getFrameCount() >= 5000 &&
		BWAPI::Broodwar->self()->supplyUsed() <= 4 &&
		wilenemy::supply_army >= 24) {
		BWAPI::Broodwar->sendText("human win");
		BWAPI::Broodwar->leaveGame();
	}
	return;
}

bool has_only_island_buildings() {
	for (BWAPI::Unit u : wilbuild::buildings)
		if (u->exists() && get_ground_dist(u) != -1)
			return false;
	return true;
}

bool scout_near(BWAPI::Position my_pos) {
	for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits())
		if (u->getType().isWorker())
			if (sqdist(u->getPosition(), my_pos) < 262144)
				return true;
	return false;
}