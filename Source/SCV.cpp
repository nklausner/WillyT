#include "SCV.h"

SCV::SCV(BWAPI::Unit my_unit)
{
	unit = my_unit;
	id = my_unit->getID();

	is_miner = false;
	is_militia = false;
	is_builder = false;
	is_janitor = false;
	is_repair = false;
	is_scout = false;
	is_trapped = false;
	has_resource = false;
	called_transport = false;
	is_entering = false;
	transport_unit = NULL;

	destination = BWAPI::Positions::None;
	expo_pos = BWAPI::Positions::None;

	secure_pos = willyt::retreat_pos;
	attack_pos = willyt::gather_pos;

	build_type = BWAPI::UnitTypes::None;
	build_pos = BWAPI::Positions::None;
	build_tile = BWAPI::TilePositions::None;
	build_sqdist = 0;
	build_queue = 0;
	block_count = 0;
	x0 = 0; y0 = 0;
	x1 = 0; y1 = 0;
	damaged_unit = NULL;
	resource_container = NULL;

	circle_increment = -1;
	scout_pos = BWAPI::Positions::None;
	stuck_queue = 0;
	min_target_distance = 0;
	//BWAPI::Broodwar->printf("created SCV %d", id);
	return;
}
//SCV::~SCV()
//{
//	end_mining();
//	if (is_builder) end_build_mission();
//	BWAPI::Broodwar->printf("destroyed SCV %d", id);
//	return;
//}



void SCV::update()
{
	if (!unit->exists() ||
		unit->isLockedDown() || unit->isMaelstrommed() ||
		unit->isStasised() || unit->getTransport() != NULL ||
		is_entering) {
		return;
	}
	if (is_builder) {
		build();
	}
	else if (unit->isConstructing()) {
		return;
	}
	else if (is_repair) {
		update_repair();
	}
	else if (is_militia) {
		update_militia();
	}
	else if (is_scout) {
		scout();
	}
	else {
		mine();
	}
	//if (!unit->isConstructing() &&
	//	!unit->isGatheringMinerals() &&
	//	!unit->isGatheringGas()) {
	//	check_transport();
	//}
	if (is_trapped) {
		BWAPI::Broodwar->drawTextMap(unit->getPosition(), "%cTRAPPED: %d", 6, stuck_queue);
	}
	return;
}



void SCV::move(BWAPI::Position p)
{
	unit->move(p);
}
void SCV::set_attack(BWAPI::Position p)
{
	attack_pos = p;
}
void SCV::set_swarm(bool immediately)
{
	if (immediately || unit->isIdle())
		set_attack(get_random_position());
}
void SCV::change_resource(BWAPI::Unit my_unit, bool remove_stack)
{
	end_mining();
	resource_container = my_unit;
	set_resource(resource_container, +1);
	unit->gather(resource_container);
	if (remove_stack) { build_tile = my_unit->getTilePosition(); }
	//BWAPI::Broodwar->printf("SCV %d change to %s", id, resource_container->getType().c_str());
	return;
}
void SCV::end_mining()
{
	set_resource(resource_container, -1);
	resource_container = NULL;
	is_miner = false;
}
void SCV::set_expo_pos(BWAPI::Position p)
{
	expo_pos = p;
}



