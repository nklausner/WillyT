#include "Fighter2.h"

// ------- G E N E R A L S -------

Fighter2::Fighter2(BWAPI::Unit my_unit) {

	unit = my_unit;
	id = my_unit->getID();

	is_onlygrd = false;
	is_support = false;
	is_looping = false;
	specials_allowed = true;
	changed_attack_pos = true;
	is_raider = false;
	is_attacker = false;

	tile = my_unit->getTilePosition();
	posi = my_unit->getPosition();
	secure_pos = willyt::retreat_pos;
	attack_pos = willyt::gather_pos;
	special_pos = BWAPI::Positions::None;
	destin_pos = BWAPI::Positions::None;

	attack_queue = 0;
	special_queue = 0;
	retreat_queue = 0;
	spider_mine_count = 0;
	command_frames = 10;

	target = NULL;
	transport = NULL;

	using namespace BWAPI::UnitTypes;
	switch (unit->getType())
	{
	case Terran_Marine:
		retreat_sqdst = (int)pow(112, 2);
		attack_sqdst = (int)pow(192, 2);
		break;
	case Terran_Firebat:
		is_onlygrd = true;
		retreat_sqdst = 0;
		attack_sqdst = (int)pow(192, 2);
		break;
	case Terran_Medic:
		is_support = true;
		retreat_sqdst = 0;
		attack_sqdst = (int)pow(128, 2);
		break;
	case Terran_Ghost:
		retreat_sqdst = (int)pow(224, 2);
		attack_sqdst = (int)pow(256, 2);
		break;
	case Terran_Siege_Tank_Tank_Mode:
		is_onlygrd = true;
		retreat_sqdst = (int)pow(224, 2);
		attack_sqdst = (int)pow(256, 2);
		break;
	case Terran_Goliath:
		retreat_sqdst = (int)pow(128, 2);
		attack_sqdst = (int)pow(320, 2);
		break;
	case Terran_Vulture:
		is_onlygrd = true;
		command_frames = 6;
		retreat_sqdst = (int)pow(112, 2);
		attack_sqdst = (int)pow(192, 2);
		break;
	default:
		if (unit->getType().airWeapon() == 130) { is_onlygrd = true; }
		int r = unit->getType().groundWeapon().maxRange();
		retreat_sqdst = (int)pow(r - 32, 2);
		attack_sqdst = (int)pow(r + 32, 2);
		break;
	}
	return;
}


// ------- M E T H O D S -------

