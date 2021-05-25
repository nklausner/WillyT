#include "IslandManager.h"

void IslandManager::update() {
	if (willyt::go_islands) {
		it_dropship = wilunits::dropships.begin();
		if (it_dropship != wilunits::dropships.end()) {
			switch (island_steps) {
			case 0: find_island(0); break;
			case 1: pick_up_scv(1); break;
			case 2: drop_island(2); break;
			case 3: mine_blocks(3); break;
			case 4: build_com(4); break;
			}
		}
	}
	return;
}

void IslandManager::find_island(int i) {
	int min_dist = 65536;
	it_island = wilexpo::all.end();
	for (auto it = wilexpo::all.begin(); it != wilexpo::all.end(); ++it) {
		if (!it->is_owned &&
			!it->is_enemy &&
			it->is_island) {
			if (min_dist > sqdist(wilmap::my_start, it->tile)) {
				min_dist = sqdist(wilmap::my_start, it->tile);
				it_island = it;
			}
		}
	}
	if (it_island != wilexpo::all.end()) {
		island_steps = i + 1;
		BWAPI::Broodwar->printf("Found island.");
	}
	return;
}

void IslandManager::pick_up_scv(int i) {
	if (it_dropship->unit->isIdle()) {
		auto it_scv = choose_scv(it_dropship->unit->getTilePosition(), true, true);
		if (it_scv != wilunits::scvs.end()) {
			it_scv->unit->load(it_dropship->unit);
			BWAPI::Broodwar->printf("Picking up unit.");
		}
	}
	int new_load = it_dropship->unit->getLoadedUnits().size();
	if (new_load > dropship_load) {
		dropship_load = new_load;
		island_steps = i + 1;
	}
	return;
}

void IslandManager::drop_island(int i) {
	if (it_dropship->unit->isIdle()) {
		it_dropship->unit->unloadAll(it_island->posi);
		BWAPI::Broodwar->printf("Dropping unit.");
	}
	for (auto it = wilunits::scvs.begin(); it != wilunits::scvs.end(); ++it) {
		if (it->unit->getTransport() == it_dropship->unit &&
			it->is_islander == false) {
			it->is_islander = true;
			BWAPI::Broodwar->printf("You are going to an island in the sun.");
		}
	}
	int new_load = it_dropship->unit->getLoadedUnits().size();
	if (new_load == 0) {
		dropship_load = new_load;
		island_steps = i + 1;
	}
	return;
}

void IslandManager::mine_blocks(int i) {
	int x0 = it_island->posi.x - 160;
	int x1 = it_island->posi.x + 159;
	int y0 = it_island->posi.y - 144;
	int y1 = it_island->posi.y + 143;
	auto it_scv = choose_scv(it_island->tile, true, true);
	for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsInRectangle(x0, y0, x1, y1)) {
		if (it_scv != wilunits::scvs.end() &&
			it_scv->unit->isIdle()) {
			if (it_scv->unit->canGather(u)) {
				it_scv->unit->gather(u);
				BWAPI::Broodwar->printf("Mine blocking mineral.");
				return;
			}
			if (it_scv->unit->canAttack(u)) {
				it_scv->unit->attack(u);
				BWAPI::Broodwar->printf("Attack blocking unit.");
				return;
			}
		}
	}
	island_steps = i + 1;
	return;
}

void IslandManager::build_com(int i) {
	auto it_scv = choose_scv(it_island->tile, true, true);
	if (it_scv != wilunits::scvs.end() &&
		it_scv->unit->isIdle() &&
		it_scv->unit->canBuild(BWAPI::UnitTypes::Terran_Command_Center, it_island->tile)) {
		it_scv->unit->build(BWAPI::UnitTypes::Terran_Command_Center, it_island->tile);
	}
	if (it_island->is_owned) {
		island_steps = i + 1;
	}
	return;
}