void SCV::mine()
{
	//if (resource_container) {
	//	BWAPI::Broodwar->drawLineMap(unit->getPosition(), resource_container->getPosition(), 128);
	//	BWAPI::Broodwar->drawTextMap(unit->getPosition(), "%s", unit->getOrder().c_str());
	//}
	if (!resource_container)
	{
		if (!is_none(build_tile)) {
			if (get_mineral_on(build_tile)) {
				resource_container = get_mineral_on(build_tile);
				//BWAPI::Broodwar->printf("continue clearing block");
			} else {
				build_tile = BWAPI::TilePositions::None;
				//BWAPI::Broodwar->printf("terminate clearing block");
			}
		}
		else if (willyt::mineral_count > 0 && willyt::min_scv_count_min < 3) {
			resource_container = get_mineral(true, willyt::min_scv_count_min);
		}
		else if (wilbuild::minerals.size() > 0) {
			resource_container = get_mineral(false, 6);	//long distance mining
		}
		if (resource_container) {
			set_resource(wilbuild::minerals, resource_container, +1);
		}
	}
	if (!unit->isIdle() &&
		(unit->getOrderTarget() == resource_container ||
		unit->isCarryingMinerals() ||
		unit->isCarryingGas())) {
		return;
	}
	if (resource_container && resource_container->exists())
	{
		if (has_grd_connection(unit->getPosition(), resource_container->getPosition())) {
			unit->gather(resource_container);
		} else {
			move(resource_container->getPosition());
			//BWAPI::Broodwar->drawTextMap(unit->getPosition(), "transit");
		}
	}
	return;
}
BWAPI::Unit SCV::get_mineral(bool is_owned, int min_count)
{
	BWAPI::Unit min_res = NULL;
	int min_sqdist = 65536;
	for (Resource& r : wilbuild::minerals) {
		if (r.is_owned == is_owned &&
			r.is_enemy == false &&
			r.scv_count <= min_count &&
			min_sqdist > sqdist(unit, r.unit)) {
			min_sqdist = sqdist(unit, r.unit);
			min_res = r.unit;
		}
	}
	return min_res;
}
BWAPI::Unit SCV::get_mineral_on(BWAPI::TilePosition my_tile)
{
	for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsOnTile(my_tile)) {
		if (u->getType().isMineralField()) { return u; }
	}
	return NULL;
}
void SCV::set_resource(BWAPI::Unit my_unit, int d)
{
	if (my_unit) {
		if (my_unit->getType().isMineralField()) {
			set_resource(wilbuild::minerals, my_unit, d);
			//BWAPI::Broodwar->printf("SCV %d modify mineral %d", id, d);
		}
		else {
			set_resource(wilbuild::geysers, my_unit, d);
			//BWAPI::Broodwar->printf("SCV %d modify geyser %d", id, d);
		}
	}
}
void SCV::set_resource(std::vector<Resource>& my_vec, BWAPI::Unit my_unit, int d)
{
	for (Resource& r : my_vec) {
		if (r.unit == my_unit) {
			safe_sum(r.scv_count, d);
			return;
		}
	}
}
BWAPI::Unit SCV::get_resource_container()
{
	return resource_container;
}



//BWAPI::Unit my_min = NULL;
////MINERAL_LOCK_UPDATE
//if (!wilbuild::minerals.empty() && BWAPI::Broodwar->getFrameCount() < 4800) {
//	my_min = wilbuild::minerals.at(id % wilbuild::minerals.size());
//}
//else if (!wilbuild::minerals.empty()) {
//	//MINERAL_LOCK_UPDATE
//	if (BWAPI::Broodwar->getFrameCount() % 2 == 1) {
//		my_min = get_closest(wilbuild::mainland_minerals, unit->getPosition());
//	} else {
//		my_min = get_closest(wilbuild::minerals, unit->getPosition());
//	}
//}
//else if (!wilbuild::all_minerals.empty()) {
//	//MINERAL_LOCK_UPDATE
//	my_min = get_closest(wilbuild::all_minerals, unit->getPosition());
//}
//if (my_min != NULL &&
//	unit->canGather(my_min)) {
//	if (!has_grd_connection(unit->getPosition(), my_min->getPosition())) {
//		move(my_min->getPosition());
//	} else {
//		gather(my_min);
//	}
//	is_miner = true;
//}
//BWAPI::Broodwar->drawTextMap(unit->getPosition(), "start mining");



