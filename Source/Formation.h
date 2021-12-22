#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytEnemy.h"
#include "WillytMap.h"
#include "Utilities.h"
#include "MapUtilities.h"

//2021-12-14: create formation

namespace formation
{
	extern std::vector<BWAPI::Position> line0;
	extern std::vector<BWAPI::Position> line1;
	extern std::vector<BWAPI::Position> line2;
	extern std::vector<BWAPI::Position> line3;
	extern int formcount;
	extern bool useformation;
	extern bool isassigned;
}

void update_formation(BWAPI::Position mypos);
BWAPI::Position find_formation_center(BWAPI::Position mypos, BWAPI::Position enpos);
void create_formation_orth(int x0, int y0, int dx, int dy, int ddx, int ddy);
void create_formation_diag(int x0, int y0, int dx, int dy, int ddx, int ddy);
void create_formation_line_orth(std::vector<BWAPI::Position>& myline, int x0, int y0, int dx, int dy, int ddx, int ddy);
void create_formation_line_diag(std::vector<BWAPI::Position>& myline, int x0, int y0, int dx, int dy, int ddx, int ddy);
BWAPI::WalkPosition get_maximum_extend(int x, int y, int dx, int dy, int ddx, int ddy, int imax);