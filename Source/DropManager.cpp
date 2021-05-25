#include "DropManager.h"


void DropManager::update()
{
	willyt::go_raiding = evaluate_dropping();

	if (state == "none")
	{
		if (willyt::go_raiding && get_avaiable_dropship() != wilunits::dropships.end()) {
			initiate_drop();
			state = "init";
		}
		return;
	}
	else
	{
		if (!dropship->exists() || count_remaining_raiders() <= 2) {
			terminate_drop();
			state = "none";
			return;
		}
	}

	if ((state == "load" || state == "init") && load_raiders())
	{
		fly_to_next_expo();
		state = "move";
	}

	if (state == "move")
	{
		if (reaching_area(dropship, wilthr::grddef) || reaching_area(dropship, wilthr::airdef)) {
			side_pos = find_save_location(dropship->getTilePosition(), BWAPI::TilePosition(next_pos));
			dropship->unloadAll(side_pos);
			state = "raid";
		}
		if (dropship_is_near(next_pos, 8) && raid_this_base(next_pos)) {
			unload_raiders();
			state = "raid";
		}
		if (dropship_is_near(next_pos, 2) && load_raiders()) {
			fly_to_next_expo();
			state = "move";
		}
	}

	if (state == "raid")
	{
		if (raid_this_base(next_pos)) {
			if (is_none(side_pos)) {
				raiders_update(2, next_pos, next_pos, NULL);
				unload_raiders();
			} else {
				raiders_update(2, next_pos, side_pos, NULL);
				BWAPI::Broodwar->drawLineMap(dropship->getPosition(), side_pos, 165);
			}
			leave_queue = 0;
		} else if (leave_queue < 4) {
			leave_queue++;
		} else {
			load_raiders();
			state = "load";
			side_pos = BWAPI::Positions::None;

		}
	}
	check_idle_drop();
	kills = count_kills_total() + kills_dead - kills_offset;
	return;
}



void DropManager::initiate_drop()
{
	using namespace wilunits;

	std::vector<Flyer>::iterator it = get_avaiable_dropship();
	it->is_raider = true;
	dropship = it->unit;
	BWAPI::Position my_pos = dropship->getPosition();

	ci = get_expo_index(wilmap::my_main);
	di = choose_direction();
	last_pos = wilmap::my_main;
	side_pos = BWAPI::Positions::None;

	raiders.clear();
	if (willyt::strategy == 4)
	{
		assign_from(siegetanks, 2, my_pos);
	}
	else if (2 * willyt::sup_bio >= willyt::sup_mech)
	{
		int nf = (6 * firebats.size() + 3) / (marines.size() + firebats.size());
		int nm = 6 - nf;
		assign_from(marines, nm, my_pos);
		assign_from(firebats, nf, my_pos);
		assign_from(medics, 2, my_pos);
	}
	else
	{
		int nv = (8 * vultures.size() + 4) / willyt::sup_mech;
		int nt = (4 * siegetanks.size() + 2) / willyt::sup_mech;
		int ng = 4 - nv - 2*nt;
		assign_from(vultures, nv, my_pos);
		assign_from(siegetanks, nt, my_pos);
		assign_from(goliaths, ng, my_pos);
	}
	kills_offset = count_kills_total();
	kills = 0;
	kills_alive = 0;
	kills_dead = 0;
	leave_queue = 0;
	idle_queue = 0;

	load_raiders();
	BWAPI::Broodwar->printf("initialize drop");
	return;
}
void DropManager::terminate_drop()
{
	if (dropship->exists()) { dropship->move(wilmap::my_main); }
	for (Flyer& ds : wilunits::dropships) { ds.is_raider = false; }
	dropship = NULL;
	raiders_update(0, willyt::gather_pos);
	raiders.clear();
	kills = count_kills_total() + kills_dead - kills_offset;
	if (state != "init") {
		if (di == +1) { kills_pos.push_back(kills); }
		if (di == -1) { kills_neg.push_back(kills); }
	}
	kills = 0;
	BWAPI::Broodwar->printf("terminate drop");
	return;
}
bool DropManager::evaluate_dropping()
{
	//how many units do we have?
	//where is the enemy army?
	//how may enemy expansions do we know of?
	//how are they fortified?

	//sufficient army size, defense still priority
	//army is gathering or trapped at home
	if (willyt::strategy != 4 &&
		willyt::sup_bio + willyt::sup_mech >= 32 &&
		willyt::count_trapped == 0 &&
		sqdist(wilgroup::player_grd_pos, wilmap::my_natu) < 409600 &&
		!should_avoid_drops()) {
		return true;
	}
	if (willyt::strategy == 4 &&
		willyt::count_trapped == 0 &&
		willyt::go_islands &&
		kills_pos.empty() && kills_neg.empty()) {
		return true;
	}
	if (BWAPI::Broodwar->self()->supplyUsed() >= 380 &&
		BWAPI::Broodwar->self()->minerals() > 1200 &&
		BWAPI::Broodwar->self()->gas() > 800) {
		return true;
	}
	return false;
}



