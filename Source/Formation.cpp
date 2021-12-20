#include "Formation.h"

namespace formation
{
	std::vector<BWAPI::Position> formvec;
	int formcount = 0;
}


void update_formation(BWAPI::Position mypos)
{
	if (willyt::strategy <= 2 && formation::formcount == 0 &&
		!willyt::proxy_prod_alert && !willyt::cannon_rush_alert)
	{
		BWAPI::Position enpos = get_closest(wilenemy::positions, mypos);
		if (!is_none(enpos) && sqdist(mypos, enpos) < 589824) {
			create_formation(mypos, enpos);
		}
	}
	return;
}


void create_formation(BWAPI::Position mypos, BWAPI::Position enpos)
{
	using namespace formation;
	formvec.clear();
	int h0 = BWAPI::Broodwar->getGroundHeight(mypos.x / 32, mypos.y / 32);
	double a0 = get_angle(enpos, mypos);
	int segcount = 0;
	int segstart = 0;
	for (int i = -8; i < 9; i++)
	{
		double a = std::fmod(a0 + i * 0.1, 6.2832);
		BWAPI::Position p = get_circle_pos(enpos, 320, a);
		formvec.push_back(p);

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
	formvec.erase(formvec.begin(), formvec.begin() + segstart);
	formcount++;
	BWAPI::Broodwar->printf("created formation with %d segments", formvec.size());
	return;
}




//BWAPI::Position centerpos = linear_interpol_abs(enpos, mypos, 320);
//int xs = centerpos.x - (enpos.y - centerpos.y);
//int ys = centerpos.y + (enpos.x - centerpos.x);
//BWAPI::Position sidepos = BWAPI::Position(xs, ys);
//formation::formvec.push_back(linear_interpol_abs(centerpos, sidepos, 160));
//formation::formvec.push_back(linear_interpol_abs(centerpos, sidepos, -160));