#include "ArmyPositions.h"

void calculate_army_positions()
{
	using namespace wilmap;
	//BWAPI::Broodwar->printf("calculate army positions start");
	for (int i = 0; i < mn; i++) {

		//initial guess for main defense positions
		//BWAPI::Position p2 = linear_interpol_abs(main_choke_pos[i], main_pos[i], +192);
		//BWAPI::Position p3 = linear_interpol_abs(main_choke_pos[i], natu_pos[i], -192);
		//main_def_pos[i] = BWAPI::Position{ (p2.x + p3.x) / 2 , (p2.y + p3.y) / 2 };

		//initial guess for natu defense positions
		//if (natu_pos[i].getApproxDistance(natu_choke_pos[i]) >= 480) {
		//	natu_def_pos[i] = linear_interpol_abs(natu_pos[i], natu_choke_pos[i], +192);
		//} else {
		//	natu_def_pos[i] = linear_interpol_rel(natu_pos[i], natu_choke_pos[i], 0.5f);
		//}

		//modify defense positions to fit 6x6 area
		main_def_pos[i] = find_def_area(main_choke_pos[i], main_choke_pos[i], i);
		natu_def_pos[i] = find_def_area(natu_choke_pos[i], natu_pos[i], i);
		
		//modify and choose defense building tiles
		main_def_pos[i] = sort_def_area(main_def_pos[i], main_choke_pos[i], main_def_tile[i]);
		natu_def_pos[i] = sort_def_area(natu_def_pos[i], natu_choke_pos[i], natu_def_tile[i]);

		//create a buildable line at the natural defense tiles
		int nd = create_natu_wallin(i);
		if (nd >= 0) {
			rearrange_natu_def_area(i, nd);
			//SOFT: Benzene, HearthbreakRidge, CircuitBreaker, FightingSpirit, Roadrunner, EmpireOfTheSun, LaMancha, Icarus
			//HELP: Python, Jade
			//HARD: Andromeda, Destination, NeoMoonGlaive, TauCross
			//NEW: Overwatch, MatchPoint, Eddy, GroundZero
			//NONE: Colloseum
		}
	}
	check_hardcoded_wallin();
	my_main_def = main_def_pos[mm];
	my_natu_def = natu_def_pos[mm];
	my_entrance = BWAPI::TilePosition{ linear_interpol_rel(my_natu, center_pos, 0.4f) };
	willyt::retreat_pos = calculate_retreat_pos(my_main);

	BWAPI::Position bpos = BWAPI::Position(wilmap::natu_def_tile[wilmap::mm][0]);
	bunker_natu_sqdist = sqdist(my_natu.x, my_natu.y, bpos.x+48, bpos.y+32);

	//armycircle = create_circle_vector(natu_choke_pos[mm], 256);
	//BWAPI::Broodwar->printf("calculate army positions end");
	return;
}

BWAPI::Position find_def_area(BWAPI::Position cp, BWAPI::Position bp, int i)
{
	int x0 = cp.x / 32;
	int y0 = cp.y / 32;
	int xmin = 0;
	int ymin = 0;
	int dmin = 1048576;
	int dgroundmax = wilmap::maindistarray[i][y0][x0];
	if (dgroundmax == -1) { dgroundmax = 256; }

	int x1 = mapsafesub(x0, 12);
	int y1 = mapsafesub(y0, 12);
	int x2 = mapsafeadd(x0, 12, wilmap::wt);
	int y2 = mapsafeadd(y0, 12, wilmap::ht);
	for (int x = x1; x <= x2; x++) {
		for (int y = y1; y <= y2; y++)
		{
			int dground = wilmap::maindistarray[i][y][x];
			if (dground != -1 &&
				dground < dgroundmax)
			{
				int dchoke = dist(cp, BWAPI::Position(32 * x, 32 * y));
				int dbase = dist(bp, BWAPI::Position(32 * x, 32 * y));
				int d = dground + dchoke + dbase;

				if (dchoke >= 96 &&
					dbase >= 96 &&
					check_buildable(x - 3, y - 3, 6, 6) &&
					d < dmin)
				{
					dmin = d;
					xmin = x;
					ymin = y;
				}
			}
		}
	}
	//BWAPI::Broodwar->printf("%d", dmin);
	return BWAPI::Position(32 * xmin, 32 * ymin);
}

