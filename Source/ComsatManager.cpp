#include "ComsatManager.h"

void ComsatManager::check()
{
	willyt::available_scans = get_scan_count();
	BWAPI::Unit my_coms = get_max_comsat();
	BWAPI::Unit my_unit = get_invis_unit();
	if (my_coms && my_unit) {
		detect(my_coms, my_unit);
		return;
	}
	if (my_coms && my_coms->getEnergy() >= 150 &&
		!wilmap::unscouted.empty()) {
		scan_starts(my_coms);
		return;
	}
	if (my_coms && !wilbuild::scanned_blocked_tile &&
		wilbuild::blocked_tile != BWAPI::TilePositions::None) {
		scan_blocks(my_coms);
		return;
	}
	if (my_coms && my_coms->getEnergy() >= 180) {
		if (!scan_random_expo(my_coms)) {
			scan_random_area(my_coms);
		}
	}
	return;
}
void ComsatManager::check_unscouted()
{
	for (std::vector<BWAPI::Position>::iterator it = wilmap::unscouted.begin(); it != wilmap::unscouted.end(); ++it) {
		if (BWAPI::Broodwar->isVisible(BWAPI::TilePosition{ *it }))
		{
			wilmap::unscouted.erase(it);
			//BWAPI::Broodwar->printf("unscouted mains left: %d", wilmap::unscouted.size());
			return; //important!!
		}
	}
	if (wilenemy::positions.empty() && wilmap::unscouted.size() == 1)
	{
		wilenemy::positions.push_back(wilmap::unscouted.front());
		//BWAPI::Broodwar->printf("enemy can only be one way");
	}
	return;
}



void ComsatManager::detect(BWAPI::Unit my_coms, BWAPI::Unit my_unit)
{
	if ((!my_unit->isFlying() && check_units_near_grd(my_unit)) ||
		(my_unit->isFlying() && check_units_near_air(my_unit)))
	{
		my_coms->useTech(BWAPI::TechTypes::Scanner_Sweep, my_unit->getPosition());
		if (print_debug) { BWAPI::Broodwar->printf("scan for %s", my_unit->getType().c_str()); }
	}
	return;
}
void ComsatManager::scan_starts(BWAPI::Unit my_coms)
{
	my_coms->useTech(BWAPI::TechTypes::Scanner_Sweep, wilmap::unscouted.back());
	if (print_debug) { BWAPI::Broodwar->printf("need a scan to scout"); }
	return;
}
void ComsatManager::scan_blocks(BWAPI::Unit my_coms)
{
	int x = 32 * (wilbuild::blocked_tile.x + 1);
	int y = 32 * (wilbuild::blocked_tile.y + 1);
	my_coms->useTech(BWAPI::TechTypes::Scanner_Sweep, BWAPI::Position{ x, y });
	wilbuild::scanned_blocked_tile = true;
	if (print_debug) { BWAPI::Broodwar->printf("scan blocked: %s", wilbuild::blocked_tile); }
	return;
}
bool ComsatManager::scan_random_area(BWAPI::Unit my_coms)
{
	int rw = rand() % (wilmap::wt - 12) + 6;
	int rh = rand() % (wilmap::ht - 12) + 6;

	if (!BWAPI::Broodwar->isVisible(rw, rh)) {
		my_coms->useTech(BWAPI::TechTypes::Scanner_Sweep, BWAPI::Position{ 32*rw, 32*rh });
		if (print_debug) { BWAPI::Broodwar->printf("scan random area (%d, %d)", rw, rh); }
		return true;
	}
	return false;
}
bool ComsatManager::scan_random_expo(BWAPI::Unit my_coms)
{
	unsigned i = rand() % wilexpo::all.size();
	Expo& expo = wilexpo::all.at(i);

	if (!BWAPI::Broodwar->isExplored(expo.tile)) {
		my_coms->useTech(BWAPI::TechTypes::Scanner_Sweep, expo.posi);
		if (print_debug) { BWAPI::Broodwar->printf("scan random expo (%d, %d)", expo.tile.x, expo.tile.y); }
		return true;
	}
	return false;
}



unsigned ComsatManager::get_scan_count()
{
	unsigned i = 0;
	for (BWAPI::Unit my_comsat : wilbuild::comsatstations) {
		if (my_comsat->isCompleted() &&
			my_comsat->getPlayer() == BWAPI::Broodwar->self()) {
			i += my_comsat->getEnergy() / 50;
		}
	}
	return i;
}
BWAPI::Unit ComsatManager::get_max_comsat()
{
	BWAPI::Unit max_comsat = NULL;
	int max_energy = 0;
	for (BWAPI::Unit my_comsat : wilbuild::comsatstations) {
		if (my_comsat->isCompleted() &&
			my_comsat->getPlayer() == BWAPI::Broodwar->self() &&
			my_comsat->getEnergy() >= 50 &&
			max_energy < my_comsat->getEnergy()) {
			max_energy = my_comsat->getEnergy();
			max_comsat = my_comsat;
		}
	}
	return max_comsat;
}
BWAPI::Unit ComsatManager::get_invis_unit()
{
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



bool ComsatManager::check_units_near_grd(BWAPI::Unit my_unit)
{
	using namespace wilunits;
	unsigned i = 0;

	for (auto f : cruisers) if (sqdist(my_unit, f.unit) < 64) i += 4;
	if (i >= 4) return true;
	for (auto f : wraiths) if (sqdist(my_unit, f.unit) < 49) i++;
	if (i >= 4) return true;
	for (auto f : siegetanks) if (sqdist(my_unit, f.unit) < 169) i += 2;
	if (i >= 4) return true;
	for (auto f : goliaths) if (sqdist(my_unit, f.unit) < 49) i += 2;
	if (i >= 4) return true;
	for (auto f : vultures) if (sqdist(my_unit, f.unit) < 49) i += 2;
	if (i >= 4) return true;
	for (auto f : marines) if (sqdist(my_unit, f.unit) < 49) i++;
	if (i >= 4) return true;
	for (auto f : firebats) if (sqdist(my_unit, f.unit) < 25) i++;
	if (i >= 4) return true;
	for (auto f : ghosts) if (sqdist(my_unit, f.unit) < 100) i++;
	if (i >= 4) return true;
	for (BWAPI::Unit b : wilbuild::bunkers)
		if (sqdist(my_unit->getPosition(), b->getPosition()) < 73984)
			i += 4 - b->getSpaceRemaining();
	if (i >= 4) return true;

	return false;
}
bool ComsatManager::check_units_near_air(BWAPI::Unit my_unit)
{
	using namespace wilunits;
	unsigned i = 0;

	for (auto f : cruisers) if (sqdist(my_unit, f.unit) < 64) i += 4;
	if (i >= 4) return true;
	for (auto f : valkyries) if (sqdist(my_unit, f.unit) < 49) i += 2;
	if (i >= 4) return true;
	for (auto f : wraiths) if (sqdist(my_unit, f.unit) < 49) i += 2;
	if (i >= 4) return true;
	for (auto f : goliaths) if (sqdist(my_unit, f.unit) < 100) i += 2;
	if (i >= 4) return true;
	for (auto f : marines) if (sqdist(my_unit, f.unit) < 49) i++;
	if (i >= 4) return true;
	for (auto f : ghosts) if (sqdist(my_unit, f.unit) < 100) i++;
	if (i >= 4) return true;
	for (BWAPI::Unit b : wilbuild::bunkers)
		if (sqdist(my_unit->getPosition(), b->getPosition()) < 73984)
			i += 4 - b->getSpaceRemaining();
	if (i >= 4) return true;

	return false;
}