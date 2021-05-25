#include "ExpoManager.h"

void ExpoManager::init()
{
	init_all_resources();
	fill_build_map_with_expos();
	determine_islands();
	determine_natu_all();
	determine_defs_all();
	determine_expo_circle();
	willyt::go_islands = decide_islands();
	return;
}

void ExpoManager::occupy_expo(BWAPI::Unit my_unit)
{
	BWAPI::TilePosition my_tile = my_unit->getTilePosition();
	for (Expo& my_expo : wilexpo::all) {
		if (my_expo.tile == my_tile)
		{
			if (my_unit->isCompleted()) {
				my_expo.is_constructing = false;
				set_expo_resources(my_expo, true);
			} else {
				my_expo.is_constructing = true;
			}
			my_expo.is_owned = true;
			//BWAPI::Broodwar->printf("Occupying expo at [%d, %d].", my_tile.x, my_tile.y);
		}
	}
	if (willyt::has_natural == false &&
		wilmap::natu_tiles[wilmap::mm].x == my_tile.x + 1 &&
		wilmap::natu_tiles[wilmap::mm].y == my_tile.y + 1) {
		willyt::has_natural = true;
		//BWAPI::Broodwar->printf("Occupying the natural expansion!");
	}
	return;
}

void ExpoManager::desert_expo(BWAPI::TilePosition &tile)
{
	for (Expo &my_expo : wilexpo::all) {
		if (my_expo.tile == tile) {
			set_expo_resources(my_expo, false);
			my_expo.is_owned = false;
			my_expo.is_constructing = false;
			//BWAPI::Broodwar->printf("Deserting expo at [%d, %d].", tile.x, tile.y);
		}
	}
	return;
}

void ExpoManager::set_expo_resources(Expo& my_expo, bool my_value)
{
	for (BWAPI::Unit& u : my_expo.minerals)
		if (u->exists() || my_value == false)
			set_resource_owned(wilbuild::minerals, u->getID(), my_value);
	for (BWAPI::Unit& u : my_expo.geysers)
		if (u->exists() || my_value == false)
			set_resource_owned(wilbuild::geysers, u->getID(), my_value);
	return;
}

//MINERAL_LOCK_UPDATE
//for (BWAPI::Unit& my_min : my_expo.minerals) {
//	vector_remove(wilbuild::minerals, my_min);
//	vector_remove(wilbuild::mainland_minerals, my_min);
//}
//MINERAL_LOCK_UPDATE
//for (BWAPI::Unit& my_gas : my_expo.geysers) {
//	vector_remove(wilbuild::geysers, my_gas);
//	vector_remove(wilbuild::mainland_geysers, my_gas);
//}

//MINERAL_LOCK_UPDATE
//wilbuild::minerals.push_back(my_min);
//if (!my_expo.is_island) { wilbuild::mainland_minerals.push_back(my_min); }
//MINERAL_LOCK_UPDATE
//wilbuild::geysers.push_back(my_gey);
//if (!my_expo.is_island) { wilbuild::mainland_geysers.push_back(my_gey); }

void ExpoManager::destroy_mineral(BWAPI::Unit my_min)
{
	set_map_area(wilmap::build_map_var, my_min, true);
	remove_resource(wilbuild::minerals, my_min);

	for (Expo& my_expo : wilexpo::all) {
		if (vector_holds(my_expo.minerals, my_min)) {
			vector_remove(my_expo.minerals, my_min);
			vector_remove(my_expo.min_tiles, my_min->getTilePosition());
			break;
		}
	}
	//BWAPI::Broodwar->printf("mineral patch mined out");
	return;
}

void ExpoManager::destroy_geyser(BWAPI::Unit my_gas)
{
	remove_resource(wilbuild::geysers, my_gas);

	for (Expo& my_expo : wilexpo::all) {
		if (vector_holds(my_expo.geysers, my_gas)) {
			vector_remove(my_expo.geysers, my_gas);
			break;
		}
	}
	//BWAPI::Broodwar->printf("destroyed: refinery/extractor/assimilator");
	return;
}

void ExpoManager::rebuild_geyser(BWAPI::Unit my_gas)
{
	if (BWAPI::Broodwar->getFrameCount() == 0) { return; } //dont do it on start
	wilbuild::geysers.push_back(Resource(my_gas));

	for (Expo& my_expo : wilexpo::all) {
		if (vector_holds(my_expo.gas_tiles, my_gas->getTilePosition())) {
			my_expo.geysers.push_back(my_gas);
			if (my_expo.is_owned) { wilbuild::geysers.back().is_owned = true; }
			if (my_expo.is_island) { wilbuild::geysers.back().is_island = true; }
			break;
		}
	}
	//BWAPI::Broodwar->printf("reinstalling drivers for Resource_Vespene_Geyser");
	return;
}

//MINERAL_LOCK_UPDATE
//vector_remove(wilbuild::all_minerals, my_min);
//vector_remove(wilbuild::all_mintiles, my_min->getTilePosition());
//		if (it->is_owned) {
//			vector_remove(wilbuild::minerals, my_min);
//			vector_remove(wilbuild::mainland_minerals, my_min);
//		}

