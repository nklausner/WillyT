#include "MapAnalysis.h"

void MapAnalysis::init() {
	set_map_dimension();
	set_start_locations();
	prepare_auxiliary_arrays();

	create_build_map();
	create_walk_map();
	create_walkpos_map();
	//create_space_map();
	create_dist_array();

	fill_map_int(wilmap::defense_map, -1);
	fill_map_int(wilmap::bunker_map, -1);
	fill_map_int(wilmap::turret_map, -1);
	return;
}
void MapAnalysis::init_advanced() {
	create_path_map();
	create_choke_map();
	create_defense_maps();
	create_rush_defense();
	create_third_defense_map();
	return;
}




void MapAnalysis::set_map_dimension() {
	using namespace wilmap;
	wt = BWAPI::Broodwar->mapWidth();
	ht = BWAPI::Broodwar->mapHeight();
	ww = 4 * wt;
	hw = 4 * ht;
	wp = 32 * wt;
	hp = 32 * ht;
	center_pos = BWAPI::Position(16 * wt, 16 * ht);
	return;
}
void MapAnalysis::set_start_locations() {
	using namespace wilmap;
	my_main_tile = BWAPI::Broodwar->self()->getStartLocation();
	my_main = BWAPI::Position{ 32 * my_main_tile.x + 64, 32 * my_main_tile.y + 48 };
	unscouted.clear();
	int i = 0;
	for (BWAPI::TilePosition t : BWAPI::Broodwar->getStartLocations()) {
		main_tiles[i] = BWAPI::TilePosition(t.x + 1, t.y + 1);
		main_pos[i] = BWAPI::Position{ 32 * t.x + 64, 32 * t.y + 48 };
		if (my_main_tile.x == t.x &&
			my_main_tile.y == t.y) {
			mm = i;
		} else {
			unscouted.push_back(BWAPI::Position{ 32 * t.x + 64, 32 * t.y + 48 });
		}
		i++;
		if (i == 8) { break; }
	}
	mn = i;
	//BWAPI::Broodwar->printf("number of start locations %d", BWAPI::Broodwar->getStartLocations().size());
	return;
}
void MapAnalysis::prepare_auxiliary_arrays() {
	create_circle_24();
	create_circle_r4();
	return;
}




void MapAnalysis::create_build_map() {
	for (int y = 0; y < wilmap::ht; ++y) {
		for (int x = 0; x < wilmap::wt; ++x) {
			wilmap::build_map_fix[y][x] = BWAPI::Broodwar->isBuildable(x, y);
			wilmap::build_map_var[y][x] = BWAPI::Broodwar->isBuildable(x, y);
		}
	}
	//BWAPI::Broodwar->printf("created build map");
	return;
}
void MapAnalysis::create_walk_map() {
	for (int y = 0; y < wilmap::ht; ++y) {
		for (int x = 0; x < wilmap::wt; ++x) {
			wilmap::walk_map[y][x] = tile_fully_walkable(x, y);
		}
	}
	//BWAPI::Broodwar->printf("created walk map");
	return;
}




void MapAnalysis::create_walkpos_map() {
	for (int y = 0; y < wilmap::hw - 4; ++y) {
		for (int x = 0; x < wilmap::ww; ++x) {
			wilmap::walkmap[y][x] = BWAPI::Broodwar->isWalkable(x, y);
			wilmap::walkmapfixed[y][x] = BWAPI::Broodwar->isWalkable(x, y);
		}
	}
	for (int y = 0; y < wilmap::hw - 5; ++y) {
		for (int x = 1; x < wilmap::ww - 1; ++x) {
			fix_walkpos_diagonal_gap(x, y, x + 1, y + 1);
			fix_walkpos_diagonal_gap(x, y, x - 1, y + 1);
		}
	}
	for (int y = 0; y < wilmap::hw - 4; ++y) {
		for (int x = 0; x < wilmap::ww - 2; ++x) {
			fix_walkpos_orthogonal_gap(x, y, 0, 1);
			fix_walkpos_orthogonal_gap(x, y, 1, 0);
		}
	}
	for (int y = 0; y < wilmap::hw - 6; ++y) {
		for (int x = 1; x < wilmap::ww - 1; ++x) {
			fix_walkpos_twobythree_gap(x, y, +1);
			fix_walkpos_twobythree_gap(x, y, -1);
		}
	}
	return;
}
void MapAnalysis::fix_walkpos_diagonal_gap(int x1, int y1, int x2, int y2) {
	if (!wilmap::walkmapfixed[y1][x1] &&
		!wilmap::walkmapfixed[y2][x2] &&
		wilmap::walkmapfixed[y1][x2] &&
		wilmap::walkmapfixed[y2][x1])
	{
		wilmap::walkmapfixed[y1][x2] = false;
		wilmap::walkmapfixed[y2][x1] = false;
	}
	return;
}
void MapAnalysis::fix_walkpos_orthogonal_gap(int x, int y, int dx, int dy) {
	if (!wilmap::walkmapfixed[y][x] &&
		!wilmap::walkmapfixed[y + 2*dy][x + 2*dx])
		wilmap::walkmapfixed[y + dy][x + dx] = false;
	return;
}
void MapAnalysis::fix_walkpos_twobythree_gap(int x, int y, int dx) {
	if (!wilmap::walkmap[y + 0][x] &&
		!wilmap::walkmap[y + 2][x + dx] &&
		wilmap::walkmap[y + 1][x] &&
		wilmap::walkmap[y + 1][x + dx])
	{
		wilmap::walkmapfixed[y + 1][x] = false;
		wilmap::walkmapfixed[y + 1][x + dx] = false;
	}
	return;
}





