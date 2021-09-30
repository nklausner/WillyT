#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillytExpo.h"
#include "MapUtilities.h"

// 2018-06-29: start own map analysis
// 2018-10-18: finding choke areas: take minimum distance from tile to unwakable tiles
// 2019-03-07: reorganisation, dist, path, choke maps now for all start locations
// works on sscait maps except: FightSpirit topright, Jade topright, Roadrunner
// 2020-05-10: reworked finding chokes
// 2020-05-24 known bugs: Roadrunner(done), LaMancha(done)

struct MapAnalysis {

public:
	void init();
	void init_advanced();

private:
	int max_choke_search_main = 112;
	int max_choke_search_natu = 48;
	void set_map_dimension();
	void set_start_locations();
	void prepare_auxiliary_arrays();

	void create_build_map();
	void create_walk_map();
	void create_walkpos_map();
	void fix_walkpos_diagonal_gap(int x1, int y1, int x2, int y2);
	void fix_walkpos_orthogonal_gap(int x, int y, int dx, int dy);
	void fix_walkpos_twobythree_gap(int x, int y, int dx);

	void create_space_map();
	int check_spacing(int x, int y);
	int check_single_line(int x, int y, int dx, int dy, int m);

	void create_dist_array();
	void create_dist_init(int(&my_map)[256][256], int x0, int y0, int dmax);
	void create_dist_init(int(&my_map)[1024][1024], int x0, int y0, int dmax);
	void create_dist_map(int(&my_map)[256][256], int imax);
	void create_dist_map(int(&my_map)[1024][1024], int imax);
	void fill_dist_map(int(&my_map)[256][256], int x, int y);
	void fill_dist_map(int(&my_map)[1024][1024], int x, int y);
	void set_walk_dist(int(&my_map)[256][256], int x, int y, int d);
	void set_walk_dist(int(&my_map)[1024][1024], int x, int y, int d);

	void create_path_map();
	//void create_path_single(BWAPI::TilePosition t, int(&my_map)[256][256]);
	//void create_path_single(BWAPI::WalkPosition w, int(&my_map)[1024][1024]);
	//BWAPI::TilePosition get_path_back(int(&my_map)[256][256], int x, int y);
	BWAPI::WalkPosition get_path_back(int(&my_map)[1024][1024], int x0, int y0);
	//BWAPI::TilePosition get_tile_near_center(BWAPI::TilePosition t, int(&my_map)[256][256]);
	int check_walkpath_segment(int(&my_map)[1024][1024], int x1, int y1, int x2, int y2);
	//void set_walkpath_segment(int(&my_map)[1024][1024], int x1, int y1, int x2, int y2, int z);

	void create_choke_map();
	void create_choke_single(int i, BWAPI::Position(&base_pos)[8], BWAPI::Position(&choke_pos)[8], BWAPI::WalkPosition(&choke_edge)[16], int imax);
	void find_additional_choke(int i, BWAPI::Position& base_pos, BWAPI::Position& choke_pos, int imax);

	std::vector<BWAPI::WalkPosition> create_walkpath(int(&my_map)[1024][1024], BWAPI::Position p, int imax);
	std::vector<BWAPI::WalkPosition> create_walkpath_side(std::vector<BWAPI::WalkPosition> &my_vector, int side);
	int get_maximum_shift(int x, int y, int dx, int dy, int imax);

	void find_minimum_walkwidth(std::vector<BWAPI::WalkPosition>& vr, std::vector<BWAPI::WalkPosition>& vl, int& rmin, int& lmin);
	void find_choke_edge(std::vector<BWAPI::WalkPosition>& vc, std::vector<BWAPI::WalkPosition>& vr, std::vector<BWAPI::WalkPosition>& vl, int& rmin, int& lmin);
	std::vector<BWAPI::WalkPosition> outline_choke_area(std::vector<BWAPI::WalkPosition> old_vector);
	BWAPI::WalkPosition find_choke_center(std::vector<BWAPI::WalkPosition>& my_vec);
	void fill_walkpos(int(&my_map)[1024][1024], std::vector<BWAPI::WalkPosition>& my_vector);
	void fill_walkpos(bool(&my_map)[1024][1024], std::vector<BWAPI::WalkPosition>& my_vector);
	void flood_fill_choke(bool(&my_map)[1024][1024], int x, int y);
	void transfer_choke_map_walk_tile();

	void set_map_specific_parameters();
	int get_map_specific_parameter(BWAPI::Position base_pos, int imax);
	int get_map_specific_first_step();
	int get_map_specific_side_range();
	void set_map_specific_handling(BWAPI::Position& mcp, BWAPI::Position& ncp);
	int get_map_specific_defense_area_ymin(int ymin, int ymax);
	int get_map_specific_defense_area_ymax(int ymin, int ymax);
	void handle_map_specific_additional_choke(int i, BWAPI::Position& base_pos, BWAPI::Position& choke_pos);
	int auxiliary_natu_choke_dist = -1;

	void create_defense_maps();
	void create_defense_area(BWAPI::Position mychokepos, int(&mydistmap)[256][256], bool(&mydefmap)[256][256], std::vector<BWAPI::TilePosition>& mydefvec);
	int find_alternative_choke_tile_dist(int xc, int yc, int(&mydistmap)[256][256]);
	void create_rush_defense();
	void create_third_defense_map();
	void fill_third_defense_map(int i, int x0, int y0);

	void create_grid_image();
	void create_dist_image();

	//BWAPI::TilePosition find_choke_tile(int(&my_map)[256][256], BWAPI::TilePosition t, int imin, int imax);
	//BWAPI::Position find_choke_center(BWAPI::TilePosition t);
	//void flood_fill_choke(int x, int y, int z, std::vector<int> &vx, std::vector<int> &vy, int &i);

};