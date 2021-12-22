#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytMap.h"
#include "WillytUnits.h"
#include "Formation.h"
#include <algorithm>

//2018-09-23: concurrent attacking and defending
//2018-09-23: just collect army attacking here

void army_attack(BWAPI::Position& my_pos, bool is_attack);
void army_swarm(bool immediately);
void raiders_update(int my_state, BWAPI::Position& att_pos, BWAPI::Position& def_pos = willyt::retreat_pos, BWAPI::Unit my_transport = NULL);
void army_attack_tank_only(BWAPI::Position& pa, BWAPI::Position& pt, bool is_attack);
void army_split(BWAPI::Position& dpos, BWAPI::Position& apos, int enemytotstr, int enemyairstr, bool is_attack);
void execute_split(BWAPI::Position& dpos, BWAPI::Position& apos, std::vector<SCV>& my_vec, int dcount);
void execute_split(BWAPI::Position& dpos, BWAPI::Position& apos, std::vector<Fighter2>& my_vec, int dcount, bool is_attack);

void assign_formation(BWAPI::Position& my_pos, bool is_attack);
void assign_formation_scvs(BWAPI::Position& my_pos, bool is_attack);
void assign_formation_firebats_medics(BWAPI::Position& my_pos, bool is_attack);
void assign_formation_marines(BWAPI::Position& my_pos, bool is_attack);