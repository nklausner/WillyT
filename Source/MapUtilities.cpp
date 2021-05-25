#include "MapUtilities.h"

void set_map_area(bool(&my_map)[256][256], int x0, int y0, int dx, int dy, bool v) {
	for (int y = y0; y < y0 + dy; ++y)
		for (int x = x0; x < x0 + dx; ++x)
			my_map[y][x] = v;
	return;
}
void set_map_area(bool(&my_map)[256][256], BWAPI::Unit u, bool v) {
	int x0 = u->getTilePosition().x;
	int y0 = u->getTilePosition().y;
	int w = u->getType().tileWidth();
	int h = u->getType().tileHeight();
	set_map_area(my_map, x0, y0, w, h, v);
	return;
}



void clear_build_area(bool(&my_map_var)[256][256], bool(&my_map_fix)[256][256], int x0, int y0, int dx, int dy) {
	for (int y = y0; y < y0 + dy; ++y)
		for (int x = x0; x < x0 + dx; ++x)
			if (my_map_fix[y][x])
				my_map_var[y][x] = true;
	return;
}
void clear_build_area(bool(&my_map_var)[256][256], bool(&my_map_fix)[256][256], BWAPI::Unit u) {
	int x0 = u->getTilePosition().x;
	int y0 = u->getTilePosition().y;
	int w = u->getType().tileWidth();
	int h = u->getType().tileHeight();
	clear_build_area(my_map_var, my_map_fix, x0, y0, w, h);
	return;
}



bool check_map_area(bool(&my_map)[256][256], int x0, int y0, int w, int h) {
	if (x0 < 0 || x0 + w > wilmap::wt) return false;
	if (y0 < 0 || y0 + h > wilmap::ht) return false;
	for (int y = y0; y < y0 + h; ++y)
		for (int x = x0; x < x0 + w; ++x)
			if (!my_map[y][x])
				return false;
	return true;
}
bool check_map_area(int(&my_map)[1024][1024], int x0, int y0, int w, int h, int f) {
	x0 = f * x0; w = f * w;
	y0 = f * y0; h = f * h;
	for (int y = y0; y < y0 + h; ++y)
		for (int x = x0; x < x0 + w; ++x)
			if (my_map[y][x] != 0)
				return false;
	return true;
}
bool check_buildable(int x0, int y0, int dx, int dy) {
	if (x0 < 0 || x0 + dx > wilmap::wt) return false;
	if (y0 < 0 || y0 + dy > wilmap::ht) return false;
	for (int y = y0; y < y0 + dy; ++y)
		for (int x = x0; x < x0 + dx; ++x)
			if (!BWAPI::Broodwar->isBuildable(x, y, false))
				return false;
	return true;
}
bool tile_fully_walkable(int x, int y) {
	x = 4 * x;
	y = 4 * y;
	if (!BWAPI::Broodwar->isWalkable(x + 0, y + 0)) return false;
	if (!BWAPI::Broodwar->isWalkable(x + 3, y + 0)) return false;
	if (!BWAPI::Broodwar->isWalkable(x + 0, y + 3)) return false;
	if (!BWAPI::Broodwar->isWalkable(x + 3, y + 3)) return false;
	return true;
}



void clear_map_bool(bool(&my_map)[256][256]) {
	for (int y = 0; y < 256; ++y)
		for (int x = 0; x < 256; ++x)
			my_map[y][x] = false;
	return;
}
void clear_map_bool(bool(&my_map)[1024][1024]) {
	for (int y = 0; y < 1024; ++y)
		for (int x = 0; x < 1024; ++x)
			my_map[y][x] = false;
	return;
}
void fill_map_int(int(&my_map)[256][256], int z) {
	for (int y = 0; y < 256; ++y)
		for (int x = 0; x < 256; ++x)
			my_map[y][x] = z;
	return;
}
void fill_map_int(int(&my_map)[1024][1024], int z) {
	for (int y = 0; y < 1024; ++y)
		for (int x = 0; x < 1024; ++x)
			my_map[y][x] = z;
	return;
}