void SCV::update_militia() {
	BWAPI::Position my_pos = unit->getPosition();
	if (unit->isStartingAttack() || unit->isAttackFrame() ||
		unit->isRepairing() ||
		BWAPI::Broodwar->getFrameCount() - unit->getLastCommandFrame() <= 10) {
		return;
	}
	if ((willyt::avoid_grddef && reaching_area(unit, wilthr::grddef)) ||
		reaching_area(unit, wilthr::grdmap)) {
		unit->move(secure_pos);
		//draw_arrow(my_pos, secure_pos, BWAPI::Colors::Green);
		return;
	}
	BWAPI::Unit my_target = get_target_scv(16384);
	if (my_target != NULL &&
		my_target->exists()) {
		unit->attack(my_target);
		//draw_arrow(my_pos, my_target->getPosition(), BWAPI::Colors::Red);
	} else if (sqdist(my_pos, attack_pos) > 16384) {
		unit->move(attack_pos);
		//draw_arrow(my_pos, attack_pos, BWAPI::Colors::Yellow);
	}
	return;
}



void SCV::set_build_mission(BWAPI::UnitType type, BWAPI::TilePosition &tile)
{
	end_mining();
	is_builder = true;
	build_type = type;
	build_tile = tile;

	x0 = 32 * tile.x;
	y0 = 32 * tile.y;
	x1 = x0 + 32 * build_type.tileWidth() - 1;
	y1 = y0 + 32 * build_type.tileHeight() - 1;

	build_pos = BWAPI::Position((x0 + x1) / 2, (y0 + y1) / 2);
	build_queue = 128 + 4 * unit->getPosition().getApproxDistance(build_pos);
	build_sqdist = 2304;
	if (type == BWAPI::UnitTypes::Terran_Missile_Turret) { build_queue += 8192; }
	if (type == BWAPI::UnitTypes::Terran_Refinery) { build_sqdist = 9216; }
	block_count = 0;
	wilbuild::planned_builds.push_back(type);
	//BWAPI::Broodwar->printf("SCV build queue: %d", build_queue);
	return;
}
void SCV::end_build_mission()
{
	vector_remove(wilbuild::planned_builds, build_type);
	is_builder = false;
	is_janitor = false;
	build_type = BWAPI::UnitTypes::None;
	build_tile = BWAPI::TilePositions::None;
	build_pos = BWAPI::Positions::None;
	build_queue = 0;
	block_count = 0;
	return;
}



