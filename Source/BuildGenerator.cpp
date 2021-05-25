#include "BuildGenerator.h"

BWAPI::TilePosition BuildGenerator::generate_random_tile(BWAPI::UnitType my_type, BWAPI::TilePosition my_start, int my_space) {
	n = (BWAPI::Broodwar->getFrameCount() / 8) % 4;
	t = BWAPI::TilePositions::None;
	r = 0;
	s = my_space;
	i = 1;

	int x0 = my_start.x + 2;
	int y0 = my_start.y + 1;
	w = s + my_type.tileWidth() + s;
	h = s + my_type.tileHeight() + s;
	if (my_type.canBuildAddon()) { w += 2; }

	while (r < 24) {
		if (n == 0) {
			t = check_line_top(x0, y0);
			t = check_line_rig(x0, y0);
			t = check_line_bot(x0, y0);
			t = check_line_lef(x0, y0);
		}
		else if (n == 1) {
			t = check_line_rig(x0, y0);
			t = check_line_bot(x0, y0);
			t = check_line_lef(x0, y0);
			t = check_line_top(x0, y0);
		}
		else if (n == 2) {
			t = check_line_bot(x0, y0);
			t = check_line_lef(x0, y0);
			t = check_line_top(x0, y0);
			t = check_line_rig(x0, y0);
		}
		else if (n == 3) {
			t = check_line_lef(x0, y0);
			t = check_line_top(x0, y0);
			t = check_line_rig(x0, y0);
			t = check_line_bot(x0, y0);
		}
		if (t != BWAPI::TilePositions::None) {
			//BWAPI::Broodwar->printf("Build generator cycles: %d", i);
			return t;
		}
		++r;
	}
	return t;
}

BWAPI::TilePosition BuildGenerator::check_line_top(int x0, int y0) {
	return check_line_hor(x0 - w / 2, y0 - r - (h - 1));
}
BWAPI::TilePosition BuildGenerator::check_line_bot(int x0, int y0) {
	return check_line_hor(x0 - w / 2, y0 + r);
}
BWAPI::TilePosition BuildGenerator::check_line_lef(int x0, int y0) {
	return check_line_ver(x0 - r - (w - 1), y0 - h / 2);
}
BWAPI::TilePosition BuildGenerator::check_line_rig(int x0, int y0) {
	return check_line_ver(x0 + r, y0 - h / 2);
}

BWAPI::TilePosition BuildGenerator::check_line_ver(int x0, int y0) {
	if (t != BWAPI::TilePositions::None ||
		x0 < 0 || x0 > 254) {
		return t;
	}
	int y1 = y0 - r - 2;
	int y2 = y0 + r + 2;
	if (y1 < 0) y1 = 0;
	if (y2 > 254) y2 = 254;

	for (int y = y1; y < y2; ++y) {
		if (check_map_area(wilmap::build_map_var, x0, y, w, h) &&
			check_map_area(wilthr::grddef, x0, y, w, h, 4)) {
			return BWAPI::TilePosition(x0+s, y+s);
		}
		++i;
	}
	return t;
}

BWAPI::TilePosition BuildGenerator::check_line_hor(int x0, int y0) {
	if (t != BWAPI::TilePositions::None ||
		y0 < 0 || y0 > 254) {
		return t;
	}
	int x1 = x0 - r - 2;
	int x2 = x0 + r + 2;
	if (x1 < 0) x1 = 0;
	if (x2 > 254) x2 = 254;

	for (int x = x1; x < x2; ++x) {
		if (check_map_area(wilmap::build_map_var, x, y0, w, h) &&
			check_map_area(wilthr::grddef, x, y0, w, h, 4)) {
			return BWAPI::TilePosition(x+s, y0+s);
		}
		++i;
	}
	return t;
}



