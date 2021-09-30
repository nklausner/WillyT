#include "ExpoFinder.h"

void ExpoFinder::check() {
	int g = BWAPI::Broodwar->self()->gas();
	int m = BWAPI::Broodwar->self()->minerals();
	int n_min = 21;
	if (willyt::strategy == 4) { n_min = 12; }
	if (willyt::go_islands && willyt::has_transport) { n_min += 6; }

	if (willyt::mineral_count < n_min &&
		willyt::my_time > wilbuild::locking_time &&
		allow_additional_wierd_expansion_condition() &&
		m > 400) {
		check_additional_expansion();
	}
	if (willyt::my_time > wilbuild::locking_time && m > 400) {
		check_fast_island_expansion();
		check_build_wallin();
	}
	if (any_geyser_available() &&
		willyt::my_time > wilbuild::locking_time &&
		m > 300 && g < m / 2) {
		check_additional_refinery();
	}
	if (wilenemy::goes_megamacro &&
		willyt::my_time < 300) {
		add_suspected_expo(4);
		wilbuild::locking_time = 450;
	}
	if (willyt::strategy == 4 &&
		wilenemy::sem != -1 &&
		//is_none(wilmap::my_sneaky_tile) &&
		wilmap::mn > 2) {
		define_sneaky_tile();
	}

	update_unclaimed_expo();
	count_highground_defense();
}

void ExpoFinder::check_additional_expansion()
{
	BWAPI::TilePosition enemy_center = calc_enemy_center();
	BWAPI::TilePosition min_tile = BWAPI::TilePositions::None;
	BWAPI::TilePosition my_tile = BWAPI::TilePositions::None;
	int my_dist = 1048576;
	int min_dist = 1048576;
	bool allow_min_only = allow_min_only_expansion();

	for (Expo &my_expo : wilexpo::all) {
		if (my_expo.is_owned ||
			my_expo.is_enemy ||
			(my_expo.is_island && !willyt::has_transport) ||
			(my_expo.is_island && rand() % 2 == 1) ||
			my_expo.minerals.size() < 4) {
			continue;
		}
		if (my_expo.geysers.empty() &&
			!allow_min_only) {
			continue;
		}
		my_tile = my_expo.tile;
		my_dist = get_ground_dist(my_tile);
		if (my_dist < 0) { my_dist = my_tile.getApproxDistance(wilmap::my_start); }
		my_dist = my_dist - 2 * my_tile.getApproxDistance(enemy_center);

		if (min_dist > my_dist) {
			min_dist = my_dist;
			min_tile = my_tile;
		}
	}
	if (!is_none(min_tile)) {
		build(BWAPI::UnitTypes::Terran_Command_Center, min_tile);
	}
}

BWAPI::TilePosition ExpoFinder::calc_enemy_center()
{
	int x = 16 * BWAPI::Broodwar->mapWidth();
	int y = 16 * BWAPI::Broodwar->mapHeight();
	int q = 32 + 32 * wilenemy::positions.size();
	for (BWAPI::Position &p : wilenemy::positions) {
		x += p.x;
		y += p.y;
	}
	return BWAPI::TilePosition(x/q, y/q);
}

void ExpoFinder::check_additional_refinery()
{
	BWAPI::TilePosition my_tile = BWAPI::TilePositions::None;
	for (Resource& r : wilbuild::geysers) { //MINERAL_LOCK_UPDATE
		if (r.is_owned &&
			r.unit->exists() &&
			r.unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser &&
			r.unit->getResources() >= 400 &&
			r.tile.isValid() &&
			(get_ground_dist(r.tile) > 0 || willyt::has_transport))
		{
			my_tile = r.tile;
			break;
		}
	}
	if (!is_none(my_tile)) {
		build(BWAPI::UnitTypes::Terran_Refinery, my_tile);
	}
}

