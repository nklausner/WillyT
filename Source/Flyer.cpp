#include "Flyer.h"


Flyer::Flyer(BWAPI::Unit my_unit) {
	unit = my_unit;
	id = my_unit->getID();
	attack_pos = willyt::gather_pos;
	attack_tile = (BWAPI::TilePosition)attack_pos;
	latest_pos = willyt::gather_pos;
	retreat_pos = willyt::retreat_pos;
	command_queue = 0;
	looping_queue = 0;
	ability_queue = 0;
	attack_queue = 0;
	is_circling = true;
	is_raider = false;
	ability_target = NULL;
	attack_target = NULL;
	threat_target = NULL;
	transport_pos = willyt::retreat_pos;
	transport_unit = NULL;

	dc = 0; //((rand() % 2) * 2) - 1;
	ic = 0;
	ica = 0;
	for (unsigned i = 0; i < wilmap::aircircle.size(); i++) {
		if (wilmap::aircircle.at(i) == wilmap::my_natu) { ic = (int)i; }
	}
}



//methods

void Flyer::move(BWAPI::Position my_pos, const char* s) {
	unit->move(my_pos);
	//draw_arrow(unit, my_pos, BWAPI::Colors::Yellow);
	//if (s != "") { BWAPI::Broodwar->drawTextMap(unit->getPosition(), s); }
	return;
}

void Flyer::attack() {
	unit->attack(attack_pos);
	command_queue = 128;
	return;
}

void Flyer::swarm(bool immediately) {
	if (immediately || sqdist(unit->getTilePosition(), attack_tile) < 16)
	{
		int x = rand() % wilmap::wt;
		int y = rand() % wilmap::ht;
		attack_tile = BWAPI::TilePosition{ x , y };
		attack_pos = (BWAPI::Position)attack_tile;
		move(attack_pos, "SWARMING");
	}
	return;
}

void Flyer::set_circle_diff(int new_dc) {
	dc = new_dc;
	return;
}

int Flyer::get_circle_diff() {
	return dc;
}

void Flyer::update()
{
	if (!unit->exists() || unit->isLockedDown() || unit->isStasised()) {
		return;
	}

	if (ability_queue > 0 && ability_target) {			//take time to use ability
		--ability_queue;
		draw_arrow(unit, ability_target, BWAPI::Colors::Blue);
		//BWAPI::Broodwar->drawCircleMap(unit->getPosition(), 8, BWAPI::Colors::Blue, true);
		return;
	}

	if (attack_target && looping_queue == 0) {			//start looping after attack
		--attack_queue;
		if (unit->getAirWeaponCooldown() > 0) {
			loop_to_evade(attack_target->getPosition());
			attack_target = NULL;
			attack_queue = 0;
		}
		if (attack_queue == 0) {						//reset
			attack_target = NULL;
			//BWAPI::Broodwar->drawCircleMap(unit->getPosition(), 8, BWAPI::Colors::Red, true);
			BWAPI::Broodwar->drawTextMap(unit->getPosition(), "%cRESET", BWAPI::Text::Red);
		}
		return;
	}

	if (looping_queue > 0) {							//looping phase and looping end
		--looping_queue;
		if (looping_queue == 0) {
			move(attack_pos, "RETURNING");
		}
		//BWAPI::Broodwar->drawCircleMap(unit->getPosition(), 8, BWAPI::Colors::Yellow, true);
		return;
	}

	if (evade_air_threats() &&
		(reaching_area(unit, wilthr::airdef) ||
		reaching_area(unit, wilthr::airmap))) {			//evade static air defense and spells
		loop_to_evade(attack_pos);
		return;
	}
	if (unit->getType() == BWAPI::UnitTypes::Terran_Wraith &&
		evade_air_threats())							//evade moving anti-air units
	{
		threat_target = get_close_threat();				
		if (threat_target != NULL) {
			loop_to_evade(threat_target->getPosition());
			threat_target = NULL;
			return;
		}
	}

	if (unit->getType() == BWAPI::UnitTypes::Terran_Battlecruiser &&
		BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Yamato_Gun) &&
		unit->getEnergy() >= 150) {
		check_yamato();
	}

	if (unit->getAirWeaponCooldown() < 8) {				//find attack target
		if (unit->getType() == BWAPI::UnitTypes::Terran_Wraith) {
			if (evade_air_threats()) {
				attack_target = get_target_wraith();
			}
			else {
				attack_target = get_target_all(36864);
			}
		}
		else {
			attack_target = get_target_all(65536);
		}

		if (attack_target != NULL) {					//issue attack command
			unit->attack(attack_target);
			attack_queue = 24;
			draw_arrow(unit, attack_target, BWAPI::Colors::Orange);
			//BWAPI::Broodwar->drawCircleMap(unit->getPosition(), 8, BWAPI::Colors::Orange, true);
			BWAPI::Broodwar->drawTextMap(unit->getPosition(), "%cATTACK", BWAPI::Text::Orange);
			return;
		}
	}

	if (is_circling) {									//define and change routes
		fly_in_circles();
	}
	else if (!wilenemy::positions.empty()) {
		fly_random(wilenemy::positions);
	}
	else {
		swarm(false);
	}
	if (unit->isIdle() && attack_tile.isValid() &&
		sqdist(unit->getTilePosition(), attack_tile) >= 16) {	//get moving
		move(attack_pos, "MOVING");
	}
	//BWAPI::Broodwar->drawCircleMap(unit->getPosition(), 8, BWAPI::Colors::Green, true);
	return;
}


