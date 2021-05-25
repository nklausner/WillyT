#pragma once
#include <BWAPI.h>
#include "Utilities.h"

namespace wilgroup
{
	extern std::vector<int> maxsqdist_vec;
	extern std::vector<BWAPI::Unit> player_grd_vec;
	extern std::vector<BWAPI::Unit> player_air_vec;
	extern std::vector<BWAPI::Unit> enemy_grd_vec;
	extern BWAPI::Position player_grd_pos;
	extern BWAPI::Position player_air_pos;
	extern BWAPI::Position enemy_grd_pos;
	extern int player_grd_supply;
	extern int player_air_supply;
	extern int enemy_grd_supply;
}

void update_army_groups();
void update_group(std::vector<BWAPI::Unit>& my_vec, BWAPI::Position& my_pos, int& my_sup);
BWAPI::Position calculate_center_pos(std::vector<BWAPI::Unit>& my_vec);
std::vector<BWAPI::Unit> reduce_army_group(std::vector<BWAPI::Unit>& old_vec, BWAPI::Position& my_pos, int& maxsqdist);
int calculate_army_supply(std::vector<BWAPI::Unit>& my_vec);
