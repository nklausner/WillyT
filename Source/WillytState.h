#pragma once
#include <string>
#include <BWAPI.h>

//2018-05-19: bot namespace initialized
//2018-06-17: add first use, mineral and geyser lists
//2018-06-21: transfering all unit lists here
//2019-03-08: moving all maps and positions

namespace willyt
{
	//strategy
	extern int strategy;
	extern int test_strategy;
	extern int orig_strategy;
	extern bool show_info;
	extern bool is_vs_human;
	extern bool avoid_weak_strategies;
	extern bool use_hardcoded_strategies;
	extern bool ums_to_pratice_micro;

	//general
	extern unsigned my_time; //in seconds for fastest speed

	//supply counts (natural count)
	extern int sup_scv;
	extern int sup_mil;
	extern int sup_bio;
	extern int sup_mech;
	extern int sup_air;

	//supply tresholds
	extern int attack_supply;
	extern int mining_supply;
	extern int attack_supply_modifier;

	//available resources containers
	extern int mineral_count;
	extern int geyser_count;
	extern int min_scv_count_min;
	extern int min_scv_count_gas;

	//tactic booleans
	extern bool is_swarming;
	extern bool do_bio_scv_rush;
	extern bool is_choke_def;
	extern bool is_rushing;

	extern bool go_bio;
	extern int max_bio_upgrade;
	extern bool fast_expand;
	extern bool siege_started;

	extern bool has_natural;
	extern bool has_transport;
	extern bool go_islands;
	extern bool go_raiding;
	extern bool blocked_mainchoke;
	extern unsigned available_scans;

	extern bool rush_alert;
	extern bool eight_rax;
	extern bool need_turrets;
	extern bool need_turrets_2;
	extern bool need_detection;
	extern bool stim_allowed;
	extern bool avoid_grddef;
	extern bool proxy_alert;
	extern bool proxy_prod_alert;
	extern bool cannon_rush_alert;
	extern bool carrier_rush_alert;
	extern bool lurker_rush_alert;
	extern bool hold_bunker;
	//extern bool guard_siege;
	extern bool flyer_attack_airdef;
	extern bool need_scout_natu;

	//army positions
	extern BWAPI::Position retreat_pos;
	extern BWAPI::Position gather_pos;
	extern BWAPI::Position def_target_pos;
	extern BWAPI::Position att_target_pos;
	extern BWAPI::Position leader_pos;
	//extern BWAPI::Position siege_pos;
	extern BWAPI::Position enemy_natural_pos;
	extern int leader_dist;

	//shared special ability targets
	extern std::vector<BWAPI::Unit> target_vector;

	//special counts
	extern int count_trapped;
	extern unsigned highground_defense_tanks;
	extern unsigned planned_highground_defense;

}

struct StateManager
{
	void init();
	void update(int n_ds);
};





//{
	//map dimensions
	//extern int w;
	//extern int h;
	//extern int wp;
	//extern int hp;
	//
	//build, walk, distance, defense and other maps
	//extern bool build_map[256][256];
	//extern bool walk_map[256][256];
	//extern bool choke_map[256][256];
	//
	//extern int dist_array[8][256][256];
	//extern int dist_map[256][256];
	//extern int path_map[256][256];
	//extern int space_map[256][256];
	//
	//extern int def_map[256][256];
	//extern int grddef_map[256][256];
	//extern int airdef_map[256][256];
	//
	//base locations
	//extern BWAPI::Position start_pos;
	//extern BWAPI::Position natu_pos;
	//extern BWAPI::TilePosition main_tiles[8];
	//extern BWAPI::TilePosition natu_tiles[8];
	//extern int mn; //main number
	//extern int mm; //my main
	//
	//choke locations
	//extern BWAPI::Position main_choke_pos[8];
	//extern BWAPI::Position natu_choke_pos[8];
//}



//struct State
//{
//public:
//	std::string name = "WillyT";
//	BWAPI::Race race = BWAPI::Races::Terran;
//	std::string enemy_name = "hello world";
//	BWAPI::Race enemy_race = BWAPI::Races::Unknown;
//
//	int strategy = 0;
//
//	bool is_defending = false;
//	bool is_attacking = false;
//	bool is_swarming  = false;
//	bool has_militia  = false;
//};