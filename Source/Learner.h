#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytEnemy.h"
#include "ReadWrite.h"

//2019-04-19: getting ready for opening learning
//2020-05-23: random choosing depending on win percentage
//2020-06-15: sqaure win percentage to enhance changes
//2021-09-20: changed to per opponent log file, including transition

int choose_strategy();
std::vector<std::string> filter_results(std::vector<std::string> &my_data);
void count_results();
int analyze_results_random();
int check_using_hardcoded();
std::string write_name();
std::string write_score();
void add_result_to_history(bool& iswin);
std::string write_date();
std::vector<std::string> split_line(std::string my_line);

//int analyze_results_variable();
//int analyze_results_fixed(int r[6]);