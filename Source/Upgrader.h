#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytEnemy.h"

//2019-02-23: outsourcing upgrades and research

struct Upgrader
{
public:
	void upgrade();

private:
	void upgrade_bio();
	void upgrade_ghosts();
	void upgrade_mech();
	void upgrade_air();
	BWAPI::Unit find_idle(std::vector<BWAPI::Unit> my_list);
	bool has_U_238 = false;
};