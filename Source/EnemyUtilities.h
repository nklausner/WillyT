#pragma once
#include <BWAPI.h>

void collect_intruders(std::vector<BWAPI::Unit>& my_vec, int(&my_map)[256][256]);
int evaluate_strength(std::vector<BWAPI::Unit>& my_vec);
int evaluate_airstrength(std::vector<BWAPI::Unit>& my_vec);
int count_supply(std::vector<BWAPI::Unit>& my_vec);