BWAPI::TilePosition get_center_tile(BWAPI::Position p) {
	return BWAPI::TilePosition(p.x / 32 - 1, p.y / 32);
}
BWAPI::TilePosition get_center_of_mass(std::vector<BWAPI::TilePosition> v) {
	int x = 0;
	int y = 0;
	for (BWAPI::TilePosition t : v) {
		x += t.x;
		y += t.y;
	}
	int n = (int)v.size();
	return BWAPI::TilePosition{ x/n , y/n };
}
BWAPI::TilePosition get_vary_tile(BWAPI::TilePosition t, int i) {
	int dx[19] = { 0, 1, 0, +1, -1, 2, 0, +2, -2, +1, -1, +2, -2, 3, 0, +3, -3, +1, -1 };
	int dy[19] = { 0, 0, 1, +1, +1, 0, 2, +1, +1, +2, +2, +2, +2, 0, 3, +1, +1, +3, +3 };
	if (i >= 38) return t;
	if (i % 2 == 0) return BWAPI::TilePosition(t.x - dx[i/2], t.y - dy[i/2]);
	if (i % 2 == 1) return BWAPI::TilePosition(t.x + dx[i/2], t.y + dy[i/2]);
	return t;
}



bool is_topl(int x, int y) {
	if (x < 16 && y < 16) return true;
	return false;
}
bool is_topr(int x, int y) {
	if (x > wilmap::wt - 18 && y < 16) return true;
	return false;
}
bool is_botl(int x, int y) {
	if (x < 16 && y > wilmap::ht - 18) return true;
	return false;
}
bool is_botr(int x, int y) {
	if (x > wilmap::wt - 18 && y > wilmap::ht - 18) return true;
	return false;
}
bool is_top_(int x, int y) {
	if (y < 16) return true;
	return false;
}
bool is_bot_(int x, int y) {
	if (y > wilmap::ht - 18) return true;
	return false;
}
bool is_left(int x, int y) {
	if (x < 16) return true;
	return false;
}
bool is_rigt(int x, int y) {
	if (x > wilmap::wt - 18) return true;
	return false;
}



int get_direction_4(BWAPI::Position p0, BWAPI::Position p1) {
	int dx = p1.x - p0.x;
	int dy = p1.y - p0.y;

	if (abs(dx) >= abs(dy)) {
		return (dx > 0) ? 0 : 2;		//right or left
	}
	else {
		return (dy > 0) ? 1 : 3;		//bottom or top
	}
	//left-right-top-bottom
}
int get_direction_8(int x0, int y0, int x1, int y1) {
	int dx = x1 - x0;
	int dy = y1 - y0;

	if (abs(dx) > 2 * abs(dy)) {
		return (dx > 0) ? 0 : 4;		//right or left
	}
	else if (abs(dy) > 2 * abs(dx)) {
		return (dy > 0) ? 2 : 6;		//bottom or top
	}
	else if (dx > 0) {
		return (dy > 0) ? 1 : 7;		//bot-right or top-right
	}
	else {
		return (dy > 0) ? 3 : 5;		//bot-left or top-left
	}
}



BWAPI::Position get_circle_pos(std::vector<BWAPI::Position> &my_vec, int i) {
	return my_vec.at(i % my_vec.size());
}

bool has_grd_connection(BWAPI::Position p0, BWAPI::Position p1)
{
	int d0 = get_ground_dist(p0);
	int d1 = get_ground_dist(p1);
	if (d1 < 0) { d1 = get_ground_dist(BWAPI::Position(p1.x-2, p1.y)); } //fix for: mineral_field with right tile in wall
	if (d0 >= 0 && d1 < 0) return false;
	if (d0 < 0 && d1 >= 0) return false;
	if (d0 < 0 && d1 < 0 && sqdist(p0, p1) > 65536) return false;
	return true;
}