void Flyer::update_dropship()
{
	if (!unit->exists() || unit->isLockedDown() || unit->isStasised() || is_raider ||
		BWAPI::Broodwar->getFrameCount() - unit->getLastCommandFrame() < 16) {
		return;
	}

	if (transport_pos.isValid())
	{
		if (transport_unit && transport_unit->exists()) {
			if (transport_unit->getTransport() == NULL) {
				unit->load(transport_unit);
				BWAPI::Broodwar->drawLineMap(unit->getPosition(), transport_unit->getPosition(), 135);
			} else {
				transport_unit = NULL;
				ability_queue = 8;
			}
		}
		else if (unit->getSpaceRemaining() < 8) {
			if (ability_queue == 0 &&
				has_grd_connection(unit->getPosition(), transport_pos) &&
				unit->canUnloadAll()) {
				unit->unloadAll();
			}
			unit->move(transport_pos);
			safe_sum(ability_queue, -1);
			BWAPI::Broodwar->drawLineMap(unit->getPosition(), transport_pos, 135);
		}
		else {
			transport_unit = NULL;
			transport_pos = BWAPI::Positions::None;
			unit->stop();
			//BWAPI::Broodwar->printf("Transport end.");
		}
	}

	//if (transport_pos != BWAPI::Positions::None) {
	//	draw_line(transport_pos, BWAPI::Colors::Yellow);
	//}
	return;
}


bool Flyer::evade_air_threats()
{
	if (willyt::sup_air >= 4 * (wilenemy::supply_army + wilenemy::supply_air))
		return false;
	if (wilgroup::player_air_supply >= 48 && sqdist(unit->getPosition(), wilgroup::player_air_pos) < 65536)
		return false;
	return true;
}
void Flyer::loop_to_evade(BWAPI::Position my_pos)
{
	looping_queue = (unit->getAirWeaponCooldown() > 0) ? (unit->getAirWeaponCooldown() * 3 / 5) : 16;
	move(get_retreat_vector(unit->getPosition(), my_pos, 256), "LOOPING");
	return;
}


void Flyer::correct_dropship()
{
	if (!is_raider && unit->isIdle())
	{
		if (unit->getSpaceRemaining() < 8) {
			unit->unloadAll(retreat_pos);
		} else {
			unit->move(retreat_pos);
		}
		transport_unit = NULL;
		transport_pos = BWAPI::Positions::None;
		//BWAPI::Broodwar->printf("Correct dropship.");
	}
	return;
}