BWAPI::Position sort_def_area(BWAPI::Position dp, BWAPI::Position cp, BWAPI::TilePosition(&dt)[2])
{
	int xc = cp.x / 32;
	int yc = cp.y / 32;
	int xd = dp.x / 32;
	int yd = dp.y / 32;
	std::vector<std::pair<int, BWAPI::TilePosition>> my_vec;
	my_vec.push_back(std::make_pair( sqdist(xc, yc, xd-1, yd-1), BWAPI::TilePosition(xd-1, yd-1) ));
	my_vec.push_back(std::make_pair( sqdist(xc, yc, xd+1, yd-1), BWAPI::TilePosition(xd+1, yd-1) ));
	my_vec.push_back(std::make_pair( sqdist(xc, yc, xd-1, yd+1), BWAPI::TilePosition(xd-1, yd+1) ));
	my_vec.push_back(std::make_pair( sqdist(xc, yc, xd+1, yd+1), BWAPI::TilePosition(xd+1, yd+1) ));
	sort(my_vec.begin(), my_vec.end());

	dt[0] = BWAPI::TilePosition(my_vec[0].second.x - 1, my_vec[0].second.y - 1); //bunker
	dt[1] = BWAPI::TilePosition(my_vec[1].second.x - 1, my_vec[1].second.y - 1); //turret
	if (dt[0].y == dt[1].y &&
		dt[0].x == dt[1].x - 2) {
		dt[0].x--;
	}
	if (dt[0].y != dt[1].y &&
		dt[0].x == xd) {
		dt[0].x--;
	}
	int xn = 16 * (my_vec[2].second.x + my_vec[3].second.x);
	int yn = 16 * (my_vec[2].second.y + my_vec[3].second.y);
	return BWAPI::Position(xn, yn);
}

BWAPI::Position fill_def_area(BWAPI::Position dp, BWAPI::Position cp, BWAPI::TilePosition(&dt)[2])
{
	int dx = cp.x - dp.x;
	int dy = cp.y - dp.y;
	int x = dp.x / 32;
	int y = dp.y / 32;
	//............horizontal case............
	if (abs(dx) >= abs(dy)) {
		if (dx > 0) {
			dt[0] = BWAPI::TilePosition(x - 1, y - 2);
			dt[1] = BWAPI::TilePosition(x - 1, y + 0);
			return BWAPI::Position(32*x - 64, 32*y);
		}
		else {
			dt[0] = BWAPI::TilePosition(x - 2, y - 2);
			dt[1] = BWAPI::TilePosition(x - 2, y + 0);
			return BWAPI::Position(32*x + 64, 32*y);
		}
	}
	//............vertical case............
	else { 
		if (dy > 0) {
			dt[0] = BWAPI::TilePosition(x - 3, y);
			dt[1] = BWAPI::TilePosition(x + 0, y);
			return BWAPI::Position(32*x, 32*y - 64);
		}
		else {
			dt[0] = BWAPI::TilePosition(x - 3, y - 2);
			dt[1] = BWAPI::TilePosition(x + 0, y - 2);
			return BWAPI::Position(32*x, 32*y + 64);
		}
	}
	return dp;
}

BWAPI::Position calculate_retreat_pos(BWAPI::Position my_pos)
{
	int i = 0;
	int x = 0;
	int y = 0;
	for (BWAPI::Unit my_min : BWAPI::Broodwar->getMinerals()) {
		if (sqdist(my_pos, my_min->getPosition()) < 65536) {
			x += my_min->getPosition().x;
			y += my_min->getPosition().y;
			i += 1;
		}
	}
	if (i > 0) { return linear_interpol_abs(my_pos, BWAPI::Position{ x/i, y/i }, 96); }
	return my_pos;
}

