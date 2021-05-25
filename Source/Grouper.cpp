#include "Grouper.h"

namespace wilgroup
{
	std::vector<int> maxsqdist_vec = { 1048576 , 262144 , 65536 };
	std::vector<BWAPI::Unit> player_grd_vec = {};
	std::vector<BWAPI::Unit> player_air_vec = {};
	std::vector<BWAPI::Unit> enemy_grd_vec = {};
	BWAPI::Position player_grd_pos = BWAPI::Positions::None;
	BWAPI::Position player_air_pos = BWAPI::Positions::None;
	BWAPI::Position enemy_grd_pos = BWAPI::Positions::None;
	int player_grd_supply = 0;
	int player_air_supply = 0;
	int enemy_grd_supply = 0;
}

void update_army_groups() {
	using namespace wilgroup;
	player_grd_vec.clear();
	player_air_vec.clear();
	enemy_grd_vec.clear();
	BWAPI::Position last_pos = enemy_grd_pos;

	//determine player army group

	for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
		if (!u->getType().isBuilding() && !u->getType().isWorker() &&
			u->getType().supplyRequired() != 0 && u->isCompleted())
		{
			if (u->getType().isFlyer()) {
				player_air_vec.push_back(u);
			} else {
				player_grd_vec.push_back(u);
			}
		}
	}

	update_group(player_grd_vec, player_grd_pos, player_grd_supply);
	update_group(player_air_vec, player_air_pos, player_air_supply);

	//determine enemy army group

	for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
		for (BWAPI::Unit u : p->getUnits()) {
			if (!u->getType().isBuilding() && !u->getType().isWorker() &&
				u->getType().supplyRequired() != 0 && u->isCompleted() &&
				!u->getType().isFlyer())
			{
				enemy_grd_vec.push_back(u);
			}
		}
	}
	update_group(enemy_grd_vec, enemy_grd_pos, enemy_grd_supply);

	if (enemy_grd_pos == BWAPI::Positions::None) {
		enemy_grd_pos = last_pos;
	}

	return;
}

void update_group(std::vector<BWAPI::Unit>& my_vec, BWAPI::Position& my_pos, int& my_sup) {
	if (!my_vec.empty()) {
		for (int maxsqdist : wilgroup::maxsqdist_vec) {
			my_pos = calculate_center_pos(my_vec);
			my_vec = reduce_army_group(my_vec, my_pos, maxsqdist);
		}
	}
	my_pos = calculate_center_pos(my_vec);
	my_sup = calculate_army_supply(my_vec);

}

BWAPI::Position calculate_center_pos(std::vector<BWAPI::Unit>& my_vec) {
	int s = 0;
	int x = 0;
	int y = 0;
	for (BWAPI::Unit u : my_vec) {
		x += (u->getType().supplyRequired() * u->getPosition().x);
		y += (u->getType().supplyRequired() * u->getPosition().y);
		s += u->getType().supplyRequired();
	}
	if (s != 0) { return BWAPI::Position( x/s , y/s ); }
	return BWAPI::Positions::None;
}

std::vector<BWAPI::Unit> reduce_army_group(std::vector<BWAPI::Unit>& old_vec, BWAPI::Position& my_pos, int& maxsqdist) {
	std::vector<BWAPI::Unit> new_vec = {};
	for (BWAPI::Unit u : old_vec) {
		if (sqdist(u->getPosition(), my_pos) < maxsqdist) {
			new_vec.push_back(u);
		}
	}
	return new_vec;
}

int calculate_army_supply(std::vector<BWAPI::Unit>& my_vec) {
	int s = 0;
	for (BWAPI::Unit u : my_vec) {
		s += u->getType().supplyRequired();
	}
	return s;
}