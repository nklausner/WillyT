#include "Info.h"

void Info::display() {
	unsigned min = willyt::my_time / 60;
	unsigned sec = willyt::my_time % 60;
	BWAPI::Broodwar->drawTextScreen(10, 10, "%02d:%02d", min, sec);
	BWAPI::Broodwar->drawTextScreen(60, 10, "WillyT vs %s", wilenemy::name.c_str());
	BWAPI::Broodwar->drawTextScreen(10, 20, wilenemy::score.c_str());
	BWAPI::Broodwar->drawTextScreen(100, 20, "=> %d", willyt::strategy);

	draw_upgr( 10, 40, BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Infantry_Weapons));
	draw_upgr( 30, 40, BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Infantry_Armor));
	draw_tech( 50, 40, "stim", BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Stim_Packs));
	draw_tech( 80, 40, "U238", BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::U_238_Shells));
	draw_tech(110, 40, "cloak", BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Personnel_Cloaking));
	draw_tech(140, 40, "lock", BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Lockdown));
	draw_tech(170, 40, "blind", BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Optical_Flare));

	draw_upgr(10, 50, BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Vehicle_Weapons));
	draw_upgr(30, 50, BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Vehicle_Plating));
	draw_tech(50, 50, "siege", BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Tank_Siege_Mode));
	draw_tech(80, 50, "range", BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Charon_Boosters));
	draw_tech(110, 50, "speed", BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Ion_Thrusters));
	draw_tech(140, 50, "mines", BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Spider_Mines));

	draw_upgr(10, 60, BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Ship_Weapons));
	draw_upgr(30, 60, BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Ship_Plating));
	draw_tech(50, 60, "yama", BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Yamato_Gun));

	using namespace willyt;
	draw_count(10,  80, sup_scv, 0, mining_supply, "SCV");
	draw_count(10,  90, sup_bio, sup_mech, attack_supply, "BIO");
	draw_count(10, 100, sup_mech, sup_bio, attack_supply, "MECH");
	draw_count(10, 110, sup_air, 0, attack_supply, "AIR");
	if (count_trapped > 0) {
		BWAPI::Broodwar->drawTextScreen(80, 80, "%c%d trapped", 6, count_trapped);
	}

	//BWAPI::Broodwar->drawTextScreen(10, 120, "mins: %d (%d), geys: %d (%d)", mineral_count, min_scv_count_min, geyser_count, min_scv_count_gas);
	//draw_build(10, 130);
	//BWAPI::Broodwar->drawTextScreen(10, 150, "%d targets", wilenemy::target_count);
	//display_lair_reaction_timer(10, 150);

	//BWAPI::Broodwar->drawTextScreen(10, 150, wilenemy::name.c_str());
	//BWAPI::Broodwar->drawTextScreen(10, 160, "%d buildings", wilenemy::positions.size());
	//BWAPI::Broodwar->drawTextScreen(10, 170, "%d defenses", wilenemy::grddef_count + wilenemy::airdef_count);
	//BWAPI::Broodwar->drawTextScreen(10, 180, "%d work supply" , wilenemy::supply_work / 2);
	//BWAPI::Broodwar->drawTextScreen(10, 190, "%d small supply", wilenemy::supply_small / 2);
	//BWAPI::Broodwar->drawTextScreen(10, 200, "%d large supply", wilenemy::supply_large / 2);
	//BWAPI::Broodwar->drawTextScreen(10, 210, "%d air supply"  , wilenemy::supply_air / 2);
	//BWAPI::Broodwar->drawTextScreen(10, 220, "%d cloak supply", wilenemy::supply_cloak / 2);
	//BWAPI::Broodwar->drawTextScreen(10, 200, "%d near bunker", wilenemy::supply_near_bunker / 2);

	//list_states();
	//list_units(willyt::target_vector);
	//list_tanks();
	//list_scvs_pointers();

}

void Info::draw_tech(int x, int y, const char* s, bool b) {
	if (b) {
		BWAPI::Broodwar->drawBoxScreen(x, y + 2, x + 28, y + 12, 151, true); //151
		BWAPI::Broodwar->drawTextScreen(x + 2, y, "%s", s);
	}
}

