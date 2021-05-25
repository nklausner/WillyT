#include "Upgrader.h"

void Upgrader::upgrade() {
	if (willyt::go_bio) upgrade_bio();
	upgrade_ghosts();
	upgrade_mech();
	upgrade_air();
}

void Upgrader::upgrade_bio() {
	if (willyt::strategy == 6 && wilbuild::starports.size() < 2) { return; }
	using namespace BWAPI::UpgradeTypes;
	using namespace BWAPI::TechTypes;
	int iw = BWAPI::Broodwar->self()->getUpgradeLevel(Terran_Infantry_Weapons);
	int ia = BWAPI::Broodwar->self()->getUpgradeLevel(Terran_Infantry_Armor);

	BWAPI::Unit my_engybay = find_idle(wilbuild::engineerbays);
	if (my_engybay && willyt::strategy != 6 &&
		(willyt::siege_started || wilenemy::air_percentage >= 50) &&
		willyt::sup_bio >= (1 + iw + ia) * 6) {
		if (iw < willyt::max_bio_upgrade && iw <= ia &&
			my_engybay->canUpgrade(Terran_Infantry_Weapons)) {
			my_engybay->upgrade(Terran_Infantry_Weapons);
			return;
		}
		if (ia < willyt::max_bio_upgrade && ia <= iw &&
			my_engybay->canUpgrade(Terran_Infantry_Armor)) {
			my_engybay->upgrade(Terran_Infantry_Armor);
			return;
		}
	}

	BWAPI::Unit my_academy = find_idle(wilbuild::academies);
	if (BWAPI::Broodwar->self()->hasResearched(Stim_Packs) == NULL &&
		wilunits::tcount_medic >= 2 &&
		my_academy &&
		my_academy->canResearch(Stim_Packs)) {
		my_academy->research(Stim_Packs);
		return;
	}
	if (BWAPI::Broodwar->self()->getUpgradeLevel(U_238_Shells) == 0 &&
		(willyt::siege_started || wilenemy::air_percentage >= 50) &&
		wilunits::tcount_marine >= 12 &&
		my_academy &&
		my_academy->canUpgrade(U_238_Shells)) {
		my_academy->upgrade(U_238_Shells);
		has_U_238 = true;
		return;
	}
	if (BWAPI::Broodwar->self()->hasResearched(Optical_Flare) == NULL &&
		willyt::siege_started &&
		wilunits::tcount_medic >= 6 &&
		BWAPI::Broodwar->elapsedTime() >= 1200 &&
		my_academy &&
		my_academy->canResearch(Optical_Flare)) {
		my_academy->research(Optical_Flare);
		return;
	}
}

void Upgrader::upgrade_ghosts() {
	using namespace BWAPI::UpgradeTypes;
	using namespace BWAPI::TechTypes;

	BWAPI::Unit my_covertop = find_idle(wilbuild::covertops);
	if (BWAPI::Broodwar->self()->hasResearched(Personnel_Cloaking) == NULL &&
		wilunits::tcount_ghost >= 2 &&
		my_covertop &&
		my_covertop->canResearch(Personnel_Cloaking)) {
		my_covertop->research(Personnel_Cloaking);
		return;
	}
	if (BWAPI::Broodwar->self()->hasResearched(Lockdown) == NULL &&
		wilunits::tcount_ghost >= 2 &&
		my_covertop &&
		my_covertop->canResearch(Lockdown)) {
		my_covertop->research(Lockdown);
		return;
	}
	//if (BWAPI::Broodwar->self()->getUpgradeLevel(Ocular_Implants) == 0 &&
	//wilunits::tcount_ghost >= 6 &&
	//my_covertop &&
	//my_covertop->canUpgrade(Ocular_Implants)) {
	//my_covertop->upgrade(Ocular_Implants);
	//return;
	//}
	//if (BWAPI::Broodwar->self()->getUpgradeLevel(Moebius_Reactor) == 0 &&
	//wilunits::tcount_ghost >= 6 &&
	//my_covertop &&
	//my_covertop->canUpgrade(Moebius_Reactor)) {
	//my_covertop->upgrade(Moebius_Reactor);
	//return;
	//}
	return;
}

