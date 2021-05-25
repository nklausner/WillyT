#include "ExpoList.h"

void ExpoList::occupy_expo(BWAPI::TilePosition &tile) {
	for (Expo &my_expo : all) {
		if (my_expo.tile == tile) {
			for (BWAPI::Unit &my_min : my_expo.minerals) {
				if (my_min->exists()) { wilbuild::minerals.push_back(my_min); }
			}
			for (BWAPI::Unit &my_gey : my_expo.geysers) {
				if (my_gey->exists()) { wilbuild::geysers.push_back(my_gey); }
			}
			my_expo.is_owned = true;
			BWAPI::Broodwar->printf("Occupying expo at [%d, %d].", tile.x, tile.y);
		}
	}
}

void ExpoList::desert_expo(BWAPI::TilePosition &tile) {
	for (Expo &my_expo : all) {
		if (my_expo.tile == tile) {
			for (BWAPI::Unit &my_min : my_expo.minerals) {
				vector_remove_unit(wilbuild::minerals, my_min);
			}
			for (BWAPI::Unit &my_gey : my_expo.geysers) {
				vector_remove_unit(wilbuild::geysers, my_gey);
			}
			my_expo.is_owned = false;
			BWAPI::Broodwar->printf("Deserting expo at [%d, %d].", tile.x, tile.y);
		}
	}
}

void ExpoList::mined_out(BWAPI::Unit my_min) {
	for (Expo &expo : all) {
		if (std::find(expo.minerals.begin(), expo.minerals.end(), my_min) != expo.minerals.end()) {
			vector_remove_unit(expo.minerals, my_min);
		}
	}
	set_map_area_unit(wilmap::build_map, my_min, true);
	vector_remove_unit(wilbuild::minerals, my_min);
	//BWAPI::Broodwar->printf("mineral patch mined out");
}

void ExpoList::destroy_geyser(BWAPI::Unit my_gas) {
	for (std::vector<Expo>::iterator it = all.begin(); it != all.end(); ++it) {
		if (std::find(it->geysers.begin(), it->geysers.end(), my_gas) != it->geysers.end()) {
			vector_remove_unit(it->geysers, my_gas);
			destroyed_geyser_expo = it;
		}
	}
	if (my_gas->getPlayer() == BWAPI::Broodwar->self())
		vector_remove_unit(wilbuild::geysers, my_gas);
	//BWAPI::Broodwar->printf("beware geyser destroyed");
}

void ExpoList::rebuild_geyser(BWAPI::Unit my_gas) {
	if (destroyed_geyser_expo != all.end()) {
		if (destroyed_geyser_expo->is_owned) {
			wilbuild::geysers.push_back(my_gas);
		}
		destroyed_geyser_expo->geysers.push_back(my_gas);
		destroyed_geyser_expo = all.end();
		//BWAPI::Broodwar->sendText("reinstalling drivers for Resource_Vespene_Geyser");
	}
}

void ExpoList::update() {
	for (std::vector<Expo>::iterator expo = all.begin(); expo != all.end(); ++expo) {
		if (!expo->is_owned) { continue; }
		expo->min_miners.clear();
		expo->gas_miners.clear();

		for (BWAPI::Unit u : BWAPI::Broodwar->getAllUnits()) {
			if (u->getPlayer() == BWAPI::Broodwar->self() &&
				u->getType().isWorker() &&
				u->getDistance(expo->posi) < 256) {
				if (u->isGatheringMinerals())	{ expo->min_miners.push_back(u); }
				if (u->isGatheringGas())		{ expo->gas_miners.push_back(u); }
			}
		}
	}
}