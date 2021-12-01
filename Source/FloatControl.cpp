#include "FloatControl.h"

void FloatControl::update(ExpoManager& expomanager)
{
	if (willyt::cannon_rush_alert) {
		for (BWAPI::Unit b : wilbuild::barracks) {
			check_relocation(b);
		}
		for (BWAPI::Unit u : wilbuild::commandcenters) {
			check_lift_base_when_cannon_rushed(u, expomanager);
			check_land_base_when_cannon_rushed(u, expomanager);
		}
	}
	if ((willyt::strategy == 3 || willyt::strategy == 4))
	{
		//relocate barracks to natural
		if (willyt::has_natural &&
			wilenemy::race != BWAPI::Races::Terran &&
			!willyt::cannon_rush_alert &&
			!is_none(wilmap::natu_wall_tile[wilmap::mm][0]) &&
			!wilbuild::machineshops.empty() && !wilbuild::barracks.empty())
		{
			check_relocation_wallin(wilbuild::barracks.front(), wilmap::natu_wall_tile[wilmap::mm][0]);
			check_open_close_wallin(wilbuild::barracks.front(), wilmap::natu_wall_tile[wilmap::mm][0]);
		}
		//lift engybay and rax
		if (true) {
			lift_all(wilbuild::engineerbays);
		}
		if (!wilenemy::siegetanks.empty() && willyt::my_time > 360) {
			lift_all(wilbuild::barracks);
		}
		//control floating buildings for scouting
		if (!wilenemy::siegetanks.empty()) {
			update_float_scouts();
			float_scout_enemy_siegetanks();
		}
	}
	//if (willyt::strategy == 4 &&
	//	willyt::my_time < 900 &&
	//	!is_none(wilmap::my_sneaky_tile) &&
	//	wilbuild::factories.size() == 3)
	//{
	//	relocate_to(wilbuild::factories.back(), wilmap::my_sneaky_tile);
	//}
	//if (is_none(wilmap::natu_wall_tile[wilmap::mm][0])) {
	//	BWAPI::Broodwar->printf("float test none");
	//}
	return;
}



void FloatControl::lift_all(std::vector<BWAPI::Unit>& my_vec)
{
	for (BWAPI::Unit u : my_vec) {
		if (!u->isLifted() && u->isCompleted()) {
			wilbuild::lift(u);
		}
	}
	return;
}
void FloatControl::check_relocation(BWAPI::Unit my_unit)
{
	BWAPI::TilePosition my_old = my_unit->getTilePosition();
	if (my_unit->exists() &&
		my_unit->isCompleted() &&
		my_unit->canLift() &&
		check_map_area(wilthr::grddef, my_old.x, my_old.y, 4, 3, 4) == false)
	{
		wilbuild::lift(my_unit);
		//BWAPI::Broodwar->printf("meep, meep, meep - emergency lift off");
	}
	if (my_unit->isLifted() &&
		my_unit->getOrder() != BWAPI::Orders::BuildingLand)
	{
		BWAPI::TilePosition my_new = wilplanner::choose_tile(wilmap::plan_large, 4, 3);
		if (my_new != BWAPI::TilePositions::None) {
			wilbuild::land(my_unit, my_new);
			//BWAPI::Broodwar->printf("meep, meep, meep - relocate to new place");
		}
	}
	return;
}
void FloatControl::check_relocation_wallin(BWAPI::Unit my_unit, BWAPI::TilePosition my_tile)
{
	if (my_unit->exists() &&
		my_unit->isCompleted() &&
		my_unit->getTilePosition() != my_tile &&
		my_unit->canLift())
	{
		wilbuild::lift(my_unit);
	}
	if (my_unit->isLifted() && 
		wilmap::build_map_var[my_tile.y][my_tile.x] &&
		my_unit->getOrder() != BWAPI::Orders::BuildingLand &&
		my_unit->canLand(my_tile))
	{
		wilbuild::land(my_unit, my_tile);
	}
	//BWAPI::Broodwar->printf("%d, %d", my_tile.x, my_tile.y);
	return;
}
void FloatControl::check_open_close_wallin(BWAPI::Unit my_unit, BWAPI::TilePosition my_tile)
{
	if (willyt::att_target_pos != BWAPI::Positions::None ||
		willyt::is_swarming)
	{
		if (my_unit->exists() &&
			my_unit->isCompleted() &&
			my_unit->getTilePosition() == my_tile &&
			my_unit->canLift()) {
			my_unit->lift();
		}
	}
	else {
		if (my_unit->exists() &&
			my_unit->isCompleted() &&
			my_unit->isLifted() &&
			my_unit->canLand(my_tile)) {
			my_unit->land(my_tile);
		}
	}
	return;
}
void FloatControl::relocate_to(BWAPI::Unit my_unit, BWAPI::TilePosition my_tile)
{
	if (my_unit->exists() &&
		my_unit->isCompleted() &&
		my_unit->getTilePosition() != my_tile &&
		my_unit->canLift())
	{
		wilbuild::lift(my_unit);
	}
	if (my_unit->isLifted())
	{
		if (sqdist(my_unit->getTilePosition(), my_tile) > 16) {
			BWAPI::Position my_pos = BWAPI::Position(my_tile.x * 32 + 64, my_tile.y * 32 + 32);
			my_unit->move(BWAPI::Position(my_pos));
		}
		else
		if (wilmap::build_map_var[my_tile.y][my_tile.x] &&
			my_unit->getOrder() != BWAPI::Orders::BuildingLand &&
			my_unit->canLand(my_tile)) {
			wilbuild::land(my_unit, my_tile);
		}
	}
	//BWAPI::Broodwar->printf("%d, %d", my_tile.x, my_tile.y);
	return;
}



