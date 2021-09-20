#pragma once
#include <string>
#include <vector>

//2018-07-21: writing log file to relative path
//2018-07-21: reading and writing DATA in vector<string>
//2021-01-13: found bugs: AIIDE needing files per opponent, not closing files
//2021-09-20: changed to per opponent log file, including transition

std::vector<std::string> read_learning_data(std::string ename);
std::vector<std::string> read_file(std::string &filename);
std::string write_logfile(std::vector<std::string> &ehist, std::string &ename);
void write_frame(int n);
void write_string(std::string s);