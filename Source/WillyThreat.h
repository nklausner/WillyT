#pragma once
#include <BWAPI.h>
#include "MapInfluence.h"
#include "MapUtilities.h"

//2020-04-19: handle area threats
//2020-04-21: comment everything out to analyse crash


struct Threat
{
	int x0;
	int y0;
	int size;	//tile size 12, 100
	int id;
	int ef;		//predicted end frame
	bool is_grd;
	bool is_air;
	bool is_ignored;
	int near_supply_enemy;
	int near_supply_self;

	Threat(BWAPI::Position my_posi, int my_size, int my_id, int my_timer, bool my_grd, bool my_air, bool print_debug);
	void clear();
};


namespace wilthr
{
	extern std::vector<Threat> siege;
	extern std::vector<Threat> nuke;
	extern std::vector<Threat> lurk;
	extern std::vector<Threat> swarm;
	extern std::vector<Threat> storm;
	extern std::vector<Threat> disr;

	extern int grddef[1024][1024];	//enemy ground defense area
	extern int airdef[1024][1024];	//enemy air defense area
	extern int grdmap[1024][1024];	//enemy ground threat area
	extern int airmap[1024][1024];	//enemy air threat area

	extern int stadet[1024][1024];	//enemy static detection area
	extern int dyndet[1024][1024];	//enemy dynamic detection area

	extern int getgrddef(BWAPI::Position p);
	extern int getairdef(BWAPI::Position p);
}


struct ThreatManager
{
public:
	ThreatManager();
	void update();
	void remove_destroyed(BWAPI::Unit u);
	void append_nuke(BWAPI::Position p);
	void ignore_stray_lurkers(unsigned scancount);

private:
	void append_unit(std::vector<Threat>& v, BWAPI::Unit u, int s, int timer);
	void append_spell(std::vector<Threat>& v, BWAPI::Bullet b, int s);
	bool vector_holds_id(std::vector<Threat>& v, int my_id);
	bool vector_holds_at(std::vector<Threat>& v, BWAPI::Position p);

	void remove_unit(std::vector<Threat>& v, BWAPI::Unit u);
	void remove_expired_unit(std::vector<Threat>& v);
	void remove_expired_spell(std::vector<Threat>& v);
	bool print_debug;
	void update_near_supply(std::vector<Threat>::iterator it);

	//void update_dynamic_detection();
};