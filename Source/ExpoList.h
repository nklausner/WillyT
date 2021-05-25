#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillytBuild.h"
#include "Utilities.h"
#include "MapUtilities.h"
#include "Expo.h"

//2018-05-21: expo/base list created

struct ExpoList
{
	std::vector<Expo> all;
	void occupy_expo(BWAPI::TilePosition &tile);
	void desert_expo(BWAPI::TilePosition &tile);
	void mined_out(BWAPI::Unit my_min);
	void destroy_geyser(BWAPI::Unit my_gas);
	void rebuild_geyser(BWAPI::Unit my_gas);
	void update();

	std::vector<Expo>::iterator destroyed_geyser_expo = all.end();
};