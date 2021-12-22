#include "Formation.h"

namespace formation
{
	std::vector<BWAPI::Position> line0;
	std::vector<BWAPI::Position> line1;
	std::vector<BWAPI::Position> line2;
	std::vector<BWAPI::Position> line3;
	int formcount = 0;
	bool useformation = false;
	bool isassigned = false;
}


void update_formation(BWAPI::Position mypos)
{
	if (willyt::strategy <= 2 && formation::formcount == 0 &&
		!willyt::proxy_prod_alert && !willyt::cannon_rush_alert)
	{
		BWAPI::Position enpos = get_closest(wilenemy::positions, mypos);
		if (!is_none(enpos) && sqdist(mypos, enpos) < 262144)
		{
			formation::line0.clear();
			formation::line1.clear();
			formation::line2.clear();
			formation::line3.clear();
			formation::isassigned = false;
			BWAPI::Position mycenter = find_formation_center(mypos, enpos);
			int x0 = mycenter.x / 8;
			int y0 = mycenter.y / 8;
			switch (get_direction_8(enpos, mycenter))
			{
			case 0: create_formation_orth(x0, y0, 0, 1, 1, 0); break;
			case 2: create_formation_orth(x0, y0, 1, 0, 0, 1); break;
			case 4: create_formation_orth(x0, y0, 0, -1, -1, 0); break;
			case 6: create_formation_orth(x0, y0, -1, 0, 0, -1); break;
			case 1: create_formation_diag(x0, y0, -1, 1, 1, 1); break;
			case 3: create_formation_diag(x0, y0, -1, -1, -1, 1); break;
			case 5: create_formation_diag(x0, y0, 1, -1, -1, -1); break;
			case 7: create_formation_diag(x0, y0, 1, 1, 1, -1); break;
			default: break;
			}
			formation::formcount++;
			BWAPI::Broodwar->printf("created formation");
		}
	}
	return;
}

BWAPI::Position find_formation_center(BWAPI::Position mypos, BWAPI::Position enpos)
{
	std::vector<BWAPI::Position> myvec = {};
	int h0 = BWAPI::Broodwar->getGroundHeight(mypos.x / 32, mypos.y / 32);
	double a0 = get_angle(enpos, mypos);
	int segcount = 0;
	int segstart = 0;
	for (int i = -8; i < 9; i++)
	{
		double a = std::fmod(a0 + i * 0.1, 6.2832);
		BWAPI::Position p = get_circle_pos(enpos, 320, a);
		myvec.push_back(p);

		if (BWAPI::Broodwar->isWalkable(p.x / 8, p.y / 8) &&
			BWAPI::Broodwar->getGroundHeight(p.x / 32, p.y / 32) == h0) {
			segcount++;
		}
		else {
			if (segcount >= 6) {
				segstart = i + 8 - segcount;
				break;
			}
			else {
				segcount = 0;
			}
		}
	}
	if (segcount >= 6) {
		segstart = 17 - segcount;
	}
	myvec.erase(myvec.begin(), myvec.begin() + segstart);
	return get_center_of_mass(myvec);
}



void create_formation_orth(int x0, int y0, int dx, int dy, int ddx, int ddy)
{
	create_formation_line_orth(formation::line0, x0, y0, dx, dy, ddx, ddy);
	create_formation_line_orth(formation::line1, x0 + 3 * ddx, y0 + 3 * ddy, dx, dy, ddx, ddy);
	create_formation_line_orth(formation::line2, x0 + 6 * ddx, y0 + 6 * ddy, dx, dy, ddx, ddy);
	return;
}

void create_formation_diag(int x0, int y0, int dx, int dy, int ddx, int ddy)
{
	create_formation_line_diag(formation::line0, x0, y0, dx, dy, ddx, ddy);
	create_formation_line_diag(formation::line1, x0 + 3 * ddx, y0, dx, dy, ddx, ddy);
	create_formation_line_diag(formation::line2, x0 + 3 * ddx, y0 + 3 * ddy, dx, dy, ddx, ddy);
	create_formation_line_diag(formation::line3, x0 + 6 * ddx, y0 + 3 * ddy, dx, dy, ddx, ddy);
	return;
}

void create_formation_line_orth(std::vector<BWAPI::Position>& myline, int x0, int y0, int dx, int dy, int ddx, int ddy)
{
	BWAPI::WalkPosition w1 = get_maximum_extend(x0, y0, +dx, +dy, ddx, ddy, 30);
	BWAPI::WalkPosition w2 = get_maximum_extend(x0, y0, -dx, -dy, ddx, ddy, 30);
	int n = (abs(w2.x - w1.x) + abs(w2.y - w1.y)) / 3;
	int x = 8 * w2.x + 12 * dx + 12 * ddx;
	int y = 8 * w2.y + 12 * dy + 12 * ddy;
	for (int i = 0; i < n; i++) {
		myline.push_back(BWAPI::Position(x + 24 * i * dx, y + 24 * i * dy));
	}
	//BWAPI::Broodwar->printf("line: %d spots", n);
	return;
}

void create_formation_line_diag(std::vector<BWAPI::Position>& myline, int x0, int y0, int dx, int dy, int ddx, int ddy)
{
	BWAPI::WalkPosition w1 = get_maximum_extend(x0, y0, +dx, +dy, ddx, ddy, 20);
	BWAPI::WalkPosition w2 = get_maximum_extend(x0, y0, -dx, -dy, ddx, ddy, 20);
	int n = (abs(w2.x - w1.x) + abs(w2.y - w1.y)) / 3;
	int x = 8 * w2.x + 20 * dx;
	int y = 8 * w2.y + 4 * dy;
	for (int i = 0; i < n; i++) {
		myline.push_back(BWAPI::Position(x + 24 * i * dx, y + 24 * i * dy));
	}
	//BWAPI::Broodwar->printf("line: %d spots", n);
	return;
}

BWAPI::WalkPosition get_maximum_extend(int x, int y, int dx, int dy, int ddx, int ddy, int imax)
{
	for (int i = 0; i < imax; i++) {
		if (x >= 0 && x < wilmap::ww &&
			y >= 0 && y < wilmap::hw &&
			BWAPI::Broodwar->isWalkable(x, y) &&
			BWAPI::Broodwar->isWalkable(x + ddx, y + ddy) &&
			BWAPI::Broodwar->isWalkable(x + ddx + ddx, y + ddy + ddy)) {
			x = x + dx;
			y = y + dy;
		}
		else {
			return BWAPI::WalkPosition(x, y);
		}
	}
	return BWAPI::WalkPosition(x, y);
}



//BWAPI::Position centerpos = linear_interpol_abs(enpos, mypos, 320);
//int xs = centerpos.x - (enpos.y - centerpos.y);
//int ys = centerpos.y + (enpos.x - centerpos.x);
//BWAPI::Position sidepos = BWAPI::Position(xs, ys);
//formation::formvec.push_back(linear_interpol_abs(centerpos, sidepos, 160));
//formation::formvec.push_back(linear_interpol_abs(centerpos, sidepos, -160));