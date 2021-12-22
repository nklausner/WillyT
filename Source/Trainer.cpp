#include "Trainer.h"

void Trainer::init() {
	scv_rax_count = 11;
	scv_fe_count = 15;
	tank_goli_ratio = 5;
	number_fact_per_addon = 1.5;
	n_dropship = 0;

	if (willyt::strategy == 1) {
		scv_rax_count = 10;
		//if (wilenemy::race == BWAPI::Races::Zerg) {
		//	willyt::eight_rax = true;
		//	scv_rax_count = 8;
		//}
	}
	if (willyt::strategy == 3) {
		tank_goli_ratio = (wilenemy::race == BWAPI::Races::Zerg) ? 2 : 3;
		number_fact_per_addon = 2.0;
	}
	if (willyt::strategy == 4) {
		tank_goli_ratio = (wilenemy::race == BWAPI::Races::Zerg) ? 2 : 6;
	}
	if (willyt::strategy != 6) {
		n_dropship++;
	}
	if (willyt::go_islands) {
		n_dropship++;
	}
	return;
}

void Trainer::train() {
	build_addons(wilbuild::commandcenters, BWAPI::UnitTypes::Terran_Comsat_Station);
	if (willyt::strategy != 6) {
		build_addons(wilbuild::starports, BWAPI::UnitTypes::Terran_Control_Tower);
	}

	if (willyt::strategy <= 3) {
		build_addons(wilbuild::sciencefacs, BWAPI::UnitTypes::Terran_Covert_Ops);
	}
	if (willyt::strategy == 6) {
		build_addons(wilbuild::sciencefacs, BWAPI::UnitTypes::Terran_Physics_Lab);
	}
	if (willyt::strategy != 6 &&
		number_fact_per_addon * count_addon(wilbuild::factories, 0) < (float)wilbuild::factories.size()) {
		build_addons(wilbuild::factories, BWAPI::UnitTypes::Terran_Machine_Shop);
	}

	if (!wilbuild::commandcenters.empty() &&
		BWAPI::Broodwar->self()->minerals() >= 50) {
		BWAPI::Unit my_cc = find_idle(wilbuild::commandcenters);
		if (my_cc) { train_commandc(my_cc); }
	}
	if (!wilbuild::barracks.empty() &&
		!saving_for_expo() &&
		!waiting_for_dropship() &&
		BWAPI::Broodwar->self()->minerals() >= 100) {
		BWAPI::Unit my_rax = find_idle(wilbuild::barracks);
		if (my_rax) { train_barracks(my_rax); }
	}
	if (willyt::strategy != 6 &&
		!wilbuild::factories.empty() &&
		!saving_for_expo() &&
		!waiting_for_dropship() &&
		BWAPI::Broodwar->self()->minerals() >= 100) {
		train_factories();
	}
	if (!wilbuild::starports.empty() &&
		!saving_for_expo() &&
		BWAPI::Broodwar->self()->minerals() >= 100) {
		BWAPI::Unit my_sta = find_idle(wilbuild::starports);
		if (my_sta) { train_starports(my_sta); }
	}

	if (!wilbuild::barracks.empty() && willyt::is_rushing) {
		train_for_rush();
	}
	//if (willyt::strategy == 4 && !is_none(wilmap::my_sneaky_tile)) {
	//	train_sneaky();
	//}
}

void Trainer::train_commandc(BWAPI::Unit my_cc) {
	if (wilunits::tcount_scv < willyt::mining_supply &&
		!wait_workers()) {
		my_cc->train(BWAPI::UnitTypes::Terran_SCV);
		return;
	}
}

void Trainer::train_barracks(BWAPI::Unit my_rax) {
	if (is_spooky()) {
		my_rax->train(BWAPI::UnitTypes::Terran_Ghost);
		return;
	}
	if (willyt::go_bio &&
		wilbuild::academies.size() > 0 &&
		wilbuild::academies.front()->isCompleted() &&
		BWAPI::Broodwar->self()->gas() >= 25) {
		if (5 * wilunits::tcount_medic < wilunits::tcount_marine + wilunits::tcount_firebat) {
			my_rax->train(BWAPI::UnitTypes::Terran_Medic);
			return;
		}
		if (need_a_light()) {
			my_rax->train(BWAPI::UnitTypes::Terran_Firebat);
			return;
		}
	}
	if (need_rines()) {
		my_rax->train(BWAPI::UnitTypes::Terran_Marine);
		return;
	}
}

