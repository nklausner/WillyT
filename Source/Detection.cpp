#include "Detection.h"

void Detection::check() {
	BWAPI::Unit my_coms = get_max_comsat();
	BWAPI::Unit my_unit = get_invis_unit();
	if (my_coms && my_unit) {
		if (!my_unit->isFlying() &&
			check_units_near_grd(my_unit)) {
			my_coms->useTech(BWAPI::TechTypes::Scanner_Sweep, my_unit->getPosition());
		}
		if (my_unit->isFlying() &&
			check_units_near_air(my_unit)) {
			my_coms->useTech(BWAPI::TechTypes::Scanner_Sweep, my_unit->getPosition());
		}
	}
	if (my_coms && !wilbuild::scanned_blocked_tile &&
		wilbuild::blocked_tile != BWAPI::TilePositions::None) {
		int x = 32 * wilbuild::blocked_tile.x + 32;
		int y = 32 * wilbuild::blocked_tile.y + 32;
		my_coms->useTech(BWAPI::TechTypes::Scanner_Sweep, BWAPI::Position{x, y});
		wilbuild::scanned_blocked_tile = true;
		BWAPI::Broodwar->printf("Scanning blocked build tile!");
	}
	return;
}

BWAPI::Unit Detection::get_max_comsat() {
	BWAPI::Unit max_comsat = NULL;
	int max_energy = 0;
	for (BWAPI::Unit &my_comsat : wilbuild::comsatstations) {
		if (my_comsat->getEnergy() >= 50 &&
			max_energy < my_comsat->getEnergy()) {
			max_energy = my_comsat->getEnergy();
			max_comsat = my_comsat;
		}
	}
	return max_comsat;
}

BWAPI::Unit Detection::get_invis_unit() {
	for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
		for (BWAPI::Unit u : p->getUnits()) {
			if ((u->isCloaked() || u->isBurrowed()) &&
				!u->isDetected()) {
				return u;
			}
		}
	}
	return NULL;
}

bool Detection::check_units_near_grd(BWAPI::Unit my_unit) {
	using namespace wilunits;
	unsigned i = 0;

	for (Flyer f : cruisers) if (sqdist(my_unit, f.unit) < 64) i += 3;
	if (i >= 3) return true;
	for (Flyer f : wraiths) if (sqdist(my_unit, f.unit) < 36) i++;
	if (i >= 3) return true;
	for (auto f : marines) if (sqdist(my_unit, f.unit) < 36) i++;
	if (i >= 3) return true;
	for (Fighter f :firebats) if (sqdist(my_unit, f.unit) < 36) i++;
	if (i >= 3) return true;
	for (Fighter f : goliaths) if (sqdist(my_unit, f.unit) < 36) i++;
	if (i >= 3) return true;
	for (Fighter f : vultures) if (sqdist(my_unit, f.unit) < 36) i++;
	if (i >= 3) return true;
	for (Fighter f : ghosts) if (sqdist(my_unit, f.unit) < 81) i++;
	if (i >= 3) return true;
	for (Fighter f : siegetanks) if (sqdist(my_unit, f.unit) < 144) i++;
	if (i >= 3) return true;
	else return false;
}

bool Detection::check_units_near_air(BWAPI::Unit my_unit) {
	using namespace wilunits;
	unsigned i = 0;

	for (Flyer f : cruisers) if (sqdist(my_unit, f.unit) < 64) i += 3;
	if (i >= 3) return true;
	for (Flyer f : valkyries) if (sqdist(my_unit, f.unit) < 64) i++;
	if (i >= 3) return true;
	for (Flyer f : wraiths) if (sqdist(my_unit, f.unit) < 36) i++;
	if (i >= 3) return true;
	for (auto f : marines) if (sqdist(my_unit, f.unit) < 36) i++;
	if (i >= 3) return true;
	for (Fighter f : goliaths) if (sqdist(my_unit, f.unit) < 81) i++;
	if (i >= 3) return true;
	for (Fighter f : ghosts) if (sqdist(my_unit, f.unit) < 81) i++;
	if (i >= 3) return true;
	else return false;
}