//specials

void Flyer::fly_in_circles() {
	if (sqdist(unit->getTilePosition(), attack_tile) < 16 && dc != 0)
	{
		int imax = (int)wilmap::aircircle.size();
		if (ica >= imax) { //end circling with full circle
			is_circling = false;
		}
		ic += dc;
		ica++;
		BWAPI::Position pa = wilmap::aircircle.at((ic + imax) % imax);
		int dx = (rand() % 256) - 128;
		int dy = (rand() % 256) - 128;
		latest_pos = unit->getPosition();
		attack_pos = BWAPI::Position{ pa.x + dx, pa.y + dy };
		attack_tile = (BWAPI::TilePosition)attack_pos;
		move(attack_pos, "CIRCLING");
	}
	//BWAPI::Broodwar->drawTextMap(unit->getPosition(), "%d", ic);
	return;
}
void Flyer::fly_random(std::vector<BWAPI::Position>& my_vec) {
	if (!my_vec.empty() && sqdist(unit->getTilePosition(), attack_tile) < 16) {
		int i = rand() % my_vec.size();
		attack_pos = my_vec[i];
		attack_tile = (BWAPI::TilePosition)attack_pos;
		move(attack_pos, "RANDOM");
	}
}


void Flyer::check_yamato()
{
	ability_queue = 0;
	ability_target = get_target_yamato();
	if (ability_target != NULL &&
		unit->canUseTech(BWAPI::TechTypes::Yamato_Gun, ability_target)) {
		unit->useTech(BWAPI::TechTypes::Yamato_Gun, ability_target);
		willyt::target_vector.push_back(ability_target);
		ability_queue = 64;
		//BWAPI::Broodwar->printf("yamato %s", my_target->getType().c_str());
	}
	return;
}
BWAPI::Unit Flyer::get_target_yamato() {
	for (BWAPI::Unit u : wilenemy::tlyama)
	{
		if (sqdist(unit, u) <= 196 &&
			u->isCompleted() &&
			u->getHitPoints() >= 60 &&
			!u->isLockedDown() &&
			!vector_holds(willyt::target_vector, u)) {
			return u;
		}
	}
	return NULL;
}

BWAPI::Unit Flyer::get_target_wraith() {
	BWAPI::Unit u = NULL;
	int r = 36864; //radius 6
	u = get_target_from(wilenemy::tla, u, unit->getPosition(), r);
	u = get_target_from(wilenemy::tlgw, u, unit->getPosition(), r);
	u = get_target_from(wilenemy::tlgm, u, unit->getPosition(), r);
	u = get_target_from(wilenemy::tlgl, u, unit->getPosition(), r);
	return u;
}
BWAPI::Unit Flyer::get_target_all(int sqd) {
	BWAPI::Unit u = NULL;
	u = get_target_from(wilenemy::tlaa, u, unit->getPosition(), sqd);
	u = get_target_from(wilenemy::tlga, u, unit->getPosition(), sqd);
	u = get_target_from(wilenemy::tla, u, unit->getPosition(), sqd);
	u = get_target_from(wilenemy::tlg, u, unit->getPosition(), sqd);
	return u;
}
BWAPI::Unit Flyer::get_close_threat() {
	for (BWAPI::Unit u : wilenemy::tlga)
	{
		if (u->getType() == BWAPI::UnitTypes::Terran_Goliath &&
			sqdist(u, unit) < 100) //radius 10
			return u;
		if (sqdist(u, unit) < 64) //radius 8
			return u;
	}
	for (BWAPI::Unit u : wilenemy::tlaa)
	{
		if (u->getType() == BWAPI::UnitTypes::Protoss_Carrier &&
			sqdist(u, unit) < 100) //radius 10
			return u;
		if (sqdist(u, unit) < 64) //radius 8
			return u;
	}
	return NULL;
}