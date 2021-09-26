#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytMap.h"
#include "WillytEnemy.h"
#include "MapUtilities.h"
#include "Utilities.h"

//20200501: outsourcing of fighter methods

bool reaching_area(BWAPI::Unit my_unit, int(&my_map)[1024][1024]);
BWAPI::Position get_retreat_vector(BWAPI::Position p0, BWAPI::Position pe, int d);

BWAPI::Unit get_target_from(std::vector<BWAPI::Unit>& v, BWAPI::Unit u, BWAPI::Position p, int r);
BWAPI::Unit get_maximum_target(std::vector<BWAPI::Unit>& v, BWAPI::TilePosition t, int r);

BWAPI::Position get_random_pos(std::vector<BWAPI::TilePosition>& my_vec);
BWAPI::Position get_mine_position();
BWAPI::Position get_current_target_position(BWAPI::Unit u);

BWAPI::Position choose_defense_siegetile();
bool check_defense_siegetile(BWAPI::TilePosition t);

bool check_venture_out(BWAPI::Position my_pos);

void draw_arrow(BWAPI::Position p0, BWAPI::Position p1, BWAPI::Color c);
void draw_arrow(BWAPI::Unit u, BWAPI::Position p, BWAPI::Color c);
void draw_arrow(BWAPI::Unit u0, BWAPI::Unit u1, BWAPI::Color c);
void draw_text(BWAPI::Unit u, const char* s, BWAPI::Text::Enum c);
void draw_pos_box(BWAPI::Position p, int s, BWAPI::Color c);
