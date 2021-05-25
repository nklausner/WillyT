#include "MapAnalysis.h"

void MapAnalysis::init() {
	set_map_dimension();
	set_start_locations();

	create_build_map();
	create_walk_map();
	create_walkpos_map();
	create_space_map();
	create_dist_array();

	fill_map_int(wilmap::def_map, -1);
	fill_map_int(wilmap::bunker_map, -1);
	fill_map_int(wilmap::turret_map, -1);
}

void MapAnalysis::init_advanced() {
	create_path_map();
	create_choke_map();
	create_defense_maps();
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
}

void MapAnalysis::set_start_locations() {
	using namespace wilmap;
	my_start = BWAPI::Broodwar->self()->getStartLocation();
	my_main = BWAPI::Position{ 32 * my_start.x + 64, 32 * my_start.y + 48 };
	unscouted.clear();
	int i = 0;
	for (BWAPI::TilePosition t : BWAPI::Broodwar->getStartLocations()) {
		main_tiles[i] = BWAPI::TilePosition(t.x + 1, t.y + 1);
		main_pos[i] = BWAPI::Position{ 32 * t.x + 64, 32 * t.y + 48 };
		if (my_start.x == t.x &&
			my_start.y == t.y) {
			mm = i;
		} else {
			unscouted.push_back(BWAPI::Position{ 32 * t.x + 64, 32 * t.y + 48 });
		}
		i++;
		if (i == 8) { break; }
	}
	mn = i;
}



void MapAnalysis::create_build_map() {
	for (int y = 0; y < wilmap::ht; ++y)
		for (int x = 0; x < wilmap::wt; ++x)
			wilmap::build_map[y][x] = BWAPI::Broodwar->isBuildable(x, y);
	//BWAPI::Broodwar->printf("created build map");
}

void MapAnalysis::create_walk_map() {
	for (int y = 0; y < wilmap::ht; ++y)
		for (int x = 0; x < wilmap::wt; ++x)
			wilmap::walk_map[y][x] = tile_is_walkable(x, y);
	//BWAPI::Broodwar->printf("created walk map");
}

bool MapAnalysis::tile_is_walkable(int x, int y) {
	int z = 0;
	if (BWAPI::Broodwar->isWalkable(4 * x + 0, 4 * y + 0)) { ++z; }
	if (BWAPI::Broodwar->isWalkable(4 * x + 3, 4 * y + 0)) { ++z; }
	if (BWAPI::Broodwar->isWalkable(4 * x + 0, 4 * y + 3)) { ++z; }
	if (BWAPI::Broodwar->isWalkable(4 * x + 3, 4 * y + 3)) { ++z; }
	if (z == 4) { return true; }
	else { return false; }
}

void MapAnalysis::create_walkpos_map() {
	for (int y = 0; y < wilmap::hw - 4; ++y) {
		for (int x = 0; x < wilmap::ww; ++x) {
			wilmap::walkmapfixed[y][x] = BWAPI::Broodwar->isWalkable(x, y);
		}
	}
	for (int y = 0; y < wilmap::hw - 5; ++y) {
		for (int x = 1; x < wilmap::ww - 1; ++x) {
			fix_walkpos_diagonal_hole(x, y, x + 1, y + 1);
			fix_walkpos_diagonal_hole(x, y, x - 1, y + 1);
		}
	}
	return;
}