void Info::draw_upgr(int x, int y, int l) {
	if (l > 0) {
		BWAPI::Broodwar->drawBoxScreen(x, y + 2, x + 18, y + 12, 151, true); //151
		BWAPI::Broodwar->drawTextScreen(x + 2, y, "+%d", l);
	}
}

void Info::draw_count(int x, int y, int n1, int n2, int n0, const char* s) {
	float f = (float) (n1 + n2) / n0;
	BWAPI::Broodwar->drawTextScreen(x, y, "%c%d / %d %s", get_text_color(f), n1, n0, s);
}

BWAPI::Color Info::get_text_color(float f) {
	if (f >= 1.0)		{ return BWAPI::Text::Green; }
	else if (f >= 0.7)	{ return BWAPI::Text::Yellow; }
	else if (f >= 0.4)	{ return BWAPI::Text::Orange; }
	else				{ return BWAPI::Text::Red; }
}

void Info::draw_build(int x, int y) {
	if (!wilbuild::planned_builds.empty()) {
		BWAPI::Broodwar->drawTextScreen(x, y, wilbuild::planned_builds.front().c_str());
	}
}

void Info::draw_256_colors() {
	for (int x = 0; x < 16; ++x) {
		for (int y = 0; y < 16; ++y) {
			int x0 = 16 * x;
			int y0 = 16 * y;
			BWAPI::Broodwar->drawBoxScreen(x0, y0, x0+16, y0+16, x+y*16, true);
		}
	}
}

void Info::list_states() {
	using namespace BWAPI;
	if (willyt::need_turrets) Broodwar->drawTextScreen(10, 170, "%cneed turrets", Text::Red);
	if (willyt::avoid_grddef) Broodwar->drawTextScreen(10, 180, "%cavoid grddef", Text::Red);
	if (willyt::proxy_alert)  Broodwar->drawTextScreen(10, 190, "%cproxy alert", Text::Red);
	if (willyt::rush_alert)   Broodwar->drawTextScreen(10, 200, "%crush alert", Text::Red);
}

void Info::list_types(std::vector<BWAPI::UnitType> &my_vec) {
	using namespace BWAPI;
	int x = 280;
	int y = 50;
	if (!my_vec.empty()) {
		Broodwar->drawTextScreen(x, y, "&cplanned builds", Text::Yellow);
		y += 10;
	}
	for (BWAPI::UnitType my_type : my_vec) {
		Broodwar->drawTextScreen(x, y, my_type.c_str());
		y += 10;
	}
}

void Info::list_units(std::vector<BWAPI::Unit> &my_vec) {
	using namespace BWAPI;
	int x = 280;
	int y = 50;
	if (!my_vec.empty()) {
		Broodwar->drawTextScreen(x, y, "%cspecial targets", Text::Yellow);
		y += 10;
	}
	for (BWAPI::Unit my_unit : my_vec) {
		if (!my_unit->exists()) { continue; }
		Broodwar->drawTextScreen(x, y, my_unit->getType().c_str());
		y += 10;
	}
}

void Info::list_tanks() {
	//for (Fighter2& t : wilunits::siegetanks) {
	//	if (t.unit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) {
	//		BWAPI::Broodwar->drawTextMap(t.unit->getPosition(), "tank mode");
	//	} else {
	//		BWAPI::Broodwar->drawTextMap(t.unit->getPosition(), "siege mode");
	//	}
	//}
	return;
}

void Info::list_scvs_pointers() {
	//int x = 540;
	//int y = 40;
	//BWAPI::Broodwar->drawTextScreen(x, y-10, "%d", sizeof(SCV));
	//for (std::vector<SCV>::iterator it = wilunits::scvs.begin(); it < wilunits::scvs.end(); it++) {
	//	BWAPI::Broodwar->drawTextScreen(x, y, "%d", it);
	//	y += 10;
	//}
}

void Info::display_lair_reaction_timer(int x, int y) {
	if (wilenemy::time_lair_reaction != 7200) {
		int t = wilenemy::time_lair_reaction - willyt::my_time;
		if (t > -60) { BWAPI::Broodwar->drawTextScreen(x, y, "reacting to lair in: %d s", t); }
	}
	return;
}