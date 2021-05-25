#pragma once
#include <BWAPI.h>
#include "ArmyAttacker.h"
#include "WillytState.h"
#include "WillytUnits.h"
#include "WillytMap.h"
#include "WillyThreat.h"
#include "Utilities.h"
#include "MicroUtilities.h"
#include "Grouper.h"

//2018-08-25: initiate manager for vulture raid on expansions
//2020-06-18: vulture raids now done via mining missions
//			  rewrite to manage drop play: biosquads or vultures

struct DropManager
{
public:
	void update();
	void show_info(int x, int y);
	void keep_kills(BWAPI::Unit my_unit);

private:
	BWAPI::Unit dropship = NULL;
	std::vector<BWAPI::Unit> raiders = {};
	std::string state = "none";
	int ci = 0;
	int di = 0;
	BWAPI::Position last_pos = BWAPI::Positions::None;
	BWAPI::Position next_pos = BWAPI::Positions::None;
	BWAPI::Position side_pos = BWAPI::Positions::None;
	int leave_queue = 0;
	int idle_queue = 0;

	int kills = 0;
	int kills_offset = 0;
	int kills_alive = 0;
	int kills_dead = 0;
	std::vector<int> kills_pos = {};
	std::vector<int> kills_neg = {};

	void initiate_drop();
	void terminate_drop();
	bool evaluate_dropping();

	void assign_from(std::vector<Fighter2>& my_vec, unsigned i, BWAPI::Position my_pos);
	std::vector<Flyer>::iterator get_avaiable_dropship();
	int count_remaining_raiders();
	bool load_raiders();
	void unload_raiders();

	int count_kills_total();
	int choose_direction();
	int choose_direction_random();
	bool should_avoid_drops();
	void check_idle_drop();

	int get_expo_index(BWAPI::Position praid);
	void fly_to_next_expo();
	bool dropship_is_near(BWAPI::Position my_pos, int s);
	bool raid_this_base(BWAPI::Position my_pos);
	bool enemy_workers_near(BWAPI::Position my_pos);
	bool enemy_buildings_near(BWAPI::Position my_pos);
	BWAPI::Position find_save_location(BWAPI::TilePosition td, BWAPI::TilePosition tn);


};