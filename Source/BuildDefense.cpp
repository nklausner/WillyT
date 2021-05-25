#include "BuildDefense.h"



void determine_defs_all() {
	for (std::vector<Expo>::iterator it = wilexpo::all.begin(); it != wilexpo::all.end(); ++it) {
		bool db[4] = { false };

		if (it->is_island) {
			db[0] = true;
			db[1] = true;
			db[2] = true;
			db[3] = true;
		}
		else {
			int ddc = 0;
			for (BWAPI::TilePosition tg : it->gas_tiles) {
				BWAPI::Position pg = BWAPI::Position{ 32 * tg.x + 64 , 32 * tg.y + 32 };
				int i = get_defense_orientation(pg, it->posi);
				if (!db[i]) db[i] = true;
				else ddc++;
			}
			if (!it->min_tiles.empty()) {
				BWAPI::TilePosition tm = get_center_of_mass(it->min_tiles);
				BWAPI::Position pm = BWAPI::Position{ 32 * tm.x + 32 , 32 * tm.y + 16 };
				int i = get_defense_orientation(pm, it->posi);
				if (!db[i]) db[i] = true;
				else ddc++;
			}
			int i = 0;
			while (ddc > 0 && i < 4) {
				if (db[i] == false) {
					db[i] = true;
					ddc--;
				}
				i++;
			}
		}

		if (db[0]) it->def_tiles.push_back(BWAPI::TilePosition{ it->tile.x + 4 , it->tile.y - 1 }); //right
		if (db[1]) it->def_tiles.push_back(BWAPI::TilePosition{ it->tile.x + 1 , it->tile.y + 3 }); //bottom
		if (db[2]) it->def_tiles.push_back(BWAPI::TilePosition{ it->tile.x - 2 , it->tile.y + 1 }); //left
		if (db[3]) it->def_tiles.push_back(BWAPI::TilePosition{ it->tile.x + 1 , it->tile.y - 2 });	//up

		//write_string("detdef total: expo at " + std::to_string(it->tile.x) + "," + std::to_string(it->tile.y));
	}
}

int get_defense_orientation(BWAPI::Position r, BWAPI::Position c) {
	if (abs(r.y - c.y) >= abs(r.x - c.x))		//up or down
		return (r.y <= c.y) ? 3 : 1;
	else										//left or right
		return (r.x <= c.x) ? 2 : 0;
}



void define_turret_tiles(std::vector<BWAPI::TilePosition>& my_vector, BWAPI::TilePosition t0, bool need_more)
{
	my_vector.clear();
	if (wilenemy::race != BWAPI::Races::Terran || willyt::my_time < 600) {
		my_vector.push_back(t0);
	}
	for (Expo expo : wilexpo::all) {
		if (expo.is_owned) {
			for (BWAPI::TilePosition my_tile : expo.def_tiles) {
				my_vector.push_back(my_tile);
			}
		}
	}
	if (need_more) {
		for (BWAPI::Unit my_unit : wilbuild::barracks) {
			BWAPI::TilePosition my_tile = my_unit->getTilePosition();
			my_vector.push_back(BWAPI::TilePosition{ my_tile.x + 4, my_tile.y + 1 });
		}
		if (willyt::strategy == 6 &&
			!wilbuild::factories.empty()) {
			BWAPI::TilePosition my_tile = wilbuild::factories.front()->getTilePosition();
			my_vector.push_back(BWAPI::TilePosition{ my_tile.x + 4, my_tile.y + 1 });
		}
	}
	return;
}

BWAPI::TilePosition get_max_turret_tile(std::vector<BWAPI::TilePosition> &my_vector) {
	BWAPI::TilePosition max_tile = my_vector.front();
	int max_dist = 0;
	for (BWAPI::TilePosition my_tile : my_vector) {
		int my_dist = get_min_sqdist(my_tile, wilbuild::turrets);
		if (max_dist < my_dist) {
			max_dist = my_dist;
			max_tile = my_tile;
		}
	}
	return max_tile;
}

