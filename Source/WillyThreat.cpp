#include "WillyThreat.h"


namespace wilthr
{
	std::vector<Threat> siege;
	std::vector<Threat> nuke;
	std::vector<Threat> lurk;
	std::vector<Threat> swarm;
	std::vector<Threat> storm;
	std::vector<Threat> disr;

	int grddef[1024][1024];
	int airdef[1024][1024];
	int grdmap[1024][1024];
	int airmap[1024][1024];

	int stadet[1024][1024];
	int dyndet[1024][1024];

	int getgrddef(BWAPI::Position p) { return grddef[ p.y/8 ][ p.x/8 ]; }
	int getairdef(BWAPI::Position p) { return airdef[ p.y/8 ][ p.x/8 ]; }
}


Threat::Threat(BWAPI::Position my_posi, int my_size, int my_id, int my_timer, bool my_grd, bool my_air, bool print_debug) {
	x0 = my_posi.x;
	y0 = my_posi.y;
	size = my_size;
	id = my_id;
	ef = BWAPI::Broodwar->getFrameCount() + my_timer;
	is_grd = my_grd;
	is_air = my_air;
	is_ignored = false;
	near_supply_enemy = 0;
	near_supply_self = 0;

	if (is_grd) { set_influence(wilthr::grdmap, x0, y0, size, +1); }
	if (is_air) { set_influence(wilthr::airmap, x0, y0, size, +1); }
	if (print_debug) {
		switch (size)
		{
		case 16: BWAPI::Broodwar->printf("detected psionic storm"); break;
		case 56: BWAPI::Broodwar->printf("detected lurking lurker"); break;
		case 72: BWAPI::Broodwar->printf("detected nuke area"); break;
		case 104: BWAPI::Broodwar->printf("detected sieged tank"); break;
		default: BWAPI::Broodwar->printf("detected threat"); break;
		}
	}
	return;
}
void Threat::clear() {
	if (!is_ignored) {
		if (is_grd) { set_influence(wilthr::grdmap, x0, y0, size, -1); }
		if (is_air) { set_influence(wilthr::airmap, x0, y0, size, -1); }
		is_ignored = true;
	}
	return;
}



ThreatManager::ThreatManager() {
	fill_map_int(wilthr::grddef, 0);
	fill_map_int(wilthr::airdef, 0);
	fill_map_int(wilthr::grdmap, 0);
	fill_map_int(wilthr::airmap, 0);
	fill_map_int(wilthr::stadet, 0);
	fill_map_int(wilthr::dyndet, 0);
	print_debug = false;
}
void ThreatManager::update() {
	remove_expired_unit(wilthr::siege);
	remove_expired_unit(wilthr::lurk);
	remove_expired_spell(wilthr::nuke);
	remove_expired_spell(wilthr::swarm);
	remove_expired_spell(wilthr::storm);
	remove_expired_spell(wilthr::disr);

	//update_dynamic_detection();

	using namespace BWAPI::UnitTypes;
	using namespace BWAPI::Orders;
	for (BWAPI::Bullet b : BWAPI::Broodwar->getBullets()) {
		if (b->exists()) {
			if (b->getType() == BWAPI::BulletTypes::Psionic_Storm) {
				append_spell(wilthr::storm, b, 16);
			}
		}
	}
	for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
		for (BWAPI::Unit u : p->getUnits()) {
			if (u->exists()) {
				if (u->getType() == Zerg_Lurker) {
					if (u->isBurrowed() ||
						u->getOrder() == Burrowing)	{ append_unit(wilthr::lurk, u, 56, 480); }
					if (u->getOrder() == Unburrowing) { remove_unit(wilthr::lurk, u); }
				}
				if (u->getType() == Terran_Siege_Tank_Siege_Mode) {
					if (u->isSieged() ||
						u->getOrder() == Sieging) { append_unit(wilthr::siege, u, 104, 480); }
					if (u->getOrder() == Unsieging) { remove_unit(wilthr::siege, u); }
				}
			}
		}
	}
	return;
}
void ThreatManager::remove_destroyed(BWAPI::Unit u) {
	if (u->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode) {
		remove_unit(wilthr::siege, u);
		if (print_debug) { BWAPI::Broodwar->printf("tank destroyed"); }
	}
	if (u->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
		remove_unit(wilthr::lurk, u);
		if (print_debug) { BWAPI::Broodwar->printf("lurker destroyed"); }
	}
	return;
}
void ThreatManager::append_nuke(BWAPI::Position p) {
	if (p != BWAPI::Positions::Unknown) {
		Threat threat(p, 72, 0, 420, true, true, print_debug);
		wilthr::nuke.push_back(threat);
	}
	return;
}