void ExpoFinder::build(BWAPI::UnitType my_type, BWAPI::TilePosition my_tile)
{
	if (!is_none(my_tile) && !vector_holds(wilbuild::planned_builds, my_type)) {
		auto it = choose_scv(my_tile);
		if (it != wilunits::scvs.end()) {
			it->set_build_mission(my_type, my_tile);
			//BWAPI::Broodwar->printf("build %s", my_type.c_str());
		}
	}
}

bool ExpoFinder::allow_min_only_expansion()
{
	int min = calc_theo_income_min() + BWAPI::Broodwar->self()->minerals();
	int gas = calc_theo_income_gas() + BWAPI::Broodwar->self()->gas();
	//BWAPI::Broodwar->printf("calculated %d min", min);
	//BWAPI::Broodwar->printf("calculated %d gas", gas);
	int fgm = (willyt::strategy <= 2) ? 3 : 2;
	if (min < fgm * gas ||
		willyt::mineral_count < 4) {
		return true;
	}
	return false;
}

bool ExpoFinder::any_geyser_available() {
	for (Resource& r : wilbuild::geysers) {
		if (r.is_owned &&
			r.unit->exists() &&
			r.unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser &&
			r.unit->getResources() >= 400 &&
			r.tile.isValid()) {
			return true;
		}
	}
	return false;
}

int ExpoFinder::calc_theo_income_min() {
	int i = 0;
	for (Resource& r : wilbuild::minerals) {
		if (r.is_owned && r.unit->getResources() > 100) {
			i += 100;
		} else {
			i += 25;
		}
	}
	return i;
}

int ExpoFinder::calc_theo_income_gas() {
	int i = 0;
	for (Resource& r : wilbuild::geysers) {
		if (r.is_owned && r.unit->getResources() > 200) {
			i += 200;
		} else {
			i += 50;
		}
	}
	return i;
}

void ExpoFinder::update_unclaimed_expo() {
	wilenemy::unclaimed_expo_pos.clear();
	for (Expo& expo : wilexpo::all) {
		if (!expo.is_owned &&
			!expo.is_enemy &&
			!expo.is_island &&
			!buildspot_visible(expo.tile)) {
			wilenemy::unclaimed_expo_pos.push_back(expo.posi);
		}
	}
	return;
}

void ExpoFinder::add_suspected_expo(int imax) {
	int i = 0;
	for (BWAPI::Position p : wilenemy::positions) {
		for (Expo expo : wilexpo::all) {
			if (expo.posi == p) { i++; }
		}
	}
	if (i >= imax || wilenemy::main_pos.empty()) {
		return;
	}
	BWAPI::Position pos0 = wilenemy::main_pos.front();
	BWAPI::Position posmin = BWAPI::Positions::None;
	int sqdmin = 67108864;
	for (Expo& expo : wilexpo::all) {
		if (!expo.is_owned &&
			!expo.is_enemy &&
			!expo.is_island &&
			!vector_holds(wilenemy::positions, expo.posi))
		{
			int sqd = sqdist(pos0, expo.posi);
			if (sqd < sqdmin) {
				sqdmin = sqd;
				posmin = expo.posi;
			}
		}
	}
	if (posmin != BWAPI::Positions::None) {
		wilenemy::positions.push_back(posmin);
		//BWAPI::Broodwar->printf("added suspected expo at %d, %d", posmin.x / 32, posmin.y / 32);
	}
	return;
}

bool ExpoFinder::allow_additional_wierd_expansion_condition()
{
	if (willyt::strategy == 4 &&
		wilbuild::commandcenters.size() >= 2 &&
		willyt::sup_bio + willyt::sup_mech < willyt::attack_supply) {
		return false;
	}
	return true;
}

