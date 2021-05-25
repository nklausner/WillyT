#include "Learner.h"

int choose_strategy( std::vector<std::string> &my_data )
{
	wilenemy::name = write_name();
	count_results(my_data);
	wilenemy::score = write_score();
	return analyze_results_random();
}

void count_results(std::vector<std::string> &my_data)
{
	for (std::string my_line : my_data) {
		std::vector<std::string> my_split = split_line(my_line);
		if (my_split.size() == 5 &&
			my_split.at(1) == wilenemy::name) {
			int e = 2 * std::stoi(my_split.at(3)) - std::stoi(my_split.at(4)) - 1;
			if (e >= 0 && e < sizeof(wilenemy::result) / 4) {
				wilenemy::result[e]++;
			}
		}
	}
}

int analyze_results_random()
{
	if (willyt::test_strategy != 0) {	//force strategy for testing
		return willyt::test_strategy;
	}
	if (willyt::use_hardcoded_strategies &&
		check_using_hardcoded() != 0) {	//force strategy for certain opponents
		return check_using_hardcoded();
	}

	using namespace wilenemy;
	const unsigned nstrats = 4;		//alternatively sizeof(result)/8
	double wp[nstrats] = { 0.0 };	//squared win percentage
	double wpsum = 0.0;				//squared win percentage sum
	int cp[nstrats] = { 0 };		//accumulated percentage

	//win percentage, avoid 0 % and 100 %
	for (unsigned i = 0; i < nstrats; i++) {
		wp[i] = (pow(1 + result[2*i], 2)) / (pow (2 + result[2*i] + result[2*i+1], 2));
		wpsum += wp[i];
	}

	//if true: if one strategy stands out, take it
	if (willyt::avoid_weak_strategies) {
		unsigned imax = 0;
		for (unsigned i = 0; i < nstrats; i++) {
			if (wp[i] > wp[imax]) {
				imax = i;
			}
		}
		bool stands_out = true;
		for (unsigned i = 0; i < nstrats; i++) {
			if (i != imax &&
				wp[imax] < 2 * wp[i]) {
				stands_out = false;
			}
		}
		if (stands_out) {
			//BWAPI::Broodwar->printf("stand out strategy %d, squared win percentage %f", imax + 1, wp[imax]);
			return imax + 1;
		}
	}

	//accumulated strategies
	for (unsigned i = 0; i < nstrats; i++) {
		cp[i] = (int)((100.0 * wp[i]) / wpsum + 0.5);
		if (cp[i] == 0) { cp[i] = 1; }
		if (i > 0) { cp[i] += cp[i-1]; }
	}

	//choose random from 0...99, intervals according to winrate
	int n = rand() % 100;
	for (unsigned i = 0; i < nstrats; i++) {
		if (n < cp[i]) {
			//BWAPI::Broodwar->printf("sqwinrates %4.2f %4.2f %4.2f %4.2f", wp[0], wp[1], wp[2], wp[3]);
			//BWAPI::Broodwar->printf("%d %d %d %d - random %d", cp[0], cp[1], cp[2], cp[3], n);
			return i+1;
		}
	}
	return 1;
}

int check_using_hardcoded()
{
	if (wilenemy::name == "XIAOYICOG2019") { return 3; }
	if (wilenemy::name == "Dave Churchill" || wilenemy::name == "UAlbertaBot") { return 1; }
	if (wilenemy::name == "Chris Coxe" || wilenemy::name == "ZZZKBot") { return 1; }
	if (wilenemy::name == "Stardust" || wilenemy::name == "Locutus") { return 4; }
	return 0;
}

std::string write_name()
{
	int n = BWAPI::Broodwar->enemies().size();
	return (n == 1) ? BWAPI::Broodwar->enemy()->getName() : "enemies";
}

std::string write_score()
{
	std::string s = "";
	for (int i = 0; i < sizeof(wilenemy::result) / 4; i++) {
		s += std::to_string( wilenemy::result[i] );
		s += (i % 2 == 0) ? "-" : " ";
	}
	return s;
}

std::vector<std::string> split_line(std::string my_line)
{
	std::vector<std::string> my_split;
	std::stringstream ss(my_line);
	std::string my_part;
	while (getline(ss, my_part, ','))
		my_split.push_back(my_part);
	return my_split;
}

//int analyze_results_variable() {
//	if (willyt::test_strategy != 0) {	//force strategy for testing
//		return willyt::test_strategy;
//	}
//
//	using namespace wilenemy;
//	const unsigned imax = sizeof(result) / 8;
//	int tg[imax] = { 0 };
//	double wp[imax] = { 0.0 };
//	bool try_new_strategies = true;
//	//BWAPI::Broodwar->printf("analyzing results for %d strategies", imax);
//
//	//total games per strategy
//	for (unsigned i = 0; i < imax; i++) {
//		tg[i] = result[2 * i] + result[2 * i + 1];
//	}
//	//win percentage, avoid 0 % and 100 %
//	for (unsigned i = 0; i < imax; i++) {
//		wp[i] = (double)(1 + result[2*i]) / (double)(2 + tg[i]);
//	}
//
//	//try other strategies eventually
//	if (try_new_strategies) {
//		for (unsigned i = 0; i < imax; i++) {
//			for (unsigned j = 0; j < imax; j++) {
//				if (tg[i] < tg[j] / 24) {
//					return (int)(i + 1);
//				}
//			}
//		}
//	}
//	//prefer higher win percentage
//	for (unsigned i = 0; i < imax; i++) {
//		unsigned c = 0;
//		for (unsigned j = 0; j < imax; j++) {
//			if (wp[i] >= wp[j]) { c++; }
//		}
//		if (c == imax) {
//			return (int)(i + 1);
//		}
//	}
//	return 1;
//}

//int analyze_results_fixed(int r[6]) {
//	if (willyt::test_strategy != 0) {	//force strategy for testing
//		return willyt::test_strategy;
//	}
//
//	//total games
//	int tg1 = r[0] + r[1];
//	int tg2 = r[2] + r[3];
//	int tg3 = r[4] + r[5];
//	int tga = tg1 + tg2 + tg3;
//	//win percentage seeded with 50 %
//	double wp1 = (1.0 + (double)r[0]) / (2.0 + (double)tg1);
//	double wp2 = (1.0 + (double)r[2]) / (2.0 + (double)tg2);
//	double wp3 = (1.0 + (double)r[4]) / (2.0 + (double)tg3);
//	//try other strategies eventually
//	if (tg1 < tga / 24) return 1;
//	if (tg2 < tga / 24) return 2;
//	if (tg3 < tga / 24) return 3;
//	//prefer higher win percentage
//	if (wp1 == wp1 && wp1 >= wp2 && wp1 >= wp3) return 1;
//	if (wp2 >= wp1 && wp2 == wp2 && wp2 >= wp3) return 2;
//	if (wp3 >= wp1 && wp3 >= wp2 && wp3 == wp3) return 3;
//	return 1;
//}