BWAPI::Position find_gathering_pos(BWAPI::Position p1, BWAPI::Position p2)
{
	int d1 = get_ground_dist(p1);
	int d2 = get_ground_dist(p2);
	if (d1 == -1) { d1 = d2 + 24; }
	if (d2 == -1) { d2 = d1 - 24; }
	int dm = (d1 + d2) / 2;

	BWAPI::Position min_pos = wilmap::center_pos;
	int min_dist = 67108864;

	for (int y = 2; y < (wilmap::ht - 2); y++) {
		for (int x = 2; x < (wilmap::wt - 2); x++)
		{
			if (min_dist > abs(get_ground_dist(x, y) - dm) &&
				get_ground_dist(x, y) > d1 &&
				get_ground_dist(x, y) < d2 &&
				wilmap::walk_map[y - 2][x - 1] &&
				wilmap::walk_map[y + 2][x + 1] &&
				wilmap::walk_map[y + 1][x - 2] &&
				wilmap::walk_map[y - 1][x + 2])
			{
				min_dist = abs(get_ground_dist(x, y) - dm);
				min_pos = BWAPI::Position{ 32 * x + 16 , 32 * y + 16 };
			}
		}
	}

	if (min_pos != wilmap::center_pos) {
		BWAPI::Broodwar->printf("found gather position");
	}
	return min_pos;
}




int create_natu_wallin(int i)
{
	int min_allowed_radius = 6;
	const int max_search_radius = 14;
	BWAPI::WalkPosition wr = wilmap::natu_choke_edge[2 * i];
	BWAPI::WalkPosition wl = wilmap::natu_choke_edge[2 * i + 1];
	int ar = get_direction_8(wr.x, wr.y, wl.x, wl.y);
	ar = get_map_specific_wallin_angle(wilmap::natu_tiles[i], ar);
	int ac = (ar + 6) % 8;
	int al = (ar + 4) % 8;
	//BWAPI::Broodwar->printf("directions: %d %d %d", al, ac, ar);
	if (ac % 4 == 0) { min_allowed_radius++; }

	int x0 = wilmap::natu_tiles[i].x;
	int y0 = wilmap::natu_tiles[i].y;
	if (ac == 0 || ac == 1 || ac == 7) { x0++; }
	int x1 = x0 + wilmap::cdx8[ac];
	int y1 = y0 + wilmap::cdy8[ac];
	int sv[max_search_radius] = { 0 };
	int smin = 256;
	int jmin = -1;
	int sr = -1;
	int sl = -1;
	bool has_wall_border = true;
	bool walkable_border = get_map_specific_use_walkable_border();
	BWAPI::TilePosition rv[max_search_radius] = { BWAPI::TilePositions::None };
	BWAPI::TilePosition lv[max_search_radius] = { BWAPI::TilePositions::None };

	for (int j = 0; j < max_search_radius; j++) {
		wilmap::wall_map[y0][x0] = true;
		has_wall_border = true;
		sr = -1;
		sl = -1;
		get_wall_space(sr, has_wall_border, x0, y0, x1, y1, ar);
		get_wall_space(sl, has_wall_border, x0, y0, x1, y1, al);

		if (has_wall_border || walkable_border) {
			sv[j] = 1 + sr + sl;
			rv[j] = BWAPI::TilePosition(x0 + sr * wilmap::cdx8[ar], y0 + sr * wilmap::cdy8[ar]);
			lv[j] = BWAPI::TilePosition(x0 + sl * wilmap::cdx8[al], y0 + sl * wilmap::cdy8[al]);
		}
		if (sr > sl && sl >= 0) {
			x0 += wilmap::cdx8[ar];
			y0 += wilmap::cdy8[ar];
		}
		if (sl > sr && sr >= 0) {
			x0 += wilmap::cdx8[al];
			y0 += wilmap::cdy8[al];
		}
		x1 = x0;
		y1 = y0;
		x0 += wilmap::cdx8[ac];
		y0 += wilmap::cdy8[ac];
	}
	for (int j = min_allowed_radius; j < max_search_radius; j++) {
		if (sv[j] > 0 &&
			sv[j] < smin) {
			smin = sv[j];
			jmin = j;
		}
	}
	jmin = get_map_specific_minimum(wilmap::natu_tiles[i], jmin);

	if (smin <= 16) {
		for (int j = 0; j < smin; j++) {
			x0 = rv[jmin].x + j * wilmap::cdx8[al];
			y0 = rv[jmin].y + j * wilmap::cdy8[al];
			x1 = x0 - wilmap::cdx8[ac];
			y1 = y0 - wilmap::cdy8[ac];
			wilmap::wall_map[y0][x0] = true;
			wilmap::wall_map[y1][x1] = true;
		}
		wilmap::natu_wall_tile[i][0] = rv[jmin];
		wilmap::natu_wall_tile[i][1] = lv[jmin];
		//BWAPI::Broodwar->printf("main: %d - index: %d, space: %d", i, jmin, smin);
		return jmin;
	}
	else {
		wilmap::natu_wall_tile[i][0] = BWAPI::TilePositions::None;
		wilmap::natu_wall_tile[i][1] = BWAPI::TilePositions::None;
		return -1;
	}

	//for (BWAPI::WalkPosition w : create_linear_interpolation(w0, w1)) {
	//	if (BWAPI::Broodwar->isBuildable(w.x / 4, w.y / 4)) {
	//		wilmap::wall_map[w.y / 4][w.x / 4] = true;
	//	}
	//}
	return 0;
}

