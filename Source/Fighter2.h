#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytEnemy.h"
#include "WillyThreat.h"
#include "Utilities.h"
#include "MicroUtilities.h"

//2019-12-12: rewriting of fighter class for more independent behaviour

class Fighter2
{
public:

	//general

	Fighter2(BWAPI::Unit my_unit);
	BWAPI::Unit unit;
	unsigned id;

	//methods

	void update();
	void update_vulture();
	void set_target(BWAPI::Position& my_pos, bool is_attack);
	void set_swarm(bool immediately);
	void update_raider(int my_state, BWAPI::Position& att_pos, BWAPI::Position& def_pos = willyt::retreat_pos, BWAPI::Unit my_transport = NULL);

	//specials

	void check_stim();
	void check_fallb();
	void check_flare();
	void check_lockd();
	void check_cloak();

	//mechanicals

	void check_siege();
	void check_mines();

	//exception

	bool is_raider;
	bool is_attacker;

private:

	//methods

	void set_retreat();
	void check_cohesion_retreat();
	void force_unsiege();
	bool check_sneakiness();

	//booleans

	bool is_onlygrd;
	bool is_support;
	bool is_looping;
	bool specials_allowed;
	bool changed_attack_pos;

	//positions

	int attack_sqdst;
	int retreat_sqdst;
	BWAPI::TilePosition tile;
	BWAPI::Position posi;
	BWAPI::Position secure_pos;
	BWAPI::Position attack_pos;
	BWAPI::Position special_pos;
	BWAPI::Position destin_pos;
	void set_special_position(BWAPI::Position my_pos);

	//queues

	int attack_queue;
	int special_queue;
	int retreat_queue;
	int spider_mine_count;
	int command_frames;

	//targeting

	BWAPI::Unit target;
	BWAPI::Unit get_target_all(int r);
	BWAPI::Unit get_target_heal();
	BWAPI::Unit get_target_siege();
	BWAPI::Unit get_target_goliath();
	BWAPI::Unit get_target_vulture();

	//transport

	BWAPI::Unit transport;


};