void ExpoFinder::check_fast_island_expansion()
{
	if (willyt::strategy == 4 &&
		willyt::has_natural &&
		willyt::has_transport)
	{
		BWAPI::TilePosition my_tile = BWAPI::TilePositions::None;
		for (Expo& expo : wilexpo::all) {
			if (expo.is_island &&
				!expo.is_owned &&
				!expo.is_enemy &&
				sqdist(wilmap::my_start, expo.tile) < 72*72) {
				my_tile = expo.tile;
			}
		}
		if (!is_none(my_tile)) {
			build(BWAPI::UnitTypes::Terran_Command_Center, my_tile);
			//BWAPI::Broodwar->printf("take fast island expansion");
		}
	}
	return;
}

void ExpoFinder::check_build_wallin()
{
	BWAPI::TilePosition& my_tile = wilmap::natu_wall_tile[wilmap::mm][1];
	if (willyt::strategy == 4 &&
		willyt::has_natural &&
		willyt::my_time < wilbuild::locking_time + 300 &&
		!is_none(my_tile) &&
		check_map_area(wilmap::build_map_var, my_tile.x, my_tile.y, 3, 2))
	{
		build(BWAPI::UnitTypes::Terran_Supply_Depot, my_tile);
		//BWAPI::Broodwar->printf("complete wall in");
	}
	return;
}

void ExpoFinder::define_sneaky_tile()
{
	// -1 anticlockwise, +1 clockwise
	int mmi = get_circle_number(wilmap::my_main, wilmap::grdcircle);
	int mni = get_circle_number(wilmap::my_natu, wilmap::grdcircle);
	int emi = get_circle_number(wilmap::main_pos[wilenemy::sem], wilmap::grdcircle);
	int n = wilmap::grdcircle.size();
	if (emi == -1) { return; }
	int d1 = (mmi - emi + n) % n;
	int d2 = (emi - mmi + n) % n;
	if (d1 == d2) {
		wilmap::my_sneak_direction = (mni > mmi) ? -1 : +1;
	}
	else {
		wilmap::my_sneak_direction = (d1 > d2) ? -1 : +1;
	}
	//for (int j = 0; j < 6; j++) {
	//	int i = (mmi + j * wilmap::my_sneak_direction + n) % n;
	//	int x = wilmap::grdcircle[i].x / 32;
	//	int y = wilmap::grdcircle[i].y / 32;
	//	if (wilmap::maindistarray[wilmap::mm][y][x] > 120 &&
	//		(is_top_(x ,y) || is_bot_(x, y) || is_rigt(x, y) || is_left(x, y))) {
	//		wilmap::my_sneaky_tile = find_sneaky_tile(x, y);
	//	}
	//	if (!is_none(wilmap::my_sneaky_tile)) { return; }
	//}
	return;
}

BWAPI::TilePosition ExpoFinder::find_sneaky_tile(int x0, int y0)
{
	BWAPI::TilePosition max_tile = BWAPI::TilePositions::None;
	int max_dist = 0;
	int x1 = mapsafesub(x0, 12);
	int y1 = mapsafesub(y0, 12);
	int x2 = mapsafeadd(x0, 12, wilmap::wt) - 4;
	int y2 = mapsafeadd(y0, 12, wilmap::ht) - 4;
	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			if (check_map_area(wilmap::build_map_var, x, y, 4, 4)) {
				int my_metric = 2 * wilmap::centerdistmap[y][x] - wilmap::maindistarray[wilmap::mm][y][x];
				if (my_metric > max_dist) {
					max_dist = my_metric;
					max_tile = BWAPI::TilePosition(x, y);
				}
			}
		}
	}
	return max_tile;
}

void ExpoFinder::count_highground_defense()
{
	willyt::highground_defense_tanks = 0;
	willyt::planned_highground_defense = 0;

	for (Fighter2& tank : wilunits::siegetanks) {
		if (wilmap::main_map_orig[tank.unit->getTilePosition().y][tank.unit->getTilePosition().x] &&
			wilmap::natudefmap[tank.unit->getTilePosition().y][tank.unit->getTilePosition().x]) {
			willyt::highground_defense_tanks++;
		}
	}
	//BWAPI::Broodwar->printf("highground natu def: %d tanks", willyt::highground_defense_tanks);
	return;
}