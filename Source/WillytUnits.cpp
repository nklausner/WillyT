#include "WillytUnits.h"

namespace wilunits {

	std::vector<SCV> scvs;
	std::vector<Fighter2> marines;
	std::vector<Fighter2> firebats;
	std::vector<Fighter2> medics;
	std::vector<Fighter2> ghosts;
	std::vector<Fighter2> siegetanks;
	std::vector<Fighter2> goliaths;
	std::vector<Fighter2> vultures;
	std::vector<Flyer> wraiths;
	std::vector<Flyer> dropships;
	std::vector<Flyer> valkyries;
	std::vector<Flyer> vessels;
	std::vector<Flyer> cruisers;

	int tcount_scv = 0;
	int tcount_marine = 0;
	int tcount_firebat = 0;
	int tcount_medic = 0;
	int tcount_ghost = 0;
	int tcount_siegetank = 0;
	int tcount_goliath = 0;
	int tcount_vulture = 0;
	int tcount_wraith = 0;
	int tcount_dropship = 0;
	int tcount_valkyrie = 0;
	int tcount_vessel = 0;
	int tcount_cruiser = 0;

	bool has_scout = false;

}





void UnitManager::tcount_unit(BWAPI::Unit unit) {
	using namespace wilunits;
	using namespace BWAPI::UnitTypes;

	switch (unit->getType()) {
	case Terran_SCV:					++tcount_scv; break;
	case Terran_Marine:					++tcount_marine; break;
	case Terran_Firebat:				++tcount_firebat; break;
	case Terran_Medic:					++tcount_medic; break;
	case Terran_Ghost:					++tcount_ghost; break;
	case Terran_Siege_Tank_Tank_Mode:	++tcount_siegetank; break;
	case Terran_Siege_Tank_Siege_Mode:	++tcount_siegetank; break;
	case Terran_Goliath:				++tcount_goliath; break;
	case Terran_Vulture:				++tcount_vulture; break;
	case Terran_Wraith:					++tcount_wraith; break;
	case Terran_Dropship:				++tcount_dropship; break;
	case Terran_Valkyrie:				++tcount_valkyrie; break;
	case Terran_Science_Vessel:			++tcount_vessel; break;
	case Terran_Battlecruiser:			++tcount_cruiser; break;
	}
}


void UnitManager::append_unit(BWAPI::Unit unit) {
	using namespace wilunits;
	using namespace BWAPI::UnitTypes;
	if (is_tank(unit->getType()) &&
		already_in(unit, siegetanks)) { return; }

	switch (unit->getType()) {
	case Terran_SCV:					append_to(unit, scvs); break;
	case Terran_Marine:					append_to(unit, marines); break;
	case Terran_Firebat:				append_to(unit, firebats); break;
	case Terran_Medic:					append_to(unit, medics); break;
	case Terran_Ghost:					append_to(unit, ghosts); break;
	case Terran_Siege_Tank_Tank_Mode:	append_to(unit, siegetanks); break;
	case Terran_Siege_Tank_Siege_Mode:	append_to(unit, siegetanks); break;
	case Terran_Goliath:				append_to(unit, goliaths); break;
	case Terran_Vulture:				append_to(unit, vultures); break;
	case Terran_Wraith:					append_to(unit, wraiths); break;
	case Terran_Dropship:				append_to(unit, dropships); break;
	case Terran_Valkyrie:				append_to(unit, valkyries); break;
	case Terran_Science_Vessel:			append_to(unit, vessels); break;
	case Terran_Battlecruiser:			append_to(unit, cruisers); break;
	}
}


void UnitManager::remove_unit(BWAPI::Unit unit) {
	using namespace wilunits;
	using namespace BWAPI::UnitTypes;

	switch (unit->getType()) {
	case Terran_SCV:					remove_from(scvs, tcount_scv); break;
	case Terran_Marine:					remove_from(marines, tcount_marine); break;
	case Terran_Firebat:				remove_from(firebats, tcount_firebat); break;
	case Terran_Medic:					remove_from(medics, tcount_medic); break;
	case Terran_Ghost:					remove_from(ghosts, tcount_ghost); break;
	case Terran_Siege_Tank_Tank_Mode:	remove_from(siegetanks, tcount_siegetank); break;
	case Terran_Siege_Tank_Siege_Mode:	remove_from(siegetanks, tcount_siegetank); break;
	case Terran_Goliath:				remove_from(goliaths, tcount_goliath); break;
	case Terran_Vulture:				remove_from(vultures, tcount_vulture); break;
	case Terran_Wraith:					remove_from(wraiths, tcount_wraith); break;
	case Terran_Dropship:				remove_from(dropships, tcount_dropship); break;
	case Terran_Valkyrie:				remove_from(valkyries, tcount_valkyrie); break;
	case Terran_Science_Vessel:			remove_from(vessels, tcount_vessel); break;
	case Terran_Battlecruiser:			remove_from(cruisers, tcount_cruiser); break;
	}
}


