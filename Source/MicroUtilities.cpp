#include "MicroUtilities.h"


bool reaching_area(BWAPI::Unit my_unit, int(&my_map)[1024][1024]) {
	BWAPI::Position p0 = my_unit->getPosition();
	BWAPI::Position p1 = estimate_next_pos(my_unit, 16);
	if (my_map[p0.y / 8][p0.x / 8] > 0) return true;
	if (my_map[p1.y / 8][p1.x / 8] > 0) return true;
	return false;
}
BWAPI::Position get_retreat_vector(BWAPI::Position p0, BWAPI::Position pe, int d) {
	BWAPI::Position vr = linear_interpol_abs(p0, willyt::retreat_pos, d);
	BWAPI::Position ve = linear_interpol_abs(p0, pe, -d);
	BWAPI::Position vm = BWAPI::Position((vr.x + ve.x) / 2, (vr.y + ve.y) / 2);
	BWAPI::Position p1 = linear_interpol_abs(p0, vm, d);

	if (p1.x < 16 || p1.x > (wilmap::wp - 16) ||
		p1.y < 16 || p1.y > (wilmap::hp - 48)) {
		vm = linear_interpol_abs(p1, wilmap::center_pos, d);
		vm = BWAPI::Position((p1.x + vm.x) / 2, (p1.y + vm.y) / 2);
		p1 = linear_interpol_abs(p0, vm, d);
	}
	return p1;
}



BWAPI::Unit get_target_from(std::vector<BWAPI::Unit>& v, BWAPI::Unit u, BWAPI::Position p, int r) {
	if (u != NULL) return u;
	u = get_closest(v, p);
	if (u != NULL && sqdist(p, u->getPosition()) < r) return u;
	return NULL;
}
BWAPI::Unit get_maximum_target(std::vector<BWAPI::Unit>& v, BWAPI::TilePosition t, int r) {
	BWAPI::Unit max_unit = NULL;
	int max_hp = 0;
	for (BWAPI::Unit u : v) {
		if (sqdist(u->getTilePosition(), t) <= r &&
			!vector_holds(willyt::target_vector, u) &&
			max_hp < u->getHitPoints() + u->getShields()) {
			max_hp = u->getHitPoints() + u->getShields();
			max_unit = u;
		}
	}
	return max_unit;
}



BWAPI::Position get_mine_position() {
	int x = 0;
	int y = 0;
	int n = wilenemy::unclaimed_expo_pos.size();
	if (n > 0 && rand() % 3 == 0) {
		BWAPI::Position p = wilenemy::unclaimed_expo_pos[rand() % n];
		x = rand() % 12 - 6 + p.x / 32;
		y = rand() % 11 - 6 + p.y / 32;
	}
	else if (BWAPI::Broodwar->getFrameCount() < 14400) {
		x = rand() % 32 - 16 + wilmap::my_entrance.x;
		y = rand() % 32 - 16 + wilmap::my_entrance.y;
	}
	else {
		x = rand() % wilmap::wt;
		y = rand() % (wilmap::ht - 1);
	}
	if (x != 0 && y != 0 &&
		!BWAPI::Broodwar->isVisible(x, y) &&
		get_ground_dist(x, y) > 0 &&
		wilmap::build_map_var[y][x]) {
		return BWAPI::Position(32 * x + 16, 32 * y + 16);
	}
	return BWAPI::Positions::None;
}
BWAPI::Position get_current_target_position(BWAPI::Unit u) {
	if (u->getTarget() != NULL &&
		u->getTarget()->getPosition().isValid()) {
		return u->getTarget()->getPosition();
	}
	if (u->getTargetPosition().isValid()) {
		return u->getTargetPosition();
	}
	return BWAPI::Positions::None;
}



bool check_defense_siegetile(BWAPI::TilePosition t) {
	if (!willyt::has_natural && wilmap::maindefmap[t.y][t.x]) { return true; }
	if (willyt::has_natural && wilmap::natudefmap[t.y][t.x]) { return true; }
	return false;
}
BWAPI::Position choose_defense_siegetile() {
	if (willyt::strategy == 4 && willyt::my_time > 600) { return get_natural_defense_pos(); }
	if (willyt::has_natural) { return get_natural_defense_pos(); }
	if (!willyt::has_natural) { return get_random_pos(wilmap::my_maindefvec); }
	return BWAPI::Positions::None;
}
BWAPI::Position get_random_pos(std::vector<BWAPI::TilePosition>& my_vec) {
	if (!my_vec.empty()) {
		int i = rand() % my_vec.size();
		BWAPI::TilePosition t = my_vec[i];
		if (!is_none(t) &&
			wilmap::build_map_var[t.y][t.x] &&
			BWAPI::Broodwar->getUnitsOnTile(t).empty()) {
			return BWAPI::Position(32 * t.x + 16, 32 * t.y + 16);
		}
	}
	return BWAPI::Positions::None;
}
BWAPI::Position get_natural_defense_pos() {
	if (willyt::highground_defense_tanks + willyt::planned_highground_defense < 4 &&
		wilmap::my_highdefvec.size() > 3)
	{
		willyt::planned_highground_defense++;
		return get_random_pos(wilmap::my_highdefvec);
	}
	return get_random_pos(wilmap::my_natudefvec);
}



bool check_venture_out(BWAPI::Position my_pos) {
	int bunker_sqdist = sqdist(my_pos, wilmap::my_bunker_def_pos);
	if (bunker_sqdist <= 25600 &&
		sqdist(my_pos, wilmap::my_natu) >= bunker_sqdist + wilmap::bunker_natu_sqdist) {
		return true;
	}
	return false;
}



void draw_arrow(BWAPI::Position p0, BWAPI::Position p1, BWAPI::Color c) {
	if (sqdist(p0, p1) <= 65536) {
		BWAPI::Broodwar->drawLineMap(p0, p1, c);
	} else {
		BWAPI::Broodwar->drawLineMap(p0, linear_interpol_abs(p0, p1, 128), c);
	}
	return;
}
void draw_arrow(BWAPI::Unit u, BWAPI::Position p, BWAPI::Color c) {
	draw_arrow(u->getPosition(), p, c);
	return;
}
void draw_arrow(BWAPI::Unit u0, BWAPI::Unit u1, BWAPI::Color c) {
	draw_arrow(u0->getPosition(), u1->getPosition(), c);
	return;
}
void draw_text(BWAPI::Unit u, const char* s, BWAPI::Text::Enum c) {
	BWAPI::Broodwar->drawTextMap(u->getPosition(), "%c%s", c, s);
	return;
}
void draw_pos_box(BWAPI::Position p, int s, BWAPI::Color c) {
	if (p.isValid()) {
		BWAPI::Broodwar->drawBoxMap(p.x - s, p.y - s, p.x + s, p.y + s, c);
	}
}