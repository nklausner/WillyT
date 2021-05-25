#include "Cleaner.h"

void clean() {
	adress_renegade_scv();
	reset_planned_builds();
	reset_blocked_tiles();
	//correct_all_minerals();
	willyt::blocked_mainchoke = check_blocked_entrance();
	clear_blocked_entrance();
	return;
}

void adress_renegade_scv() {
	std::vector<int> my_vec = {};
	my_vec.reserve(64);
	for (SCV& scv : wilunits::scvs) {
		my_vec.push_back(scv.id);
	}
	for (BWAPI::Unit unit : BWAPI::Broodwar->self()->getUnits()) {
		if (unit->getType().isWorker() &&
			unit->exists() &&
			unit->isIdle() &&
			unit->isCompleted() &&
			std::find(my_vec.begin(), my_vec.end(), unit->getID()) == my_vec.end())
		{
			wilunits::scvs.push_back(SCV(unit));
			BWAPI::Broodwar->printf("adressing renegade scv %d", unit->getID());
			return;
		}
	}
	return;
}

void reset_planned_builds() {
	using namespace wilbuild;
	if (wilbuild::planned_builds.size() == 0) {
		planned_build_queue = 0;
	}
	else {
		++planned_build_queue;
	}
	if (planned_build_queue >= 8) {
		planned_build_queue = 0;
		planned_builds.clear();
		//BWAPI::Broodwar->printf("clearing planned builds");
	}
	return;
}

void reset_blocked_tiles() {
	wilbuild::blocked_tile = BWAPI::TilePositions::None;
	wilbuild::scanned_blocked_tile = false;
	return;
}

void correct_all_minerals() {
	//MINERAL_LOCK_UPDATE
	//for (unsigned i = 0; i < wilbuild::all_mintiles.size(); i++) {
	//	if (BWAPI::Broodwar->isVisible(wilbuild::all_mintiles.at(i)) &&
	//		wilbuild::all_minerals.at(i)->exists() == false) {
	//		wilbuild::all_minerals.erase(wilbuild::all_minerals.begin() + i);
	//		wilbuild::all_mintiles.erase(wilbuild::all_mintiles.begin() + i);
	//		//BWAPI::Broodwar->printf("removing mined out mineral");
	//		return;
	//	}
	//}
	return;
}

bool check_blocked_entrance() {
	using namespace wilunits;
	if (BWAPI::Broodwar->getFrameCount() > 21600 &&
		BWAPI::Broodwar->self()->supplyUsed() > 200 &&
		willyt::strategy != 6) {
		int n = 0;
		int d = get_ground_dist(wilmap::main_choke_pos[wilmap::mm]);
		if (willyt::def_target_pos != BWAPI::Positions::None &&
			get_ground_dist(willyt::def_target_pos) < d) {	//except defense in main
			return false;
		}
		for (Fighter2& f : siegetanks)
			if (get_ground_dist(f.unit) < d) n += 2;
		for (Fighter2& f : goliaths)
			if (get_ground_dist(f.unit) < d) n += 2;
		for (Fighter2& f : vultures)
			if (get_ground_dist(f.unit) < d) n += 2;
		if (2 * n >= willyt::sup_mech) {			//criterium: more than half of mech units in main
			BWAPI::Broodwar->printf("main choke must be blocked");
			return true;
		}
	}
	return false;
}

void clear_blocked_entrance() {
	wilenemy::tlself.clear();
	if (willyt::blocked_mainchoke) {
		BWAPI::Position pc = wilmap::main_choke_pos[wilmap::mm];
		BWAPI::Position pd = wilmap::main_def_pos[wilmap::mm];
		for (BWAPI::Unit u : wilbuild::bunkers)
			if (sqdist(u->getPosition(), pc) <= 65536 &&
				sqdist(u->getPosition(), pd) <= 65536)
				wilenemy::tlself.push_back(u);
		for (BWAPI::Unit u : wilbuild::turrets)
			if (sqdist(u->getPosition(), pc) <= 65536 &&
				sqdist(u->getPosition(), pd) <= 65536)
				wilenemy::tlself.push_back(u);
	}
	return;
}
