#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytExpo.h"

//2018-03-12: manager for gas mining
//2018-06-23: add miner distribution control
//2018-10-27: rewrite miner distribution control (failed hope to fix lategame crashes)

struct MiningManager
{
public:
	void update();
	void check_gather();
	void start_gas_mining(BWAPI::Unit my_refinery);
	void start_min_mining(BWAPI::Unit my_cc);
	void end_min_mining(BWAPI::Unit my_mineral);

private:
	void check_gas();
	void check_min();
	void append_gas_miner();
	void remove_gas_miner(int factor);
	std::vector<Expo>::iterator get_oversaturated_expo();
	std::vector<Expo>::iterator get_undrsaturated_expo();
	void count_resource_containers();
	void check_remove_mineral_blocks();
	int remove_mineral_block_queue = 0;
};