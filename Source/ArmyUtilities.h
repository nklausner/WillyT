#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillytState.h"
#include "WillytUnits.h"
#include "WillytBuild.h"
#include "WillytEnemy.h"
#include "WillyThreat.h"
#include "Utilities.h"
#include "MapInfluence.h"
#include "Grouper.h"

//2018-06-16: initialize defense manager for rush defense bunker
//2018-09-16: resolve defense manager, reuse as army utilities

//defend evaluation
bool check_base_under_attack();
bool check_main_or_natu_choke_defense();

//target finding
BWAPI::Unit choose_enemy_from(std::vector<BWAPI::Unit>& my_vec);
BWAPI::Position choose_enemy_unit();
BWAPI::Position choose_enemy_building_away(std::vector<BWAPI::Position> &ep, std::vector<BWAPI::Position> &em);
BWAPI::Position choose_enemy_building_near(std::vector<BWAPI::Position> &ep, BWAPI::Position& em);

//militia handling
void call_militia(int n);
void stop_militia();
bool check_stim_medics();

//cohesion related
BWAPI::Position calc_leading_pos(BWAPI::Position my_target);
//BWAPI::Position calc_leading_pos(BWAPI::Position my_target, std::vector<Fighter> &my_vec);
BWAPI::Position calc_leading_pos(BWAPI::Position my_target, std::vector<Fighter2> &my_vec);
int count_around(BWAPI::Position my_pos, int r);
int count_around_scvs(BWAPI::Position my_pos, int r);
BWAPI::Position calc_tank_center();
bool hold_back_siegetank();
int remaining_siege_research_time();

//clear and refill siegetank influence boolean map
void update_siegetank_influence();
bool check_siege_guarding();

//repulsive field to gather in front of enemy natural
void create_repulsive_field_at_enemy_natural();
void clear_repulsive_field_at_enemy_natural();

//check for a safe spot (no ground defense) to attack a position
bool can_attack_safe_r4(BWAPI::Position &my_pos);
//check if defense target position is outside natural choke
BWAPI::Position hold_back_defense_at_natural_choke(BWAPI::Position& my_pos);