void Fighter2::update()
{
	if (!unit->exists() ||
		unit->isLockedDown() || unit->isMaelstrommed() ||
		unit->isStasised() || unit->getTransport() != NULL ||
		unit->isStartingAttack() || unit->isAttackFrame() ||
		unit->getOrder() == BWAPI::Orders::MedicHeal ||
		unit->getOrder() == BWAPI::Orders::Sieging ||
		unit->getOrder() == BWAPI::Orders::Unsieging) {							//--ability usage
		return;
	}
	specials_allowed = false;
	tile = unit->getTilePosition();
	posi = unit->getPosition();
	target = NULL;
	safe_sum(attack_queue, -1);
	//if (is_attacker) { BWAPI::Broodwar->drawTextMap(posi, "%cATTACK", 6); }
	//BWAPI::Broodwar->drawTextMap(posi, "%d", attack_queue);

	if (attack_queue == 0 && !is_support && wilenemy::target_count < 16) {		//--medic no targeting!
		target = get_target_all(attack_sqdst);
		//BWAPI::Broodwar->drawTextMap(posi, "alert");
	}
	if (target == NULL &&
		BWAPI::Broodwar->getFrameCount() <= unit->getLastCommandFrame() + command_frames) {
		return;
	}
	if ((willyt::avoid_grddef && reaching_area(unit, wilthr::grddef)) ||
		reaching_area(unit, wilthr::grdmap)) {
		set_retreat();
		return;
	}
	using namespace BWAPI::UnitTypes;
	switch (unit->getType())
	{
	case Terran_Medic:					target = get_target_heal(); break;		//--medic check proximity!
	case Terran_Siege_Tank_Siege_Mode:	target = get_target_siege(); break;
	case Terran_Goliath:				target = get_target_goliath(); break;
	case Terran_Vulture:				target = get_target_vulture(); break;
	default:							target = get_target_all(attack_sqdst); break;
	}

	if (target != NULL) {
		if (!is_support) {
			if (sqdist(posi, target->getPosition()) < retreat_sqdst &&			//--firebat retreat_sdst 0!
				sqdist(posi, secure_pos) > 9216) {
				set_retreat();
			} else {
				unit->attack(target);
				attack_queue = 256;
				BWAPI::Broodwar->drawLineMap(posi, target->getPosition(), BWAPI::Colors::Red);

				//if (unit->getType() == Terran_Vulture) {
				//	unit->patrol(target->getPosition());
				//	attack_queue = 256;
				//	BWAPI::Broodwar->drawLineMap(posi, target->getPosition(), BWAPI::Colors::Purple);
				//} else {
				//}
			}
		} else {
			unit->move(target->getPosition());									//--medic keep proximity!
			draw_arrow(posi, target->getPosition(), BWAPI::Colors::Yellow);
			//BWAPI::Broodwar->drawTextMap(posi, "get close");
		}
	}
	else {
		BWAPI::Position my_dest = (!is_none(special_pos)) ? special_pos : attack_pos;
		if (my_dest.isValid() &&
			sqdist(posi, my_dest) > 9216) {
			unit->attack(my_dest);
			//draw_arrow(posi, my_dest, BWAPI::Colors::Yellow);
		}
		check_cohesion_retreat();
	}
	specials_allowed = true;
	return;
}
void Fighter2::update_vulture()
{
	//if (is_attacker) { BWAPI::Broodwar->drawTextMap(posi, "%cATTACK", 6); }
	specials_allowed = false;
	tile = unit->getTilePosition();
	posi = unit->getPosition();
	destin_pos = (!is_none(special_pos)) ? special_pos : attack_pos;

	if (!unit->exists() || unit->getTransport() != NULL ||
		unit->isLockedDown() || unit->isStasised()) {
		return;
	}

	if (attack_queue > 0 && !is_looping) {					//looping start after attack
		--attack_queue;
		if (unit->getGroundWeaponCooldown() > 0) {
			set_retreat();
			is_looping = true;
			target = NULL;
		}
		if (attack_queue == 0) {
			target = NULL;
		}
		return;
	}

	if (is_looping) {										//looping phase: check back or forth
		if (unit->getGroundWeaponCooldown() % 5 == 0) {
			target = get_target_vulture();
		}
		if (target) {
			int d = dist(posi, target->getPosition());
			int s = unit->getGroundWeaponCooldown() * 8;
			if (s < d) {
				unit->patrol(target->getPosition());
				BWAPI::Broodwar->drawLineMap(posi, target->getPosition(), BWAPI::Colors::Purple);
				is_looping = false;
			}
		}
		if (unit->getGroundWeaponCooldown() < 15 && !target) {
			unit->move(destin_pos);
			is_looping = false;
			BWAPI::Broodwar->drawLineMap(posi, destin_pos, BWAPI::Colors::Yellow);
		}
		return;
	}

	if ((willyt::avoid_grddef && reaching_area(unit, wilthr::grddef)) ||
		reaching_area(unit, wilthr::grdmap)) {				//avoid ground threats
		set_retreat();
		retreat_queue = 32;
		return;
	}

	target = get_target_vulture();
	if (target) {											//issue attack command
		//unit->patrol(target->getPosition());
		unit->attack(target);
		attack_queue = 8;
		BWAPI::Broodwar->drawLineMap(posi, target->getPosition(), BWAPI::Colors::Purple);
		return;
	}

	if (unit->isIdle() && destin_pos.isValid() &&
		sqdist(posi, destin_pos) >= 16384) {				//get moving
		unit->move(destin_pos);
		BWAPI::Broodwar->drawLineMap(posi, destin_pos, BWAPI::Colors::Yellow);
		return;
	}
	check_cohesion_retreat();
	specials_allowed = true;
	return;
}



