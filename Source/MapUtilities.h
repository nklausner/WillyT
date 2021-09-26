#pragma once
#include <BWAPI.h>
#include <math.h>
#include "WillytMap.h"
#include "Utilities.h"

//2018-09-02: functions for boolean maps

//set area on boolean map to true/false
void set_map_area(bool(&my_map)[256][256], int x0, int y0, int dx, int dy, bool v);
void set_map_area(bool(&my_map)[256][256], BWAPI::Unit u, bool v);

//special case, clear variable build area and keep fixed build area
void clear_build_area(bool(&my_map_var)[256][256], bool(&my_map_fix)[256][256], int x0, int y0, int dx, int dy);
void clear_build_area(bool(&my_map_var)[256][256], bool(&my_map_fix)[256][256], BWAPI::Unit u);

//check area of boolean map for true/false
bool check_map_area(bool(&my_map)[256][256], int x0, int y0, int dx, int dy);
bool check_map_area(int(&my_map)[1024][1024], int x0, int y0, int dx, int dy, int f);
bool check_buildable(int x0, int y0, int dx, int dy);
bool tile_fully_walkable(int x, int y);

//clear map total
void clear_map_bool(bool(&my_map)[256][256]);
void clear_map_bool(bool(&my_map)[1024][1024]);
void fill_map_int(int(&my_map)[256][256], int z);
void fill_map_int(int(&my_map)[1024][1024], int z);

//convert expo center position to expo center tilepos
BWAPI::TilePosition get_center_tile(BWAPI::Position p);
BWAPI::TilePosition get_center_of_mass(std::vector<BWAPI::TilePosition> v);

//get vary tile
BWAPI::TilePosition get_vary_tile(BWAPI::TilePosition t, int i);

//find corners and sides
bool is_topl(int x, int y);
bool is_topr(int x, int y);
bool is_botl(int x, int y);
bool is_botr(int x, int y);
bool is_top_(int x, int y);
bool is_bot_(int x, int y);
bool is_left(int x, int y);
bool is_rigt(int x, int y);

//find relative position out of 4 directions
int get_direction_4(BWAPI::Position p0, BWAPI::Position p1);
//find relative position out of 8 directions
int get_direction_8(int x0, int y0, int x1, int y1);
int get_direction_8(BWAPI::Position p0, BWAPI::Position p1);
//find angle between to points (0-2pi)
double get_angle(BWAPI::Position p0, BWAPI::Position p1);

//get position on circle by index
BWAPI::Position get_circle_pos(std::vector<BWAPI::Position> &my_vec, int i);

//check ground connenction between tile positions
bool has_grd_connection(BWAPI::Position p0, BWAPI::Position p1);

//get ground distance to main base
int get_ground_dist(int x, int y);
int get_ground_dist(BWAPI::TilePosition my_tile);
int get_ground_dist(BWAPI::Position my_pos);
int get_ground_dist(BWAPI::Unit my_unit);

//keep value within map for substraction
int mapsafesub(int x, int dx);
//keep value within map for addition
int mapsafeadd(int x, int dx, int xmax);
//keep value within map general
int keepmapsafe(int x, int xmax);

//find walkpos between both walkpos via linear interpolation
std::vector<BWAPI::WalkPosition> create_linear_interpolation(BWAPI::WalkPosition w1, BWAPI::WalkPosition w2);

//check visibility of four corners of a 4x3 building spot
bool buildspot_visible(BWAPI::TilePosition t);
bool buildspot_visible(BWAPI::Position p);

//create circle positions, circumference / spacing > radian
std::vector<BWAPI::Position> create_circle_vector(BWAPI::Position p0, int r);
//create circle24 with tileradius 2 in pixels (cdx24, cdy24)
void create_circle_24();
//create circle24 with tileradius 4 (effectively 5) in walkpositions (cr4dx, cr4dy)
void create_circle_r4();
//get position on circle with variable radius (<= r0) around main (n) by increment (i out of 24)
BWAPI::Position get_circle_pos_24(int n, int r0, int i);