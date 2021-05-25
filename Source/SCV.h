#pragma once
#include <BWAPI.h>
#include "WillytBuild.h"
#include "WillytMap.h"
#include "WillytState.h"
#include "WillytEnemy.h"
#include "WillyThreat.h"
#include "Utilities.h"
#include "MapUtilities.h"
#include "MicroUtilities.h"

//2018-03-08: SCV class, first single unit object

class SCV
{
public:

	//general

	SCV(BWAPI::Unit my_unit);
	//~SCV();
	BWAPI::Unit unit;
	unsigned id;

	//states

	bool is_miner;
	bool is_builder;
	bool is_militia;
	bool is_janitor;
	bool is_repair;
	bool is_scout;
	bool is_trapped;
	bool called_transport;

	//positions

	BWAPI::Position destination;
	BWAPI::Position expo_pos;
	
	//methods

	void update();
	void move(BWAPI::Position p);
	void set_attack(BWAPI::Position p);
	void set_swarm(bool immediately);
	void change_resource(BWAPI::Unit my_unit, bool remove_stack);
	void end_mining();
	void set_expo_pos(BWAPI::Position p);
	BWAPI::Unit get_resource_container();
	void set_build_mission(BWAPI::UnitType type, BWAPI::TilePosition &tile);
	void end_build_mission();
	void set_repair_mission(BWAPI::Unit my_unit);
	void check_being_trapped(int &count);

private:

	//private methods

	void mine();
	BWAPI::Unit get_mineral(bool is_owned, int min_count);
	BWAPI::Unit get_mineral_on(BWAPI::TilePosition my_tile);
	void set_resource(BWAPI::Unit my_unit, int d);
	void set_resource(std::vector<Resource>& my_vec, BWAPI::Unit my_unit, int d);
	void update_militia();
	void build();
	void update_repair();
	void scout();
	BWAPI::Unit get_target_scv(int r);

	//positions and integers

	BWAPI::Position secure_pos;
	BWAPI::Position attack_pos;

	BWAPI::UnitType build_type;
	BWAPI::Position build_pos;
	BWAPI::TilePosition build_tile;
	int build_queue;
	int build_sqdist;
	int block_count;
	int x0;
	int y0;
	int x1;
	int y1;
	BWAPI::Unit damaged_unit;
	BWAPI::Unit resource_container;

	//other queues and states

	int circle_increment;
	int scout_queue;
	bool has_resource;
	int stuck_queue;
	int min_target_distance;

	//building auxiliaries

	bool started_construction();
	std::vector<BWAPI::Unit> get_blockers();
	void clear_build_tiles(std::vector<BWAPI::Unit> &my_vec);
	void check_build_block(std::vector<BWAPI::Unit> &my_vec);
	void draw_build_info();
};