void MapAnalysis::create_space_map() {
	fill_map_int(wilmap::space_map, -1);
	for (int y = 0; y < wilmap::ht; ++y)
		for (int x = 0; x < wilmap::wt; ++x)
			wilmap::space_map[y][x] = check_spacing(x, y);
}
int MapAnalysis::check_spacing(int x, int y) {
	if (!wilmap::walk_map[y][x]) { return -1; }
	int i1 = 2 + 2 * check_single_line(x, y, +1, 0, 12) + 2 * check_single_line(x, y, -1, 0, 12);
	int i2 = 2 + 2 * check_single_line(x, y, 0, +1, 12) + 2 * check_single_line(x, y, 0, -1, 12);
	int i3 = 3 + 3 * check_single_line(x, y, +1, +1, 9) + 3 * check_single_line(x, y, -1, -1, 9);
	int i4 = 3 + 3 * check_single_line(x, y, -1, +1, 9) + 3 * check_single_line(x, y, +1, -1, 9);
	return std::min({ i1, i2 , i3, i4 });
}
int MapAnalysis::check_single_line(int x, int y, int dx, int dy, int m) {
	int i = 0;
	for (int n = 1; n <= m; n++) {
		if (wilmap::walk_map[y + n * dy][x + n * dx]) { i++; }
		else { break; }
	}
	return i;
}




void MapAnalysis::create_dist_array() {
	using namespace wilmap;
	for (int i = 0; i < mn; i++) {
		BWAPI::TilePosition t = main_tiles[i];
		fill_map_int(maindistarray[i], -1);
		maindistarray[i][t.y][t.x] = 0;
		create_dist_map(maindistarray[i], 1024);
	}
	fill_map_int(centerdistmap, -1);
	create_dist_init(centerdistmap, wt/2, ht/2 - 1, 32);
	create_dist_map(centerdistmap, 512);

	fill_map_int(centerdistwalkmap, -1);
	create_dist_init(centerdistwalkmap, ww/2, hw/2 - 2, 100);
	create_dist_map(centerdistwalkmap, 1024);
	//BWAPI::Broodwar->printf("created distance map");
	return;
}
void MapAnalysis::create_dist_init(int(&my_map)[256][256], int x0, int y0, int dmax) {
	using namespace wilmap;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < dmax / cdz8[i]; j++)
			if (wilmap::walk_map[y0 + cdy8[i] * j][x0 + cdx8[i] * j])
				my_map[y0 + cdy8[i] * j][x0 + cdx8[i] * j] = 0 + j * cdz8[i];
	return;
}
void MapAnalysis::create_dist_init(int(&my_map)[1024][1024], int x0, int y0, int dmax) {
	using namespace wilmap;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < dmax / cdz8[i]; j++)
			if (walkmapfixed[y0 + cdy8[i] * j][x0 + cdx8[i] * j])
				my_map[y0 + cdy8[i] * j][x0 + cdx8[i] * j] = 0 + j * cdz8[i];
	return;
}
void MapAnalysis::create_dist_map(int(&my_map)[256][256], int imax) {
	for (int i = 0; i < imax; ++i)
		for (int y = 0; y < wilmap::ht - 1; ++y)
			for (int x = 0; x < wilmap::wt; ++x)
				if (my_map[y][x] == i)
					fill_dist_map(my_map, x, y);
	return;
}
void MapAnalysis::create_dist_map(int(&my_map)[1024][1024], int imax) {
	for (int i = 0; i < imax; ++i)
		for (int y = 1; y < wilmap::hw - 5; ++y)
			for (int x = 1; x < wilmap::ww - 1; ++x)
				if (my_map[y][x] == i)
					fill_dist_map(my_map, x, y);
	return;
}
void MapAnalysis::fill_dist_map(int(&my_map)[256][256], int x, int y) {
	using namespace wilmap;
	for (int i = 0; i < 8; i++)
		set_walk_dist(my_map, x + cdx8[i], y + cdy8[i], my_map[y][x] + cdz8[i]);
	return;
}
void MapAnalysis::fill_dist_map(int(&my_map)[1024][1024], int x, int y) {
	using namespace wilmap;
	for (int i = 0; i < 8; i++)
		set_walk_dist(my_map, x + cdx8[i], y + cdy8[i], my_map[y][x] + cdz8[i]);
	return;
}
void MapAnalysis::set_walk_dist(int(&my_map)[256][256], int x, int y, int d) {
	if (y >= 0 && y < wilmap::ht &&
		x >= 0 && x < wilmap::wt &&
		wilmap::walk_map[y][x] &&
		my_map[y][x] == -1) {
		my_map[y][x] = d;
	}
	return;
}
void MapAnalysis::set_walk_dist(int(&my_map)[1024][1024], int x, int y, int d) {
	if (wilmap::walkmapfixed[y][x] &&
		my_map[y][x] == -1) {
		my_map[y][x] = d;
	}
	return;
}




