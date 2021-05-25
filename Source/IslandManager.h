#pragma once
#include <BWAPI.h>
#include "WillytUnits.h"
#include "WillytBuild.h"
#include "WillytExpo.h"
#include "WillytState.h"

//2019-12-09: happy days on the island in the sun

struct IslandManager
{
public:
	void update();

private:
	std::vector<Expo>::iterator it_island;
	std::vector<Flyer>::iterator it_dropship;
	int island_steps = 0;
	int dropship_load = 0;
	void find_island(int i);
	void pick_up_scv(int i);
	void drop_island(int i);
	void mine_blocks(int i);
	void build_com(int i);
};