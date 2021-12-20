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
	extern std::vector<BWAPI::Position> formvec;
	extern int formcount;
}

void update_formation(BWAPI::Position mypos);
void create_formation(BWAPI::Position mypos, BWAPI::Position enpos);