bool UnitManager::is_tank(BWAPI::UnitType my_type) {
	if (my_type == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) return true;
	if (my_type == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) return true;
	return false;
}
bool UnitManager::already_in(BWAPI::Unit my_unit, std::vector<Fighter2> &my_vector) {
	for (std::size_t i = 0; i < my_vector.size(); ++i)
		if (my_vector[i].id == my_unit->getID())
			return true;
	return false;
}


void UnitManager::append_to(BWAPI::Unit my_unit, std::vector<SCV> &my_vector) {
	my_vector.push_back(SCV(my_unit));
}
void UnitManager::append_to(BWAPI::Unit my_unit, std::vector<Fighter2> &my_vector) {
	my_vector.push_back(Fighter2(my_unit));
}
void UnitManager::append_to(BWAPI::Unit my_unit, std::vector<Flyer> &my_vector) {
	my_vector.push_back(Flyer(my_unit));
}


void UnitManager::remove_from(std::vector<SCV> &my_vector, int &my_count) {
	for (std::size_t i = 0; i < my_vector.size(); ++i) {
		if (!my_vector[i].unit->exists()) {
			my_vector[i].end_mining();
			my_vector[i].end_build_mission();
			my_vector.erase(my_vector.begin() + i);
			my_count--;
			break;
		}
	}
}
void UnitManager::remove_from(std::vector<Fighter2> &my_vector, int &my_count) {
	for (std::size_t i = 0; i < my_vector.size(); ++i) {
		if (!my_vector[i].unit->exists()) {
			my_vector.erase(my_vector.begin() + i);
			my_count--;
			break;
		}
	}
}
void UnitManager::remove_from(std::vector<Flyer> &my_vector, int &my_count) {
	for (std::size_t i = 0; i < my_vector.size(); ++i) {
		if (!my_vector[i].unit->exists()) {
			my_vector.erase(my_vector.begin() + i);
			my_count--;
			break;
		}
	}
}


void UnitManager::confirm_existence(BWAPI::Unit my_unit) {
	if (my_unit->exists()) {
		BWAPI::Broodwar->printf("Unit still exists ...!");
	} else {
		BWAPI::Broodwar->printf("Unit doesnt exist ...!");
	}
	return;
}
void UnitManager::correct() {
	remove_from(wilunits::scvs, wilunits::tcount_scv);
	for (Flyer& d : wilunits::dropships) { d.correct_dropship(); }
	return;
}





void UnitManager::update_all() {
	using namespace wilunits;

	for (SCV& scv : scvs) { scv.update(); }
	for (Fighter2& m : marines) { m.update(); m.check_stim(); }
	for (Fighter2& f : firebats) { f.update(); f.check_fallb(); }
	for (Fighter2& m : medics) { m.update(); m.check_flare(); }
	for (Fighter2& g : ghosts) { g.update(); g.check_lockd(); g.check_cloak(); }
	for (Fighter2& s : siegetanks) { s.update(); s.check_siege(); }
	for (Fighter2& g : goliaths) { g.update(); }
	for (Fighter2& v : vultures) { v.update_vulture(); v.check_mines(); }

	for (Flyer& c : cruisers) { c.update(); }
	for (Flyer& w : wraiths) { w.update(); }
	for (Flyer& d : dropships) { d.update_dropship(); }

	return;
}
void UnitManager::update_eight() {
	willyt::count_trapped = 0;
	for (SCV& scv : wilunits::scvs) { scv.check_being_trapped(willyt::count_trapped); }
	return;
}
void UnitManager::assign_scout(BWAPI::Unit my_unit) {
	if (!wilunits::has_scout && !wilmap::unscouted.empty())
	{
		wilunits::scvs.back().is_scout = true;
		wilunits::has_scout = true;
		//BWAPI::Broodwar->printf("initiate scouting");
		//BWAPI::Broodwar->printf("unscouted mains left: %d", wilmap::unscouted.size());
	}
	return;
}