void ThreatManager::append_unit(std::vector<Threat> &v, BWAPI::Unit u, int s, int timer) {
	if (!vector_holds_at(v, u->getPosition())) {
		Threat threat(u->getPosition(), s, u->getID(), timer, true, false, print_debug);
		v.push_back(threat);
	}
	return;
}
void ThreatManager::append_spell(std::vector<Threat>& v, BWAPI::Bullet b, int s) {
	if (!vector_holds_at(v, b->getPosition())) {
		Threat threat(b->getPosition(), s, b->getID(), b->getRemoveTimer(), true, true, print_debug);
		v.push_back(threat);
	}
	return;
}
bool ThreatManager::vector_holds_id(std::vector<Threat>& v, int my_id) {
	for (std::vector<Threat>::iterator it = v.begin(); it != v.end(); ++it) {
		if (it->id == my_id) { return true; }
	}
	return false;
}
bool ThreatManager::vector_holds_at(std::vector<Threat> &v, BWAPI::Position p) {
	for (std::vector<Threat>::iterator it = v.begin(); it != v.end(); ++it) {
		if (it->x0 == p.x && it->y0 == p.y) { return true; }
	}
	return false;
}



void ThreatManager::remove_unit(std::vector<Threat> &v, BWAPI::Unit u) {
	std::vector<Threat>::iterator it = v.begin();
	while (it != v.end()) {
		if (u->getID() == it->id) {
			it->clear();
			it = v.erase(it);
		} else {
			++it;
		}
	}
	return;
}
void ThreatManager::remove_expired_unit(std::vector<Threat> &v) {
	std::vector<Threat>::iterator it = v.begin();
	while (it != v.end()) {
		if (BWAPI::Broodwar->getFrameCount() > it->ef) {
			BWAPI::Unit u = BWAPI::Broodwar->getUnit(it->id);
			if (u->exists() &&
				(u->isSieged() || u->isBurrowed())) {
				it->ef += 480;
				++it;
				if (print_debug) { BWAPI::Broodwar->printf("unit still there"); }
			} else {
				it->clear();
				it = v.erase(it);
			}
		} else {
			++it;
		}
	}
	return;
}
void ThreatManager::remove_expired_spell(std::vector<Threat>& v) {
	std::vector<Threat>::iterator it = v.begin();
	while (it != v.end()) {
		if (BWAPI::Broodwar->getFrameCount() > it->ef) {
			it->clear();
			it = v.erase(it);
		} else {
			++it;
		}
	}
	return;
}



void ThreatManager::ignore_stray_lurkers(unsigned scancount) {
	if (!wilthr::lurk.empty() &&
		scancount > 0) {
		int i = (BWAPI::Broodwar->getFrameCount() / 8) % wilthr::lurk.size();
		update_near_supply(wilthr::lurk.begin() + i);
	}
	return;
}
void ThreatManager::update_near_supply(std::vector<Threat>::iterator it) {
	it->near_supply_enemy = 0;
	it->near_supply_self = 0;

	for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
		for (BWAPI::Unit u : p->getUnits()) {
			if (u->getType().supplyRequired() > 0 &&
				!u->getType().isWorker() &&
				sqdist(it->x0, it->y0, u->getPosition().x, u->getPosition().y) < 147456) {
				it->near_supply_enemy += u->getType().supplyRequired();
			}
		}
	}
	for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
		if (u->getType().supplyRequired() > 0 &&
			!u->getType().isWorker()&&
			sqdist(it->x0, it->y0, u->getPosition().x, u->getPosition().y) < 147456) {
			it->near_supply_self += u->getType().supplyRequired();
		}
	}
	if (!it->is_ignored &&
		it->near_supply_self >= 12 &&
		it->near_supply_self >= 3 * it->near_supply_enemy) {
		it->clear();
		//BWAPI::Broodwar->printf("overrun that lurker");
	}
	return;
}



//void ThreatManager::update_dynamic_detection()
//{
//	using namespace BWAPI::UnitTypes;
//	//reset dynamic map to static map
//	for (int y = 0; y < wilmap::hw; ++y) {
//		for (int x = 0; x < wilmap::ww; ++x) {
//			wilthr::dyndet[y][x] = wilthr::stadet[y][x];
//		}
//	}
//	for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
//		for (BWAPI::Unit u : p->getUnits()) {
//			if (u->exists() &&
//				(u->getType() == Zerg_Overlord ||
//				u->getType() == Protoss_Observer ||
//				u->getType() == Terran_Science_Vessel ||
//				u->getType() == Spell_Scanner_Sweep) &&
//				!u->isBlind()) {
//				set_influence(wilthr::dyndet, u->getPosition(), 96, wilmap::circ_area_r11, +1);
//			}
//		}
//	}
//	return;
//}

