#include "MapInfo.h"

void MapInfo::display() {

	update_viewport();

	//display_bool(wilmap::build_map_var, false, BWAPI::Colors::Orange);
	//display_bool(wilmap::build_map_fix, false, BWAPI::Colors::Red);
	//display_bool(wilmap::walk_map, false, BWAPI::Colors::Red);
	//display_space_colors();

	//display_int(wilmap::centerdistmap, 0);
	//display_bool(wilmap::main_map, true, BWAPI::Colors::Red);
	//display_bool(wilmap::choke_map, true, BWAPI::Colors::Orange);
	//display_int(wilmap::mainpathmap, 0);
	//display_int(wilmap::space_map, 0);
	//display_int(wilmap::maindistarray[wilmap::mm], 0);
	//display_bool_walkpos(wilmap::walkmapfixed, false, BWAPI::Colors::Yellow);
	//display_bool_walkpos(wilmap::walkmap, false, BWAPI::Colors::Red);
	//display_int_walkpos(wilmap::centerdistwalkmap);
	//display_int_walkpos(wilmap::pathwalkmap);
	display_bool_walkpos(wilmap::chokewalkmap, true, BWAPI::Colors::Brown);
	//display_bool(wilmap::wall_map, true, BWAPI::Colors::Red);
	//display_bool(wilmap::maindefmap, true, BWAPI::Colors::Green);
	//display_bool(wilmap::natudefmap, true, BWAPI::Colors::Cyan);

	//display_start_positions();
	display_defense_positions();
	display_expansions();
	display_resources();
	display_army_positions();
	display_army_groups();
	display_enemy_positions();
	//display_enemy_positions_surround();
	//display_blocked_expo();
	//display_buildplan();
	//display_choke_directions();
	//draw_line(wilmap::grdcircle, BWAPI::Colors::Blue);
	//draw_line(wilmap::aircircle, BWAPI::Colors::Blue);
	//draw_line(wilmap::flycircle, BWAPI::Colors::Blue);
	//draw_pos_vec(wilmap::armycircle, BWAPI::Colors::Yellow);

	//display_int(wilmap::defense_map);
	//display_int(wilmap::bunker_map, -1);
	//display_int(wilmap::turret_map);
	//display_int_walkpos(wilthr::grddef, BWAPI::Colors::Orange);
	//display_int_walkpos(wilthr::airdef, BWAPI::Colors::Orange);
	//display_int_walkpos(wilthr::grdmap, BWAPI::Colors::Red);
	//display_int_walkpos(wilthr::airmap, BWAPI::Colors::Red);
	display_threats_all();
	//display_bool(wilmap::siege_map, true, BWAPI::Colors::White);
	display_holding_bunker();
	display_available_scans();
	display_choke_defense_status();

	//BWAPI::Broodwar->drawCircleMap(wilenemy::grdcenter, 32, BWAPI::Colors::Red, false);
	//BWAPI::Broodwar->drawCircleMap(wilenemy::aircenter, 32, BWAPI::Colors::Blue, false);

	return;
}

void MapInfo::display_bool(bool(&my_map)[256][256], bool bool_modus, BWAPI::Color c) {
	for (int y = ty0; y <= ty1; ++y) {
		for (int x = tx0; x <= tx1; ++x) {
			if (my_map[y][x] == bool_modus) {
				int xd = x * 32;
				int yd = y * 32;
				BWAPI::Broodwar->drawLineMap(xd + 2, yd + 2, xd + 29, yd + 29, c);
			}
		}
	}
	return;
}

void MapInfo::display_int(int(&my_map)[256][256], int threshold) {
	for (int y = ty0; y <= ty1; ++y) {
		for (int x = tx0; x <= tx1; ++x) {
			if (my_map[y][x] >= threshold) {
				int xd = x * 32 + 10;
				int yd = y * 32 + 10;
				BWAPI::Broodwar->drawTextMap(xd, yd, "%d", my_map[y][x]);
			}
		}
	}
	return;
}

void MapInfo::display_int_bool(int(&my_map)[256][256], BWAPI::Color c) {
	for (int y = ty0; y <= ty1; ++y) {
		for (int x = tx0; x <= tx1; ++x) {
			if (my_map[y][x] >= 0) {
				int xd = x * 32;
				int yd = y * 32;
				BWAPI::Broodwar->drawLineMap(xd + 12, yd + 12, xd + 19, yd + 19, c);
			}
		}
	}
	return;
}

