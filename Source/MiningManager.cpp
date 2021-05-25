#include "MiningManager.h"

void MiningManager::update()
{
	count_resource_containers();
	check_gas();
	check_min();
	check_remove_mineral_blocks();
	return;
}


void MiningManager::check_gas() {
	if (wilbuild::refineries.empty() ||
		!wilbuild::refineries.back()->isCompleted()) {
		return;
	}
	int t = BWAPI::Broodwar->elapsedTime();
	int g = BWAPI::Broodwar->self()->gas();
	int m = BWAPI::Broodwar->self()->minerals();
	unsigned i = 0;
	for (Expo &my_expo : wilexpo::all) {
		i += my_expo.gas_miner_count;
	}
	if (i < 3 * wilbuild::refineries.size()) {
		if (g < t / 4 ||
			g < m / 4) {
			append_gas_miner();
		}
	}
	if (willyt::strategy == 1 &&
		i > 0 &&
		g > m &&
		g > t - 300) {
		remove_gas_miner(0);
	}
	if (willyt::strategy == 3 &&
		BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode) &&
		i > 0 &&
		g > m &&
		g > t) {
		remove_gas_miner(0);
	}
	if (willyt::strategy == 6 &&
		BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Yamato_Gun) &&
		i > 0 &&
		g > m &&
		g > t) {
		remove_gas_miner(0);
	}
	remove_gas_miner(3);
	return;
}
void MiningManager::start_gas_mining(BWAPI::Unit my_refinery) {
	append_gas_miner();
	return;
}


void MiningManager::append_gas_miner()
{
	for (Expo &expo : wilexpo::all) {
		for (BWAPI::Unit &geys : expo.geysers) {
			if (geys->getType().isRefinery() &&
				geys->isCompleted() &&
				geys->getPlayer() == BWAPI::Broodwar->self() &&
				geys->isBeingGathered() == false)
			{
				for (SCV& scv : wilunits::scvs) {
					if (scv.expo_pos == expo.posi &&
						scv.unit->isGatheringMinerals() &&
						!scv.unit->isCarryingMinerals())
					{
						scv.change_resource(geys, false);
						//BWAPI::Broodwar->printf("append SCV %d to gas", scv.id);
						return;
					}
				}
			}
		}
	}
	return;
}
void MiningManager::remove_gas_miner(int factor)
{
	for (Expo &expo : wilexpo::all) {
		if (expo.gas_miner_count > factor * expo.geysers.size())
		{
			for (SCV& scv : wilunits::scvs) {
				if (scv.expo_pos == expo.posi &&
					scv.unit->getOrder() == BWAPI::Orders::MoveToGas)
				{
					scv.end_mining();
					scv.unit->stop();
					//BWAPI::Broodwar->printf("remove SCV %d from gas", scv.id);
					return;
				}
			}
		}
	}
	return;
}


void MiningManager::check_min()
{
	for (SCV& scv : wilunits::scvs) {
		if (scv.unit->isIdle() &&
			!scv.unit->getTransport() &&
			scv.get_resource_container() &&
			(!scv.get_resource_container()->exists() ||
			scv.get_resource_container()->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser))
		{
			scv.end_mining();
			scv.unit->stop();
			//BWAPI::Broodwar->printf("redistribute SCV %d", scv.id);
			return;
		}
	}
	BWAPI::Unit my_min = NULL;
	if (willyt::min_scv_count_min < 2) {
		for (Resource& res : wilbuild::minerals) {
			if (res.is_owned && res.scv_count == 3)
			{
				my_min = res.unit;
				break;
			}
		}
	}
	if (my_min) {
		for (SCV& scv : wilunits::scvs) {
			if (scv.get_resource_container() == my_min &&
				scv.unit->getOrder() == BWAPI::Orders::MoveToMinerals)
			{
				scv.end_mining();
				scv.unit->stop();
				//BWAPI::Broodwar->printf("redistribute SCV %d", scv.id);
				return;
			}
		}
	}
	return;
}
void MiningManager::start_min_mining(BWAPI::Unit my_cc)
{
	for (SCV& scv : wilunits::scvs) {
		if (scv.expo_pos == my_cc->getPosition() &&
			scv.unit->getOrder() == BWAPI::Orders::ReturnMinerals)
		{
			scv.unit->rightClick(my_cc);
			//BWAPI::Broodwar->printf("reroute SCV %d to new cc", scv.id);
		}
	}
}
void MiningManager::end_min_mining(BWAPI::Unit my_mineral)
{
	for (SCV& scv : wilunits::scvs) {
		if (scv.get_resource_container() == my_mineral)
		{
			scv.end_mining();
			scv.unit->stop();
			//BWAPI::Broodwar->printf("mined out, stop SCV %d", scv.id);
		}
	}
}