void MapAnalysis::create_path_map() {
	using namespace wilmap;
	fill_map_int(mainpathmap, -1);
	fill_map_int(pathwalkmap, -1);
	//for (int i = 0; i < mn; i++) {
	//	BWAPI::TilePosition my_center = get_tile_near_center(main_tiles[i], maindistarray[i]);
	//	create_path_single(my_center, maindistarray[i]);
	//	create_path_single(natu_tiles[i], maindistarray[i]);
	//}
	//BWAPI::WalkPosition walkmain = BWAPI::WalkPosition{ main_pos[i].x / 8, main_pos[i].y / 8 };
	//BWAPI::WalkPosition walknatu = BWAPI::WalkPosition{ natu_pos[i].x / 8, natu_pos[i].y / 8 };
	//create_path_single(walkmain, distwalkmap);
	//create_path_single(walknatu, distwalkmap);
	//BWAPI::Broodwar->printf("created path map");
	return;
}




//void MapAnalysis::create_path_single(BWAPI::TilePosition t, int(&my_map)[256][256]) {
//	while (t != BWAPI::TilePositions::None) {
//		wilmap::mainpathmap[t.y][t.x] = my_map[t.y][t.x];
//		t = get_path_back(my_map, t.x, t.y);
//	}
//	return;
//}
//void MapAnalysis::create_path_single(BWAPI::WalkPosition w, int(&my_map)[1024][1024]) {
//	while (w != BWAPI::WalkPositions::None) {
//		wilmap::pathwalkmap[w.y][w.x] = my_map[w.y][w.x];
//		w = get_path_back(my_map, w.x, w.y);
//	}
//	return;
//}
//BWAPI::TilePosition MapAnalysis::get_path_back(int(&my_map)[256][256], int x, int y) {
//	using namespace wilmap;
//	for (int i = 0; i < 8; i++) {
//		int x0 = x + cdx8[i];
//		int y0 = y + cdy8[i];
//		if (my_map[y0][x0] == my_map[y][x] - cdz8[i]) {
//			return BWAPI::TilePosition(x0, y0);
//		}
//	}
//	return BWAPI::TilePositions::None;
//}
BWAPI::WalkPosition MapAnalysis::get_path_back(int(&my_map)[1024][1024], int x0, int y0) {
	BWAPI::WalkPosition walk_min = BWAPI::WalkPositions::None;
	int dist_min = 65536;
	for (int i = 0; i < 8; i++) {
		int x1 = x0 + wilmap::cdx8[i];
		int y1 = y0 + wilmap::cdy8[i];
		int dist = check_walkpath_segment(my_map, x0, y0, x1, y1);
		if (dist != -1 &&
			dist < dist_min) {
			dist_min = dist;
			walk_min = BWAPI::WalkPosition(x1, y1);
		}
	}
	return walk_min;
}
//BWAPI::TilePosition MapAnalysis::get_tile_near_center(BWAPI::TilePosition t, int(&my_map)[256][256]) {
//	int x0 = wilmap::wt / 2;
//	int y0 = wilmap::ht / 2;
//	float dx = (float)(t.x - x0) / 32;
//	float dy = (float)(t.y - y0) / 32;
//
//	for (int i = 0; i < 32; i++) {
//		int x = (int)(x0 + i * dx);
//		int y = (int)(y0 + i * dy);
//		if (my_map[y][x] != -1) {
//			return BWAPI::TilePosition(x, y);
//		}
//	}
//	return t;
//}
int MapAnalysis::check_walkpath_segment(int(&my_map)[1024][1024], int x1, int y1, int x2, int y2) {
	int imax = abs(x2 - x1);
	if (imax == 0) { imax = abs(y2 - y1); }
	if (imax == 0) { return false; }
	int dx = (x2 - x1) / imax;
	int dy = (y2 - y1) / imax;
	int z = 0;
	for (int i = 1; i <= imax; i++) {
		if (my_map[y1 + i * dy][x1 + i * dx] == -1) { return -1; }
		else { z += my_map[y1 + i * dy][x1 + i * dx]; }
	}
	return (z / imax);
}
//void MapAnalysis::set_walkpath_segment(int(&my_map)[1024][1024], int x1, int y1, int x2, int y2, int z) {
//	int imax = abs(x2 - x1);
//	if (imax == 0) { imax = abs(y2 - y1); }
//	if (imax == 0) { return; }
//	int dx = (x2 - x1) / imax;
//	int dy = (y2 - y1) / imax;
//	for (int i = 1; i <= imax; i++) {
//		my_map[y1 + i * dy][x1 + i * dx] = z;
//	}
//	return;
//}