void get_wall_space(int& s, bool& can_use, int x0, int y0, int x1, int y1, int a)
{
	while (BWAPI::Broodwar->isBuildable(x0, y0) &&
		BWAPI::Broodwar->isBuildable(x1, y1) &&
		x0 >= 0 && x0 < wilmap::wt &&
		y0 >= 0 && y0 < wilmap::ht) {
		x0 += wilmap::cdx8[a];
		y0 += wilmap::cdy8[a];
		x1 += wilmap::cdx8[a];
		y1 += wilmap::cdy8[a];
		s++;
	}
	if (tile_fully_walkable(x0, y0) &&
		tile_fully_walkable(x1, y1)) {
		can_use = false;
		//BWAPI::Broodwar->printf("cant use this (%d, %d)", x0, y0);
	}
	return;
}

void rearrange_natu_def_area(int i, int nd)
{
	using namespace wilmap;
	BWAPI::TilePosition tr = natu_wall_tile[i][0];
	BWAPI::TilePosition tl = natu_wall_tile[i][1];
	BWAPI::TilePosition tmc = (BWAPI::TilePosition)main_choke_pos[i];
	//set_map_specific_compare_pos(tmc, i);
	natu_wall_tile[i][0] = BWAPI::TilePositions::None;
	natu_wall_tile[i][1] = BWAPI::TilePositions::None;
	int ar = get_direction_8(tr.x, tr.y, tl.x, tl.y);
	int ac = (ar + 2) % 8;
	int al = (ar + 4) % 8;
	int space = 1 + abs(tl.x - tr.x) + abs(tl.y - tr.y);

	if (ac == 6 && space >= 7) { //up
		if (sqdist(tmc, tr) < sqdist(tmc, tl)) {
			natu_def_tile[i][0] = BWAPI::TilePosition(tr.x - 6, tr.y + 1);
			natu_def_tile[i][1] = BWAPI::TilePosition(tr.x - 3, tr.y + 1);
			natu_wall_tile[i][0] = BWAPI::TilePosition(tr.x - 3, tr.y);
			//BWAPI::Broodwar->printf("created wall facing up (r)");
		} else {
			natu_def_tile[i][0] = BWAPI::TilePosition(tl.x + 4, tl.y + 1);
			natu_def_tile[i][1] = BWAPI::TilePosition(tl.x + 2, tl.y + 1);
			natu_wall_tile[i][0] = BWAPI::TilePosition(tl.x, tl.y);
			//BWAPI::Broodwar->printf("created wall facing up (l)");
		}
	}
	if (ac == 2 && space >= 7) { //down
		if (sqdist(tmc, tr) < sqdist(tmc, tl)) {
			natu_def_tile[i][0] = BWAPI::TilePosition(tr.x + 4, tr.y - 2);
			natu_def_tile[i][1] = BWAPI::TilePosition(tr.x + 2, tr.y - 2);
			natu_wall_tile[i][0] = BWAPI::TilePosition(tr.x, tr.y - 2);
			//BWAPI::Broodwar->printf("created wall facing down (r)");
		} else {
			natu_def_tile[i][0] = BWAPI::TilePosition(tl.x - 6, tl.y - 2);
			natu_def_tile[i][1] = BWAPI::TilePosition(tl.x - 3, tl.y - 2);
			natu_wall_tile[i][0] = BWAPI::TilePosition(tl.x - 3, tl.y - 2);
			//BWAPI::Broodwar->printf("created wall facing down (l)");
		}
	}
	if (ac == 0 && space >= 6) { //right
		if (sqdist(tmc, tr) < sqdist(tmc, tl)) {
			natu_def_tile[i][0] = BWAPI::TilePosition(tr.x - 3, tr.y - 4);
			natu_def_tile[i][1] = BWAPI::TilePosition(tr.x - 2, tr.y - 2);
			natu_wall_tile[i][0] = BWAPI::TilePosition(tr.x - 3, tr.y - 2);
			//BWAPI::Broodwar->printf("created wall facing right (r)");
		}
		else {
			natu_def_tile[i][0] = BWAPI::TilePosition(tl.x - 3, tl.y + 3);
			natu_def_tile[i][1] = BWAPI::TilePosition(tl.x - 2, tl.y + 1);
			natu_wall_tile[i][0] = BWAPI::TilePosition(tl.x - 3, tl.y);
			//BWAPI::Broodwar->printf("created wall facing right (l)");
		}
		//avoid too tight space
		int xnew = natu_tiles[i].x + 6;
		int ynew = std::min(natu_def_tile[i][0].y, natu_wall_tile[i][0].y);
		if (nd < 8 && check_map_area(build_map_fix, xnew, ynew, 4, 5)) {
			natu_def_tile[i][0].x = xnew;
			natu_def_tile[i][1].x = xnew + 1;
			natu_wall_tile[i][0].x = xnew;
			//BWAPI::Broodwar->printf("move it");
		}
	}
	if (ac == 4 && space >= 6) { //left
		if (sqdist(tmc, tr) < sqdist(tmc, tl)) {
			natu_def_tile[i][0] = BWAPI::TilePosition(tr.x + 1, tr.y + 3);
			natu_def_tile[i][1] = BWAPI::TilePosition(tr.x + 1, tr.y + 1);
			natu_wall_tile[i][0] = BWAPI::TilePosition(tr.x, tr.y);
			//BWAPI::Broodwar->printf("created wall facing left (r)");
		}
		else {
			natu_def_tile[i][0] = BWAPI::TilePosition(tl.x + 1, tl.y - 4);
			natu_def_tile[i][1] = BWAPI::TilePosition(tl.x + 1, tl.y - 2);
			natu_wall_tile[i][0] = BWAPI::TilePosition(tl.x, tl.y - 2);
			//BWAPI::Broodwar->printf("created wall facing left (l)");
		}
		//avoid too tight space
		int xnew = natu_tiles[i].x - 8;
		int ynew = std::min(natu_def_tile[i][0].y, natu_wall_tile[i][0].y);
		if (nd < 8 && check_map_area(build_map_fix, xnew, ynew, 4, 5)) {
			natu_def_tile[i][0].x = xnew + 1;
			natu_def_tile[i][1].x = xnew + 1;
			natu_wall_tile[i][0].x = xnew;
			//BWAPI::Broodwar->printf("move it");
		}
	}
	return;
}



