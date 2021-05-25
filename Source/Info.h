#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytEnemy.h"

//2018-03-25: screen info

struct Info
{
public:
	void display();

private:
	void draw_256_colors();
	void list_states();
	void list_types(std::vector<BWAPI::UnitType> &my_vec);
	void list_units(std::vector<BWAPI::Unit> &my_vec);
	void list_tanks();
	void list_scvs_pointers();
	void display_lair_reaction_timer(int x, int y);

	void draw_tech(int x, int y, const char* s, bool b);
	void draw_upgr(int x, int y, int l);
	void draw_count(int x, int y, int n1, int n2, int n0, const char* s);
	void draw_build(int x, int y);
	BWAPI::Color get_text_color(float f);
};