void MapAnalysis::create_choke_map() {
	using namespace wilmap;
	clear_map_bool(choke_map);
	clear_map_bool(chokewalkmap);
	set_map_specific_parameters();
	for (int i = 0; i < mn; i++)
	{
		create_choke_single(i, main_pos, main_choke_pos, main_choke_edge, max_choke_search_main);
		create_choke_single(i, natu_pos, natu_choke_pos, natu_choke_edge, max_choke_search_natu);
		set_map_specific_handling(main_choke_pos[i], natu_choke_pos[i]);
		handle_map_specific_additional_choke(i, natu_pos[i], natu_choke_pos[i]);

		//only in case choke analysis failed
		if (main_choke_pos[i] == BWAPI::Positions::None)
			main_choke_pos[i] = linear_interpol_rel(main_pos[i], natu_pos[i], 0.7f);
		if (natu_choke_pos[i] == BWAPI::Positions::None)
			natu_choke_pos[i] = linear_interpol_abs(natu_pos[i], center_pos, 192);
	}
	transfer_choke_map_walk_tile();
	return;
}
void MapAnalysis::create_choke_single(int i, BWAPI::Position(&base_pos)[8], BWAPI::Position(&choke_pos)[8], BWAPI::WalkPosition(&choke_edge)[16], int imax) {
	using namespace wilmap;
	//imax = get_map_specific_parameter(base_pos, imax);
	std::vector<BWAPI::WalkPosition> vm = create_walkpath(centerdistwalkmap, base_pos[i], imax);
	std::vector<BWAPI::WalkPosition> vr = create_walkpath_side(vm, +1);
	std::vector<BWAPI::WalkPosition> vl = create_walkpath_side(vm, -1);
	fill_walkpos(pathwalkmap, vm);
	fill_walkpos(pathwalkmap, vr);
	fill_walkpos(pathwalkmap, vl);

	std::vector<BWAPI::WalkPosition> vc = {};
	vc.reserve(26);
	int rmin = 0;
	int lmin = 0;
	find_minimum_walkwidth(vr, vl, rmin, lmin);
	find_choke_edge(vc, vr, vl, rmin, lmin);
	vc = outline_choke_area(vc);
	BWAPI::WalkPosition choke_walk = find_choke_center(vc);
	fill_walkpos(chokewalkmap, vc);
	flood_fill_choke(chokewalkmap, choke_walk.x, choke_walk.y);

	choke_pos[i] = (BWAPI::Position)choke_walk;
	choke_edge[2 * i] = vr[rmin];
	choke_edge[2 * i + 1] = vl[lmin];
	return;
}
void MapAnalysis::find_additional_choke(int i, BWAPI::Position& base_pos, BWAPI::Position& choke_pos, int imax) {
	using namespace wilmap;
	std::vector<BWAPI::WalkPosition> vm = create_walkpath(centerdistwalkmap, base_pos, imax);
	std::vector<BWAPI::WalkPosition> vr = create_walkpath_side(vm, +1);
	std::vector<BWAPI::WalkPosition> vl = create_walkpath_side(vm, -1);
	fill_walkpos(pathwalkmap, vm);

	std::vector<BWAPI::WalkPosition> vc = {};
	vc.reserve(26);
	int rmin = 0;
	int lmin = 0;
	find_minimum_walkwidth(vr, vl, rmin, lmin);
	find_choke_edge(vc, vr, vl, rmin, lmin);
	vc = outline_choke_area(vc);
	BWAPI::WalkPosition choke_walk = find_choke_center(vc);
	fill_walkpos(chokewalkmap, vc);
	flood_fill_choke(chokewalkmap, choke_walk.x, choke_walk.y);

	auxiliary_natu_choke_dist = maindistarray[i][choke_walk.y / 4][choke_walk.x / 4];
	choke_pos.x = (choke_pos.x + choke_walk.x * 8) / 2;
	choke_pos.y = (choke_pos.y + choke_walk.y * 8) / 2;
	return;
}