int get_map_specific_wallin_angle(BWAPI::TilePosition my_tile, int my_angle) {
	if (BWAPI::Broodwar->mapHash() == "de2ada75fbc741cfa261ee467bf6416b10f9e301" &&
		my_tile.x > 16 && my_tile.x < wilmap::wt - 16 && my_tile.y > wilmap::ht - 16) {
		//BWAPI::Broodwar->printf("set specific wall in angle: Python");
		return 0;
	}
	if (BWAPI::Broodwar->mapHash() == "1e983eb6bcfa02ef7d75bd572cb59ad3aab49285" &&
		my_tile.x < 24 && my_tile.y < 24) {
		//BWAPI::Broodwar->printf("set specific wall in angle: Andromeda");
		return 3;
	}
	return my_angle;
}

int get_map_specific_minimum(BWAPI::TilePosition my_tile, int my_min) {
	if (BWAPI::Broodwar->mapHash() == "df21ac8f19f805e1e0d4e9aa9484969528195d9f" &&
		my_tile.x > wilmap::wt / 2 && my_tile.y > wilmap::ht - 16) {
		//BWAPI::Broodwar->printf("set specific wall minimum: Jade");
		return 6;
	}
	return my_min;
}

void set_map_specific_compare_pos(BWAPI::TilePosition& my_tile, int i) {
	if (BWAPI::Broodwar->mapHash() == "0a41f144c6134a2204f3d47d57cf2afcd8430841") {
		my_tile = wilmap::natu_tiles[i];
		//BWAPI::Broodwar->printf("using specific parameters for map: MatchPoint");
	}
	return;
}

