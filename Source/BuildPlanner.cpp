#include "BuildPlanner.h"

namespace wilplanner
{

	void create_buildplan() {
		int x0 = wilmap::my_start.x;
		int y0 = wilmap::my_start.y;
		wilmap::plan_small.clear();
		wilmap::plan_large.clear();
		wilmap::plan_small_tech.clear();
		wilmap::plan_large_tech.clear();

		clear_map_bool(wilmap::main_map);
		fill_main_map(x0 - 1, y0 - 1);
		fill_mining_space(x0, y0);
		fill_center_roundabout(x0, y0);
		fill_main_corridors();

		create_buildplan_position(x0, y0);
		balance_buildplan();
		avoid_refinery_trap();
		avoid_depot_trap();
		sort_vector_tile(wilmap::plan_small, wilmap::my_start);
		sort_vector_tile(wilmap::plan_large, wilmap::my_start);
		fill_vector_tech(wilmap::plan_small, wilmap::plan_small_tech, 3, 2);
		fill_vector_tech(wilmap::plan_large, wilmap::plan_large_tech, 6, 3);
		//BWAPI::Broodwar->printf("create build plan");
		return;
	}



	void fill_main_map(int x, int y) {
		using namespace wilmap;
		if (main_map[y][x] == false &&
			choke_map[y][x] == false &&
			build_map_var[y][x] == true) {
			main_map[y][x] = true;
			fill_main_map(x - 1, y);
			fill_main_map(x + 1, y);
			fill_main_map(x, y - 1);
			fill_main_map(x, y + 1);
		}
		return;
	}

	void fill_mining_space(int x, int y) {
		BWAPI::Position p0 = BWAPI::Position{ 32 * x + 64, 32 * y + 48 };
		for (BWAPI::Unit unit : BWAPI::Broodwar->getNeutralUnits()) {
			if (unit->getType().isResourceContainer() &&
				sqdist(unit->getPosition(), p0) < 102400) {

				BWAPI::Position p = unit->getPosition();
				BWAPI::TilePosition t = unit->getTilePosition();
				int w = unit->getType().tileWidth();
				int h = unit->getType().tileHeight();

				switch (get_direction_4(p0, p)) {
				case 0: set_map_area(wilmap::main_map, t.x - 2, t.y, 2, h, false); break;
				case 1: set_map_area(wilmap::main_map, t.x, t.y - 2, w, 2, false); break;
				case 2: set_map_area(wilmap::main_map, t.x + w, t.y, 2, h, false); break;
				case 3: set_map_area(wilmap::main_map, t.x, t.y + h, w, 2, false); break;
				}
			}
		}
		return;
	}

	void fill_center_roundabout(int x, int y) {
		set_map_area(wilmap::main_map, x - 1, y - 1, 8, 5, false);
	}

	void fill_main_corridors() {
		using namespace wilmap;
		for (int i = 0; i < ht / 14; i++) {
			set_map_area(main_map, 4, 6 + i * 7, wt - 8, 1, false);
			set_map_area(main_map, 4, ht - 8 - i * 7, wt - 8, 1, false);
		}
		return;
	}



	void create_buildplan_position(int x0, int y0) {
		using namespace wilmap;
		int x1 = x0 + 1;
		int y1 = y0 + 1;

		if (x0 < 0 || y0 < 0 || x0 > 255 || y0 > 255) return;

		else if (is_topl(x1, y1)) create_buildplan_columns(0, 0);
		else if (is_topr(x1, y1)) create_buildplan_columns(wt - 42, 0);
		else if (is_botl(x1, y1)) create_buildplan_columns(0, ht - 35);
		else if (is_botr(x1, y1)) create_buildplan_columns(wt - 42, ht - 35);

		else if (is_top_(x1, y1)) create_buildplan_columns(x0 - 18, 0);
		else if (is_bot_(x1, y1)) create_buildplan_columns(x0 - 18, ht - 35);
		else if (is_left(x1, y1)) create_buildplan_columns(0, y0 - 15);
		else if (is_rigt(x1, y1)) create_buildplan_columns(wt - 42, y0 - 15);

		else create_buildplan_columns(x0 - 18, y0 - 15);
		return;
	}

