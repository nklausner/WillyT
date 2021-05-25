#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"

//2018-03-25: cancel, continue and repair buildings
//2018-06-15: registered floating rax as proxy :-/

void update_repairing();
void check_canceling(BWAPI::Unit u);
void check_continuing(BWAPI::Unit u);
void check_repairing(BWAPI::Unit u);
void hold_the_bunker();
int count_repairers();
void update_transport();