void Fighter2::set_target(BWAPI::Position& my_pos, bool is_attack) {
	if (attack_pos != my_pos) {
		attack_pos = my_pos;
		is_attacker = is_attack;
		changed_attack_pos = true;
	}
	return;
}
void Fighter2::set_swarm(bool immediately) {
	if (immediately || unit->isIdle()) {
		set_target(get_random_position(), false);
	}
	return;
}
void Fighter2::update_raider(int my_state, BWAPI::Position& att_pos, BWAPI::Position& def_pos, BWAPI::Unit my_transport)
{
	secure_pos = def_pos;
	attack_pos = att_pos;

	switch (my_state)
	{
	case 0: //terminate raid
		is_raider = false;
		transport = NULL;
		break;
	case 1: //initiate raid and load dropship
		is_raider = true;
		transport = my_transport;
		force_unsiege();
		break;
	case 2: //set raiding waypoint
		is_raider = true;
		transport = NULL;
		break;
	default:
		break;
	}
	return;
}
void Fighter2::set_retreat() {
	if (!is_raider) {
		unit->move(secure_pos);
		draw_arrow(posi, secure_pos, BWAPI::Colors::Green);
	}
}
void Fighter2::check_cohesion_retreat() {
	//if leader distance defined, keep cohesion
	if (retreat_queue > 0)
	{
		--retreat_queue;
		if (retreat_queue == 0) {
			unit->move(destin_pos);
			draw_arrow(posi, destin_pos, BWAPI::Colors::White);
		}
		return;
	}
	if (is_attacker && !is_raider &&
		is_none(special_pos) &&
		willyt::leader_dist > 0 &&
		get_ground_dist(tile) > willyt::leader_dist)
	{
		unit->move(secure_pos);
		retreat_queue = 32;
		draw_arrow(posi, secure_pos, BWAPI::Colors::White);
	}
	return;
}


// ------- S P E C I A L S -------

void Fighter2::check_stim() {
	if (willyt::stim_allowed && !unit->isStimmed() &&
		unit->isAttacking() && unit->getHitPoints() >= 40) {
		unit->useTech(BWAPI::TechTypes::Stim_Packs);
	}
	return;
}
void Fighter2::check_fallb() {
	if (specials_allowed &&
		unit->getHitPoints() < 20) {
		unit->move(secure_pos);
		draw_arrow(posi, secure_pos, BWAPI::Colors::Green);
		//BWAPI::Broodwar->drawTextMap(posi, "fall back");
	}
	return;
}
void Fighter2::check_flare() {
	if (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Optical_Flare) &&
		specials_allowed && unit->getEnergy() >= 100)
	{
		BWAPI::Unit u = get_maximum_target(wilenemy::tlflare, tile, 100);
		if (u != NULL &&
			unit->canUseTech(BWAPI::TechTypes::Optical_Flare, u)) {
			unit->useTech(BWAPI::TechTypes::Optical_Flare, u);
			willyt::target_vector.push_back(u);
			BWAPI::Broodwar->drawLineMap(posi, u->getPosition(), BWAPI::Colors::White);
			//BWAPI::Broodwar->printf("blinding %s", u->getType().c_str());
		}
	}
	return;
}
void Fighter2::check_lockd() {
	if (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lockdown) &&
		specials_allowed && unit->getEnergy() >= 100)
	{
		BWAPI::Unit u = get_maximum_target(wilenemy::tllockd, tile, 100);
		if (u != NULL &&
			unit->canUseTech(BWAPI::TechTypes::Lockdown, u)) {
			unit->useTech(BWAPI::TechTypes::Lockdown, u);
			willyt::target_vector.push_back(u);
			BWAPI::Broodwar->drawLineMap(posi, u->getPosition(), BWAPI::Colors::White);
			//BWAPI::Broodwar->printf("locking %s", u->getType().c_str());
		}
	}
	return;
}
void Fighter2::check_cloak() {
	if (!unit->isCloaked() && unit->isUnderAttack() &&
		unit->canUseTech(BWAPI::TechTypes::Personnel_Cloaking)) {
		unit->cloak();
	}
	if (unit->isCloaked() && !unit->isUnderAttack()) {
		unit->decloak();
	}
}