bool get_map_specific_use_walkable_border() {
	if (BWAPI::Broodwar->mapHash() == "19f00ba3a407e3f13fb60bdd2845d8ca2765cf10") {
		//BWAPI::Broodwar->printf("using specific parameters for map: NeoAztec2.1");
		return true;
	}
	return false;
}

bool check_hardcoded_wallin() {
	using namespace wilmap;
	if (BWAPI::Broodwar->mapHash() == "1e983eb6bcfa02ef7d75bd572cb59ad3aab49285")
	{
		natu_wall_tile[0][0] = BWAPI::TilePosition(22, 28);
		natu_def_tile[0][0] = BWAPI::TilePosition(25, 26);
		natu_def_tile[0][1] = BWAPI::TilePosition(23, 26);

		natu_wall_tile[1][0] = BWAPI::TilePosition(102, 28);
		natu_def_tile[1][0] = BWAPI::TilePosition(100, 26);
		natu_def_tile[1][1] = BWAPI::TilePosition(103, 26);

		natu_wall_tile[2][0] = BWAPI::TilePosition(102, 98);
		natu_def_tile[2][0] = BWAPI::TilePosition(100, 101);
		natu_def_tile[2][1] = BWAPI::TilePosition(103, 101);

		natu_wall_tile[3][0] = BWAPI::TilePosition(22, 98);
		natu_def_tile[3][0] = BWAPI::TilePosition(25, 101);
		natu_def_tile[3][1] = BWAPI::TilePosition(23, 101);

		//BWAPI::Broodwar->printf("use hardcoded wallin for: Andromeda");
		return true;
	}
	if (BWAPI::Broodwar->mapHash() == "4e24f217d2fe4dbfa6799bc57f74d8dc939d425b")
	{
		natu_wall_tile[0][0] = BWAPI::TilePosition(56, 20);
		natu_wall_tile[1][0] = BWAPI::TilePosition(36, 106);
		//BWAPI::Broodwar->printf("use hardcoded wallin for: Destination");
		return true;
	}
	if (BWAPI::Broodwar->mapHash() == "c8386b87051f6773f6b2681b0e8318244aa086a6")
	{
		natu_wall_tile[0][0] = BWAPI::TilePosition(34, 102);
		natu_def_tile[0][0] = BWAPI::TilePosition(36, 105);
		natu_def_tile[0][1] = BWAPI::TilePosition(34, 105);
		//BWAPI::Broodwar->printf("use hardcoded wallin for: Neo Moon Glaive");
		return true;
	}
	if (BWAPI::Broodwar->mapHash() == "9bfc271360fa5bab3707a29e1326b84d0ff58911")
	{
		natu_wall_tile[0][0] = BWAPI::TilePosition(21, 17);
		natu_def_tile[0][0] = BWAPI::TilePosition(22, 15);
		natu_def_tile[0][1] = BWAPI::TilePosition(22, 17);

		natu_wall_tile[1][0] = BWAPI::TilePosition(106, 41);
		natu_def_tile[1][0] = BWAPI::TilePosition(103, 42);
		natu_def_tile[1][1] = BWAPI::TilePosition(106, 42);

		natu_wall_tile[2][0] = BWAPI::TilePosition(50, 108);
		natu_def_tile[2][0] = BWAPI::TilePosition(54, 108);
		natu_def_tile[2][1] = BWAPI::TilePosition(52, 108);
		//BWAPI::Broodwar->printf("use hardcoded wallin for: Tau Cross");
		return true;
	}
	if (BWAPI::Broodwar->mapHash() == "4493525656b56eb7272c1997016345dc221fea7e")
	{
		natu_wall_tile[0][0] = BWAPI::TilePosition(109, 81);
		natu_wall_tile[0][1] = BWAPI::TilePosition(109, 77);
		natu_def_tile[0][0] = BWAPI::TilePosition(109, 79);
		natu_def_tile[0][1] = BWAPI::TilePosition(112, 79);

		natu_wall_tile[1][0] = BWAPI::TilePosition(56, 12);
		natu_wall_tile[1][1] = BWAPI::TilePosition(52, 15);
		natu_def_tile[1][0] = BWAPI::TilePosition(53, 14);
		natu_def_tile[1][1] = BWAPI::TilePosition(54, 12);

		natu_wall_tile[2][0] = BWAPI::TilePosition(14, 88);
		natu_wall_tile[2][1] = BWAPI::TilePosition(18, 92);
		natu_def_tile[2][0] = BWAPI::TilePosition(16, 91);
		natu_def_tile[2][1] = BWAPI::TilePosition(14, 91);
		//BWAPI::Broodwar->printf("use hardcoded wallin for: Medusa");
		return true;
	}
	if (BWAPI::Broodwar->mapHash() == "0a41f144c6134a2204f3d47d57cf2afcd8430841")
	{
		natu_wall_tile[1][0] = BWAPI::TilePosition(104, 51);
		natu_def_tile[1][0] = BWAPI::TilePosition(101, 51);
		natu_def_tile[1][1] = BWAPI::TilePosition(104, 51);

		natu_wall_tile[0][0] = BWAPI::TilePosition(4, 74);
		natu_def_tile[0][0] = BWAPI::TilePosition(8, 75);
		natu_def_tile[0][1] = BWAPI::TilePosition(6, 75);
		//BWAPI::Broodwar->printf("use hardcoded wallin for: Match Point");
		return true;
	}
	return false;
}





