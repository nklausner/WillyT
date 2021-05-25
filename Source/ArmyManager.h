#pragma once
#include <BWAPI.h>
#include "ArmyAttacker.h"
#include "ArmyUtilities.h"
#include "ArmyPositions.h"
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytEnemy.h"
#include "MicroUtilities.h"

//2018-03-10: army manager for attacking
//2018-09-??: rewrite to enable concurrent attacking and defending
//2020-05-20: reworking, removed queue which was needed for fighter1

struct ArmyManager
{
public:
	void update();

private:
	bool decide_attack(int n);
	BWAPI::Position get_gathering_position();
	BWAPI::Position get_defending_target_position(std::vector<BWAPI::Unit>& my_vec);
	BWAPI::Position get_attacking_target_position(bool my_cond);
	bool check_swarming(bool my_cond);
	void check_militia(bool is_under_attack, int def_strength);
	void check_cohesion(int &n);
	void check_flyers(std::vector<Flyer>& my_vec, int gs);
	void attack_to_pratice_micro();

	//void check_defend(int d, int a, BWAPI::Unit enemy_attacker);
	//void check_attack(int n);
	//void update_positions();
};