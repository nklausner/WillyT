#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytEnemy.h"

//2018-03-17: outsource unit training

struct Trainer
{
public:
	void init();
	void train();

private:
	void train_commandc(BWAPI::Unit my_cc);
	void train_barracks(BWAPI::Unit my_rax);
	void train_factories();
	void train_starports(BWAPI::Unit my_sta);
	void train_for_rush();
	void build_addons(std::vector<BWAPI::Unit> my_vec, BWAPI::UnitType my_addon);
	unsigned count_addon(std::vector<BWAPI::Unit> my_vec, int i);
	unsigned count_naked(std::vector<BWAPI::Unit> my_vec, int i);

	BWAPI::Unit find_idle(std::vector<BWAPI::Unit> &my_list);
	BWAPI::Unit find_idle_addon(std::vector<BWAPI::Unit> &my_list);
	BWAPI::Unit find_idle_naked(std::vector<BWAPI::Unit> &my_list);
	bool need_a_light();
	bool need_rines();
	bool need_missiles_normal();
	bool need_missiles_urgent();
	bool can_build_goliath();
	bool bring_it_on();
	bool is_spooky();
	bool saving_for_expo();
	bool saving_for_refinery();
	bool saving_for_siege();
	bool waiting_for_dropship();
	bool gas_is_stolen();

	bool wait_workers();

	int scv_rax_count;
	int scv_fe_count;
	int tank_goli_ratio;
	float number_fact_per_addon; //double of factories per machineshops
	unsigned n_dropship;
};