std::vector<BWAPI::WalkPosition> MapAnalysis::create_walkpath(int(&my_map)[1024][1024], BWAPI::Position p, int imax) {
	std::vector<BWAPI::WalkPosition> new_vector = {};
	BWAPI::WalkPosition w = (BWAPI::WalkPosition) p;
	int i = 0;
	while (w != BWAPI::WalkPositions::None && i < imax) {
		//wilmap::pathwalkmap[w.y][w.x] = 0;
		new_vector.push_back(w);
		w = get_path_back(my_map, w.x, w.y);
		i++;
	}
	return new_vector;
}
std::vector<BWAPI::WalkPosition> MapAnalysis::create_walkpath_side(std::vector<BWAPI::WalkPosition>& my_vector, int side) {
	std::vector<BWAPI::WalkPosition> new_vector = {};
	int imin = get_map_specific_first_step();
	int wpsr = get_map_specific_side_range();
	for (unsigned i = imin; i < my_vector.size(); i++) {
		BWAPI::WalkPosition w0 = my_vector[i-1];
		BWAPI::WalkPosition w1 = my_vector[i];
		int dx = side * (w1.y - w0.y);
		int dy = side * (w0.x - w1.x);
		if (dx == 0 && dy == 0) { continue; }
		int di = get_maximum_shift(w1.x, w1.y, dx, dy, wpsr);
		if (di != -1) {
			new_vector.push_back(BWAPI::WalkPosition(w1.x + di * dx, w1.y + di * dy));
		}
	}
	return new_vector;
}
int MapAnalysis::get_maximum_shift(int x, int y, int dx, int dy, int imax) {
	int i = 0;
	if (dx != 0 && dy != 0) { imax = 32; }
	while (i < imax &&
		y + i * dy > 0 &&
		x + i * dx > 0 &&
		wilmap::walkmapfixed[y + i * dy][x + i * dx]) {
		i++;
	}
	if (i < imax) { return (i - 1); }
	return -1;
}
void MapAnalysis::find_minimum_walkwidth(std::vector<BWAPI::WalkPosition>& vr, std::vector<BWAPI::WalkPosition>& vl, int& rmin, int& lmin) {
	int min_sqdist = 4096;
	for (unsigned r = 0; r < vr.size(); r++) {
		for (unsigned l = 0; l < vl.size(); l++) {
			if (min_sqdist > sqdist(vr[r], vl[l])) {
				min_sqdist = sqdist(vr[r], vl[l]);
				rmin = r;
				lmin = l;
			}
		}
	}
	return;
}
void MapAnalysis::find_choke_edge(std::vector<BWAPI::WalkPosition>& vc, std::vector<BWAPI::WalkPosition>& vr, std::vector<BWAPI::WalkPosition>& vl, int& rmin, int& lmin) {
	int min_dist = dist(vr[rmin], vl[lmin]);
	for (int di = -6; di <= +6; di++) {
		if (rmin + di >= 0 && rmin + di < (int)vr.size() &&
			lmin + di >= 0 && lmin + di < (int)vl.size() &&
			dist(vr[rmin + di], vl[lmin + di]) <= min_dist + 6) {
			vc.insert(vc.begin(), vr[rmin + di]);
			vc.push_back(vl[lmin + di]);
		}
	}
	//BWAPI::Broodwar->printf("choke at %d, %d, width %d", rmin, lmin, min_dist);
	return;
}
std::vector<BWAPI::WalkPosition> MapAnalysis::outline_choke_area(std::vector<BWAPI::WalkPosition> old_vector) {
	std::vector<BWAPI::WalkPosition> new_vector = {};
	for (unsigned i = 0; i < old_vector.size(); i++) {
		unsigned j = (i + 1) % old_vector.size();
		new_vector.push_back(old_vector[i]);
		for (BWAPI::WalkPosition w : create_linear_interpolation(old_vector[i], old_vector[j])) {
			new_vector.push_back(w);
		}
	}
	return new_vector;
}
BWAPI::WalkPosition MapAnalysis::find_choke_center(std::vector<BWAPI::WalkPosition>& my_vec) {
	if (my_vec.empty()) { return BWAPI::WalkPositions::None; }
	int x = 0; int y = 0;
	for (BWAPI::WalkPosition w : my_vec) { x += w.x; y += w.y; }
	return BWAPI::WalkPosition(x / my_vec.size() + 1, y / my_vec.size() + 1);
}
void MapAnalysis::fill_walkpos(int(&my_map)[1024][1024], std::vector<BWAPI::WalkPosition>& my_vector) {
	int i = 0;
	for (BWAPI::WalkPosition w : my_vector) {
		my_map[w.y][w.x] = i;
		i++;
	}
	return;
}
void MapAnalysis::fill_walkpos(bool(&my_map)[1024][1024], std::vector<BWAPI::WalkPosition>& my_vector) {
	for (BWAPI::WalkPosition w : my_vector)
		my_map[w.y][w.x] = true;
	return;
}
void MapAnalysis::flood_fill_choke(bool(&my_map)[1024][1024], int x, int y) {
	if (my_map[y][x] == false) {
		my_map[y][x] = true;
		flood_fill_choke(my_map, x + 1, y);
		flood_fill_choke(my_map, x, y + 1);
		flood_fill_choke(my_map, x - 1, y);
		flood_fill_choke(my_map, x, y - 1);
	}
}
void MapAnalysis::transfer_choke_map_walk_tile() {
	for (int y = 0; y < wilmap::hw; y++)
		for (int x = 0; x < wilmap::ww; x++)
			if (wilmap::chokewalkmap[y][x] &&
				wilmap::walk_map[y/4][x/4])
				wilmap::choke_map[y/4][x/4] = true;
}