void DropManager::assign_from(std::vector<Fighter2>& my_vec, unsigned i, BWAPI::Position my_pos)
{
	std::vector<Fighter2>::reverse_iterator it = my_vec.rbegin();
	while (i > 0 && it != my_vec.rend()) {
		//it->update_raider(1, my_pos, willyt::retreat_pos, dropship);
		raiders.push_back(it->unit);
		i--; it++;
	}
	return;
}
std::vector<Flyer>::iterator DropManager::get_avaiable_dropship() {
	for (std::vector<Flyer>::iterator it = wilunits::dropships.begin(); it != wilunits::dropships.end(); ++it) {
		if (it->unit->exists() &&
			it->unit->getSpaceRemaining() == 8 &&
			sqdist(it->unit->getPosition(), wilmap::my_main) < 262144) {
			return it;
		}
	}
	return wilunits::dropships.end();
}
int DropManager::count_remaining_raiders()
{
	int my_space = 0;
	for (BWAPI::Unit u : raiders) {
		if (u->exists()) {
			my_space += u->getType().spaceRequired();
		}
	}
	return my_space;
}



bool DropManager::load_raiders()
{
	raiders_update(1, dropship->getPosition(), dropship->getPosition(), dropship);
	bool all_loaded = true;
	for (BWAPI::Unit u : raiders) {
		if (u->exists() && !u->isLoaded())
		{
			if (all_loaded) { dropship->load(u); }
			all_loaded = false;
			BWAPI::Broodwar->drawLineMap(u->getPosition(), dropship->getPosition(), 135);
		}
	}
	return all_loaded;
}
void DropManager::unload_raiders()
{
	for (BWAPI::Unit u : dropship->getLoadedUnits()) {
		dropship->unload(u);
		return;
	}
}



int DropManager::count_kills_total()
{
	int i = 0;
	for (BWAPI::Unit u : raiders) {
		i += u->getKillCount();
	}
	return i;
}
void DropManager::keep_kills(BWAPI::Unit my_unit)
{
	for (BWAPI::Unit u : raiders) {
		if (u == my_unit) {
			vector_remove(raiders, my_unit);
			kills_dead += my_unit->getKillCount();
			return;
		}
	}
}
int DropManager::choose_direction()
{
	if (!kills_pos.empty() && kills_neg.empty()) { return -1; }
	if (kills_pos.empty() && !kills_neg.empty()) { return +1; }

	if (!kills_pos.empty() && !kills_neg.empty() &&
		kills_pos.back() + kills_neg.back() >= 8)
	{
		if (kills_pos.back() >= kills_neg.back()) { return +1; }
		if (kills_neg.back() >= kills_pos.back()) { return -1; }
	}
	//if (willyt::go_islands) {}
	return choose_direction_random();
}
int DropManager::choose_direction_random()
{
	return ((rand() % 2) * 2) - 1; //-1 or +1
}
bool DropManager::should_avoid_drops()
{
	if (kills_pos.size() + kills_neg.size() >= 4 &&
		kills_pos.back() + kills_neg.back() < 6) {
		return true;
	}
	return false;
}
void DropManager::check_idle_drop()
{
	if (dropship->isIdle() &&
		dropship->getSpaceRemaining() < 8) {
		idle_queue++;
	}
	if (idle_queue > 16) {
		terminate_drop();
		state = "none";
	}
	return;
}