int get_ground_dist(int x, int y)
{
	x = keepmapsafe(x, wilmap::wt);
	y = keepmapsafe(y, wilmap::ht);
	return wilmap::maindistarray[wilmap::mm][y][x];
}
int get_ground_dist(BWAPI::TilePosition my_tile) {
	return get_ground_dist(my_tile.x, my_tile.y);
}
int get_ground_dist(BWAPI::Position my_pos) {
	return get_ground_dist(my_pos.x / 32, my_pos.y / 32);
}
int get_ground_dist(BWAPI::Unit my_unit) {
	return get_ground_dist(my_unit->getTilePosition());
}



int mapsafesub(int x, int dx) {
	return (x - dx < 0) ? 0 : x - dx;
}
int mapsafeadd(int x, int dx, int xmax) {
	return (x + dx > xmax) ? xmax : x + dx;
}
int keepmapsafe(int x, int xmax) {
	if (x < 0) { return 0; }
	if (x > xmax) { return xmax; }
	return x;
}



std::vector<BWAPI::WalkPosition> create_linear_interpolation(BWAPI::WalkPosition w1, BWAPI::WalkPosition w2) {
	std::vector<BWAPI::WalkPosition> new_vector = {};
	float dx = (float)(w2.x - w1.x);
	float dy = (float)(w2.y - w1.y);
	float imax = (abs(dy) > abs(dx)) ? abs(dy) : abs(dx);
	dx = dx / imax;
	dy = dy / imax;
	for (float i = 1.0; i < imax; i++) {
		new_vector.push_back(BWAPI::WalkPosition(w1.x + (int)(i * dx), w1.y + (int)(i * dy)));
	}
	return new_vector;
}



bool buildspot_visible(BWAPI::TilePosition t) {
	if (BWAPI::Broodwar->isVisible(t.x + 0, t.y + 0)) return true;
	if (BWAPI::Broodwar->isVisible(t.x + 3, t.y + 0)) return true;
	if (BWAPI::Broodwar->isVisible(t.x + 0, t.y + 2)) return true;
	if (BWAPI::Broodwar->isVisible(t.x + 3, t.y + 2)) return true;
	return false;
}
bool buildspot_visible(BWAPI::Position p) {
	return buildspot_visible(BWAPI::TilePosition(p.x / 32 - 2, p.y / 32 - 1));
}



std::vector<BWAPI::Position> create_circle_vector(BWAPI::Position p0, int r) {
	std::vector<BWAPI::Position> new_vector = {};
	double rad = 24.0 / r;
	int imax = (int)(6.283 * r) / 24;
	for (int i = 0; i < imax; i++) {
		int x = (int)(p0.x + r * cos(i * rad));
		int y = (int)(p0.y + r * sin(i * rad));
		new_vector.push_back(BWAPI::Position(x, y));
	}
	BWAPI::Broodwar->printf("armycircvec %d", imax);
	return new_vector;
}
void create_circle_24() {
	double rad = 2.0 * 3.14159265358979323846 / 24.0;
	for (int i = 0; i < 24; i++) {
		wilmap::cdx24[i] = (int)(64 * cos(i * rad));
		wilmap::cdy24[i] = (int)(64 * sin(i * rad));
	}
}
void create_circle_r4() {
	for (int i = 0; i < 24; i++) {
		wilmap::cr4dx[i] = (wilmap::cdx24[i] * 5) / 16;
		wilmap::cr4dy[i] = (wilmap::cdy24[i] * 5) / 16;
	}
}
BWAPI::Position get_circle_pos_24(int n, int r0, int i) {
	r0 = 2 * r0;
	for (int j = r0; j > 6; j--) {
		int x = wilmap::main_pos[n].x + ((j * wilmap::cdx24[i]) / 4);
		int y = wilmap::main_pos[n].y + ((j * wilmap::cdy24[i]) / 4);
		int xt = x / 32;
		int yt = y / 32;
		if (xt > 0 && xt < wilmap::wt &&
			yt > 0 && yt < wilmap::ht &&
			wilmap::build_map_var[yt][xt] &&
			wilmap::maindistarray[n][yt][xt] < r0) {
			return BWAPI::Position{ x, y };
		}
	}
	return BWAPI::Positions::None;
}