#include "ArmyUtilities.h"

//---------------------------------------------------------

bool check_base_under_attack() {
	for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
		if (u->isUnderAttack()) {
			BWAPI::TilePosition t = u->getTilePosition();
			if (wilmap::defense_map[t.y][t.x] >= 0) { return true; }
		}
	}
	return false;
}
bool check_main_or_natu_choke_defense() {
	using namespace willyt;
	using namespace wilmap;
	if (willyt::is_rushing) {
		return false;
	}
	if (!willyt::is_swarming &&
		att_target_pos == BWAPI::Positions::None &&
		def_target_pos == BWAPI::Positions::None) {
		return true;
	}
	if (att_target_pos == BWAPI::Positions::None &&
		def_target_pos != BWAPI::Positions::None)
	{
		BWAPI::TilePosition my_tile = (BWAPI::TilePosition)def_target_pos;
		if (wilmap::defense_map[my_tile.y][my_tile.x] < 0) {
			return true;
		}
		if (!has_natural &&
			(sqdist(def_target_pos, wilmap::main_def_pos[wilmap::mm]) < 102400 ||
			sqdist(def_target_pos, wilmap::main_choke_pos[wilmap::mm]) < 102400)) {
			return true;
		}
		if (has_natural &&
			(sqdist(def_target_pos, wilmap::natu_def_pos[wilmap::mm]) < 102400 ||
			sqdist(def_target_pos, wilmap::natu_choke_pos[wilmap::mm]) < 102400)) {
			return true;
		}
	}
	return false;
}



//---------------------------------------------------------

//choose non-buildings with not-none ground weapons, else anything
BWAPI::Unit choose_enemy_from(std::vector<BWAPI::Unit>& my_vec) {
	for (BWAPI::Unit u : my_vec) {
		if (u->getType().isBuilding() == false &&
			u->getType().groundWeapon() != 130) {
			return u;
		}
	}
	if (!my_vec.empty()) {
		return my_vec.front();
	}
	return NULL;
}

BWAPI::Position choose_enemy_unit() {
	for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
		for (BWAPI::Unit u : p->getUnits()) {
			if (u->exists() &&
				!u->getType().isInvincible()) {
				return u->getPosition();
			}
		}
	}
	return BWAPI::Positions::None;
}

BWAPI::Position choose_enemy_building_away(std::vector<BWAPI::Position> &ep, std::vector<BWAPI::Position> &em) {
	BWAPI::Position max_pos = BWAPI::Positions::None;
	int max_dist = -1;
	if (ep.empty()) { return max_pos; }
	if (em.empty()) { return ep.front(); }

	for (std::vector<BWAPI::Position>::iterator itp = ep.begin(); itp != ep.end(); ++itp) {
		if (get_ground_dist(*itp) == -1) { continue; }
		int my_dist = 0;
		for (std::vector<BWAPI::Position>::iterator itm = em.begin(); itm != em.end(); ++itm) {
			my_dist += dist(*itp, *itm);
		}
		if (max_dist < my_dist) {
			max_dist = my_dist;
			max_pos = *itp;
		}
	}
	//if (max_dist > -1) { BWAPI::Broodwar->printf("Max dist: %d", max_sq_dist); }
	return max_pos;
}

BWAPI::Position choose_enemy_building_near(std::vector<BWAPI::Position>& ep, BWAPI::Position& em)
{
	BWAPI::Position min_pos = BWAPI::Positions::None;
	int min_sqdist = 67108864;
	if (is_none(em)) { return ep.front(); }

	for (BWAPI::Position p : ep) {
		if (get_ground_dist(p) > -1 &&
			min_sqdist > sqdist(em, p)) {
			min_sqdist = sqdist(em, p);
			min_pos = p;
		}
	}
	return min_pos;
}



//---------------------------------------------------------

void call_militia(int n) {
	if (n > 12) { n = 12; }
	//BWAPI::Broodwar->printf("Call %d militia!", n);
	for (SCV &scv : wilunits::scvs) {
		if (n > 0 &&
			scv.unit->isCompleted() &&
			scv.unit->isGatheringMinerals()) {
			scv.is_militia = true;
			scv.end_mining();
			n--;
		}
	}
	return;
}