void MapAnalysis::set_map_specific_parameters() {
	if (BWAPI::Broodwar->mapHash() == "d2f5633cc4bb0fca13cd1250729d5530c82c7451") {
		max_choke_search_natu = 36;
		//BWAPI::Broodwar->printf("using specific parameters for map: Fighting Spirit");
	}
	if (BWAPI::Broodwar->mapHash() == "0a41f144c6134a2204f3d47d57cf2afcd8430841") {
		max_choke_search_natu = 36;
		//BWAPI::Broodwar->printf("using specific parameters for map: MatchPoint");
	}
	return;
}
int MapAnalysis::get_map_specific_parameter(BWAPI::Position base_pos, int imax) {
	//if (BWAPI::Broodwar->mapHash() == "d16719e736252d77fdbb0d8405f7879f564bfe56" &&
	//	base_pos.x == 1024 && base_pos.y == 3792) {
	//	BWAPI::Broodwar->printf("using specific parameters for map: Longinus2");
	//	BWAPI::Broodwar->printf("base at: %d, %d", base_pos.x, base_pos.y);
	//	return 72;
	//}
	return imax;
}
int MapAnalysis::get_map_specific_first_step() {
	if (BWAPI::Broodwar->mapHash() == "1e983eb6bcfa02ef7d75bd572cb59ad3aab49285") {
		return 21;
		//BWAPI::Broodwar->printf("using specific parameters for map: Andromeda");
	}
	return 11;
}
int MapAnalysis::get_map_specific_side_range() {
	if (BWAPI::Broodwar->mapHash() == "0a41f144c6134a2204f3d47d57cf2afcd8430841") {
		return 48;
		//BWAPI::Broodwar->printf("using specific parameters for map: MatchPoint");
	}
	return 36;
}
void MapAnalysis::set_map_specific_handling(BWAPI::Position& mcp, BWAPI::Position& ncp) {
	if (BWAPI::Broodwar->mapHash() == "130c43f080bf497b946b6b9118eb4cba594a35ac") {
		ncp = mcp;
		//BWAPI::Broodwar->printf("set specific handling for map: Colosseum");
	}
	return;

}
int MapAnalysis::get_map_specific_defense_area_ymin(int ymin, int ymax) {
	if (BWAPI::Broodwar->mapHash() == "af618ea3ed8a8926ca7b17619eebcb9126f0d8b1" &&
		(ymin + ymax) / 2 == 64) {
		//BWAPI::Broodwar->printf("set specific ymin for: Benzene bot-left");
		return 65;
	}
	return ymin;
}
int MapAnalysis::get_map_specific_defense_area_ymax(int ymin, int ymax) {
	if (BWAPI::Broodwar->mapHash() == "af618ea3ed8a8926ca7b17619eebcb9126f0d8b1" &&
		(ymin + ymax) / 2 == 48) {
		//BWAPI::Broodwar->printf("set specific ymax for: Benzene top-right");
		return 47;
	}
	return ymax;
}
void MapAnalysis::handle_map_specific_additional_choke(int i, BWAPI::Position& base_pos, BWAPI::Position& choke_pos) {
	if (BWAPI::Broodwar->mapHash() == "4e24f217d2fe4dbfa6799bc57f74d8dc939d425b") {
		int z = 0;
		if (base_pos.x < wilmap::wp / 2) { z = +1; }
		if (base_pos.x > wilmap::wp / 2) { z = -1; }
		int x = base_pos.x + z * 256;
		int y = base_pos.y - z * 128;
		find_additional_choke(i, BWAPI::Position(x, y), choke_pos, 24);
		//BWAPI::Broodwar->printf("add specific choke for: Destination");
	}
	return;
}