//BWAPI::TilePosition tn = get_center_tile(willyt::natu_pos);
//BWAPI::TilePosition t1 = find_path_min(8, willyt::dist_map[tn.y][tn.x] - 8);
//BWAPI::TilePosition t2 = find_path_min(willyt::dist_map[t1.y][t1.x] + 16, willyt::dist_map[tn.y][tn.x] + 24);
//clear_map_bool(willyt::choke_map);
//
//if (t1) {
//	std::vector<int> v1;
//	flood_fill_choke(t1.x, t1.y, willyt::space_map[t1.y][t1.x] + 3, v1);
//	main_choke_pos = get_center(v1);
//	my_main_def = find_def_area(tmc, 0, dist_map[tmc.y][tmc.x] - 6);
//	my_main_def = fill_def_area(BWAPI::Position(tmc), my_main_def, main_def_tiles);
//}
//
//if (t2) {
//	std::vector<int> v2;
//	flood_fill_choke(t2.x, t2.y, willyt::space_map[t2.y][t2.x] + 3, v2);
//	natu_choke_pos = get_center(v2);
//	my_natu_def = find_def_area(tnc, 0, dist_map[tnc.y][tnc.x] - 6);
//	my_natu_def = fill_def_area(BWAPI::Position(tnc), my_natu_def, natu_def_tiles);
//}