BWAPI::TilePosition BuildGenerator::vary_build_tile(BWAPI::UnitType my_type, BWAPI::TilePosition my_tile) {
	x = my_tile.x;
	y = my_tile.y;
	w = my_type.tileWidth();
	h = my_type.tileHeight();
	int dx[19] = { 0, 1, 0, +1, -1, 2, 0, +2, -2, +1, -1, +2, -2, 3, 0, +3, -3, +1, -1 };
	int dy[19] = { 0, 0, 1, +1, +1, 0, 2, +1, +1, +2, +2, +2, +2, 0, 3, +1, +1, +3, +3 };

	for (i = 0; i < 19; i++) {
		if (check_map_area(wilmap::build_map_var, x - dx[i], y - dy[i], w, h) &&
			check_map_area(wilthr::grddef, x - dx[i], y - dy[i], w, h, 4)) {
			return BWAPI::TilePosition(x - dx[i], y - dy[i]);
		}
		if (check_map_area(wilmap::build_map_var, x + dx[i], y + dy[i], w, h) &&
			check_map_area(wilthr::grddef, x + dx[i], y + dy[i], w, h, 4)) {
			return BWAPI::TilePosition(x + dx[i], y + dy[i]);
		}
	}
	BWAPI::Broodwar->printf("found no build tile by variation");
	return BWAPI::TilePositions::None;
}



BWAPI::TilePosition BuildGenerator::generate_pair_tile(std::vector<BWAPI::Unit> &my_vector) {
	for (BWAPI::Unit u : my_vector) {
		x = u->getTilePosition().x;
		y = u->getTilePosition().y;
		if (check_pair_tile(my_vector)) continue;
		if (check_map_area(wilmap::build_map_var, x-1, y-3, 5, 3) && y-3 > 0) return BWAPI::TilePosition(x, y-2);
		if (check_map_area(wilmap::build_map_var, x-1, y+2, 5, 3) && y+2 < 254) return BWAPI::TilePosition(x, y+2);
	}
	return BWAPI::TilePositions::None;
}

bool BuildGenerator::check_pair_tile(std::vector<BWAPI::Unit> &my_vector) {
	for (BWAPI::Unit u : my_vector) {
		if (u->getTilePosition().x != x) continue;
		if (u->getTilePosition().y == y - 2) return true;
		if (u->getTilePosition().y == y + 2) return true;
	}
	return false;
}



BWAPI::TilePosition BuildGenerator::generate_proxy_tile(BWAPI::UnitType my_type, BWAPI::TilePosition my_start, std::vector<BWAPI::Position>& my_vec, BWAPI::Position my_choke, int(&my_dist_map)[256][256]) {
	BWAPI::TilePosition max_tile = BWAPI::TilePositions::None;
	int max_sqdist = 0;
	std::vector<BWAPI::Position> my_vec_corr = {};

	for (BWAPI::Position p : my_vec) {
		if (my_dist_map[p.y / 32][p.x / 32] < 48) {
			my_vec_corr.push_back(p);
		}
	}
	for (int i = 0; i < 128; i++)
	{
		int xt = my_start.x + rand() % 49 - 24;
		int yt = my_start.y + rand() % 49 - 24;
		int xp = 32 * xt + 16 * my_type.tileWidth();
		int yp = 32 * yt + 16 * my_type.tileHeight();
		int my_sqdist = 0;

		if (check_map_area(wilmap::build_map_var, xt, yt, my_type.tileWidth(), my_type.tileHeight()) == false ||
			my_dist_map[yt][xt] < 24 || my_dist_map[yt][xt] > 48) {
			continue;
		}
		for (BWAPI::Position p : my_vec_corr) {
			my_sqdist += sqdist(p.x, p.y, xp, yp);
		}
		if (my_choke != BWAPI::Positions::None) {
			my_sqdist += sqdist(xp, yp, my_choke.x, my_choke.y) / 4;
		}
		if (my_sqdist > max_sqdist) {
			max_sqdist = my_sqdist;
			max_tile = BWAPI::TilePosition(xt, yt);
		}
	}
	return max_tile;
}