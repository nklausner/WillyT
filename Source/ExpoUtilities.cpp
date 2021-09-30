#include "ExpoUtilities.h"



void fill_build_map_with_expos() {
	for (BWAPI::Unit unit : BWAPI::Broodwar->getNeutralUnits()) {
		if (unit->getType().isBuilding()) {
			set_map_area(wilmap::build_map_var, unit, false);
			if (unit->isInvincible() &&
				!unit->getType().isMineralField()) {
				set_map_area(wilmap::build_map_fix, unit, false);
			}
		}
	}
	for (Expo &expo : wilexpo::all) {
		set_map_area(wilmap::build_map_fix, expo.tile.x, expo.tile.y, 4, 3, false);
		set_map_area(wilmap::build_map_var, expo.tile.x, expo.tile.y, 4, 3, false);
		set_map_area(wilmap::build_map_fix, expo.tile.x + 4, expo.tile.y + 1, 2, 2, false);
		set_map_area(wilmap::build_map_var, expo.tile.x + 4, expo.tile.y + 1, 2, 2, false);
	}
	return;
}



void determine_islands() {
	for (Expo &expo : wilexpo::all) {
		int d = 0;
		int s = 0;
		for (int i = 0; i < wilmap::mn; i++) {
			d = wilmap::maindistarray[i][expo.tile.y][expo.tile.x];
			s += d;
			if (d >= 0) { expo.is_continent = true; }
		}
		if (s < 0) { expo.is_island = true; }
	}
	return;
}



void determine_natu_all() {
	for (int i = 0; i < wilmap::mn; i++) { determine_natu(i); }
	determine_my_natu();
	return;
}

void determine_natu(int i) {
	BWAPI::TilePosition min_tile = BWAPI::TilePositions::None;
	int min_dist = 1024;
	for (Expo& e : wilexpo::all) {
		int my_dist = wilmap::maindistarray[i][e.tile.y + 1][e.tile.x + 1];
		if (!e.geysers.empty() &&
			my_dist > 0 &&
			my_dist < min_dist) {
			min_dist = my_dist;
			min_tile = e.tile;
		}
	}
	if (min_tile != BWAPI::TilePositions::None && min_dist != 1024) {
		wilmap::natu_tiles[i] = BWAPI::TilePosition{ min_tile.x + 1, min_tile.y + 1 };
		wilmap::natu_pos[i] = BWAPI::Position{ 32 * min_tile.x + 64, 32 * min_tile.y + 48 };

		for (Expo& e : wilexpo::all) {
			if (e.tile == min_tile) {
				e.is_natu = true;
				e.corresponding_main = wilmap::main_pos[i];
			}
		}
	}
	else {
		wilmap::natu_tiles[i] = wilmap::main_tiles[i];
		wilmap::natu_pos[i] = wilmap::main_pos[i];
	}
	//BWAPI::Broodwar->printf("found natural %d", i);
	return;
}

void determine_my_natu() {
	BWAPI::TilePosition t = wilmap::natu_tiles[wilmap::mm];
	wilmap::my_natu = BWAPI::Position{ 32 * t.x + 32, 32 * t.y + 16 };
	//BWAPI::Broodwar->printf("natural base: %d, %d", t.x, t.y);
	return;
}



void determine_expo_circle() {
	wilmap::aircircle.clear();
	wilmap::grdcircle.clear();
	wilmap::flycircle.clear();
	std::vector<BWAPI::Position> aircache;
	std::vector<BWAPI::Position> grdcache;
	std::vector<double> airangle;
	std::vector<double> grdangle;

	int xc = wilmap::center_pos.x;
	int yc = wilmap::center_pos.y;
	double rc = pow(pow(xc, 2) + pow(yc, 2), 0.5) / 3.0;
	for (Expo &expo : wilexpo::all) {
		int xr = expo.posi.x - xc;
		int yr = expo.posi.y - yc;
		double r = pow(pow(xr, 2) + pow(yr, 2), 0.5);
		double a = (yr < 0) ? -acos(xr / r) : +acos(xr / r);
		if (r > rc) {
			aircache.push_back(expo.posi);
			airangle.push_back(a);
		}
		if (r > rc && !expo.is_island) {
			grdcache.push_back(expo.posi);
			grdangle.push_back(a);
		}
		//BWAPI::Broodwar->printf("( %f , %f ) = %f", xr/r, yr/r, a);
	}

	unsigned air_max = aircache.size();
	for (unsigned i = 0; i < air_max; i++) {
		double a_min = 4.0;
		unsigned j_min = 0;
		for (unsigned j = 0; j < aircache.size(); j++) {
			if (airangle[j] < a_min) {
				a_min = airangle[j];
				j_min = j;
			}
		}
		wilmap::aircircle.push_back(aircache[j_min]);
		aircache.erase(aircache.begin() + j_min);
		airangle.erase(airangle.begin() + j_min);
	}
	//BWAPI::Broodwar->printf("expansions air circle: %d", wilexpo::aircircle.size());

	unsigned grd_max = grdcache.size();
	for (unsigned i = 0; i < grd_max; i++) {
		double a_min = 4.0;
		unsigned j_min = 0;
		for (unsigned j = 0; j < grdcache.size(); j++) {
			if (grdangle[j] < a_min) {
				a_min = grdangle[j];
				j_min = j;
			}
		}
		wilmap::grdcircle.push_back(grdcache[j_min]);
		grdcache.erase(grdcache.begin() + j_min);
		grdangle.erase(grdangle.begin() + j_min);
	}
	//BWAPI::Broodwar->printf("expansions grd circle: %d", wilexpo::grdcircle.size());

	for (BWAPI::Position pe : wilmap::aircircle) {
		BWAPI::Position pf = linear_interpol_abs(pe, wilmap::center_pos, -128);
		if (!pf.isValid()) { pf = pe; }
		wilmap::flycircle.push_back(pf);
	}
	return;
}

int get_circle_number(BWAPI::Position my_pos, std::vector<BWAPI::Position>& my_vec) {
	for (unsigned i = 0; i < my_vec.size(); i++) {
		if (sqdist(my_vec[i], my_pos) < 65536) {
			return int(i);
		}
	}
	return -1;
}


//BWAPI::Position my_pos = wilmap::my_main;
//int i_max = grdcache.size();
//for (int i = 0; i < i_max; i++) {
//	BWAPI::Position min_pos = get_closest(grdcache, my_pos);
//	if (min_pos != BWAPI::Positions::None) {
//		my_pos = min_pos;
//		wilexpo::grdcircle.push_back(my_pos);
//		vector_remove(grdcache, my_pos);
//	}
//}
//my_pos = wilmap::my_main;
//i_max = aircache.size();
//for (int i = 0; i < i_max; i++) {
//	BWAPI::Position min_pos = get_closest(aircache, my_pos);
//	if (min_pos != BWAPI::Positions::None) {
//		my_pos = min_pos;
//		wilexpo::aircircle.push_back(my_pos);
//		vector_remove(aircache, my_pos);
//	}
//}