void MapAnalysis::create_defense_maps() {
	using namespace wilmap;
	clear_map_bool(maindefmap);
	clear_map_bool(natudefmap);
	my_maindefvec.clear();
	my_natudefvec.clear();
	for (int i = 0; i < mn; i++) {
		if (i == mm) {
			create_defense_area(main_choke_pos[i], maindistarray[i], maindefmap, my_maindefvec);
			create_defense_area(natu_choke_pos[i], maindistarray[i], natudefmap, my_natudefvec);
			select_highground_defense();
		} else {
			std::vector<BWAPI::TilePosition> my_vec;
			create_defense_area(main_choke_pos[i], maindistarray[i], maindefmap, my_vec);
			create_defense_area(natu_choke_pos[i], maindistarray[i], natudefmap, my_vec);
			my_vec.clear();
		}
	}
	//BWAPI::Broodwar->printf("defense main %d, natu %d", my_maindefvec.size(), my_natudefvec.size());
	return;
}
void MapAnalysis::create_defense_area(BWAPI::Position mychokepos, int(&mydistmap)[256][256], bool(&mydefmap)[256][256], std::vector<BWAPI::TilePosition>& mydefvec) {
	using namespace wilmap;
	int xc = mychokepos.x / 32;
	int yc = mychokepos.y / 32;
	int mychokedist = mydistmap[yc][xc];
	if (mychokedist == -1) {
		mychokedist = find_alternative_choke_tile_dist(xc, yc, mydistmap);
	}
	if (mychokedist == -1) {
		BWAPI::Broodwar->printf("couldnt find choke distance for %d, %d", xc, yc);
		return;
	}

	int xmin = mapsafesub(xc, 12);
	int xmax = mapsafeadd(xc, 12, wt);
	int ymin = mapsafesub(yc, 12);
	int ymax = mapsafeadd(yc, 12, ht);
	ymin = get_map_specific_defense_area_ymin(ymin, ymax);
	ymax = get_map_specific_defense_area_ymax(ymin, ymax);

	for (int y = ymin; y < ymax; y++) {
		for (int x = xmin; x < xmax; x++) {
			if (choke_map[y][x] == false &&
				mydistmap[y][x] != -1 &&
				mydistmap[y][x] < mychokedist &&
				sqdist(x, y, xc, yc) <= 144 &&
				sqdist(x, y, xc, yc) >= 16) {
				mydefmap[y][x] = true;
				mydefvec.push_back(BWAPI::TilePosition(x, y));
			}
		}
	}
	return;
}
void MapAnalysis::select_highground_defense() {
	for (BWAPI::TilePosition tile : wilmap::my_natudefvec) {
		if (wilmap::main_map_orig[tile.y][tile.x] &&
			wilmap::build_map_var[tile.y][tile.x]) {
			wilmap::my_highdefvec.push_back(tile);
		}
	}
	return;
}
int MapAnalysis::find_alternative_choke_tile_dist(int xc, int yc, int(&mydistmap)[256][256]) {
	if (auxiliary_natu_choke_dist != -1) {
		return auxiliary_natu_choke_dist;
	}
	for (unsigned i = 0; i < 8; i++) {
		int x = xc + wilmap::cdx8[i];
		int y = yc + wilmap::cdy8[i];
		if (wilmap::chokewalkmap[4 * y + 1][4 * x + 1] &&
			wilmap::chokewalkmap[4 * y + 2][4 * x + 2] &&
			wilmap::chokewalkmap[4 * y + 2][4 * x + 1] &&
			wilmap::chokewalkmap[4 * y + 1][4 * x + 2] &&
			mydistmap[y][x] != -1) {
			return mydistmap[y][x];
		}
	}
	return -1;
}
void MapAnalysis::create_rush_defense() {
	using namespace wilmap;
	for (int i = 0; i < mn; i++) {
		BWAPI::Position p1 = linear_interpol_abs(main_pos[i], center_pos, 128);
		BWAPI::Position p2 = linear_interpol_abs(main_pos[i], natu_pos[i], 128);
		rush_def_tile[i] = BWAPI::TilePosition( (p1.x + p2.x) / 64 - 1 , (p1.y + p2.y) / 64 - 1 );
	}
	return;
}
void MapAnalysis::create_third_defense_map() {
	//find expansions that are close enough to be sieged from the main
	using namespace wilmap;
	clear_map_bool(thirddefmap);
	my_thirddefvec.clear();
	for (int i = 0; i < mn; i++) {
		for (Expo &expo : wilexpo::all) {
			if (sqdist(main_tiles[i], expo.tile) < 2000 &&
				maindistarray[i][expo.tile.y][expo.tile.x] > 100) {
				fill_third_defense_map(i, expo.tile.x, expo.tile.y);
			}
		}
	}
	return;
}
void MapAnalysis::fill_third_defense_map(int i, int x0, int y0) {
	x0++;
	y0++;
	int x1 = mapsafesub(x0, 11);
	int y1 = mapsafesub(y0, 11);
	int x2 = mapsafeadd(x0, 13, wilmap::wt);
	int y2 = mapsafeadd(y0, 12, wilmap::ht);
	for (int y = y1; y < y2; y++) {
		for (int x = x1; x < x2; x++) {
			if (wilmap::maindistarray[i][y][x] > 0 &&
				wilmap::maindistarray[i][y][x] < 60 &&
				sqdist(x0, y0, x, y) < 200) {
				wilmap::thirddefmap[y][x] = true;
				wilmap::my_thirddefvec.push_back(BWAPI::TilePosition(x, y));
			}
		}
	}
	return;
}