void MapInfo::display_bool_walkpos(bool(&my_map)[1024][1024], bool bool_modus, BWAPI::Color c) {
	for (int y = wy0; y <= wy1; ++y) {
		for (int x = wx0; x <= wx1; ++x) {
			if (my_map[y][x] == bool_modus) {
				BWAPI::Broodwar->drawDotMap(x * 8 + 3, y * 8 + 3, c);
				//BWAPI::Broodwar->drawLineMap(8*x + 1, 8*y + 1, 8*x + 6, 8*y + 6, c);
			}
		}
	}
	return;
}

void MapInfo::display_int_walkpos(int(&my_map)[1024][1024], BWAPI::Color c) {
	for (int y = wy0; y <= wy1; ++y) {
		for (int x = wx0; x <= wx1; ++x) {
			if (my_map[y][x] != 0) {
				BWAPI::Broodwar->drawDotMap(x * 8 + 3, y * 8 + 3, c);
				//BWAPI::Broodwar->drawTextMap(x * 8 + 1, y * 8 - 3, "%d", my_map[y][x]);
			}
		}
	}
	return;
}

void MapInfo::display_int_walkpos(int(&my_map)[1024][1024]) {
	for (int y = wy0; y <= wy1; ++y) {
		for (int x = wx0; x <= wx1; ++x) {
			if (my_map[y][x] != -1) {
				//BWAPI::Broodwar->drawLineMap(x * 8, y * 8, x * 8 + 7, y * 8 + 7, BWAPI::Colors::Red);
				BWAPI::Broodwar->drawTextMap(x * 8 + 1, y * 8 - 3, "%d", my_map[y][x] / 10);
			}
		}
	}
	return;
}

void MapInfo::display_walkability() {
	for (int wy = wy0; wy <= wy1; ++wy) {
		for (int wx = wx0; wx <= wx1; ++wx) {
			if (!BWAPI::Broodwar->isWalkable(wx, wy)) {
				BWAPI::Broodwar->drawDotMap(wx * 8 + 3, wy * 8 + 3, BWAPI::Colors::Red);
			}
		}
	}
	return;
}

void MapInfo::display_space_colors() {
	for (int y = ty0; y <= ty1; ++y) {
		for (int x = tx0; x <= tx1; ++x) {
			if (wilmap::space_map[y][x] != -1) {
				int xp = 32 * x + 15;
				int yp = 32 * y + 16;
				BWAPI::Color c = get_space_color(wilmap::space_map[y][x]);
				BWAPI::Broodwar->drawLineMap(xp, yp, xp+2, yp, c);
			}
		}
	}
	return;
}

BWAPI::Color MapInfo::get_space_color(int i) {
	if (i > 30) { return BWAPI::Colors::Green; }
	else if (i > 20) { return BWAPI::Colors::Yellow; }
	else if (i > 10) { return BWAPI::Colors::Orange; }
	else { return BWAPI::Colors::Red; }
}

void MapInfo::display_start_positions() {
	int i = 0;
	for (BWAPI::TilePosition t : BWAPI::Broodwar->getStartLocations()) {
		BWAPI::Broodwar->drawTextMap((BWAPI::Position)t, "%d", i);
		draw_tile_box(t, 4, 3, BWAPI::Colors::Red);
		i++;
	}
	BWAPI::Broodwar->drawTextScreen(10, 140, BWAPI::Broodwar->mapFileName().c_str());
	BWAPI::Broodwar->drawTextScreen(10, 150, BWAPI::Broodwar->mapHash().c_str());
}

void MapInfo::display_expansions() {
	for (Expo &expo : wilexpo::all) {
		if (in_viewport(expo.posi)) {
			if (expo.is_owned) {
				BWAPI::Color cc = BWAPI::Colors::Brown;
				if (expo.is_undrsaturated) { cc = BWAPI::Colors::Grey; }
				if (expo.is_oversaturated) { cc = BWAPI::Colors::Purple; }
				draw_tile_box(expo.tile, 4, 3, cc);
				draw_tile_vec(expo.def_tiles, 2, 2, cc);
				BWAPI::Broodwar->drawCircleMap(expo.posi, 20, cc, true);
				BWAPI::Broodwar->drawTextMap(expo.tpos_min, "min: %d", expo.min_miner_count);
				BWAPI::Broodwar->drawTextMap(expo.tpos_gas, "gas: %d", expo.gas_miner_count);
			} else {
				BWAPI::Color cc = BWAPI::Colors::Grey;
				if (expo.is_enemy) { cc = BWAPI::Colors::Red; }
				draw_tile_box(expo.tile, 4, 3, cc);
				draw_tile_vec(expo.def_tiles, 2, 2, cc);
				BWAPI::Broodwar->drawTextMap(expo.tpos_min, "%c min: %d", BWAPI::Text::Grey, expo.minerals.size());
				BWAPI::Broodwar->drawTextMap(expo.tpos_gas, "%c gas: %d", BWAPI::Text::Grey, expo.geysers.size());
			}
			BWAPI::Broodwar->drawTextMap(32 * expo.tile.x, 32 * expo.tile.y, "%c %d, %d", BWAPI::Text::Grey, expo.tile.x, expo.tile.y);
			//if (expo.is_island) { BWAPI::Broodwar->drawTextMap(expo.posi, "is island"); }
			//if (expo.is_natu) { BWAPI::Broodwar->drawTextMap(expo.posi, "is natural"); }
		}
	}
	return;
}