void SCV::build() {
	if (build_queue > 128) {
		if (unit->isUnderAttack() ||
			started_construction()) {
			build_queue = 128;
		}
		if (sqdist(unit->getPosition(), build_pos) > build_sqdist &&
			!is_janitor) {
			unit->move(build_pos);
			build_queue -= 1;
			//destination = build_pos;
		}
		else {
			std::vector<BWAPI::Unit> my_blockers = get_blockers();
			clear_build_tiles(my_blockers);
			check_build_block(my_blockers);
			if (is_janitor) {
				build_queue -= 1;
			} else {
				unit->build(build_type, build_tile);
				build_queue -= 8;
			}
			//destination = build_pos;
		}
	}
	else if (build_queue > 0) {
		build_queue -= 1;
		if (build_queue < 96 &&
			!unit->isConstructing()) {
			end_build_mission();
			//BWAPI::Broodwar->printf("SCV abort build mission");
		}
	}
	else {
		if (build_type.isRefinery()) {
			resource_container = wilbuild::refineries.back();
			set_resource(wilbuild::geysers, resource_container, +1);
		}
		end_build_mission();
	}
	draw_build_info();
	return;
}
bool SCV::started_construction() {
	for (BWAPI::Unit u : wilbuild::buildings) {
		if (build_type == u->getType() &&
			build_tile == u->getTilePosition()) {
			return true;
		}
	}
	return false;
}
std::vector<BWAPI::Unit> SCV::get_blockers() {
	std::vector<BWAPI::Unit> v;
	for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsInRectangle(x0, y0, x1, y1)) {
		if (u != unit &&
			!u->isFlying() &&
			!u->getType().isBuilding()) {
			v.push_back(u);
		}
	}
	if (build_type == BWAPI::UnitTypes::Terran_Command_Center) {
		for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsInRectangle(x0 - 96, y0 - 96, x1 + 96, y1 + 96)) {
			if (u->getType().isMineralField() &&
				u->getResources() <= 8) {
				v.push_back(u);
			}
		}
	}
	return v;
}
void SCV::clear_build_tiles(std::vector<BWAPI::Unit> &my_vec) {
	int dx = 0;
	int dy = 0;
	switch ( (BWAPI::Broodwar->getFrameCount() * 128) % 4 ) {
		case 0: dx = +128;
		case 1: dy = +128;
		case 2: dx = -128;
		case 3: dy = -128;
	}
	for (BWAPI::Unit blocker : my_vec) {
		if (blocker->getPlayer() == BWAPI::Broodwar->self()) {
			int x = blocker->getPosition().x + dx;
			int y = blocker->getPosition().y + dy;
			blocker->move(BWAPI::Position(x, y));
		}
		if (blocker->getType().isMineralField() &&
			blocker->getResources() <= 8) {
			if (!unit->isGatheringMinerals()) {
				unit->gather(blocker);
				//BWAPI::Broodwar->printf("clear blocking mineral");
			}
			is_janitor = true;
			return;
		}
		if (blocker->isBurrowed()) {
			if (!unit->isAttacking()) {
				unit->attack(blocker);
				//BWAPI::Broodwar->printf("clear blocking enemy");
			}
			is_janitor = true;
			return;
		}
	}
	is_janitor = false;
	return;
}
void SCV::check_build_block(std::vector<BWAPI::Unit> &my_vec) {
	if (BWAPI::Broodwar->self()->minerals() >= build_type.mineralPrice()  &&
		BWAPI::Broodwar->self()->gas() >= build_type.gasPrice() &&
		unit->canBuild(build_type) &&
		my_vec.empty() &&
		!BWAPI::Broodwar->hasCreep(build_tile)) {
		++block_count;
	}
	if (block_count > 16) {
		wilbuild::blocked_tile = build_tile;
		//BWAPI::Broodwar->printf("Building site blocked by something!");
	}
	return;
}
void SCV::draw_build_info() {
	BWAPI::Broodwar->drawBoxMap(x0, y0, x1, y1, 179, false);
	BWAPI::Broodwar->drawTextMap((BWAPI::Position)build_tile, build_type.c_str());
	BWAPI::Broodwar->drawTextMap(unit->getPosition(), "%d", build_queue);
	//BWAPI::Broodwar->drawLineMap(unit->getPosition(), build_pos, 179);
	return;
}



void SCV::set_repair_mission(BWAPI::Unit my_unit)
{
	end_mining();
	is_repair = true;
	damaged_unit = my_unit;
	unit->repair(my_unit);
	return;
}
void SCV::update_repair()
{
	if (damaged_unit->exists() == false ||
		damaged_unit->getHitPoints() == damaged_unit->getType().maxHitPoints() ||
		(BWAPI::Broodwar->self()->minerals() == 0 && sqdist(unit, damaged_unit) <= 9216)) {
		if (!willyt::hold_bunker) { is_repair = false; }
	}
	if (BWAPI::Broodwar->getFrameCount() - unit->getLastCommandFrame() >= 24 &&
		!unit->isRepairing() &&
		damaged_unit->exists() &&
		damaged_unit->getHitPoints() < damaged_unit->getType().maxHitPoints()) {
		unit->repair(damaged_unit);
	}
	//draw_arrow(unit->getPosition(), damaged_unit->getPosition(), BWAPI::Colors::Orange);
	return;
}



