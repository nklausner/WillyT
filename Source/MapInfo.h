#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytMap.h"
#include "WillytExpo.h"
#include "WillytBuild.h"
#include "WillytEnemy.h"
#include "WillyThreat.h"
#include "Grouper.h"

//2019-03-05: gather drawing infos to the map

struct MapInfo
{
public:
	//display stuff on map
	void display();

private:
	//display positions and expansions
	void display_start_positions();
	void display_defense_positions();
	void display_expansions();
	void display_resources();
	void display_buildplan();
	void display_blocked_expo();
	void display_choke_directions();

	//display army positions
	void display_army_positions();
	void display_enemy_positions();
	void display_enemy_positions_surround();
	void display_enemy_units(std::vector<UnitInfo>& my_vec);
	void display_army_groups();
	void display_army_group(std::vector<BWAPI::Unit>& my_vec, BWAPI::Position& my_pos, int& my_sup, int color);

	//draw map at viewport (20x15 tiles, 640x480)
	void display_bool(bool(&my_map)[256][256], bool bool_modus, BWAPI::Color c);
	void display_int(int(&my_map)[256][256], int threshold);
	void display_int_bool(int(&my_map)[256][256], BWAPI::Color c);
	void display_bool_walkpos(bool(&my_map)[1024][1024], bool bool_modus, BWAPI::Color c);
	void display_int_walkpos(int(&my_map)[1024][1024], BWAPI::Color c);
	void display_int_walkpos(int(&my_map)[1024][1024]);
	void display_walkability();

	void display_space_colors();
	BWAPI::Color get_space_color(int i);

	//general drawing utilities
	void draw_unit_vec(std::vector<BWAPI::Unit> &my_list, int w, int h, int my_color);
	void draw_tile_vec(std::vector<BWAPI::TilePosition> &my_list, int w, int h, int my_color);
	void draw_tile_box(BWAPI::TilePosition &tile, int dx, int dy, int color);
	void draw_tile_box_check(BWAPI::TilePosition &tile, int dx, int dy, int color);
	void draw_tile_vec_index(std::vector<BWAPI::TilePosition> &my_list);
	void draw_line(std::vector<BWAPI::Position> &v, int c);
	void draw_pos_vec(std::vector<BWAPI::Position> &v, int c);
	void draw_cross(BWAPI::Position p, int s, BWAPI::Color c);
	void draw_crosshair(BWAPI::Position p, int s, BWAPI::Color c);
	void display_pair_vectors(std::vector<BWAPI::Position> &v0, std::vector<BWAPI::Position> &v1);
	void display_threats_all();
	void display_threats(std::vector<Threat> &v);
	void display_holding_bunker();
	void display_available_scans();
	void display_choke_defense_status();

	bool in_viewport(BWAPI::Position p);
	bool in_viewport(int x, int y);
	void update_viewport();

	int px0 = 0;
	int px1 = 0;
	int py0 = 0;
	int py1 = 0;
	int wx0 = 0;
	int wx1 = 0;
	int wy0 = 0;
	int wy1 = 0;
	int tx0 = 0;
	int tx1 = 0;
	int ty0 = 0;
	int ty1 = 0;
};