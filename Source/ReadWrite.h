#pragma once
#include <string>
#include <vector>

//2018-07-21: writing log file to relative path
//2018-07-21: reading and writing DATA in vector<string>
//2021-01-13: found bugs: AIIDE needing files per opponent, not closing files

std::vector<std::string> read_logfile();
const char* write_logfile(std::string ename, std::string erace, int strat, bool iswin);
std::string write_date();
void write_frame(int n);
void write_string(std::string s);