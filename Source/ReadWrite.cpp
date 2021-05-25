#include "ReadWrite.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iterator>

std::vector<std::string> read_logfile() {
	const char* my_path = ".\\bwapi-data\\read\\willytlog.txt";
	std::vector<std::string> my_data;
	std::string my_line;

	//open read file and read file content into vector
	std::ifstream my_file(my_path);
	if (my_file.is_open()) {
		while (std::getline(my_file, my_line)) {
			my_data.push_back(my_line);
		}
		my_file.close();
	}
	return my_data;
}

const char* write_logfile(std::string ename, std::string erace, int strat, bool iswin) {
	const char* my_path = ".\\bwapi-data\\write\\willytlog.txt";
	std::vector<std::string> my_data = read_logfile();

	//assemble new entry
	std::string new_line;
	new_line += write_date() + ",";
	new_line += ename + ",";
	new_line += erace.substr(0,1) + ",";
	new_line += "0" + std::to_string(strat) + ",";
	new_line += iswin ? "1" : "0";

	//append new entry
	my_data.push_back(new_line);

	//open write file and append line into write file
	std::ofstream my_file(my_path);
	if (my_file.is_open()) {
		for (std::string my_line : my_data) {
			my_file << my_line << std::endl;
		}
		my_file.close();
		return my_path;
	}
	else {
		return "unable to write log file"; 
	}
}

//const char* my_path = "K:\\bwapi\\willytlog.txt";
//std::vector<std::string> my_data = read_logfile();
//my_data.push_back(my_line);
//std::ostream_iterator<std::string> output_iterator(my_file, "\n");
//std::copy(my_data.begin(), my_data.end(), output_iterator);

std::string write_date() {
	//generate string components of the date
	std::time_t timer = std::time(0);
	std::tm*    tinfo = std::localtime(&timer);
	std::string syear = std::to_string(tinfo->tm_year + 1900);
	std::string smont = std::to_string(tinfo->tm_mon + 1);
	std::string smday = std::to_string(tinfo->tm_mday);
	if (smont.length() == 1) { smont.insert(0, "0"); }
	if (smday.length() == 1) { smday.insert(0, "0"); }
	return (syear + smont + smday);
}

void write_frame(int n) {
	const char* my_path = "D:\\bwapi\\willytframe.txt";
	std::ofstream my_file(my_path);
	if (!my_file) { return; }
	my_file << n;
}

void write_string(std::string s) {
	const char* my_path = "D:\\bwapi\\willytframe.txt";
	std::ofstream my_file(my_path);
	if (!my_file) { return; }
	my_file << s;
}