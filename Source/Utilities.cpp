#include "Utilities.h"
#include <cmath>

void safe_sum(int &n, int dn) {
	n = (n > -dn) ? n + dn : 0;
}
//bool equals(BWAPI::TilePosition& t0, BWAPI::TilePosition& t1) {
//	return (t0.x == t1.x && t0.y == t1.y) ? true : false;
//}
bool is_none(BWAPI::TilePosition& t) {
	return (t == BWAPI::TilePositions::None) ? true : false;
}
bool is_none(BWAPI::Position& p) {
	return (p == BWAPI::Positions::None) ? true : false;
}



void vector_remove(std::vector<BWAPI::Unit> &v, BWAPI::Unit u) {
	v.erase(std::remove(v.begin(), v.end(), u), v.end());
	return;
}
void vector_remove(std::vector<BWAPI::UnitType> &v, BWAPI::UnitType t) {
	v.erase(std::remove(v.begin(), v.end(), t), v.end());
	return;
}
void vector_remove(std::vector<BWAPI::Position> &v, BWAPI::Position p) {
	v.erase(std::remove(v.begin(), v.end(), p), v.end());
	return;
}
void vector_remove(std::vector<BWAPI::TilePosition> &v, BWAPI::TilePosition t) {
	v.erase(std::remove(v.begin(), v.end(), t), v.end());
	return;
}
void vector_remove(std::vector<double> &v, double &d) {
	v.erase(std::remove(v.begin(), v.end(), d), v.end());
	return;
}



bool vector_holds(std::vector<BWAPI::Unit> &v, BWAPI::Unit u) {
	if (std::find(v.begin(), v.end(), u) != v.end()) return true;
	return false;
}
bool vector_holds(std::vector<BWAPI::UnitType> &v, BWAPI::UnitType t) {
	if (std::find(v.begin(), v.end(), t) != v.end()) return true;
	return false;
}
bool vector_holds(std::vector<BWAPI::Position> &v, BWAPI::Position p) {
	if (std::find(v.begin(), v.end(), p) != v.end()) return true;
	return false;
}
bool vector_holds(std::vector<BWAPI::TilePosition> &v, BWAPI::TilePosition t) {
	if (std::find(v.begin(), v.end(), t) != v.end()) return true;
	return false;
}



BWAPI::Position linear_interpol_rel(BWAPI::Position p0, BWAPI::Position p1, float f) {
	int x = p0.x + (int)(f * (p1.x - p0.x));
	int y = p0.y + (int)(f * (p1.y - p0.y));
	return BWAPI::Position{ x, y };
}
BWAPI::Position linear_interpol_abs(BWAPI::Position p0, BWAPI::Position p1, int d) {
	int l = dist(p0, p1);
	if (l != 0) {
		int x = p0.x + (int)(d * (p1.x - p0.x) / l);
		int y = p0.y + (int)(d * (p1.y - p0.y) / l);
		return BWAPI::Position{ x, y };
	}
	return p0;
}
BWAPI::Position estimate_next_pos(BWAPI::Unit u, int n) {
	int x = u->getPosition().x + (int)(n * u->getVelocityX());
	int y = u->getPosition().y + (int)(n * u->getVelocityY());
	BWAPI::Position my_pos = BWAPI::Position{ x , y };
	if (my_pos.isValid()) { return my_pos; }
	else { return u->getPosition(); }
}
BWAPI::Position get_random_position() {
	int x = rand() % BWAPI::Broodwar->mapWidth();
	int y = rand() % BWAPI::Broodwar->mapHeight();
	return BWAPI::Position{ 32 * x , 32 * y };
}



int dist(BWAPI::Position p0, BWAPI::Position p1) {
	return (int)(pow(sqdist(p0, p1), 0.5));
}
int dist(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1) {
	return (int)(pow(sqdist(w0, w1), 0.5));
}
int sqdist(BWAPI::TilePosition t0, BWAPI::TilePosition t1) {
	return (int)(pow(t0.x - t1.x, 2) + pow(t0.y - t1.y, 2));
}
int sqdist(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1) {
	return (int)(pow(w0.x - w1.x, 2) + pow(w0.y - w1.y, 2));
}
int sqdist(BWAPI::Position p0, BWAPI::Position p1) {
	return (int)(pow(p0.x - p1.x, 2) + pow(p0.y - p1.y, 2));
}
int sqdist(BWAPI::Unit u0, BWAPI::Unit u1) {
	return sqdist(u0->getTilePosition(), u1->getTilePosition());
}
int sqdist(BWAPI::Unit u0, BWAPI::Position p1) {
	return sqdist(u0->getPosition(), p1);
}
int sqdist(int x0, int y0, int x1, int y1) {
	return (int)(pow(x0 - x1, 2) + pow(y0 - y1, 2));
}



BWAPI::Position get_closest(std::vector<BWAPI::Position> &v, BWAPI::Position &p) {
	BWAPI::Position min_pos = BWAPI::Positions::None;
	int min_dist = 134217728;
	int my_dist = 0;
	for (BWAPI::Position my_pos : v) {
		my_dist = sqdist(my_pos, p);
		if (min_dist > my_dist) {
			min_dist = my_dist;
			min_pos = my_pos;
		}
	}
	return min_pos;
}
BWAPI::Unit get_closest(std::vector<BWAPI::Unit> &v, BWAPI::Position &p) {
	BWAPI::Unit min_unit = NULL;
	int min_dist = 134217728;
	int my_dist = 0;
	for (BWAPI::Unit my_unit : v) {
		if (!my_unit->exists()) { continue; }
		my_dist = sqdist(my_unit->getPosition(), p);
		if (min_dist > my_dist) {
			min_dist = my_dist;
			min_unit = my_unit;
		}
	}
	return min_unit;
}
BWAPI::Unit get_closest(std::vector<BWAPI::Unit> &v, BWAPI::TilePosition &t) {
	BWAPI::Unit min_unit = NULL;
	int min_dist = 131072;
	int my_dist = 0;
	for (BWAPI::Unit my_unit : v) {
		if (!my_unit->exists()) { continue; }
		my_dist = sqdist(t, my_unit->getTilePosition());
		if (min_dist > my_dist) {
			min_dist = my_dist;
			min_unit = my_unit;
		}
	}
	return min_unit;
}
BWAPI::Unit get_in_range(std::vector<BWAPI::Unit> &v, BWAPI::TilePosition &t, int sqr) {
	if (t != BWAPI::TilePositions::None) {
		for (BWAPI::Unit u : v) {
			if (sqdist(t, u->getTilePosition()) <= sqr) { return u; }
		}
	}
	return NULL;
}
BWAPI::Unit get_in_range(std::vector<BWAPI::Unit> &v, BWAPI::Position &p, int sqd) {
	if (p != BWAPI::Positions::None) {
		for (BWAPI::Unit u : v) {
			if (sqdist(u, p) <= sqd) { return u; }
		}
	}
	return NULL;
}



void copy_array(bool(&my_src)[256][256], bool(&my_dest)[256][256]) {
	for (unsigned y = 0; y < 256; y++) {
		for (unsigned x = 0; x < 256; x++) {
			my_dest[y][x] = my_src[y][x];
		}
	}
	return;
}