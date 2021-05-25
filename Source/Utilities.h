#pragma once
#include <BWAPI.h>


//avoid negative values
void safe_sum(int &n, int dn);
//comparison by coordinates
//bool equals(BWAPI::TilePosition& t0, BWAPI::TilePosition& t1);
//check for none
bool is_none(BWAPI::TilePosition& t);
//check for none
bool is_none(BWAPI::Position& p);


//just more nice
void vector_remove(std::vector<BWAPI::Unit> &v, BWAPI::Unit u);
//just more nice
void vector_remove(std::vector<BWAPI::UnitType> &v, BWAPI::UnitType t);
//just more nice
void vector_remove(std::vector<BWAPI::Position> &v, BWAPI::Position p);
//just more nice
void vector_remove(std::vector<BWAPI::TilePosition> &v, BWAPI::TilePosition);
//just more nice
void vector_remove(std::vector<double> &v, double &d);


//checks if given vector contains: unit
bool vector_holds(std::vector<BWAPI::Unit> &v, BWAPI::Unit u);
//checks if given vector contains: unittype
bool vector_holds(std::vector<BWAPI::UnitType> &v, BWAPI::UnitType t);
//checks if given vector contains: position
bool vector_holds(std::vector<BWAPI::Position> &v, BWAPI::Position p);
//checks if given vector contains: tileposition
bool vector_holds(std::vector<BWAPI::TilePosition> &v, BWAPI::TilePosition t);


//linear interpolation from two points, fraction of distance
BWAPI::Position linear_interpol_rel(BWAPI::Position p0, BWAPI::Position p1, float f);
//linear interpolation from two points, absolute distance
BWAPI::Position linear_interpol_abs(BWAPI::Position p0, BWAPI::Position p1, int d);
//calculate estimated unit position after number of frames
BWAPI::Position estimate_next_pos(BWAPI::Unit u, int n);
//get random position on map
BWAPI::Position get_random_position();


//distance of positions
int dist(BWAPI::Position p0, BWAPI::Position p1);
//distance of walkpositions
int dist(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1);
//no need for square root when comparing
int sqdist(BWAPI::TilePosition t0, BWAPI::TilePosition t1);
//no need for square root when comparing
int sqdist(BWAPI::WalkPosition w0, BWAPI::WalkPosition w1);
//no need for square root when comparing
int sqdist(BWAPI::Position p0, BWAPI::Position p1);
//no need for square root when comparing
int sqdist(BWAPI::Unit u0, BWAPI::Unit u1);
//no need for square root when comparing
int sqdist(BWAPI::Unit u0, BWAPI::Position p1);
//no need for square root when comparing
int sqdist(int x0, int y0, int x1, int y1);


//get closest position of a vector
BWAPI::Position get_closest(std::vector<BWAPI::Position> &v, BWAPI::Position &p);
//get closest unit of a vector by position
BWAPI::Unit get_closest(std::vector<BWAPI::Unit> &v, BWAPI::Position &p);
//get closest unit of a vector by tileposition
BWAPI::Unit get_closest(std::vector<BWAPI::Unit> &v, BWAPI::TilePosition &t);
//get first unit of a vector within square distance to tileposition
BWAPI::Unit get_in_range(std::vector<BWAPI::Unit> &v, BWAPI::TilePosition &t, int sqr);
//get first unit of a vector within sqaure distance to position
BWAPI::Unit get_in_range(std::vector<BWAPI::Unit> &v, BWAPI::Position &p, int sqd);