std::vector<Expo>::iterator MiningManager::get_oversaturated_expo() {
	std::vector<Expo>::iterator it_max = wilexpo::all.end();
	int dn_max = 0;
	for (std::vector<Expo>::iterator it = wilexpo::all.begin(); it != wilexpo::all.end(); ++it) {
		it->is_oversaturated = false;
		int dn = (it->min_miner_count - 1) - 2 * it->minerals.size();
		if (it->is_owned &&
			dn > dn_max) {
			dn_max = dn;
			it_max = it;
		}
	}
	if (it_max != wilexpo::all.end()) {
		it_max->is_oversaturated = true;
	}
	return it_max;
}
std::vector<Expo>::iterator MiningManager::get_undrsaturated_expo() {
	std::vector<Expo>::iterator it_max = wilexpo::all.end();
	int dn_max = 0;
	for (std::vector<Expo>::iterator it = wilexpo::all.begin(); it != wilexpo::all.end(); ++it) {
		it->is_undrsaturated = false;
		int dn = (it->min_miner_count + 1) - 2 * it->minerals.size();
		if (it->is_owned &&
			dn < dn_max) {
			dn_max = dn;
			it_max = it;
		}
	}
	if (it_max != wilexpo::all.end()) {
		it_max->is_undrsaturated = true;
	}
	return it_max;
}
////MINERAL_LOCK_UPDATE
//std::vector<Expo>::iterator it_over = get_oversaturated_expo();
//std::vector<Expo>::iterator it_undr = get_undrsaturated_expo();
//if (it_over != wilexpo::all.end() &&
//	it_undr != wilexpo::all.end()) {
//	if (it_over->is_island || it_undr->is_island) {
//		it_over->min_miners.front().move(it_undr->posi);
//	} else {
//		it_over->min_miners.front().change_resource(it_undr->minerals.front());
//	}
//	//BWAPI::Broodwar->printf("transfer miner");
//}


void MiningManager::count_resource_containers()
{
	willyt::mineral_count = 0;
	willyt::min_scv_count_min = 256;
	for (Resource& r : wilbuild::minerals) {
		if (r.is_owned) {
			willyt::mineral_count++;
			if (willyt::min_scv_count_min > r.scv_count) {
				willyt::min_scv_count_min = r.scv_count;
			}
		}
	}
	willyt::geyser_count = 0;
	willyt::min_scv_count_gas = 256;
	for (Resource& r : wilbuild::geysers) {
		if (r.is_owned) {
			willyt::geyser_count++;
			if (willyt::min_scv_count_gas > r.scv_count) {
				willyt::min_scv_count_gas = r.scv_count;
			}
		}
	}
	return;
}


void MiningManager::check_gather() {
	//for (BWAPI::Unit min : wilbuild::minerals) {
	//	if (min->isBeingGathered()) {
	//		BWAPI::Broodwar->drawCircleMap(min->getPosition(), 8, BWAPI::Colors::Purple, true);
	//	} else {
	//		BWAPI::Broodwar->drawCircleMap(min->getPosition(), 8, BWAPI::Colors::Cyan, true);
	//	}
	//}
	//for (std::vector<SCV>::iterator it = wilunits::scvs.begin(); it != wilunits::scvs.end(); ++it) {
	//	BWAPI::Unit u = it->unit->getOrderTarget();
	//	if (it->is_miner == false && u != NULL && u->getType().isResourceContainer()) {
	//		it->is_miner = true;
	//		BWAPI::Broodwar->drawLineMap(it->unit->getPosition(), u->getPosition(), BWAPI::Colors::White);
	//		BWAPI::Broodwar->drawTextMap(it->unit->getPosition(), "append miner");
	//	}
	//	if ((it->is_miner == true && u == NULL) ||
	//		(it->is_miner == true && !u->getType().isResourceContainer() && !u->getType().isResourceDepot())) {
	//		it->is_miner = false;
	//		BWAPI::Broodwar->drawTextMap(it->unit->getPosition(), "remove miner");
	//	}
	//}
	return;
}

void MiningManager::check_remove_mineral_blocks() {
	safe_sum(remove_mineral_block_queue, -1);

	if (remove_mineral_block_queue == 0 &&
		willyt::my_time > wilbuild::locking_time &&
		vector_holds(wilbuild::planned_builds, BWAPI::UnitTypes::Terran_Command_Center))
	{
		for (BWAPI::Unit n : BWAPI::Broodwar->getNeutralUnits()) {
			if (n->getType().isMineralField() &&
				n->getResources() <= 8 &&
				n->isBeingGathered() == false) {
				std::vector<SCV>::iterator it = choose_scv(n->getTilePosition(), true, true);
				if (it != wilunits::scvs.end()) {
					it->change_resource(n, true);
					remove_mineral_block_queue = 8;
					//BWAPI::Broodwar->printf("remove zero mineral block");
					return;
				}
			}
		}
		//BWAPI::Broodwar->printf("check for mineral blocks");
	}
	return;
}