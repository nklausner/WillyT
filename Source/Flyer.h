#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytMap.h"
#include "WillyThreat.h"
#include "WillytEnemy.h"
#include "Grouper.h"
#include "Utilities.h"
#include "MapUtilities.h"
#include "MicroUtilities.h"


//2019-11-15: Flyer class
//2021-01-20: first time working annoying wraiths


class Flyer
{
public:

	//general

	Flyer(BWAPI::Unit my_unit);
	BWAPI::Unit unit;
	unsigned id;

	//states

	bool is_circling;
	bool is_raider;

	//methods

	void move(BWAPI::Position my_pos, const char* s="");
	void attack();
	void swarm(bool immediatly);
	void set_circle_diff(int new_dc);
	int get_circle_diff();
	void update();
	void update_dropship();
	void correct_dropship();
	BWAPI::Position transport_pos;
	BWAPI::Unit transport_unit;

private:

	//Positions

	BWAPI::Position attack_pos;
	BWAPI::TilePosition attack_tile;
	BWAPI::Position latest_pos;
	BWAPI::Position retreat_pos;
	int command_queue;
	int looping_queue;
	int ability_queue;
	int attack_queue;

	//circle

	int dc; //diff circle -1/+1
	int ic; //index circle
	int ica; //index circle absolute

	//units

	BWAPI::Unit ability_target;
	BWAPI::Unit attack_target;
	BWAPI::Unit threat_target;

	//methods

	bool evade_air_threats();
	void loop_to_evade(BWAPI::Position my_pos);

	//specials

	void fly_in_circles();
	void fly_random(std::vector<BWAPI::Position>& my_vec);
	void check_yamato();
	BWAPI::Unit get_target_yamato();
	BWAPI::Unit get_target_wraith();
	BWAPI::Unit get_target_all(int sqd);
	BWAPI::Unit get_close_threat();

};