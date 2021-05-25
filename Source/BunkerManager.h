#pragma once
#include <BWAPI.h>
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytEnemy.h"
#include "Utilities.h"

struct BunkerManager
{
public:
	void update();

private:
	void load_bunkers();
	void unload_bunkers();
	int bunker_queue = 0;
};