void MapAnalysis::create_grid_image() {
	const char* my_path = "K:\\bwapi\\map_grid.pgm";
	/*FILE *my_file = fopen(my_path, "wb");
	fprintf(my_file, "P2 %d %d 2", willyt::w, willyt::h);

	for (int y = 0; y < willyt::h; ++y) {
		fprintf(my_file, "\n");
		for (int x = 0; x < willyt::w; ++x) {
			char* z = "0 ";
			if (walk_grid[y][x]) { z = "1 "; }
			if (build_grid[y][x]) { z = "2 "; }
			fprintf(my_file, z);
		}
	}
	fclose(my_file);*/
	BWAPI::Broodwar->printf(my_path);
	return;
}
void MapAnalysis::create_dist_image() {
	const char* my_path = "K:\\bwapi\\map_dist.pgm";
	/*FILE *my_file = fopen(my_path, "wb");
	fprintf(my_file, "P2 %d %d 511", willyt::w, willyt::h);

	for (int y = 0; y < willyt::h; ++y) {
		fprintf(my_file, "\n");
		for (int x = 0; x < willyt::w; ++x) {
			int z = dist_grid[y][x];
			if (z == -1) { fprintf(my_file, "0 "); }
			else { fprintf(my_file, "%d ", 511 - z); }
		}
	}
	fclose(my_file);*/
	BWAPI::Broodwar->printf(my_path);
	return;
}




//BWAPI::TilePosition tcm = find_choke_tile(maindistarray[i], natu_tiles[i], 0, 256);
//main_choke_pos[i] = find_choke_center(tcm);
//BWAPI::TilePosition tcn = find_choke_tile(centerdistmap, natu_tiles[i], 0, 12);
//natu_choke_pos[i] = find_choke_center(tcn);
//BWAPI::TilePosition MapAnalysis::find_choke_tile(int(&my_map)[256][256], BWAPI::TilePosition t, int imin, int imax) {
//	BWAPI::TilePosition min_tile = t;
//	int min_space = 1024;
//	int i = 0;
//	while (t != BWAPI::TilePositions::None && i < imax) {
//		//wilmap::mainpathmap[t.y][t.x] = my_map[t.y][t.x];
//		if (min_space > wilmap::space_map[t.y][t.x]) {
//			min_space = wilmap::space_map[t.y][t.x];
//			min_tile = t;
//		}
//		t = get_path_back(my_map, t.x, t.y);
//		++i;
//	}
//	//BWAPI::Broodwar->printf("find choke %d", min_space);
//	return min_tile;
//}
//BWAPI::Position MapAnalysis::find_choke_center(BWAPI::TilePosition t) {
//	std::vector<int> vx;
//	std::vector<int> vy;
//	int i = 0;
//	flood_fill_choke(t.x, t.y, wilmap::space_map[t.y][t.x] + 3, vx, vy, i);
//	int xc = 0;
//	int yc = 0;
//	for (int x : vx) xc += x;
//	for (int y : vy) yc += y;
//
//	if (vx.empty() || vy.empty()) return BWAPI::Positions::None;
//	return BWAPI::Position(16 + 32 * xc / vx.size(), 16 + 32 * yc / vy.size());
//}
//void MapAnalysis::flood_fill_choke(int x, int y, int z, std::vector<int> &vx, std::vector<int> &vy, int& i) {
//	if (i < 16 &&
//		wilmap::space_map[y][x] != -1 &&
//		wilmap::space_map[y][x] <= z &&
//		wilmap::choke_map[y][x] == false) {
//		wilmap::choke_map[y][x] = true;
//		vx.push_back(x);
//		vy.push_back(y);
//		i++;
//		flood_fill_choke(x + 1, y, z, vx, vy, i);
//		flood_fill_choke(x, y + 1, z, vx, vy, i);
//		flood_fill_choke(x - 1, y, z, vx, vy, i);
//		flood_fill_choke(x, y - 1, z, vx, vy, i);
//	}
//}