	void create_buildplan_columns(int x0, int y0) {
		int dxl[5] = { 4, 11, 18, 25, 32 };
		for (int i = 0; i < 5; i++) {
			for (int dy = 0; dy < 33; dy++) {
				if (check_map_area(wilmap::main_map, x0 + dxl[i], y0 + dy, 6, 3)) {
					wilmap::plan_large.push_back(BWAPI::TilePosition{ x0 + dxl[i], y0 + dy });
					set_map_area(wilmap::main_map, x0 + dxl[i], y0 + dy, 6, 3, false);
				}
			}
		}
		int dxs[12] = { 0, 4,7, 11,14, 18,21, 25,28, 32,35, 39 };
		for (int i = 0; i < 12; i++) {
			for (int dy = 0; dy < 34; dy++) {
				if (check_map_area(wilmap::main_map, x0 + dxs[i], y0 + dy, 3, 2)) {
					wilmap::plan_small.push_back(BWAPI::TilePosition{ x0 + dxs[i], y0 + dy });
					set_map_area(wilmap::main_map, x0 + dxs[i], y0 + dy, 3, 2, false);
				}
			}
		}
		return;
	}

	void balance_buildplan() {
		using namespace wilmap;
		while (plan_small.size() < 2 * plan_large.size() - 1 &&
			plan_large.size() > 9 &&
			plan_small.size() < 24) {
			int i = rand() % plan_large.size();
			int j = rand() % 2;
			BWAPI::TilePosition t = plan_large[i];
			vector_remove(plan_large, t);
			plan_small.push_back(BWAPI::TilePosition{ t.x , t.y + j });
			plan_small.push_back(BWAPI::TilePosition{ t.x + 3 , t.y + j });
		}
		return;
	}

	void avoid_refinery_trap() {
		using namespace wilmap;
		for (BWAPI::Unit unit : BWAPI::Broodwar->getNeutralUnits()) {
			if (unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser &&
				unit->getTilePosition().y <= 1)
			{
				int xref = unit->getTilePosition().x;
				for (auto it = plan_small.begin(); it != plan_small.end(); ++it)
				{
					if (it->y <= 3 && it->x == xref - 3 &&
						check_map_area(main_map, xref - 4, it->y, 1, 2)) {
						it->x = xref - 4;
						set_map_area(main_map, xref - 4, it->y, 1, 2, false);
						set_map_area(main_map, xref - 1, it->y, 1, 2, true);
						//BWAPI::Broodwar->printf("avoid refinery trap, move small building spot to: %d, %d", it->x, it->y);
					}
				}
			}
		}
		return;
	}

	void avoid_depot_trap() {
		using namespace wilmap;
		for (auto it = plan_small.begin(); it != plan_small.end(); ++it) {
			if (it->y != 0) { continue; }
			int x = it->x;
			int y = it->y;
			if (!main_map[y + 0][x - 1] &&
				!main_map[y + 1][x - 1] &&
				!main_map[y + 0][x + 3] &&
				!main_map[y + 1][x + 3])
			{
				BWAPI::TilePosition tblock = BWAPI::TilePosition(x, y + 2);
				if (vector_holds(plan_small, tblock)) {
					vector_remove(plan_small, tblock);
					//BWAPI::Broodwar->printf("removing small spot: %d, %d", x, y + 2);
				}
			}
		}
		return;
	}



	void sort_vector_tile(std::vector<BWAPI::TilePosition>& v, BWAPI::TilePosition t0) {
		std::vector<std::pair<int, BWAPI::TilePosition>> dv;
		for (auto it = v.begin(); it != v.end(); ++it) {
			dv.push_back(std::make_pair(sqdist(t0, *it), *it));
		}
		sort(dv.begin(), dv.end());
		v.clear();
		for (auto it = dv.begin(); it != dv.end(); ++it) {
			v.push_back(it->second);
		}
		return;
	}

