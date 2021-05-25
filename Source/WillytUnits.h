#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "SCV.h"
#include "Fighter2.h"
#include "Flyer.h"
#include "Utilities.h"

//2018-03-07: unit list
//2019-03-17: replacing struct with namespace

namespace wilunits
{
	//unit vectors
	extern std::vector<SCV> scvs;
	extern std::vector<Fighter2> marines;
	extern std::vector<Fighter2> firebats;
	extern std::vector<Fighter2> medics;
	extern std::vector<Fighter2> ghosts;
	extern std::vector<Fighter2> siegetanks;
	extern std::vector<Fighter2> goliaths;
	extern std::vector<Fighter2> vultures;
	extern std::vector<Flyer> wraiths;
	extern std::vector<Flyer> dropships;
	extern std::vector<Flyer> valkyries;
	extern std::vector<Flyer> vessels;
	extern std::vector<Flyer> cruisers;

	//unit counts
	extern int tcount_scv;
	extern int tcount_marine;
	extern int tcount_firebat;
	extern int tcount_medic;
	extern int tcount_ghost;
	extern int tcount_siegetank;
	extern int tcount_goliath;
	extern int tcount_vulture;
	extern int tcount_wraith;
	extern int tcount_dropship;
	extern int tcount_valkyrie;
	extern int tcount_vessel;
	extern int tcount_cruiser;

	//special units
	extern bool has_scout;

}

struct UnitManager
{
public:
	void tcount_unit(BWAPI::Unit unit);
	void append_unit(BWAPI::Unit unit);
	void remove_unit(BWAPI::Unit unit);

	void count_supplies();
	void update_all();
	void update_eight();
	void correct();
	void assign_scout(BWAPI::Unit my_unit);

private:
	bool is_tank(BWAPI::UnitType my_type);
	bool already_in(BWAPI::Unit my_unit, std::vector<Fighter2> &my_vector);
	void append_to(BWAPI::Unit my_unit, std::vector<SCV> &my_vector);
	void append_to(BWAPI::Unit my_unit, std::vector<Fighter2> &my_vector);
	void append_to(BWAPI::Unit my_unit, std::vector<Flyer> &my_vector);
	void remove_from(std::vector<SCV> &my_vector, int &my_count);
	void remove_from(std::vector<Fighter2> &my_vector, int &my_count);
	void remove_from(std::vector<Flyer> &my_vector, int &my_count);
	void confirm_existence(BWAPI::Unit my_unit);
};

std::vector<SCV>::iterator choose_scv(BWAPI::TilePosition my_tile, bool no_militia = true, bool no_carry = true);
std::vector<SCV>::iterator get_scout();
void stop_all_build_missions();



//void update_marines();
//void update_firebats();
//void update_medics();
//void update_tanks();
//void update_goliaths();
//void update_vultures();