#include "ReadWrite.h"
#include <iostream>
#include <fstream>
#include <ctime>

std::vector<std::string> read_learning_data(std::string ename)
{
	//check per opponent file and eventually original log file
	std::string my_path = ".\\bwapi-data\\read\\WillyT_" + ename + ".txt";
	std::string my_path_orig = ".\\bwapi-data\\read\\willytlog.txt";
	std::vector<std::string> my_data;

	my_data = read_file(my_path);
	if (my_data.empty()) {
		my_data = read_file(my_path_orig);
		if (!my_data.empty()) {
			my_data.push_back("unfiltered");
		}
	}
	return my_data;
}

std::vector<std::string> read_file(std::string &filename)
{
	//open file and read lines into vector
	std::vector<std::string> my_data;
	std::string my_line;
	std::ifstream my_file(filename);
	if (my_file.is_open()) {
		while (std::getline(my_file, my_line)) {
			my_data.push_back(my_line);
		}
		my_file.close();
	}
	return my_data;
}

std::string write_logfile(std::vector<std::string>& ehist, std::string& ename)
{
	//open write file and append line into write file
	std::string my_path = ".\\bwapi-data\\write\\WillyT_" + ename + ".txt";
	std::ofstream my_file(my_path);
	if (my_file.is_open()) {
		for (std::string my_line : ehist) {
			my_file << my_line << std::endl;
		}
		my_file.close();
		return my_path;
	}
	else {
		return "unable to write log file"; 
	}
}

void write_frame(int n)
{
	//helper for debugging
	const char* my_path = "D:\\bwapi\\willytframe.txt";
	std::ofstream my_file(my_path);
	if (!my_file) { return; }
	my_file << n;
}

void write_string(std::string s)
{
	//helper for debugging
	const char* my_path = "D:\\bwapi\\willytframe.txt";
	std::ofstream my_file(my_path);
	if (!my_file) { return; }
	my_file << s;
}

//const char* my_path = "K:\\bwapi\\willytlog.txt";
//std::vector<std::string> my_data = read_logfile();
//my_data.push_back(my_line);
//std::ostream_iterator<std::string> output_iterator(my_file, "\n");
//std::copy(my_data.begin(), my_data.end(), output_iterator);