// ------- M E C H A N I C A L S -------

void Fighter2::check_siege() {
	if (!BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode)) {
		return;
	}

	//general sieging behavior
	if (!unit->isSieged() && !transport) {
		target = get_target_siege();
		if (target != NULL &&
			sqdist(posi, estimate_next_pos(target, 24)) >= 16384) {
			unit->siege();
			special_queue = 128;
		}
	} else {
		safe_sum(special_queue, -1);
		if (unit->isStartingAttack() ||
			unit->isAttackFrame()) {
			special_queue = 128;
		} else if (special_queue == 0) {
			unit->unsiege();
		}
		//if (!specials_allowed) {
		//	unit->unsiege();
		//	BWAPI::Broodwar->drawTextMap(posi, "%cLEAVE", BWAPI::Text::Red);
		//	return;
		//}
	}

	//choke defense case
	if (willyt::is_choke_def) {
		if (special_pos == BWAPI::Positions::None) {
			special_pos = choose_defense_siegetile();
		}
		else if (posi == special_pos) {
			if (!unit->isSieged()) {
				unit->siege();
			}
			if (special_queue < 32) {
				special_queue = 32;
			}
		}
		BWAPI::Broodwar->drawTextMap(posi, "%cDEF", BWAPI::Text::Green);
	}

	//change state case, clear special position
	if (changed_attack_pos) {
		changed_attack_pos = false;
		if (special_pos != BWAPI::Positions::None &&
			(!willyt::is_choke_def || !check_defense_siegetile(tile))) {
			special_pos = BWAPI::Positions::None;
			BWAPI::Broodwar->drawTextMap(posi, "%cLEAVE", BWAPI::Text::Red);
		}
	}
	//BWAPI::Broodwar->drawTextMap(posi, "%d", special_queue);
	//if (special_pos != BWAPI::Positions::None) {
	//	BWAPI::Broodwar->drawLineMap(posi, special_pos, BWAPI::Colors::White);
	//}
	return;
}
void Fighter2::check_mines() {
	if (!specials_allowed ||
		!BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Spider_Mines)) {
		return;
	}
	if (spider_mine_count != unit->getSpiderMineCount()) {
		spider_mine_count = unit->getSpiderMineCount();
		special_pos = BWAPI::Positions::None;
		//BWAPI::Broodwar->printf("update spider mine count");
	}
	if (spider_mine_count > 0 && !special_pos.isValid()) {
		special_pos = get_mine_position();
		if (special_pos.isValid()) {
			int dt = 64 + (dist(unit->getPosition(), special_pos) / 3);
			special_queue = BWAPI::Broodwar->getFrameCount() + dt;
			//BWAPI::Broodwar->printf("set position and time to lay spider mine");
		}
	}
	if (special_pos.isValid()) {
		if (sqdist(posi, special_pos) <= 1024 && attack_queue == 0) {
			unit->useTech(BWAPI::TechTypes::Spider_Mines, special_pos);
			attack_queue = 16;
		}
		if (BWAPI::Broodwar->getFrameCount() > special_queue) {
			special_pos = BWAPI::Positions::None;
			//BWAPI::Broodwar->printf("laying spider mine failed");
		}
		//BWAPI::Broodwar->printf("set position and time to lay spider mine");
	}
	return;
}
void Fighter2::set_special_position(BWAPI::Position my_pos)
{
	special_pos = my_pos;
	destin_pos = (!is_none(my_pos)) ? my_pos : attack_pos;
}
void Fighter2::force_unsiege()
{
	special_queue = 0;
	if (unit->isSieged()) {
		unit->unsiege();
		BWAPI::Broodwar->drawTextMap(posi, "%cLEAVE", BWAPI::Text::Red);
	}
}