void Trainer::train_factories() {
	BWAPI::Unit facaddon = find_idle_addon(wilbuild::factories);
	BWAPI::Unit facnaked = find_idle_naked(wilbuild::factories);
	if (count_naked(wilbuild::factories, 0) == 0) { facnaked = facaddon; }

	if (can_build_goliath()) {
		if (facnaked && need_missiles_normal()) {
			facnaked->train(BWAPI::UnitTypes::Terran_Goliath);
			return;
		}
		if (facnaked && need_missiles_urgent()) {
			facnaked->train(BWAPI::UnitTypes::Terran_Goliath);
			return;
		}
		if (facaddon && need_missiles_urgent()) {
			facaddon->train(BWAPI::UnitTypes::Terran_Goliath);
			return;
		}
	}
	if (facnaked && bring_it_on()) {
		facnaked->train(BWAPI::UnitTypes::Terran_Vulture);
		return;
	}
	if (facaddon && !saving_for_siege() && !saving_for_starport()) {
		facaddon->train(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
		return;
	}
}

void Trainer::train_starports(BWAPI::Unit my_sta) {
	if (!wilbuild::controltowers.empty() &&
		wilunits::dropships.size() < n_dropship) {
		my_sta->train(BWAPI::UnitTypes::Terran_Dropship);
		return;
	}
	if (willyt::strategy == 6) {
		my_sta->train(BWAPI::UnitTypes::Terran_Wraith);
		return;
	}
	//if (!wilbuild::physicslabs.empty()) {
	//	my_sta->train(BWAPI::UnitTypes::Terran_Battlecruiser);
	//	return;
	//}
}

void Trainer::train_for_rush() {
	BWAPI::Unit my_rax = get_in_range(wilbuild::barracks, wilmap::en_start, 1024);
	if (my_rax != NULL &&
		my_rax->isCompleted() &&
		my_rax->isIdle() &&
		!my_rax->isLifted() &&
		BWAPI::Broodwar->self()->minerals() >= 50) {
		my_rax->train(BWAPI::UnitTypes::Terran_Marine);
	}
}

void Trainer::train_sneaky() {
	for (BWAPI::Unit& my_unit : wilbuild::factories) {
		if (my_unit->getTilePosition() == wilmap::my_sneaky_tile &&
			my_unit->isIdle() &&
			!my_unit->isLifted() &&
			BWAPI::Broodwar->self()->minerals() >= 1000 &&
			BWAPI::Broodwar->self()->gas() < 1000) {
			my_unit->train(BWAPI::UnitTypes::Terran_Vulture);
		}
	}
}



BWAPI::Unit Trainer::find_idle(std::vector<BWAPI::Unit> &my_list) {
	for (std::vector<BWAPI::Unit>::reverse_iterator rit = my_list.rbegin(); rit != my_list.rend(); ++rit)
		if ((*rit)->isCompleted() && (*rit)->isIdle() && !(*rit)->isLifted() &&
			!is_under_threat_static_def(*rit))
			return (*rit);
	return NULL;
}
BWAPI::Unit Trainer::find_idle_addon(std::vector<BWAPI::Unit> &my_list) {
	for (BWAPI::Unit& my_unit : my_list)
		if (my_unit->isCompleted() && my_unit->isIdle() && !my_unit->isLifted() &&
			my_unit->getAddon() != NULL)
			return my_unit;
	return NULL;
}
BWAPI::Unit Trainer::find_idle_naked(std::vector<BWAPI::Unit> &my_list) {
	for (BWAPI::Unit &my_unit : my_list)
		if (my_unit->isCompleted() && my_unit->isIdle() && !my_unit->isLifted() &&
			my_unit->getAddon() == NULL &&
			my_unit->getTilePosition() != wilmap::my_sneaky_tile)
			return my_unit;
	return NULL;
}



bool Trainer::need_a_light() {
	int f = 4;
	if (wilenemy::race != BWAPI::Races::Terran &&
		wilenemy::small_percentage >= 75) {
		f = 1;
	}
	if (wilenemy::race == BWAPI::Races::Zerg &&
		willyt::my_time < 600 && wilenemy::has_lair) {
		f = 6; //incoming mutalisks or lurkers
	}
	if (f * wilunits::tcount_firebat < wilunits::tcount_marine &&
		willyt::strategy != 6) {
		return true;
	}
	return false;
}
bool Trainer::need_rines() {
	if (willyt::rush_alert)
		return true;
	if (saving_for_refinery())
		return false;
	if ((willyt::strategy == 3 || willyt::strategy == 4) &&
		(BWAPI::Broodwar->elapsedTime() >= 600 ||
		wilunits::tcount_marine >= 4 ||
		(willyt::avoid_grddef && willyt::proxy_alert)))
		return false;
	if (willyt::strategy == 6 &&
		wilunits::tcount_marine >= 16 &&
		wilbuild::starports.size() >= 2)
		return false;
	return true;
}



bool Trainer::need_missiles_normal() {
	if (wilunits::goliaths.size() * tank_goli_ratio < wilunits::siegetanks.size())
		return true;
	if (wilunits::goliaths.size() == 0 && wilenemy::supply_air > 0)
		return true;
	return false;
}
bool Trainer::need_missiles_urgent() {
	if (wilenemy::air_percentage >= 50)
		return true;
	return false;
}
bool Trainer::can_build_goliath() {
	if (!wilbuild::armories.empty() && wilbuild::armories.front()->isCompleted())
		return true;
	return false;
}



bool Trainer::bring_it_on() {
	if (willyt::strategy <= 3 &&
		wilunits::siegetanks.size() >= 2 &&
		wilunits::siegetanks.size() > wilunits::vultures.size() &&
		BWAPI::Broodwar->self()->minerals() >= (int)willyt::my_time &&
		BWAPI::Broodwar->self()->minerals() >= 2 * BWAPI::Broodwar->self()->gas()) {
		return true;
	}
	if (willyt::strategy == 3 &&
		BWAPI::Broodwar->elapsedTime() < 900 &&
		!wilbuild::machineshops.empty() &&
		(wilunits::siegetanks.size() >= 2 || wilenemy::race == BWAPI::Races::Zerg) &&
		wilunits::vultures.size() < 2) {
		return true;
	}
	if (willyt::strategy == 4 &&
		wilbuild::factories.size() >= 3 &&
		wilbuild::factories.back()->isCompleted() &&
		wilunits::vultures.size() < 6 &&
		BWAPI::Broodwar->self()->minerals() >= 1500 &&
		BWAPI::Broodwar->self()->gas() < 1000) {
		return true;
	}	
	return false;
}
bool Trainer::is_spooky() {
	if (wilbuild::covertops.size() > 0 &&
		wilbuild::covertops.front()->isCompleted() &&
		(BWAPI::Broodwar->self()->gas() > BWAPI::Broodwar->self()->minerals() ||
		is_spooky_time())) {
		return true;
	}
	return false;
}
bool Trainer::is_spooky_time() {
	if (wilenemy::race != BWAPI::Races::Zerg &&
		wilenemy::air_percentage >= 50 &&
		BWAPI::Broodwar->self()->gas() > 200 * (int)wilunits::ghosts.size() + 200) {
		return true;
	}
	return false;
}



bool Trainer::saving_for_expo() {
	if (BWAPI::Broodwar->elapsedTime() > 720 &&
		BWAPI::Broodwar->self()->minerals() < 500 &&
		wilunits::tcount_scv >= willyt::mining_supply &&
		willyt::mineral_count < 12)
		return true;
	if (willyt::fast_expand &&
		wilbuild::commandcenters.size() < 2)
		return true;
	return false;
}
bool Trainer::saving_for_refinery() {
	if (wilbuild::refineries.empty() &&
		willyt::geyser_count > 0 && !gas_is_stolen())
	{
		if (willyt::fast_expand &&
			willyt::strategy == 3 &&
			wilenemy::race != BWAPI::Races::Zerg &&
			wilenemy::supply_army == 0) {
			return true;
		}
		if (wilbuild::academies.empty() &&
			willyt::strategy == 1 &&
			BWAPI::Broodwar->self()->supplyUsed() >= 42) {
			return true;
		}
	}
	return false;
}
bool Trainer::saving_for_siege() {
	if ((willyt::strategy == 3 ||
		willyt::strategy == 4) &&
		!willyt::siege_started)
		return true;
	return false;
}
bool Trainer::waiting_for_dropship() {
	if (wilunits::dropships.size() < n_dropship &&
		find_idle_addon(wilbuild::starports))
		return true;
	return false;
}
bool Trainer::saving_for_starport() {
	if (willyt::strategy == 4 &&
		wilbuild::starports.empty() &&
		willyt::my_time > 450 &&
		BWAPI::Broodwar->self()->gas() < 200) {
		return true;
	}
	return false;
}
bool Trainer::gas_is_stolen() {
	for (Resource& r : wilbuild::geysers) {
		if (r.is_owned &&
			r.unit->exists() &&
			r.unit->getType().isRefinery() &&
			r.unit->getPlayer() != BWAPI::Broodwar->self()) {
			return true;
		}
	}
	return false;
}



void Trainer::build_addons(std::vector<BWAPI::Unit> my_vec, BWAPI::UnitType my_addon) {
	for (BWAPI::Unit &my_unit : my_vec) {
		if (my_unit->isCompleted() &&
			my_unit->isIdle() &&
			!my_unit->getAddon() &&
			my_unit->canBuildAddon() &&
			my_unit->getTilePosition() != wilmap::my_sneaky_tile) {
			my_unit->buildAddon(my_addon);
			return;
		}
	}
}
unsigned Trainer::count_addon(std::vector<BWAPI::Unit> my_vec, int i) {
	for (BWAPI::Unit& my_unit : my_vec)
		if (my_unit->isCompleted() && my_unit->getAddon() != NULL)
			i++;
	return i;
}
unsigned Trainer::count_naked(std::vector<BWAPI::Unit> my_vec, int i) {
	for (BWAPI::Unit& my_unit : my_vec)
		if (my_unit->isCompleted() && my_unit->getAddon() == NULL)
			i++;
	return i;
}



bool Trainer::wait_workers() {
	if (willyt::strategy == 1 && wilunits::tcount_scv >= 10) {
		int su = BWAPI::Broodwar->self()->supplyUsed(BWAPI::Races::Terran);
		int st = BWAPI::Broodwar->self()->supplyTotal(BWAPI::Races::Terran);
		if (st - su <= 4) {
			//BWAPI::Broodwar->printf("Halt SCV production.");
			return true;
		}
	}
	if (wilbuild::barracks.empty() && wilunits::tcount_scv >= scv_rax_count &&
		BWAPI::Broodwar->self()->minerals() < 200) {
		return true;
	}
	if (wilbuild::factories.empty() && wilunits::tcount_scv >= 18 &&
		willyt::strategy == 3 && !willyt::fast_expand) {
		return true;
	}
	if (wilbuild::commandcenters.size() < 2 && wilunits::tcount_scv >= scv_fe_count &&
		willyt::fast_expand &&
		BWAPI::Broodwar->self()->minerals() < 450) {
		return true;
	}
	return false;
}
bool Trainer::is_under_threat_static_def(BWAPI::Unit u) {
	if (willyt::cannon_rush_alert &&
		check_map_area(wilthr::grddef, u->getTilePosition().x, u->getTilePosition().y, u->getType().tileWidth(), u->getType().tileHeight(), 4) == false) {
		BWAPI::Broodwar->printf("under threat static defense");
		return true;
	}
	return false;
}