void FloatControl::check_lift_base_when_cannon_rushed(BWAPI::Unit u, ExpoManager& expomanager)
{
	if (u->isCompleted() && u->canLift() &&
		check_map_area(wilthr::grddef, u->getTilePosition().x, u->getTilePosition().y, 4, 3, 4) == false)
	{
		wilbuild::lift(u);
		expomanager.desert_expo(u->getTilePosition());
	}
	return;
}
void FloatControl::check_land_base_when_cannon_rushed(BWAPI::Unit u, ExpoManager& expomanager)
{
	if (u->isLifted() && sqdist(u->getPosition(), wilmap::my_main_def) > 4096 &&
		check_map_area(wilthr::grddef, wilmap::my_natu_tile.x, wilmap::my_natu_tile.y, 4, 3, 4) == false)
	{
		u->move(wilmap::my_main_def);
	}
	if (u->isLifted() &&
		check_map_area(wilthr::grddef, wilmap::my_natu_tile.x, wilmap::my_natu_tile.y, 4, 3, 4))
	{
		if (sqdist(u->getPosition(), wilmap::my_natu) > 4096) {
			u->move(wilmap::my_natu);
		}
		else
		if (u->canLand(wilmap::my_natu_tile) &&
			u->getOrder() != BWAPI::Orders::BuildingLand) {
			wilbuild::land(u, wilmap::my_natu_tile);
			need_occupying = true;
		}
	}
	if (need_occupying && u->getTilePosition() == wilmap::my_natu_tile)
	{
		expomanager.occupy_expo(u);
		need_occupying = false;
	}
	return;
}


void FloatControl::float_scout_enemy_siegetanks()
{
	BWAPI::Unit my_unit = NULL;
	BWAPI::Position my_pos = get_enemy_siegetank_pos();
	if (!is_none(my_pos) && !floaters.empty()) {
		my_unit = get_closest(floaters, my_pos);
		if (my_unit) {
			my_unit->move(linear_interpol_abs(my_pos, my_unit->getPosition(), 288));
		}
	}
	for (BWAPI::Unit u : floaters) {
		if (u != NULL && (u != my_unit || u->isUnderAttack())) {
			u->move(wilgroup::player_grd_pos);
		}
	}
	return;
}
void FloatControl::update_float_scouts()
{
	floaters.clear();
	for (BWAPI::Unit u : wilbuild::barracks) {
		if (u->isLifted()) {
			floaters.push_back(u);
		}
	}
	for (BWAPI::Unit u : wilbuild::engineerbays) {
		if (u->isLifted()) {
			floaters.push_back(u);
		}
	}
	return;
}
BWAPI::Position FloatControl::get_enemy_siegetank_pos()
{
	for (UnitInfo &ui : wilenemy::siegetanks) {
		if (!is_none(ui.pos) &&
			sqdist(ui.pos, wilgroup::player_grd_pos) < 1048576)
		{
			for (Fighter2 &f : wilunits::siegetanks) {
				if (f.unit->exists() &&
					sqdist(ui.pos, f.unit->getPosition()) < 200704) {
					return ui.pos;
				}
			}
		}
	}
	return BWAPI::Positions::None;
}