void MapAnalysis::fix_walkpos_diagonal_hole(int x1, int y1, int x2, int y2) {
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

void MapAnalysis::create_path_single(BWAPI::TilePosition t, int(&my_map)[256][256]) {
	while (t != BWAPI::TilePositions::None) {
		wilmap::mainpathmap[t.y][t.x] = my_map[t.y][t.x];
		t = get_path_back(my_map, t.x, t.y);
	}
	return;
}

void MapAnalysis::create_path_single(BWAPI::WalkPosition w, int(&my_map)[1024][1024]) {
	while (w != BWAPI::WalkPositions::None) {
		wilmap::pathwalkmap[w.y][w.x] = my_map[w.y][w.x];
		w = get_path_back(my_map, w.x, w.y);
	}
	return;
}

BWAPI::TilePosition MapAnalysis::get_path_back(int(&my_map)[256][256], int x, int y) {
	using namespace wilmap;
	for (int i = 0; i < 8; i++) {
		int x0 = x + cdx8[i];
		int y0 = y + cdy8[i];
		if (my_map[y0][x0] == my_map[y][x] - cdz8[i]) {
			return BWAPI::TilePosition(x0, y0);
		}
	}
	return BWAPI::TilePositions::None;
}

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

BWAPI::TilePosition MapAnalysis::get_tile_near_center(BWAPI::TilePosition t, int(&my_map)[256][256]) {
	int x0 = wilmap::wt / 2;
	int y0 = wilmap::ht / 2;
	float dx = (float)(t.x - x0) / 32;
	float dy = (float)(t.y - y0) / 32;

	for (int i = 0; i < 32; i++) {
		int x = (int)(x0 + i * dx);
		int y = (int)(y0 + i * dy);
		if (my_map[y][x] != -1) {
			return BWAPI::TilePosition(x, y);
		}
	}
	return t;
}

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

	for (int i = 0; i < mn; i++)
	{
		create_choke_single(main_pos[i], 128);
		create_choke_single(natu_pos[i], 48);

		BWAPI::TilePosition tcm = find_choke_tile(maindistarray[i], natu_tiles[i], 0, 256);
		main_choke_pos[i] = find_choke_center(tcm);

		BWAPI::TilePosition tcn = find_choke_tile(centerdistmap, natu_tiles[i], 0, 12);
		natu_choke_pos[i] = find_choke_center(tcn);
	}
}
void MapAnalysis::create_choke_single(BWAPI::Position my_pos, int imax) {
	using namespace wilmap;
	std::vector<BWAPI::WalkPosition> vm = create_walkpath(centerdistwalkmap, my_pos, imax);
	//fill_walkpos(pathwalkmap, vm);

	std::vector<BWAPI::WalkPosition> vr = create_walkpath_side(vm, +1);
	std::vector<BWAPI::WalkPosition> vl = create_walkpath_side(vm, -1);
	//fill_walkpos(pathwalkmap, vr);
	//fill_walkpos(pathwalkmap, vl);

	std::vector<BWAPI::WalkPosition> vc = find_minimum_walkwidth(vr, vl);
	fill_walkpos(chokewalkmap, vc);


	//BWAPI::Broodwar->printf("%d", vm.size());
	//for (int i = 0; i < 6; i++) {
	//	vm = shift_walkpath_mid(vm);
	//	vm = fill_walkpath_gaps(vm);
	//	vm = cut_walkpath_corners(vm);
	//	vm = cut_walkpath_bulge_diagonal(vm);
	//	cut_walkpath_bulge_orthogon(vm);
	//}
	//vm = cut_walkpath_corners(vm);
	//fill_walkpos(pathwalkmap, vm);


	//std::vector<BWAPI::WalkPosition> vcm = find_choke_walkspace(centerdistwalkmap, my_pos, imax);
	//find_choke_center(vcm);
	return;
}