void UnitManager::count_supplies() {
	using namespace willyt;
	using namespace wilunits;
	sup_scv = scvs.size();
	sup_bio = marines.size() + firebats.size() + medics.size() + ghosts.size();
	sup_mech = 2 * (siegetanks.size() + goliaths.size() + vultures.size());
	sup_air = 2 * wraiths.size() + 2 * dropships.size() + 3 * valkyries.size() + 2 * vessels.size() + 6 * cruisers.size();
	sup_mil = 0;
	for (SCV &scv : wilunits::scvs) if (scv.is_militia) ++sup_mil;
}

std::vector<SCV>::iterator choose_scv(BWAPI::TilePosition my_tile, bool no_militia, bool no_carry) {
	int min_dis = 65536;
	std::vector<SCV>::iterator min_scv = wilunits::scvs.begin();
	for (std::vector<SCV>::iterator it = wilunits::scvs.begin(); it != wilunits::scvs.end(); ++it) {
		if (it->unit->isCompleted() &&
			!it->unit->isConstructing() &&
			!it->unit->isRepairing() &&
			!it->is_builder &&
			!it->is_scout &&
			!it->is_repair &&
			(!it->is_militia || !no_militia) &&
			(!it->unit->isCarryingMinerals() || !no_carry) &&
			(!it->unit->isCarryingGas() || !no_carry) &&
			!it->unit->isGatheringGas())
		{
			int my_dis = sqdist(it->unit->getTilePosition(), my_tile);
			if (my_dis < min_dis) {
				min_dis = my_dis;
				min_scv = it;
			}
		}
	}
	return min_scv;
}

std::vector<SCV>::iterator get_scout() {
	for (std::vector<SCV>::iterator it = wilunits::scvs.begin(); it != wilunits::scvs.end(); ++it)
	{
		if (it->is_scout &&
			it->unit->exists() &&
			it->unit->isConstructing() == false) {
			return it;
		}
	}
	return wilunits::scvs.end();
}

void stop_all_build_missions() {
	for (std::vector<SCV>::iterator it = wilunits::scvs.begin(); it != wilunits::scvs.end(); ++it)
		if (it->is_builder)
			it->end_build_mission();
	BWAPI::Broodwar->printf("stop all build missions");
	return;
}





//unsigned n_scv = scvs.size();
//unsigned n_mar = marines.size();
//unsigned n_fir = firebats.size();
//unsigned n_med = medics.size();
//unsigned n_gho = ghosts.size();
//unsigned n_tan = siegetanks.size();
//unsigned n_gol = goliaths.size();
//unsigned n_vul = vultures.size();

//if (n_scv > 0) {
	//scvs.at(uc % n_scv).check_transport();
//}
//if (n_mar > 0) {
	//marines.at(uc % n_mar).avoid_areas();
	//marines.at(uc % n_mar).check_retreat(n_mar);
	//marines.at(uc % n_mar).check_stim();
	//marines.at(uc % n_mar).guard_siegetanks();
//}
//if (n_fir > 0) {
//	firebats.at(uc % n_fir).avoid_areas();
//	firebats.at(uc % n_fir).guard_siegetanks();
//	firebats.at(uc % n_fir).retreat_firebat(n_fir);
//}
//if (n_med > 0) {
//	medics.at(uc % n_med).avoid_areas();
//	medics.at(uc % n_med).guard_siegetanks();
//	medics.at(uc % n_med).retreat_medic(n_med);
//	medics.at(uc % n_med).check_flare(n_med);
//	medics.at(uc % n_med).draw_last_order();
//}
//if (n_gho > 0) {
//	ghosts.at(uc % n_gho).avoid_areas();
//	ghosts.at(uc % n_gho).check_retreat(n_gho);
//	ghosts.at(uc % n_gho).check_cloak();
//	ghosts.at(uc % n_gho).check_lockd(n_gho);
//}
//if (n_tan > 0) {
//	siegetanks.at(uc % n_tan).avoid_areas();
//	siegetanks.at(uc % n_tan).check_retreat(n_tan);
//	siegetanks.at(uc % n_tan).check_siege(n_tan);
//}
//if (n_gol > 0) {
//	goliaths.at(uc % n_gol).avoid_areas();
//	//goliaths.at(uc % n_gol).guard_siegetanks();
//	goliaths.at(uc % n_gol).check_retreat(n_gol);
//}
//if (n_vul > 0) {
//	vultures.at(uc % n_vul).avoid_areas();
//	//vultures.at(uc % n_vul).guard_siegetanks();
//	vultures.at(uc % n_vul).check_retreat(n_vul);
//	vultures.at(uc % n_vul).check_mines(n_vul);
//	//vultures.at(uc % n_vul).check_grenades();
//}