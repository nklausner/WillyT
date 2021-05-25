#pragma once
#include <BWAPI.h>
#include "SCV.h"
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytMap.h"
#include "WillytEnemy.h"

//2018-08-14: congestion of building orders, introducing the cleaner
//2021-02-09: found old bug causing doubling of scv objects

void clean();
void adress_renegade_scv();
void reset_planned_builds();
void reset_blocked_tiles();
void correct_all_minerals();
bool check_blocked_entrance();
void clear_blocked_entrance();