void SCV::scout()
{
	if (!wilmap::unscouted.empty())
	{
		if (unit->getTargetPosition() != wilmap::unscouted.back()) {
			scout_pos = wilmap::unscouted.back();
			unit->move(scout_pos);
			//BWAPI::Broodwar->printf("change scouting target");
		}
		//BWAPI::Broodwar->drawTextMap(unit->getPosition(), "tourist");
	}
	else
	{
		if (wilenemy::sem == -1 &&
			unit->getTargetPosition() != wilmap::my_natu) {
			unit->move(wilmap::my_natu);
			is_scout = false;
			//BWAPI::Broodwar->printf("terminate scouting");
			return; //scout is done, dont continue
		}
		if (willyt::need_scout_natu &&
			circle_increment == wilmap::entrance_circle_increment - 1) {
			scout_pos = wilmap::en_natu;
			unit->move(scout_pos);
			willyt::need_scout_natu = false;
			BWAPI::Broodwar->printf("quick check on  the natural");
		}
		if (circle_increment == -1) { // initiate circling
			circle_increment = (wilmap::entrance_circle_increment != -1) ? wilmap::entrance_circle_increment : 0;
			scout_circle();
		}
		if (sqdist(unit->getPosition(), scout_pos) < 16384) { // continue circling
			scout_circle();
		}
	}
	//draw_arrow(unit->getPosition(), unit->getTargetPosition(), 179);
	//draw_pos_box(unit->getTargetPosition(), 12, 179);
	return;
}
void SCV::scout_circle()
{
	circle_increment = (circle_increment + 1) % 24;
	scout_pos = get_circle_pos_24(wilenemy::sem, 14, circle_increment);
	unit->move(scout_pos);
}



BWAPI::Unit SCV::get_target_scv(int r) {
	BWAPI::Unit u = NULL;
	u = get_target_from(wilenemy::tlgg, u, unit->getPosition(), r);
	u = get_target_from(wilenemy::tlga, u, unit->getPosition(), r);
	u = get_target_from(wilenemy::tlg, u, unit->getPosition(), r);
	return u;
}



//void SCV::check_transport()
//{
//	destination = get_current_target_position(unit);
//	if (unit->getTransport() == NULL &&
//		!is_none(destination) &&
//		!has_grd_connection(unit->getPosition(), destination)) {
//		prio_transport++;
//	} else {
//		prio_transport = 0;
//	}
//	return;
//}
void SCV::check_being_trapped(int& count) {
	stuck_queue++;
	is_trapped = false;
	if (!unit->isCarryingMinerals() && !unit->isCarryingGas() && has_resource) {
		has_resource = false;
		stuck_queue = 0;
		min_target_distance = 0;
	}
	if ((unit->isCarryingMinerals() && !has_resource) ||
		(unit->isCarryingGas() && !has_resource)) {
		has_resource = true;
		stuck_queue = 0;
		min_target_distance = 0;
	}
	if (unit->getTransport() ||
		unit->getBuildUnit() ||
		unit->isAttackFrame()) {
		stuck_queue = 0;
		min_target_distance = 0;
	}
	if (called_transport && unit->getTransport()) {
		called_transport = false;
		is_entering = false;
		transport_unit = NULL;
	}

	destination = get_current_target_position(unit);
	if (stuck_queue > 0 && !is_none(destination))
	{
		int my_sqdist = sqdist(unit->getPosition(), destination);
		if (min_target_distance == 0) {
			min_target_distance = my_sqdist;
		}
		if (my_sqdist < min_target_distance) {
			min_target_distance = my_sqdist;
			stuck_queue = 0;
		}
		if (stuck_queue > 31) {
			is_trapped = true;
			count++;
		}
	}

	if (transport_unit != NULL &&
		transport_unit->exists() &&
		sqdist(transport_unit, unit) < 36) {
		is_entering = true;
		unit->rightClick(transport_unit);
		//BWAPI::Broodwar->printf("scv entering dropship");
	}
	//if (!is_none(destination)) {
	//	BWAPI::Broodwar->drawLineMap(unit->getPosition(), destination, BWAPI::Colors::White);
	//}
	return;
}