int DropManager::get_expo_index(BWAPI::Position praid)
{
	BWAPI::Position pexpo = get_closest(wilmap::aircircle, praid);
	for (unsigned i = 0; i < wilmap::aircircle.size(); i++)
		if (wilmap::aircircle[i] == pexpo)
			return (int)i;
	return 0;
}
void DropManager::fly_to_next_expo()
{
	last_pos = wilmap::flycircle.at(ci);
	ci = (ci + di + wilmap::flycircle.size()) % wilmap::flycircle.size();
	next_pos = wilmap::flycircle.at(ci);
	dropship->move(next_pos);
	return;
}
bool DropManager::dropship_is_near(BWAPI::Position my_pos, int s)
{
	return (sqdist(dropship->getPosition(), my_pos) < 1024 * s * s) ? true : false;
}
bool DropManager::raid_this_base(BWAPI::Position my_pos)
{
	if (enemy_workers_near(my_pos) || wilthr::getairdef(my_pos)) {
		return true;
	}
	if (wilmap::centerdistmap[my_pos.y / 32][my_pos.x / 32] < 0 && enemy_buildings_near(my_pos)) {
		//BWAPI::Broodwar->printf("raid destroy island base completely");
		return true;
	}
	return false;
}
bool DropManager::enemy_workers_near(BWAPI::Position my_pos)
{
	for (BWAPI::Player p : BWAPI::Broodwar->enemies())
		for (BWAPI::Unit u : p->getUnits())
			if (u->getType().isWorker() &&
				sqdist(u->getPosition(), my_pos) < 65536)
				return true;
	return false;
}
bool DropManager::enemy_buildings_near(BWAPI::Position my_pos)
{
	for (BWAPI::Player p : BWAPI::Broodwar->enemies())
		for (BWAPI::Unit u : p->getUnits())
			if (u->getType().isBuilding() &&
				sqdist(u->getPosition(), my_pos) < 65536)
				return true;
	return false;
}
BWAPI::Position DropManager::find_save_location(BWAPI::TilePosition td, BWAPI::TilePosition tn)
{
	int x0 = mapsafesub(td.x, 8);
	int x1 = mapsafeadd(td.x, 8, wilmap::wt);
	int y0 = mapsafesub(td.y, 8);
	int y1 = mapsafeadd(td.y, 8, wilmap::ht);
	int dopt = 1048576;
	int xopt = td.x;
	int yopt = td.y;
	int gn = get_ground_dist(tn);

	for (int y = y0; y < y1; y++) {
		for (int x = x0; x < x1; x++) {
			if (wilmap::walk_map[y][x] == true &&
				wilthr::airdef[4*y][4*x] == 0 &&
				wilthr::grddef[4*y][4*x] == 0)
			{
				int d = sqdist(td.x, td.y, x, y) + sqdist(tn.x, tn.y, x, y);
				if (d < dopt) {
					dopt = d;
					xopt = x;
					yopt = y;
				}
			}
		}
	}
	if (abs(get_ground_dist(tn.x, tn.y) - get_ground_dist(xopt, yopt)) > 24) {
		BWAPI::Broodwar->printf("no altering drop location found");
		return BWAPI::Position(32 * tn.x + 16, 32 * tn.y + 16);
	}
	BWAPI::Broodwar->printf("base defended, altering drop location");
	return BWAPI::Position(32 * xopt + 16, 32 * yopt + 16);
}



void DropManager::show_info(int x, int y) {
	if (!wilunits::dropships.empty() && (willyt::go_raiding || state != "none")) {
		BWAPI::Broodwar->drawTextScreen(x, y, "Drops: %d / %d", kills_pos.size(), kills_neg.size());
		BWAPI::Broodwar->drawTextScreen(x, y+10, "state: %s", state.c_str());
		BWAPI::Broodwar->drawTextScreen(x, y+20, "kills: %d", kills);
		BWAPI::Broodwar->drawTextScreen(x, y+30, "leave: %d", leave_queue);
	}
	return;
}