//MINERAL_LOCK_UPDATE
//if (my_expo.is_owned) { vector_remove(wilbuild::geysers, my_gas); }
//MINERAL_LOCK_UPDATE
//if (it->is_owned) { wilbuild::geysers.push_back(my_gas); }

void ExpoManager::set_enemy_expo(BWAPI::TilePosition tile, bool status) {
	for (Expo& my_expo : wilexpo::all) {
		if (my_expo.tile == tile) {
			my_expo.is_enemy = status;
			return;
		}
	}
	return;
}




void ExpoManager::update(std::vector<SCV>& scvs) {
	for (std::vector<Expo>::iterator it = wilexpo::all.begin(); it != wilexpo::all.end(); ++it) {
		if (it->is_constructing) { check_construction(*it); }
		if (it->is_owned) { update_miners(*it, scvs); }
	}
	return;
}

void ExpoManager::check_construction(Expo& my_expo) {
	if (all_visible(my_expo.min_tiles) &&
		all_visible(my_expo.gas_tiles)) {
		for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsOnTile(my_expo.tile)) {
			if (u->getType() == BWAPI::UnitTypes::Terran_Command_Center &&
				u->getHitPoints() > 750)
			{
				my_expo.is_constructing = false;
				set_expo_resources(my_expo, true);
			}
		}
	}
	return;
}

void ExpoManager::update_miners(Expo& my_expo, std::vector<SCV>& scvs) {
	my_expo.min_miner_count = 0;
	my_expo.gas_miner_count = 0;
	for (SCV& scv : scvs) {
		if (scv.unit->exists() && sqdist(scv.unit->getPosition(), my_expo.posi) < 65536)
		{
			if (scv.unit->isGatheringMinerals()) { my_expo.min_miner_count++; }
			if (scv.unit->isGatheringGas()) { my_expo.gas_miner_count++; }
			scv.set_expo_pos(my_expo.posi);
		}
	}
	return;
}

void ExpoManager::correct() {
	for (std::vector<Expo>::iterator expo = wilexpo::all.begin(); expo != wilexpo::all.end(); ++expo)
	{
		if (expo->minerals.empty() == false &&
			all_visible(expo->min_tiles) == true &&
			all_existing(expo->minerals) == false)
		{
			expo->minerals.clear();
			for (BWAPI::TilePosition t : expo->min_tiles)
				for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsOnTile(t))
					if (u->getType().isMineralField())
						expo->minerals.push_back(u);
			//BWAPI::Broodwar->printf("EXPO: recounting %d mineral field(s)", expo->minerals.size());
		}
		if (expo->gas_tiles.empty() == false &&
			all_visible(expo->gas_tiles) == true &&
			expo->geysers.size() < expo->gas_tiles.size())
		{
			expo->geysers.clear();
			for (BWAPI::TilePosition t : expo->gas_tiles)
				for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsOnTile(t))
					if (u->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser ||
						u->getType().isRefinery())
						expo->geysers.push_back(u);
			//BWAPI::Broodwar->printf("EXPO: recounting %d vespene geyser(s)", expo->geysers.size());
		}

		if (expo->is_enemy &&
			BWAPI::Broodwar->isVisible(expo->tile) &&
			BWAPI::Broodwar->getUnitsOnTile(expo->tile).empty()) {
			expo->is_enemy = false;
			//BWAPI::Broodwar->printf("EXPO: enemy expo disappeared");
		}
	}
	return;
}

bool ExpoManager::all_visible(std::vector<BWAPI::TilePosition>& my_vec) {
	for (BWAPI::TilePosition t : my_vec)
		if (!BWAPI::Broodwar->isVisible(t))
			return false;
	return true;
}

bool ExpoManager::all_existing(std::vector<BWAPI::Unit>& my_vec) {
	for (BWAPI::Unit u : my_vec)
		if (!u->exists())
			return false;
	return true;
}

void ExpoManager::init_all_resources()
{
	for (BWAPI::Unit u : BWAPI::Broodwar->getStaticMinerals()) {
		wilbuild::minerals.push_back(Resource(u));
	}
	for (BWAPI::Unit u : BWAPI::Broodwar->getStaticGeysers()) {
		wilbuild::geysers.push_back(Resource(u));
	}
	return;
}

//MINERAL_LOCK_UPDATE
//all_minerals.push_back(u);
//all_mintiles.push_back(u->getTilePosition());
//MINERAL_LOCK_UPDATE
//all_geysers.push_back(u);
//all_gastiles.push_back(u->getTilePosition());

bool ExpoManager::decide_islands()
{
	for (Expo &expo : wilexpo::all) {
		if (expo.is_island) { return true; }
	}
	return false;
}

void ExpoManager::set_resource_owned(std::vector<Resource>& my_vec, int id, bool my_value)
{
	for (Resource& r : my_vec) {
		if (r.unit->getID() == id) {
			r.is_owned = my_value;
			return;
		}
	}
	return;
}

void ExpoManager::remove_resource(std::vector<Resource>& my_vec, BWAPI::Unit my_unit) {
	for (std::size_t i = 0; i < my_vec.size(); ++i) {
		if (my_vec[i].unit == my_unit) {
			my_vec.erase(my_vec.begin() + i);
			return;
		}
	}
	return;
}