void stop_militia() {
	for (SCV &scv : wilunits::scvs) {
		if (scv.is_militia) {
			scv.is_militia = false;
			scv.unit->stop();
		}
	}
	return;
}

bool check_stim_medics() {
	if (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs) &&
		wilunits::medics.size() > 0) {
		int e = 0;
		for (Fighter2& m : wilunits::medics) { e += m.unit->getEnergy(); }
		if (e / wilunits::medics.size() >= 25) { return true; }
	}
	return false;
}



//---------------------------------------------------------

BWAPI::Position calc_leading_pos(BWAPI::Position my_target) {
	BWAPI::Position min_pos = BWAPI::Positions::None;
	if (willyt::strategy == 6) {
		return BWAPI::Positions::None;
	}
	if ((willyt::strategy == 1 && willyt::my_time < 600) ||
		willyt::sup_mech == 0) {
		min_pos = calc_leading_pos(my_target, wilunits::marines);
	}
	else if (8 * (int)wilunits::siegetanks.size() < willyt::sup_mech) {
		min_pos = calc_leading_pos(my_target, wilunits::goliaths);
	}
	else {
		min_pos = calc_leading_pos(my_target, wilunits::siegetanks);
	}
	if (get_ground_dist(min_pos) > get_ground_dist(my_target)) {
		return BWAPI::Positions::None;
	}
	//BWAPI::Broodwar->printf("leading distance: %d", my_dis);
	return min_pos;
}

//BWAPI::Position calc_leading_pos(BWAPI::Position my_target, std::vector<Fighter> &my_vec) {
//	int min_dist = -15000;
//	BWAPI::Position min_pos = BWAPI::Positions::None;
//	for (auto &f : my_vec) {
//		int d = 8 * get_ground_dist(f.unit) - dist(f.unit->getPosition(), my_target);
//		if (min_dist < d) {
//			min_dist = d;
//			min_pos = f.unit->getPosition();
//		}
//	}
//	return min_pos;
//}

BWAPI::Position calc_leading_pos(BWAPI::Position my_target, std::vector<Fighter2> &my_vec) {
	int min_dist = -15000;
	BWAPI::Position min_pos = BWAPI::Positions::None;
	for (Fighter2& f : my_vec) {
		if (f.unit->getTransport() == NULL) {
			int d = 8 * get_ground_dist(f.unit) - dist(f.unit->getPosition(), my_target);
			if (min_dist < d) {
				min_dist = d;
				min_pos = f.unit->getPosition();
			}
		}
	}
	return min_pos;
}

int count_around(BWAPI::Position my_pos, int r) {
	int i = 0;
	for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsInRadius(my_pos, r))
		if (u->getPlayer() == BWAPI::Broodwar->self()) {
			i += u->getType().supplyRequired();
		}
	return i / 2;
}

int count_around_scvs(BWAPI::Position my_pos, int r) {
	int i = 0;
	for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsInRadius(my_pos, r))
		if (u->getPlayer() == BWAPI::Broodwar->self() &&
			u->getType().isWorker()) {
			i++;
		}
	return i;
}

BWAPI::Position calc_tank_center() {
	int i = 0, x = 0, y = 0;
	for (Fighter2 &f : wilunits::siegetanks) {
		i++;
		x += f.unit->getPosition().x;
		y += f.unit->getPosition().y;
	}
	if (i > 0) { return BWAPI::Position{ x/i , y/i }; }
	return BWAPI::Positions::None;
}

bool hold_back_siegetank() {
	//recognize dragoon pressure on bunker and siege mode not yet done
	if (willyt::hold_bunker &&
		wilenemy::race == BWAPI::Races::Protoss &&
		wilenemy::supply_large >= wilenemy::supply_small &&
		remaining_siege_research_time() > 120 &&
		wilunits::siegetanks.size() > 0)
		return true;
	return false;
}