void MapInfo::display_resources()
{
	for (Resource& r : wilbuild::minerals) {
		if (r.is_owned && in_viewport(r.unit->getPosition())) {
			draw_tile_box(r.tile, 2, 1, 128);
			BWAPI::Broodwar->drawTextMap(r.unit->getPosition(), "%d", r.scv_count);
		}
	}
	for (Resource& r : wilbuild::geysers) {
		if (r.is_owned && in_viewport(r.unit->getPosition())) {
			draw_tile_box(r.tile, 4, 2, 128);
			BWAPI::Broodwar->drawTextMap(r.unit->getPosition(), "%d", r.scv_count);
		}
	}
	return;
}

void MapInfo::display_defense_positions() {
	for (int i = 0; i < wilmap::mn; i++) {
		if (in_viewport(wilmap::main_choke_pos[i])) {
			BWAPI::Broodwar->drawCircleMap(wilmap::main_choke_pos[i], 16, 19, false);
		}
		if (in_viewport(wilmap::main_def_pos[i])) {
			BWAPI::Broodwar->drawCircleMap(wilmap::main_def_pos[i], 16, 19, false);
			draw_tile_box(wilmap::main_def_tile[i][0], 3, 2, 19);
			draw_tile_box(wilmap::main_def_tile[i][1], 2, 2, 19);
		}
		if (in_viewport(wilmap::natu_choke_pos[i])) {
			BWAPI::Broodwar->drawCircleMap(wilmap::natu_choke_pos[i], 16, 19, false);
		}
		if (in_viewport(wilmap::natu_def_pos[i])) {
			BWAPI::Broodwar->drawCircleMap(wilmap::natu_def_pos[i], 16, 19, false);
			draw_tile_box(wilmap::natu_def_tile[i][0], 3, 2, 19);
			draw_tile_box(wilmap::natu_def_tile[i][1], 2, 2, 19);
			draw_tile_box_check(wilmap::natu_wall_tile[i][0], 4, 3, 19);
			draw_tile_box_check(wilmap::natu_wall_tile[i][1], 3, 2, 19);
		}
		if (in_viewport(wilmap::main_pos[i])) {
			draw_tile_box(wilmap::rush_def_tile[i], 3, 2, 19);
		}
		//if (wilmap::test_pos[i] != BWAPI::Positions::None) {
		//	int x = wilmap::test_pos[i].x;
		//	int y = wilmap::test_pos[i].y;
		//	BWAPI::Broodwar->drawBoxMap(x - 96, y - 96, x + 95, y + 95, BWAPI::Colors::White, false);
		//}
	}
	return;
}

void MapInfo::display_buildplan() {
	draw_tile_vec(wilmap::plan_small, 3, 2, 165);
	draw_tile_vec(wilmap::plan_large, 6, 3, 165);
	draw_tile_vec_index(wilmap::plan_small);
	draw_tile_vec_index(wilmap::plan_large);
	draw_tile_vec(wilmap::plan_small_tech, 3, 2, 159);
	draw_tile_vec(wilmap::plan_large_tech, 6, 3, 159);
	//draw_tile_box(wilmap::sas_tile, 6, 3, 74);
	//BWAPI::Broodwar->drawTextMap(BWAPI::Position(wilmap::sas_tile), "%c starport / science facility", BWAPI::Text::Grey);
	return;
}

void MapInfo::display_blocked_expo() {
	if (wilbuild::blocked_tile) {
		int x = wilbuild::blocked_tile.x * 32;
		int y = wilbuild::blocked_tile.y * 32;
		BWAPI::Broodwar->drawLineMap(x +  0, y +  0, x + 63, y + 63, 111);
		BWAPI::Broodwar->drawLineMap(x +  0, y + 63, x + 63, y +  0, 111);
	}
	return;
}