	void fill_vector_tech(std::vector<BWAPI::TilePosition>& v, std::vector<BWAPI::TilePosition>& vt, int w, int h) {
		for (auto it = v.begin(); it != v.end(); ) {
			if (vt.size() < 2 &&
				is_on_map_edge(it->x, it->y, w, h)) {
				vt.push_back(BWAPI::TilePosition{ it->x , it->y });
				v.erase(it);
			}
			else { ++it; }
		}
		return;
	}

	bool is_on_map_edge(int x, int y, int w, int h) {
		if (x == 0 || x == wilmap::wt - w ||
			y == 0 || y == wilmap::ht - h - 1) {
			return true;
		}
		return false;
	}



	BWAPI::TilePosition choose_tile(std::vector<BWAPI::TilePosition>& v, int w, int h) {
		int i = rand() % 2;
		for (auto it = v.begin(); it != v.end(); ++it) {
			if (check_map_area(wilmap::build_map_var, it->x, it->y, w, h) &&
				check_map_area(wilthr::grddef, it->x, it->y, w, h, 4)) {
				if (i == 0) { return *it; }
				if (i == 1) { i = 0; }
			}
		}
		return BWAPI::TilePositions::None;
	}
	BWAPI::TilePosition choose_tech(std::vector<BWAPI::TilePosition>& v, std::vector<BWAPI::TilePosition>& vt, int w, int h) {
		for (auto it = vt.begin(); it != vt.end(); ++it) {
			if (check_map_area(wilmap::build_map_var, it->x, it->y, w, h) &&
				check_map_area(wilthr::grddef, it->x, it->y, w, h, 4)) {
				return *it;
			}
		}
		return choose_tile(v, w, h);
	}
	BWAPI::TilePosition choose_tile_near(std::vector<BWAPI::TilePosition>& v, int w, int h, BWAPI::TilePosition t) {
		BWAPI::TilePosition min_tile = BWAPI::TilePositions::None;
		int min_sqdist = 65536;
		for (auto it = v.begin(); it != v.end(); ++it) {
			if (check_map_area(wilmap::build_map_var, it->x, it->y, w, h) &&
				check_map_area(wilthr::grddef, it->x, it->y, w, h, 4)) {
				if (sqdist(t, *it) < min_sqdist) {
					min_sqdist = sqdist(t, *it);
					min_tile = *it;
				}
			}
		}
		return min_tile;
	}

}




//void create_buildplan_large() {
//	using namespace wilmap;
//	for (auto it = plan_small.begin(); it != plan_small.end(); ++it)
//	{
//		if (plan_small.size() <= 20) {
//			break;
//		}
//		int x = it->x;
//		int y = it->y;
//		if (// ... contains_tile(plan_small, x + 0, y + 0) ..
//			vector_contains_tile(plan_small, x + 3, y + 0) &&
//			vector_contains_tile(plan_small, x + 0, y + 2) &&
//			vector_contains_tile(plan_small, x + 3, y + 2) &&
//			vector_contains_tile(plan_small, x + 0, y + 4) &&
//			vector_contains_tile(plan_small, x + 3, y + 4))
//		{
//			vector_remove_tile(plan_small, x + 0, y + 0);
//			vector_remove_tile(plan_small, x + 3, y + 0);
//			vector_remove_tile(plan_small, x + 0, y + 2);
//			vector_remove_tile(plan_small, x + 3, y + 2);
//			vector_remove_tile(plan_small, x + 0, y + 4);
//			vector_remove_tile(plan_small, x + 3, y + 4);
//
//			plan_large.push_back(BWAPI::TilePosition{ x + 0, y + 0 });
//			plan_large.push_back(BWAPI::TilePosition{ x + 0, y + 3 });
//		}
//	}
//}