void Upgrader::upgrade_mech() {
	using namespace BWAPI::UpgradeTypes;
	using namespace BWAPI::TechTypes;
	int vw = BWAPI::Broodwar->self()->getUpgradeLevel(Terran_Vehicle_Weapons);
	int vp = BWAPI::Broodwar->self()->getUpgradeLevel(Terran_Vehicle_Plating);

	BWAPI::Unit my_armory = find_idle(wilbuild::armories);
	if (my_armory && willyt::siege_started &&
		willyt::sup_mech >= (1 + vw + vp) * 6) {
		if (vw < 3 && vw <= vp &&
			my_armory->canUpgrade(Terran_Vehicle_Weapons)) {
			my_armory->upgrade(Terran_Vehicle_Weapons);
			return;
		}
		if (vp < 3 && vp <= vw &&
			my_armory->canUpgrade(Terran_Vehicle_Plating)) {
			my_armory->upgrade(Terran_Vehicle_Plating);
			return;
		}
	}

	BWAPI::Unit my_machine_shop = find_idle(wilbuild::machineshops);
	if (BWAPI::Broodwar->self()->hasResearched(Tank_Siege_Mode) == NULL &&
		my_machine_shop &&
		my_machine_shop->canResearch(Tank_Siege_Mode)) {
		my_machine_shop->research(Tank_Siege_Mode);
		willyt::siege_started = true;
		return;
	}
	if (BWAPI::Broodwar->self()->getUpgradeLevel(Charon_Boosters) == 0 &&
		(willyt::siege_started || wilenemy::air_percentage >= 50) &&
		(!willyt::go_bio || has_U_238) &&
		wilunits::tcount_goliath >= 2 &&
		my_machine_shop &&
		my_machine_shop->canUpgrade(Charon_Boosters)) {
		my_machine_shop->upgrade(Charon_Boosters);
		return;
	}
	if (BWAPI::Broodwar->self()->hasResearched(Spider_Mines) == NULL &&
		willyt::siege_started &&
		wilunits::tcount_vulture >= 2 &&
		my_machine_shop &&
		my_machine_shop->canResearch(Spider_Mines)) {
		my_machine_shop->research(Spider_Mines);
		return;
	}
	if (BWAPI::Broodwar->self()->getUpgradeLevel(Ion_Thrusters) == 0 &&
		willyt::siege_started &&
		wilunits::tcount_vulture >= 3 &&
		my_machine_shop &&
		my_machine_shop->canUpgrade(Ion_Thrusters)) {
		my_machine_shop->upgrade(Ion_Thrusters);
		return;
	}
	return;
}

void Upgrader::upgrade_air() {
	using namespace BWAPI::UpgradeTypes;
	using namespace BWAPI::TechTypes;
	int aw = BWAPI::Broodwar->self()->getUpgradeLevel(Terran_Ship_Weapons);
	int ap = BWAPI::Broodwar->self()->getUpgradeLevel(Terran_Ship_Plating);
	if (willyt::strategy == 6) { willyt::siege_started = true; }

	BWAPI::Unit my_armory = find_idle(wilbuild::armories);
	if (my_armory && willyt::sup_air >= (1 + aw + ap) * 6) {
		if (aw < 3 && aw <= ap &&
			my_armory->canUpgrade(Terran_Ship_Weapons)) {
			my_armory->upgrade(Terran_Ship_Weapons);
			return;
		}
		if (ap < 3 && ap <= aw &&
			my_armory->canUpgrade(Terran_Ship_Plating)) {
			my_armory->upgrade(Terran_Ship_Plating);
			return;
		}
	}
	BWAPI::Unit my_controltower = find_idle(wilbuild::controltowers);
	if (wilunits::tcount_wraith >= 6 &&
		BWAPI::Broodwar->self()->hasResearched(Cloaking_Field) == NULL &&
		my_controltower &&
		my_controltower->canResearch(Cloaking_Field)) {
		my_controltower->research(Cloaking_Field);
		return;
	}
	BWAPI::Unit my_physicslab = find_idle(wilbuild::physicslabs);
	if (wilunits::tcount_cruiser >= 3 &&
		BWAPI::Broodwar->self()->hasResearched(Yamato_Gun) == NULL &&
		my_physicslab &&
		my_physicslab->canResearch(Yamato_Gun)) {
		my_physicslab->research(Yamato_Gun);
		return;
	}
	return;
}

BWAPI::Unit Upgrader::find_idle(std::vector<BWAPI::Unit> my_list) {
	for (BWAPI::Unit &my_unit : my_list)
		if (my_unit->isCompleted() &&
			my_unit->isIdle() &&
			!my_unit->isLifted())
			return my_unit;
	return NULL;
}