int remaining_siege_research_time() {
	if (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode) == NULL) {
		for (BWAPI::Unit ms : wilbuild::machineshops) {
			if (ms->isResearching() &&
				ms->getTech() == BWAPI::TechTypes::Tank_Siege_Mode) {
				return ms->getRemainingResearchTime();
			}
		}
		return 2100;
	}
	return 0;
}




//---------------------------------------------------------

void update_siegetank_influence() {
	clear_map_bool( wilmap::siege_map );
	for (Fighter2 &f : wilunits::siegetanks)
		if (f.unit->exists())
			set_influence_t( wilmap::siege_map , f.unit->getTilePosition() );
	return;
}

bool check_siege_guarding() {
	//if (willyt::siege_pos != BWAPI::Positions::None &&
	//	!wilunits::siegetanks.empty() &&
	//	willyt::att_target_pos != BWAPI::Positions::None &&
	//	willyt::def_target_pos == BWAPI::Positions::None &&
	//	!willyt::is_swarming)
	//	return true;
	return false;
}



//---------------------------------------------------------

void create_repulsive_field_at_enemy_natural() {
	BWAPI::Position posmin = BWAPI::Positions::None;
	int sqdmin = 67108864;
	for (int i = 0; i < wilmap::mn; i++) {
		int sqd = sqdist(willyt::att_target_pos, wilmap::main_pos[i]);
		if (i != wilmap::mm &&
			sqdmin > sqd) {
			sqdmin = sqd;
			posmin = wilmap::natu_choke_pos[i];
		}
	}
	if (posmin != BWAPI::Positions::None)
	{
		set_influence(wilthr::grdmap, posmin.x, posmin.y, 56, +1);
		willyt::enemy_natural_pos = posmin;
		//BWAPI::Broodwar->printf("defined enemy natural position for gathering");
	}
	return;
}
void clear_repulsive_field_at_enemy_natural() {
	using namespace willyt;
	if (enemy_natural_pos != BWAPI::Positions::None &&
		wilgroup::player_grd_supply >= 28 &&
		dist(enemy_natural_pos, wilgroup::player_grd_pos) < 320)
	{
		set_influence(wilthr::grdmap, enemy_natural_pos.x, enemy_natural_pos.y, 56, -1);
		enemy_natural_pos = BWAPI::Positions::None;
		//BWAPI::Broodwar->printf("cleared enemy natural position");
	}
	return;
}



//---------------------------------------------------------

bool can_attack_safe_r4(BWAPI::Position &my_pos) {
	for (unsigned i = 0; i < 24; i++)
	{
		int xw = my_pos.x / 8 + wilmap::cr4dx[i];
		int yw = my_pos.y / 8 + wilmap::cr4dy[i];
		if (xw > 0 && xw < wilmap::ww &&
			yw > 0 && yw < wilmap::hw &&
			wilthr::grddef[yw][xw] == 0 &&
			BWAPI::Broodwar->isWalkable(xw, yw)) {
			return true;
		}
	}
	return false;
}
BWAPI::Position hold_back_defense_at_natural_choke(BWAPI::Position& my_pos)
{
	if (willyt::has_natural &&
		sqdist(my_pos, wilmap::natu_def_pos[wilmap::mm]) < 102400 &&
		get_ground_dist(my_pos) - 6 > get_ground_dist(wilmap::natu_choke_pos[wilmap::mm]))
	{
		//BWAPI::Broodwar->printf("dont walk out of the natural choke");
		return BWAPI::Positions::None;
	}
	return my_pos;
}



//---------------------------------------------------------




//---------------------------------------------------------

//void Defender::update(ArmyManager &am) {
//	if (defense_queue == 0) {
//		if (base_under_attack()) {
//			int d = count_army() + count_militia() + 2 * willyt::bunkers.size();
//			int a = count_attackers();
//			if (d < 12 && d < a) { am.call_militia(a - d); }
//			am.attack(choose_defense());
//			am.is_defending = true;
//			defense_queue = 64;
//		}
//		else {
//			if (!am.is_attacking) {
//				am.stop_militia();
//				am.attack(choose_defense());
//			}
//			am.is_defending = false;
//			defense_queue = 32;
//		}
//		update_bunkers();
//	}
//}