//BWAPI::TilePosition ArmyPositions::find_path_min(int d0, int d1) {
//	BWAPI::TilePosition tmin = BWAPI::TilePositions::None;
//	BWAPI::TilePosition t = get_center_tile(willyt::start_pos);
//	int cmin = 1024;
//	int d = 0;
//	while (d <= d1) {
//		t = path_fore(t);
//		if (d >= d0 &&
//			cmin > willyt::space_map[t.y][t.x]) {
//			cmin = willyt::space_map[t.y][t.x];
//			tmin = t;
//		}
//		if (d == willyt::dist_map[t.y][t.x]) { break; }
//		else { d = willyt::dist_map[t.y][t.x]; }
//	}
//	return tmin;
//}
//
//BWAPI::TilePosition ArmyPositions::path_fore(BWAPI::TilePosition t) {
//	int x = t.x;
//	int y = t.y;
//	int i = willyt::dist_map[y][x];
//	if (willyt::path_map[y + 1][x + 1] == i + 3) { return BWAPI::TilePosition(x + 1, y + 1); }
//	if (willyt::path_map[y - 1][x + 1] == i + 3) { return BWAPI::TilePosition(x + 1, y - 1); }
//	if (willyt::path_map[y + 1][x - 1] == i + 3) { return BWAPI::TilePosition(x - 1, y + 1); }
//	if (willyt::path_map[y - 1][x - 1] == i + 3) { return BWAPI::TilePosition(x - 1, y - 1); }
//	if (willyt::path_map[y + 1][x] == i + 2) { return BWAPI::TilePosition(x, y + 1); }
//	if (willyt::path_map[y - 1][x] == i + 2) { return BWAPI::TilePosition(x, y - 1); }
//	if (willyt::path_map[y][x + 1] == i + 2) { return BWAPI::TilePosition(x + 1, y); }
//	if (willyt::path_map[y][x - 1] == i + 2) { return BWAPI::TilePosition(x - 1, y); }
//	return t;
//}
//
//void ArmyPositions::flood_fill_choke(int x, int y, int z, std::vector<int> &v) {
//	if (willyt::space_map[y][x] != -1 &&
//		willyt::space_map[y][x] <= z &&
//		willyt::choke_map[y][x] == false) {
//		willyt::choke_map[y][x] = true;
//		v.push_back(x);
//		v.push_back(y);
//		flood_fill_choke(x + 1, y, z, v);
//		flood_fill_choke(x, y + 1, z, v);
//		flood_fill_choke(x - 1, y, z, v);
//		flood_fill_choke(x, y - 1, z, v);
//	}
//}

//BWAPI::Position ArmyPositions::get_center(std::vector<int> &v) {
//	int x = 0;
//	int y = 0;
//	for (std::vector<int>::iterator it = v.begin(); it != v.end(); it += 2) {
//		x += *it;
//		y += *(it + 1);
//	}
//	if (v.size() == 0) { return BWAPI::Positions::None; }
//	return BWAPI::Position(16 + 64 * x / v.size(), 16 + 64 * y / v.size());
//}






//void MapAnalysis::create_width_map() {
//	clear_map_int(willyt::width_map);
//	BWAPI::Position pm = willyt::start_pos;
//	BWAPI::Position pn = willyt::natu_pos;
//	BWAPI::TilePosition tm = BWAPI::TilePosition(pm.x/32 - 1, pm.y/32);
//	BWAPI::TilePosition tn = BWAPI::TilePosition(pn.x/32 - 1, pn.y/32);
//	BWAPI::TilePosition t0;
//	BWAPI::TilePosition t1 = tm;
//	BWAPI::TilePosition t2 = path_fore(tm);
//	int n = 0;
//
//	while (n < 128) {
//		t0 = t1;
//		t1 = t2;
//		t2 = path_fore(t2);
//
//		int dx1 = t1.x - t0.x;
//		int dy1 = t1.y - t0.y;
//		int dx2 = t2.x - t1.x;
//		int dy2 = t2.y - t1.y;
//
//		willyt::width_map[t1.y][t1.x] = (measure_width(t1, dx1, dy1) + measure_width(t1, dx2, dy2)) / 2;
//		if ( n == willyt::dist_map[t1.y][t1.x] ) { return; }
//		else { n = willyt::dist_map[t1.y][t1.x]; }
//	}
//}

//int MapAnalysis::measure_width(BWAPI::TilePosition t, int dx, int dy) {
//	int i = 0;
//	if (dx != 0 && dy != 0) {
//		i += 3;
//		i += measure_side(t, -dy, +dx, 3);
//		i += measure_side(t, +dy, -dx, 3);
//	}
//	else {
//		i += 2;
//		i += measure_side(t, -dy, +dx, 2);
//		i += measure_side(t, +dy, -dx, 2);
//	}
//	return i;
//}

//int MapAnalysis::measure_side(BWAPI::TilePosition t, int dx, int dy, int di) {
//	int i = 0;
//	for (int n = 1; n <= 24/di; n++) {
//		if (willyt::walk_map[t.y + n * dy][t.x + n * dx]) { i += di; }
//		else { break; }
//	}
//	return i;
//}