void MapInfo::display_choke_directions() {
	for (int i = 0; i < wilmap::mn; i++) {
		BWAPI::Position p0 = (BWAPI::Position)wilmap::natu_choke_edge[2 * i];
		BWAPI::Position p1 = (BWAPI::Position)wilmap::natu_choke_edge[2 * i + 1];
		BWAPI::Broodwar->drawLineMap(p0, p1, BWAPI::Colors::White);
	}
}

void MapInfo::display_army_positions() {
	draw_crosshair(willyt::att_target_pos, 16, BWAPI::Colors::Red);
	draw_crosshair(willyt::def_target_pos, 16, BWAPI::Colors::Red);
	draw_crosshair(willyt::leader_pos, 16, BWAPI::Colors::Green);
	return;
}

void MapInfo::display_enemy_positions() {
	for (BWAPI::Position p : wilenemy::positions) {
		BWAPI::Broodwar->drawLineMap(p.x - 8, p.y - 8, p.x + 8, p.y + 8, BWAPI::Colors::Red);
		BWAPI::Broodwar->drawLineMap(p.x - 8, p.y + 8, p.x + 8, p.y - 8, BWAPI::Colors::Red);
	}
	return;
}

void MapInfo::display_enemy_positions_surround() {
	for (BWAPI::Position p : wilenemy::positions) {
		for (int i = 0; i < 24; i++) {
			int x = p.x + 8 * wilmap::cr4dx[i];
			int y = p.y + 8 * wilmap::cr4dy[i];
			if (in_viewport(x, y)) {
				BWAPI::Broodwar->drawBoxMap(x - 8, y - 8, x + 8, y + 8, BWAPI::Colors::Red, false);
			}
		}
	}
	return;
}

void MapInfo::display_holding_bunker() {
	if (willyt::hold_bunker &&
		!wilbuild::bunkers.empty()) {
		BWAPI::Broodwar->drawTextMap(wilbuild::bunkers.front()->getPosition(), "%c hold bunker", BWAPI::Text::Red);
	}
	return;
}

void MapInfo::display_available_scans() {
	for (BWAPI::Unit u : wilbuild::comsatstations) {
		BWAPI::Broodwar->drawTextMap(u->getPosition(), "%d scans", willyt::available_scans);
	}
	return;
}

void MapInfo::display_choke_defense_status() {
	if (willyt::is_choke_def) {
		BWAPI::Position my_pos = wilmap::main_choke_pos[wilmap::mm];
		if (willyt::has_natural) { my_pos = wilmap::natu_choke_pos[wilmap::mm]; }
		BWAPI::Broodwar->drawTextMap(my_pos, "%c choke defense", BWAPI::Text::Red);
	}
	return;
}

void MapInfo::display_army_groups() {
	using namespace wilgroup;
	display_army_group(player_grd_vec, player_grd_pos, player_grd_supply, BWAPI::Colors::Yellow);
	display_army_group(player_air_vec, player_air_pos, player_air_supply, BWAPI::Colors::Cyan);
	display_army_group(enemy_grd_vec, enemy_grd_pos, enemy_grd_supply, BWAPI::Colors::Orange);

	return;
}

void MapInfo::display_army_group(std::vector<BWAPI::Unit>& my_vec, BWAPI::Position& my_pos, int& my_sup, int color) {
	if (my_pos != BWAPI::Positions::None) {
		BWAPI::Broodwar->drawCircleMap(my_pos, my_sup, color, false);
		BWAPI::Broodwar->drawTextMap(my_pos, "group: %d", my_sup / 2);
		//for (BWAPI::Unit u : my_vec) {
		//	if (u->exists()) { BWAPI::Broodwar->drawLineMap(u->getPosition(), my_pos, color); }
		//}
	}

}



void MapInfo::draw_unit_vec(std::vector<BWAPI::Unit> &my_list, int w, int h, int my_color) {
	for (BWAPI::Unit &my_unit : my_list) {
		draw_tile_box(my_unit->getTilePosition(), w, h, my_color);
	}
}

void MapInfo::draw_tile_vec(std::vector<BWAPI::TilePosition> &my_list, int w, int h, int my_color) {
	for (BWAPI::TilePosition &my_tile : my_list) {
		draw_tile_box(my_tile, w, h, my_color);
	}
}