BWAPI::TilePosition MapAnalysis::find_choke_tile(int(&my_map)[256][256], BWAPI::TilePosition t, int imin, int imax) {
	BWAPI::TilePosition min_tile = t;
	int min_space = 1024;
	int i = 0;
	while (t != BWAPI::TilePositions::None && i < imax) {
		//wilmap::mainpathmap[t.y][t.x] = my_map[t.y][t.x];
		if (min_space > wilmap::space_map[t.y][t.x]) {
			min_space = wilmap::space_map[t.y][t.x];
			min_tile = t;
		}
		t = get_path_back(my_map, t.x, t.y);
		++i;
	}
	//BWAPI::Broodwar->printf("find choke %d", min_space);
	return min_tile;
}
BWAPI::Position MapAnalysis::find_choke_center(BWAPI::TilePosition t) {
	std::vector<int> vx;
	std::vector<int> vy;
	int i = 0;
	flood_fill_choke(t.x, t.y, wilmap::space_map[t.y][t.x] + 3, vx, vy, i);
	int xc = 0;
	int yc = 0;
	for (int x : vx) xc += x;
	for (int y : vy) yc += y;

	if (vx.empty() || vy.empty()) return BWAPI::Positions::None;
	return BWAPI::Position(16 + 32 * xc / vx.size(), 16 + 32 * yc / vy.size());
}
void MapAnalysis::flood_fill_choke(int x, int y, int z, std::vector<int> &vx, std::vector<int> &vy, int& i) {
	if (i < 16 &&
		wilmap::space_map[y][x] != -1 &&
		wilmap::space_map[y][x] <= z &&
		wilmap::choke_map[y][x] == false) {
		wilmap::choke_map[y][x] = true;
		vx.push_back(x);
		vy.push_back(y);
		i++;
		flood_fill_choke(x + 1, y, z, vx, vy, i);
		flood_fill_choke(x, y + 1, z, vx, vy, i);
		flood_fill_choke(x - 1, y, z, vx, vy, i);
		flood_fill_choke(x, y - 1, z, vx, vy, i);
	}
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
	for (unsigned i = 1; i < my_vector.size(); i++) {
		BWAPI::WalkPosition w0 = my_vector[i-1];
		BWAPI::WalkPosition w1 = my_vector[i];
		int dx = side * (w1.y - w0.y);
		int dy = side * (w0.x - w1.x);
		if (dx == 0 && dy == 0) { continue; }
		int di = get_maximum_shift(w1.x, w1.y, dx, dy);
		if (di != -1) {
			new_vector.push_back(BWAPI::WalkPosition(w1.x + di * dx, w1.y + di * dy));
		}
	}
	return new_vector;
}
std::vector<BWAPI::WalkPosition> MapAnalysis::shift_walkpath_mid(std::vector<BWAPI::WalkPosition>& old_vector) {
	std::vector<BWAPI::WalkPosition> new_vector = { old_vector.front() };
	bool shift_right = false;
	bool shift_left = false;
	for (unsigned i = 1; i < old_vector.size(); i++) {
		BWAPI::WalkPosition w0 = old_vector[i-1];
		BWAPI::WalkPosition w1 = old_vector[i];
		BWAPI::WalkPosition wn = old_vector[i];
		int dx = w1.y - w0.y;
		int dy = w0.x - w1.x;
		int dr = get_maximum_shift(w1.x, w1.y, dx, dy);
		int dl = get_maximum_shift(w1.x, w1.y, -dx, -dy);
		if (dr > dl + 1) {
			if (!shift_left) {
				wn = BWAPI::WalkPosition(w1.x + dx, w1.y + dy);
				shift_right = true;
			}
			shift_left = false;
		}
		else if (dl > dr + 1) {
			if (!shift_right) {
				wn = BWAPI::WalkPosition(w1.x - dx, w1.y - dy);
				shift_left = true;
			}
			shift_right = false;
		}
		else {
			shift_right = false;
			shift_left = false;
		}
		new_vector.push_back(wn);
	}
	//BWAPI::Broodwar->printf("Old: %d -- New: %d", old_vector.size(), new_vector.size());
	return new_vector;
}
int MapAnalysis::get_maximum_shift(int x, int y, int dx, int dy) {
	int i = 0;
	int imax = 36;
	if (dx != 0 && dy != 0) { imax = 24; }
	while (i < imax &&
		y + i * dy > 0 &&
		x + i * dx > 0 &&
		wilmap::walkmapfixed[y + i * dy][x + i * dx]) {
		i++;
	}
	if (i < imax) { return (i - 1); }
	return -1;
}
std::vector<BWAPI::WalkPosition> MapAnalysis::find_minimum_walkwidth(std::vector<BWAPI::WalkPosition>& vr, std::vector<BWAPI::WalkPosition>& vl) {
	std::vector<BWAPI::WalkPosition> new_vector = {};

	int min_sqdist = 4096;
	int rmin = 0;
	int lmin = 0;
	for (unsigned r = 0; r < vr.size(); r++) {
		for (unsigned l = 0; l < vl.size(); l++) {
			if (min_sqdist > sqdist(vr[r], vl[l])) {
				min_sqdist = sqdist(vr[r], vl[l]);
				rmin = r;
				lmin = l;
			}
		}
	}
	int min_dist = dist(vr[rmin], vl[lmin]);
	BWAPI::Broodwar->printf("found choke with %d walkspace", min_dist);

	for (int di = -6; di <= +6; di++) {
		if (rmin + di >= 0 && rmin + di < (int)vr.size() &&
			lmin + di >= 0 && lmin + di < (int)vl.size() &&
			dist(vr[rmin + di], vl[lmin + di]) <= min_dist + 6) {
			new_vector.push_back(vr[rmin + di]);
			new_vector.push_back(vl[lmin + di]);
		}
	}
	return new_vector;
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
std::vector<BWAPI::WalkPosition> MapAnalysis::fill_walkpath_gaps(std::vector<BWAPI::WalkPosition>& old_vector) {
	std::vector<BWAPI::WalkPosition> new_vector = {};
	new_vector.push_back(old_vector.front());

	for (unsigned i = 0; i < old_vector.size() - 1; i++) {
		int x1 = old_vector[i].x;
		int y1 = old_vector[i].y;
		int x2 = old_vector[i+1].x;
		int y2 = old_vector[i+1].y;
		int dx = x2 - x1;
		int dy = y2 - y1;

		if (abs(dx) <= 1 && abs(dy) <= 1) {							//no gap here
			new_vector.push_back(BWAPI::WalkPosition(x2, y2));
			continue;
		}
		if (abs(dx) == 2 && dy == 0) {								//simple horizontal gap
			new_vector.push_back(BWAPI::WalkPosition(x1 + dx / 2 , y1));
			new_vector.push_back(BWAPI::WalkPosition(x2, y2));
			continue;
		}
		if (abs(dy) == 2 && dx == 0) {								//simple vertical gap
			new_vector.push_back(BWAPI::WalkPosition(x1, y1 + dy / 2));
			new_vector.push_back(BWAPI::WalkPosition(x2, y2));
			continue;
		}
		if (abs(dy) == 2 && abs(dx) == 2) {							//simple diagonal gap
			new_vector.push_back(BWAPI::WalkPosition(x1 + dx / 2, y1 + dy / 2));
			new_vector.push_back(BWAPI::WalkPosition(x2, y2));
			continue;
		}
		if (i >= 1 && i < old_vector.size() - 2) {
			int x1e = 2 * x1 - old_vector[i - 1].x;
			int y1e = 2 * y1 - old_vector[i - 1].y;
			if (abs(x2 - x1e) <= 1 && abs(y2 - y1e) <= 1) {			//extrapolated from side 1
				new_vector.push_back(BWAPI::WalkPosition(x1e, y1e));
				new_vector.push_back(BWAPI::WalkPosition(x2, y2));
				continue;
			}
			int x2e = old_vector[i + 2].x - 2 * x2;
			int y2e = old_vector[i + 2].y - 2 * y2;
			if (abs(x2e - x1) <= 1 && abs(y2e - y1) <= 1) {			//extrapolated from side 2
				new_vector.push_back(BWAPI::WalkPosition(x2e, y2e));
				new_vector.push_back(BWAPI::WalkPosition(x2, y2));
				continue;
			}
		}
		std::vector<BWAPI::WalkPosition> my_interpolation = create_linear_interpolation(old_vector[i], old_vector[i+1]);
		for (BWAPI::WalkPosition w : my_interpolation) {
			new_vector.push_back(w);
		}
		new_vector.push_back(BWAPI::WalkPosition(x2, y2));
		continue;

	}
	//BWAPI::Broodwar->printf("Old: %d -- New: %d", old_vector.size(), new_vector.size());
	return new_vector;
}
std::vector<BWAPI::WalkPosition> MapAnalysis::cut_walkpath_corners(std::vector<BWAPI::WalkPosition>& old_vector) {
	std::vector<BWAPI::WalkPosition> new_vector = {};
	unsigned i = 0;
	for (i = 0; i < old_vector.size() - 3; i++) {
		new_vector.push_back(old_vector[i]);
		int dx2 = old_vector[i + 2].x - old_vector[i].x;
		int dy2 = old_vector[i + 2].y - old_vector[i].y;
		int dx3 = old_vector[i + 3].x - old_vector[i].x;
		int dy3 = old_vector[i + 3].y - old_vector[i].y;
		if (abs(dx3) <= 1 && abs(dy3) <= 1) { i++; i++; }
		else if (abs(dx2) <= 1 && abs(dy2) <= 1) { i++; }
	}
	while (i < old_vector.size()) {
		new_vector.push_back(old_vector[i]);
		i++;
	}
	return new_vector;
}
std::vector<BWAPI::WalkPosition> MapAnalysis::cut_walkpath_bulge_diagonal(std::vector<BWAPI::WalkPosition>& old_vector) {
	std::vector<BWAPI::WalkPosition> new_vector = { old_vector.front() };
	unsigned i = 1;
	for (i = 0; i < old_vector.size() - 3; i++) {
		int dx = old_vector[i + 2].x - old_vector[i - 1].x;
		int dy = old_vector[i + 2].y - old_vector[i - 1].y;
		if (abs(dx) == 2 && abs(dy) == 2) {
			int x0 = old_vector[i - 1].x;
			int y0 = old_vector[i - 1].y;
			new_vector.push_back(BWAPI::WalkPosition(x0 + dx / 2, y0 + dy / 2));
			i++;
		} else {
			new_vector.push_back(old_vector[i]);
		}
	}
	while (i < old_vector.size()) {
		new_vector.push_back(old_vector[i]);
		i++;
	}
	return new_vector;
}
void MapAnalysis::cut_walkpath_bulge_orthogon(std::vector<BWAPI::WalkPosition>& my_vector) {
	for (unsigned i = 1; i < my_vector.size() - 1; i++) {
		int dx = my_vector[i + 1].x - my_vector[i - 1].x;
		int dy = my_vector[i + 1].y - my_vector[i - 1].y;
		if (abs(dx) + abs(dy) == 2) {
			int x0 = my_vector[i - 1].x;
			int y0 = my_vector[i - 1].y;
			my_vector[i] = BWAPI::WalkPosition(x0 + dx/2 , y0 + dy/2);
			//BWAPI::Broodwar->printf("cut wave");
		}
	}
	return;
}




//std::tuple<int, int> MapAnalysis::check_linearity(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1, BWAPI::WalkPosition w2) {
//	if (w2.x - w1.x == w1.x - w0.x &&
//		w2.y - w1.y == w1.y - w0.y)
//		return { w1.x - w0.x, w1.y - w0.y };
//	return { 0 , 0 };
//}
//std::tuple<int, int> MapAnalysis::calc_intersection(BWAPI::WalkPosition w1, BWAPI::WalkPosition w2, int dx1, int dy1, int dx2, int dy2) {
//	return { 0 , 0 };
//}
//std::vector<BWAPI::WalkPosition> MapAnalysis::fill_discontinuties(std::vector<BWAPI::WalkPosition>& my_vec) {
//	std::vector<BWAPI::WalkPosition> new_vector = {};
//
//	for (std::vector<BWAPI::WalkPosition>::iterator it1 = my_vec.begin(); it1 != my_vec.end(); ++it1) {
//		std::vector<BWAPI::WalkPosition>::iterator it2 = it1 + 1;
//		new_vector.push_back(*it1);
//		if (it2 != my_vec.end()) {
//			if (sqdist(*it1, *it2) > 2) {
//				std::vector<BWAPI::WalkPosition> my_interpolation = create_linear_interpolation(*it1, *it2);
//				for (BWAPI::WalkPosition w : my_interpolation) {
//					new_vector.push_back(w);
//				}
//			}
//		}
//	}
//	return new_vector;
//}
std::vector<BWAPI::WalkPosition> MapAnalysis::create_linear_interpolation(BWAPI::WalkPosition w1, BWAPI::WalkPosition w2) {
	std::vector<BWAPI::WalkPosition> new_vector = {};
	float dx = (float)(w2.x - w1.x);
	float dy = (float)(w2.y - w1.y);
	float imax = abs(dx);
	if (abs(dy) > abs(dx)) { imax = abs(dy); }
	dx = dx / imax;
	dy = dy / imax;
	for (float i = 1.0; i < imax; i++) {
		new_vector.push_back( BWAPI::WalkPosition(w1.x + (int)(i * dx) , w1.y + (int)(i * dy)) );
	}
	return new_vector;
}
//void MapAnalysis::shift_walkpath_mid(std::vector<BWAPI::WalkPosition>& vm, std::vector<BWAPI::WalkPosition>& vr, std::vector<BWAPI::WalkPosition>& vl) {
//	for (unsigned i = 1; i < vm.size(); i++) {
//		int dr = sqdist(vm[i], vr[i-1]);
//		int dl = sqdist(vm[i], vl[i-1]);
//		if (dr > dl) { vm[i] = shift_walkpos(vm[i], vr[i-1], 1); }
//		if (dl > dr) { vm[i] = shift_walkpos(vm[i], vl[i-1], 1); }
//	}
//	return;
//}
//BWAPI::WalkPosition MapAnalysis::shift_walkpos(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1, int ds) {
//	int x = w0.x;
//	if (w1.x > w0.x) { x += ds; }
//	if (w1.x < w0.x) { x -= ds; }
//	int y = w0.y;
//	if (w1.y > w0.y) { y += ds; }
//	if (w1.y < w0.y) { y -= ds; }
//	return BWAPI::WalkPosition( x, y );
//
//}




//std::vector<BWAPI::WalkPosition> MapAnalysis::find_choke_walkspace(int(&my_map)[1024][1024], BWAPI::Position p, unsigned imax) {
//	BWAPI::WalkPosition w0 = (BWAPI::WalkPosition) p;
//	BWAPI::WalkPosition w1 = get_path_back(my_map, w0.x, w0.y);
//	std::vector<BWAPI::WalkPosition> min_vec = {};
//	unsigned min_space = 1024;
//	unsigned i = 0;
//	while (w1 != BWAPI::WalkPositions::None && i < imax) {
//		std::vector<BWAPI::WalkPosition> my_vec = get_path_walkspace(w0, w1);
//		if (my_vec.size() >= 28 &&
//			my_vec.size() < min_space) {
//			min_space = my_vec.size();
//			min_vec = my_vec;
//		}
//		wilmap::pathwalkmap[w0.y][w0.x] = my_vec.size();
//		w0 = w1;
//		w1 = get_path_back(my_map, w1.x, w1.y);
//		++i;
//	}
//	BWAPI::Broodwar->printf("found choke with %d walkspace", min_space);
//	return min_vec;
//}

//std::vector<BWAPI::WalkPosition> MapAnalysis::get_path_walkspace(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1) {
//	int du = 1;
//	int dd = 1;
//	std::vector<BWAPI::WalkPosition> my_vec = {};
//	if (w0.y == w1.y) {							//orthogonal horizontal
//		if (w0.x > w1.x) { w0.x = w1.x; }
//		check_walkspace_segment(w0.x - 3, w0.y, 1, 0, 10, my_vec);
//		while (check_walkspace_segment(w0.x - 3, w0.y + du, 1, 0, 10, my_vec)) { du++; }
//		while (check_walkspace_segment(w0.x - 3, w0.y - dd, 1, 0, 10, my_vec)) { dd++; }
//	}
//	else if (w0.x == w1.x) {					//orthogonal vertical
//		if (w0.y > w1.y) { w0.y = w1.y; }
//		check_walkspace_segment(w0.x, w0.y - 3, 0, 1, 10, my_vec);
//		while (check_walkspace_segment(w0.x - dd, w0.y - 3, 0, 1, 10, my_vec)) { dd++; }
//		while (check_walkspace_segment(w0.x + du, w0.y - 3, 0, 1, 10, my_vec)) { du++; }
//	}
//	else if (w1.x - w0.x == w1.y - w0.y) {			//diagonal left-up right-down
//		if (w0.x > w1.x && w0.y > w1.y) { w0.x = w1.x; w0.y = w1.y; }
//		check_walkspace_segment(w0.x - 2, w0.y - 2, +1, +1, 7, my_vec);
//		while (check_walkspace_segment(w0.x - 2 + du, w0.y - 2 - du, +1, +1, 7, my_vec) &&
//				check_walkspace_segment(w0.x - 2 + du, w0.y - 1 - du, +1, +1, 6, my_vec)) { du++; }
//		while (check_walkspace_segment(w0.x - 2 - dd, w0.y - 2 + dd, +1, +1, 7, my_vec) &&
//				check_walkspace_segment(w0.x - 1 - dd, w0.y - 2 + dd, +1, +1, 6, my_vec)) { dd++; }
//	}
//	else if (w1.x - w0.x == w0.y - w1.y) {			//diagonal left-down right-up
//		if (w0.x > w1.x && w1.y > w0.y) { w0.x = w1.x; w0.y = w1.y; }
//		check_walkspace_segment(w0.x - 2, w0.y + 2, +1, -1, 7, my_vec);
//		while (check_walkspace_segment(w0.x - 2 - du, w0.y + 2 - du, +1, -1, 7, my_vec) &&
//				check_walkspace_segment(w0.x - 1 - du, w0.y + 2 - du, +1, -1, 6, my_vec)) { du++; }
//		while (check_walkspace_segment(w0.x - 2 + dd, w0.y + 2 + dd, +1, -1, 7, my_vec) &&
//				check_walkspace_segment(w0.x - 2 + dd, w0.y + 1 + dd, +1, -1, 6, my_vec)) { dd++; }
//	}
//	return my_vec;
//}

//bool MapAnalysis::check_walkspace_segment(int x0, int y0, int dx, int dy, int imax, std::vector<BWAPI::WalkPosition>& my_vec) {
//	for (int i = 0; i < imax; i++)
//		if (!BWAPI::Broodwar->isWalkable(x0 + i * dx, y0 + i * dy))
//			return false;
//	for (int i = 0; i < imax; i++)
//		my_vec.push_back(BWAPI::WalkPosition(x0 + i * dx, y0 + i * dy));
//	return true;
//}

//BWAPI::Position  MapAnalysis::find_choke_center(std::vector<BWAPI::WalkPosition>& my_vec) {
//	int x = 0; int y = 0;
//	for (BWAPI::WalkPosition w : my_vec) {
//		wilmap::chokewalkmap[w.y][w.x] = true;
//		x += w.x; y += w.y;
//	}
//	return BWAPI::Position( x / my_vec.size() * 8 , y / my_vec.size() * 8 );
//}



void MapAnalysis::create_defense_maps() {
	using namespace wilmap;
	clear_map_bool(maindefmap);
	clear_map_bool(natudefmap);
	for (int i = 0; i < mn; i++) {
		create_defense_area(main_choke_pos[i], maindistarray[i], maindefmap);
		create_defense_area(natu_choke_pos[i], maindistarray[i], natudefmap);
	}
	return;
}

void MapAnalysis::create_defense_area(BWAPI::Position mychokepos, int(&mydistmap)[256][256], bool(&mydefmap)[256][256]) {
	using namespace wilmap;
	int xc = mychokepos.x / 32;
	int yc = mychokepos.y / 32;
	int mychokedist = mydistmap[yc][xc];
	if (mychokedist == -1) { return; }

	for (int y = mapsafesub(yc, 12); y < mapsafeadd(yc, 12, ht); y++) {
		for (int x = mapsafesub(xc, 12); x < mapsafeadd(xc, 12, wt); x++) {
			if (choke_map[y][x] == false &&
				mydistmap[y][x] != -1 &&
				mydistmap[y][x] < mychokedist &&
				sqdist(x, y, xc, yc) <= 144 &&
				sqdist(x, y, xc, yc) >= 16) {
				mydefmap[y][x] = true;
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
}