// ------- T A R G E T I N G -------

BWAPI::Unit Fighter2::get_target_all(int r) {
	BWAPI::Unit u = NULL;
	if (is_onlygrd) {
		u = get_target_from(wilenemy::tlgm, u, posi, r);
		u = get_target_from(wilenemy::tlgg, u, posi, r);
		u = get_target_from(wilenemy::tlga, u, posi, r);
		u = get_target_from(wilenemy::tlg, u, posi, r);
		u = get_target_from(wilenemy::tlself, u, posi, r);
	} else {
		u = get_target_from(wilenemy::tlgg, u, posi, r);
		u = get_target_from(wilenemy::tlag, u, posi, r);
		u = get_target_from(wilenemy::tlaa, u, posi, r);
		u = get_target_from(wilenemy::tlga, u, posi, r);
		u = get_target_from(wilenemy::tla, u, posi, r);
		u = get_target_from(wilenemy::tlg, u, posi, r);
		u = get_target_from(wilenemy::tlself, u, posi, r);
	}
	return u;
}
BWAPI::Unit Fighter2::get_target_heal() {
	BWAPI::Unit umin = NULL;
	int dmin = 65536;
	for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
		if (u->getType().isOrganic() &&
			!u->getType().isWorker() &&
			u->getType() != BWAPI::UnitTypes::Terran_Medic &&
			u->isCompleted())
		{
			int d = sqdist(unit, u); //using squared tile distance
			if (d < dmin) { dmin = d; umin = u; }
			if (d <= 9) { return NULL; }
		}
	}
	return umin;
}
BWAPI::Unit Fighter2::get_target_siege() {
	BWAPI::Unit u = NULL;
	int r = 173056; //radius 13
	u = get_target_from(wilenemy::tlgl, u, posi, r);
	u = get_target_from(wilenemy::tldefg, u, posi, r);
	u = get_target_from(wilenemy::tlgm, u, posi, r);
	u = get_target_from(wilenemy::tlself, u, posi, r);
	return u;
}
BWAPI::Unit Fighter2::get_target_goliath() {
	BWAPI::Unit u = NULL;
	int rg = 36864; //radius 6
	int ra = 82944; //radius 9
	 //prio for air units
	u = get_target_from(wilenemy::tlag, u, posi, ra);
	u = get_target_from(wilenemy::tlas, u, posi, ra);
	u = get_target_from(wilenemy::tlgg, u, posi, rg);
	u = get_target_from(wilenemy::tlaa, u, posi, ra);
	u = get_target_from(wilenemy::tlga, u, posi, rg);
	u = get_target_from(wilenemy::tlgs, u, posi, rg);
	u = get_target_from(wilenemy::tlg, u, posi, rg);
	u = get_target_from(wilenemy::tlself, u, posi, rg);
	return u;
}
BWAPI::Unit Fighter2::get_target_vulture() {
	BWAPI::Unit u = NULL;
	int ri = 16384; //radius 4
	int ro = 82944; //radius 9
	//prio for small/medium units
	u = get_target_from(wilenemy::tlgm, u, posi, ro);
	u = get_target_from(wilenemy::tlgl, u, posi, ri);
	u = get_target_from(wilenemy::tlg, u, posi, ri);
	u = get_target_from(wilenemy::tlself, u, posi, ri);
	return u;
}


//if (attack_queue = 0) {
//	BWAPI::Unit my_target = get_closest(wilenemy::all_enemies, tileposi);
//	if (my_target != NULL &&
//		sqdist(my_target->getPosition(), position) <= 65536) {
//		unit->attack(my_target);
//		attack_pos = my_target->getPosition();
//		attack_queue = 3;
//	}
//} else {
//	attack_queue--;
//}
//if (command_queue == 0) {
//	if (sqdist(position, attack_pos) > 16384) {
//		unit->attack(attack_pos);
//		command_queue = 128;
//	}
//} else {
//	command_queue--;
//	BWAPI::Broodwar->drawLineMap(position, attack_pos, 135);
//}