int get_min_sqdist(BWAPI::TilePosition my_tile, std::vector<BWAPI::Unit> &my_vector) {
	int min_dist = 65536;
	for (BWAPI::Unit my_unit : my_vector) {
		int my_dist = sqdist(my_tile, my_unit->getTilePosition());
		if (min_dist > my_dist) {
			min_dist = my_dist;
		}
	}
	return min_dist;
}

//bool is_turret_tile(BWAPI::TilePosition my_tile) {
//	for (BWAPI::Unit my_turret : wilbuild::turrets)
//		if (equals(my_turret->getTilePosition(), my_tile))
//			return true;
//	return false;
//}
//
//bool is_turret_near(BWAPI::TilePosition my_tile) {
//	for (BWAPI::Unit my_turret : wilbuild::turrets)
//		if (sqdist(my_turret->getTilePosition(), my_tile) <= 16)
//			return true;
//	return false;
//}



//BWAPI::TilePosition determine_sas_tile() {
//	using namespace wilmap;
//	int x1 = my_start.x + 1;
//	int y1 = my_start.y + 1;
//	BWAPI::TilePosition t1 = BWAPI::TilePositions::None;
//	BWAPI::TilePosition t2 = BWAPI::TilePositions::None;
//
//	if (x1 < 0 || y1 < 0 || x1 > 255 || y1 > 255) {
//		return BWAPI::TilePositions::None;
//	}
//	else if (is_topl(x1, y1)) {
//		t1 = get_sas_tile_line(0, 0, +1, 0);
//		t2 = get_sas_tile_line(0, 0, 0, +1);
//	}
//	else if (is_topr(x1, y1)) {
//		t1 = get_sas_tile_line(wt - 6, 0, -1, 0);
//		t2 = get_sas_tile_line(wt - 6, 0, 0, +1);
//	}
//	else if (is_botl(x1, y1)) {
//		t1 = get_sas_tile_line(0, ht - 4, +1, 0);
//		t2 = get_sas_tile_line(0, ht - 4, 0, -1);
//	}
//	else if (is_botr(x1, y1)) {
//		t1 = get_sas_tile_line(wt - 6, ht - 4, -1, 0);
//		t2 = get_sas_tile_line(wt - 6, ht - 4, 0, -1);
//	}
//	else if (is_top_(x1, y1)) {
//		t1 = get_sas_tile_line(x1 - 1, 0, +1, 0);
//		t2 = get_sas_tile_line(x1 - 1, 0, -1, 0);
//	}
//	else if (is_bot_(x1, y1)) {
//		t1 = get_sas_tile_line(x1 - 1, ht - 4, +1, 0);
//		t2 = get_sas_tile_line(x1 - 1, ht - 4, -1, 0);
//	}
//	else if (is_left(x1, y1)) {
//		t1 = get_sas_tile_line(0, y1 - 1, 0, +1);
//		t2 = get_sas_tile_line(0, y1 - 1, 0, -1);
//	}
//	else if (is_rigt(x1, y1)) {
//		t1 = get_sas_tile_line(wt - 6, y1 - 1, 0, +1);
//		t2 = get_sas_tile_line(wt - 6, y1 - 1, 0, -1);
//	}
//	else {
//		return BWAPI::TilePositions::None;
//	}
//
//	if (!t1) return t2;
//	if (!t2) return t1;
//	int d1 = sqdist(t1, BWAPI::TilePosition(my_natu));
//	int d2 = sqdist(t2, BWAPI::TilePosition(my_natu));
//	if (d1 >= d2) return t1;
//	if (d2 >= d1) return t2;
//	return BWAPI::TilePositions::None;
//}

//BWAPI::TilePosition get_sas_tile_line(int x0, int y0, int dx, int dy) {
//	for (int i = 0; i < 24; i++)
//		if (check_map_area(wilmap::build_map, x0 + i * dx, y0 + i * dy, 6, 3))
//			return BWAPI::TilePosition{ x0 + i * dx , y0 + i * dy };
//	return BWAPI::TilePositions::None;
//}