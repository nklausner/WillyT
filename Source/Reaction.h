#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytEnemy.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytMap.h"
#include "Utilities.h"
#include "MapUtilities.h"

//2020-02-17: gather reactions

void check_all_reactions();

void check_need_turrets();
bool check_zergling_rush();
bool check_defense_focus();
bool check_proxy_buildings();
bool check_proxy_prod_buildings();
bool check_pressure_on_bunker();

void check_mega_macro();

void change_strategies();
void check_resign();
bool has_only_island_buildings();
bool scout_near(BWAPI::Position my_pos);