void MapInfo::draw_tile_vec_index(std::vector<BWAPI::TilePosition> &my_list) {
	for (unsigned i = 0; i < my_list.size(); i++) {
		BWAPI::Broodwar->drawTextMap(BWAPI::Position(my_list.at(i)), "%d", i);
	}
}

void MapInfo::draw_tile_box(BWAPI::TilePosition &tile, int dx, int dy, int color) {
	int x1 = 32 * tile.x;
	int y1 = 32 * tile.y;
	int x2 = x1 + 32 * dx;
	int y2 = y1 + 32 * dy;
	BWAPI::Broodwar->drawBoxMap(x1, y1, x2, y2, BWAPI::Color(color), false);
	return;
}

void MapInfo::draw_tile_box_check(BWAPI::TilePosition& tile, int dx, int dy, int color) {
	if (tile != BWAPI::TilePositions::None) {
		draw_tile_box(tile, dx, dy, color);
	}
}

void MapInfo::draw_line(std::vector<BWAPI::Position> &v, int c) {
	for (unsigned i = 1; i < v.size(); i++) {
		BWAPI::Broodwar->drawLineMap(v.at(i - 1), v.at(i), BWAPI::Color(c));
	}
	return;
}

void MapInfo::draw_pos_vec(std::vector<BWAPI::Position> &v, int c) {
	for (BWAPI::Position& p : v) {
		BWAPI::Broodwar->drawDotMap(p.x, p.y, c);
	}
	return;
}

void MapInfo::draw_crosshair(BWAPI::Position p, int s, BWAPI::Color c) {
	if (p != BWAPI::Positions::None) {
		BWAPI::Broodwar->drawLineMap(p.x - s, p.y, p.x + s, p.y, c);
		BWAPI::Broodwar->drawLineMap(p.x, p.y + s, p.x, p.y - s, c);
		BWAPI::Broodwar->drawCircleMap(p, s, c, false);
	}
	return;
}

void MapInfo::display_pair_vectors(std::vector<BWAPI::Position> &v0, std::vector<BWAPI::Position> &v1) {
	if (v0.size() == v1.size()) {
		for (unsigned i = 0; i < v0.size(); i++) {
			BWAPI::Broodwar->drawLineMap(v0[i], v1[i], BWAPI::Colors::Yellow);
		}
	}
	return;
}

bool MapInfo::in_viewport(BWAPI::Position p) {
	return (in_viewport(p.x, p.y));
}
bool MapInfo::in_viewport(int x, int y) {
	if (y < py0) return false;
	if (y > py1) return false;
	if (x < px0) return false;
	if (x > px1) return false;
	return true;
}

void MapInfo::update_viewport() {
	px0 = BWAPI::Broodwar->getScreenPosition().x - 64;
	py0 = BWAPI::Broodwar->getScreenPosition().y - 64;
	px1 = BWAPI::Broodwar->getScreenPosition().x + 64 + 1280;
	py1 = BWAPI::Broodwar->getScreenPosition().y + 64 + 720;
	if (px0 < 0) px0 = 0;
	if (py0 < 0) py0 = 0;
	if (px1 > wilmap::wp - 1) { px1 = wilmap::wp - 1; }
	if (py1 > wilmap::hp - 1) { py1 = wilmap::hp - 1; }
	wx0 = px0 / 8;
	wy0 = py0 / 8;
	wx1 = px1 / 8;
	wy1 = py1 / 8;
	tx0 = px0 / 32;
	ty0 = py0 / 32;
	tx1 = px1 / 32;
	ty1 = py1 / 32;
	return;
}

void MapInfo::display_threats_all() {
	//display_threats(wilthr::siege);
	display_threats(wilthr::nuke);
	//display_threats(wilthr::lurk);
	//display_threats(wilthr::swarm);
	display_threats(wilthr::storm);
	//display_threats(wilthr::disr);
	return;
}

void MapInfo::display_threats(std::vector<Threat> &v) {
	for (Threat& thr : v) {
		BWAPI::Broodwar->drawCircleMap(thr.x0, thr.y0, 8, BWAPI::Colors::Red, false);
		BWAPI::Broodwar->drawCircleMap(thr.x0, thr.y0, 4 * (thr.size - 4), BWAPI::Colors::Red, false);
		if (thr.near_supply_enemy != 0) {
			BWAPI::Broodwar->drawTextMap(thr.x0, thr.y0 - 8, "%d", thr.near_supply_enemy / 2);
			BWAPI::Broodwar->drawTextMap(thr.x0, thr.y0, "%d", thr.near_supply_self / 2);
		}
	}
	return;
}