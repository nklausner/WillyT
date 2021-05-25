#include "EnemyUtilities.h"


void collect_intruders(std::vector<BWAPI::Unit>& my_vec, int(&my_map)[256][256]) {
	my_vec.clear();
	for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
		for (BWAPI::Unit u : p->getUnits()) {
			if (u->exists() && !u->getType().isInvincible()) {
				BWAPI::TilePosition t = u->getTilePosition();
				if (my_map[t.y][t.x] >= 0) {
					my_vec.push_back(u);
				}
			}
		}
	}
	return;
}
int evaluate_strength(std::vector<BWAPI::Unit>& my_vec) {
	int i = 0;
	for (BWAPI::Unit u : my_vec) {
		if (u->getType() == BWAPI::UnitTypes::Zerg_Overlord) { i += 2; }
		else if (u->getType().isWorker()) { ++i; }
		else { i += u->getType().supplyRequired(); }
	}
	return i * 3 / 2;
}
int evaluate_airstrength(std::vector<BWAPI::Unit>& my_vec) {
	int i = 0;
	for (BWAPI::Unit u : my_vec) {
		if (u->isFlying()) { i += u->getType().supplyRequired(); }
		if (u->getType() == BWAPI::UnitTypes::Zerg_Overlord) { i += 2; }
	}
	return i * 3 / 2;
}
int count_supply(std::vector<BWAPI::Unit>& my_vec) {
	int s = 0;
	for (BWAPI::Unit u : my_vec)
		s += u->getType().supplyRequired();
	return s;
}

