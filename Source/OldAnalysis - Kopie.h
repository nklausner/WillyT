#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillytExpo.h"
#include "MapUtilities.h"

// 2018-06-29: start own map analysis
// 2018-10-18: finding choke areas: take minimum distance from tile to unwakable tiles
// 2019-03-07: reorganisation, dist, path, choke maps now for all start locations
// works on sscait maps except: FightSpirit topright, Jade topright, Roadrunner

struct MapAnalysis {

public:
	void init();
	void init_advanced();

private:
	void set_map_dimension();
	void set_start_locations();

	void create_build_map();
	void create_walk_map();
	bool tile_is_walkable(int x, int y);
	void create_walkpos_map();
	void fix_walkpos_diagonal_hole(int x1, int y1, int x2, int y2);

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
	void create_path_single(BWAPI::TilePosition t, int(&my_map)[256][256]);
	void create_path_single(BWAPI::WalkPosition w, int(&my_map)[1024][1024]);
	BWAPI::TilePosition get_path_back(int(&my_map)[256][256], int x, int y);
	BWAPI::WalkPosition get_path_back(int(&my_map)[1024][1024], int x0, int y0);
	BWAPI::TilePosition get_tile_near_center(BWAPI::TilePosition t, int(&my_map)[256][256]);
	int check_walkpath_segment(int(&my_map)[1024][1024], int x1, int y1, int x2, int y2);
	//void set_walkpath_segment(int(&my_map)[1024][1024], int x1, int y1, int x2, int y2, int z);

	void create_choke_map();
	void create_choke_single(BWAPI::Position my_pos, int imax);
	BWAPI::TilePosition find_choke_tile(int(&my_map)[256][256], BWAPI::TilePosition t, int imin, int imax);
	BWAPI::Position find_choke_center(BWAPI::TilePosition t);
	void flood_fill_choke(int x, int y, int z, std::vector<int> &vx, std::vector<int> &vy, int &i);

	std::vector<BWAPI::WalkPosition> create_walkpath(int(&my_map)[1024][1024], BWAPI::Position p, int imax);
	std::vector<BWAPI::WalkPosition> create_walkpath_side(std::vector<BWAPI::WalkPosition> &my_vector, int side);
	std::vector<BWAPI::WalkPosition> shift_walkpath_mid(std::vector<BWAPI::WalkPosition>& old_vector);
	int get_maximum_shift(int x, int y, int dx, int dy);
	std::vector<BWAPI::WalkPosition> fill_walkpath_gaps(std::vector<BWAPI::WalkPosition>& old_vector);
	std::vector<BWAPI::WalkPosition> cut_walkpath_corners(std::vector<BWAPI::WalkPosition>& old_vector);
	std::vector<BWAPI::WalkPosition> cut_walkpath_bulge_diagonal(std::vector<BWAPI::WalkPosition>& my_vector);
	void cut_walkpath_bulge_orthogon(std::vector<BWAPI::WalkPosition>& my_vector);

	//std::tuple<int, int> check_linearity(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1, BWAPI::WalkPosition w2);
	//std::tuple<int, int> calc_intersection(BWAPI::WalkPosition w1, BWAPI::WalkPosition w2, int dx1, int dy1, int dx2, int dy2);
	//void shift_walkpath_mid(std::vector<BWAPI::WalkPosition>& vm, std::vector<BWAPI::WalkPosition>& vr, std::vector<BWAPI::WalkPosition>& vl);
	//BWAPI::WalkPosition shift_walkpos(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1, int ds);

	std::vector<BWAPI::WalkPosition> find_minimum_walkwidth(std::vector<BWAPI::WalkPosition>& vr, std::vector<BWAPI::WalkPosition>& vl);
	void fill_walkpos(int(&my_map)[1024][1024], std::vector<BWAPI::WalkPosition>& my_vector);
	void fill_walkpos(bool(&my_map)[1024][1024], std::vector<BWAPI::WalkPosition>& my_vector);

	//std::vector<BWAPI::WalkPosition> fill_discontinuties(std::vector<BWAPI::WalkPosition>& my_vec);
	std::vector<BWAPI::WalkPosition> create_linear_interpolation(BWAPI::WalkPosition w1, BWAPI::WalkPosition w2);

	//std::vector<BWAPI::WalkPosition> find_choke_walkspace(int(&my_map)[1024][1024], BWAPI::Position p, unsigned imax);
	//std::vector<BWAPI::WalkPosition> get_path_walkspace(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1);
	//bool check_walkspace_segment(int x0, int y0, int dx, int dy, int imax, std::vector<BWAPI::WalkPosition> &my_vec);
	//BWAPI::Position find_choke_center(std::vector<BWAPI::WalkPosition> &my_vec);

	void create_defense_maps();
	void create_defense_area(BWAPI::Position mychokepos, int(&mydistmap)[256][256], bool(&mydefmap)[256][256]);

	void create_grid_image();
	void create_dist_image();

};