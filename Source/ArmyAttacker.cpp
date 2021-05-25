#include "ArmyAttacker.h"

void army_attack(BWAPI::Position& my_pos, bool is_attack)
{
	if (willyt::strategy == 4 && !is_none(willyt::leader_pos)) {
		army_attack_tank_only(my_pos, willyt::leader_pos, is_attack);
		return;
	}

	using namespace wilunits;
	for (SCV &s : scvs)				if (s.is_militia)	s.set_attack(my_pos);
	for (Fighter2 &m : marines)		if (!m.is_raider)	m.set_target(my_pos, is_attack);
	for (Fighter2 &f : firebats)	if (!f.is_raider)	f.set_target(my_pos, is_attack);
	for (Fighter2 &m : medics)		if (!m.is_raider)	m.set_target(my_pos, is_attack);
	for (Fighter2 &g : ghosts)		if (!g.is_raider)	g.set_target(my_pos, is_attack);
	for (Fighter2 &s : siegetanks)	if (!s.is_raider)	s.set_target(my_pos, is_attack);
	for (Fighter2 &g : goliaths)	if (!g.is_raider)	g.set_target(my_pos, is_attack);
	for (Fighter2 &v : vultures)	if (!v.is_raider)	v.set_target(my_pos, is_attack);
	return;
}
void army_swarm(bool imm)
{
	using namespace wilunits;
	for (SCV &s : scvs)				if (s.is_militia)	s.set_swarm(imm);
	for (Fighter2 &m : marines)		if (!m.is_raider)	m.set_swarm(imm);
	for (Fighter2 &f : firebats)	if (!f.is_raider)	f.set_swarm(imm);
	for (Fighter2 &m : medics)		if (!m.is_raider)	m.set_swarm(imm);
	for (Fighter2 &g : ghosts)		if (!g.is_raider)	g.set_swarm(imm);
	for (Fighter2 &t : siegetanks)	if (!t.is_raider)	t.set_swarm(imm);
	for (Fighter2 &g : goliaths)	if (!g.is_raider)	g.set_swarm(imm);
	for (Fighter2 &v : vultures)	if (!v.is_raider)	v.set_swarm(imm);
	return;
}
void raiders_update(int my_state, BWAPI::Position& att_pos, BWAPI::Position& def_pos, BWAPI::Unit my_transport)
{
	using namespace wilunits;
	for (Fighter2& m : marines)		if (m.is_raider)	m.update_raider(my_state, att_pos, def_pos, my_transport);
	for (Fighter2& f : firebats)	if (f.is_raider)	f.update_raider(my_state, att_pos, def_pos, my_transport);
	for (Fighter2& m : medics)		if (m.is_raider)	m.update_raider(my_state, att_pos, def_pos, my_transport);
	for (Fighter2& g : ghosts)		if (g.is_raider)	g.update_raider(my_state, att_pos, def_pos, my_transport);
	for (Fighter2& t : siegetanks)	if (t.is_raider)	t.update_raider(my_state, att_pos, def_pos, my_transport);
	for (Fighter2& g : goliaths)	if (g.is_raider)	g.update_raider(my_state, att_pos, def_pos, my_transport);
	for (Fighter2& v : vultures)	if (v.is_raider)	v.update_raider(my_state, att_pos, def_pos, my_transport);
	return;
}
void army_attack_tank_only(BWAPI::Position& pa, BWAPI::Position& pt, bool is_attack)
{
	using namespace wilunits;
	for (SCV& s : scvs)				if (s.is_militia)	s.set_attack(pa);
	for (Fighter2& m : marines)		if (!m.is_raider)	m.set_target(pt, is_attack);
	for (Fighter2& f : firebats)	if (!f.is_raider)	f.set_target(pt, is_attack);
	for (Fighter2& m : medics)		if (!m.is_raider)	m.set_target(pt, is_attack);
	for (Fighter2& g : ghosts)		if (!g.is_raider)	g.set_target(pt, is_attack);
	for (Fighter2& s : siegetanks)	if (!s.is_raider)	s.set_target(pa, is_attack);
	for (Fighter2& g : goliaths)	if (!g.is_raider)	g.set_target(pt, is_attack);
	for (Fighter2& v : vultures)	if (!v.is_raider)	v.set_target(pt, is_attack);
	return;
}


void army_split(BWAPI::Position& dpos, BWAPI::Position& apos, int enemytotstr, int enemyairstr, bool is_attack)
{
	if (willyt::strategy == 4 && !is_none(willyt::leader_pos)) {
		army_attack_tank_only(dpos, willyt::leader_pos, is_attack);
		return;
	}

	using namespace wilunits;
	int aa = marines.size() + medics.size() / 2 + ghosts.size() + 2 * goliaths.size();
	int ag = willyt::sup_mil + firebats.size() + medics.size() / 2 + 2 * siegetanks.size() + 2 * vultures.size();
	float qa = 0.5;
	float qg = 0.5;

	if (enemytotstr >= 2 * enemyairstr) {						//just send every type
		if (aa + ag > 0) {
			qa = enemytotstr / (float)(aa + ag);
			qg = enemytotstr / (float)(aa + ag);
		}
	} else {													//only send anti air
		if (aa > 0) {
			qa = enemytotstr / (float)(aa);
			qg = 0.0;
			if (qa > 1.5) { qg = 0.5; }
		}
	}

	execute_split(dpos, apos, marines,  (int)(qa * marines.size()) + 1,  is_attack);
	execute_split(dpos, apos, medics,   (int)(qa * medics.size()) + 1,   is_attack);
	execute_split(dpos, apos, ghosts,   (int)(qa * ghosts.size()) + 1,   is_attack);
	execute_split(dpos, apos, goliaths, (int)(qa * goliaths.size()) + 1, is_attack);

	execute_split(dpos, apos, scvs,     (int)(qg * willyt::sup_mil) + 1);
	execute_split(dpos, apos, firebats, (int)(qg * firebats.size()) + 1, is_attack);
	execute_split(dpos, apos, vultures, (int)(qg * vultures.size()) + 1, is_attack);
	execute_split(dpos, apos, siegetanks, (int)(qg * siegetanks.size()) + 1, is_attack);

	//BWAPI::Broodwar->printf("defense fraction: %4.2f air, %4.2f grd", qa, qg);
	return;
}
void execute_split(BWAPI::Position& dpos, BWAPI::Position& apos, std::vector<SCV>& my_vec, int dcount)
{
	for (std::vector<SCV>::reverse_iterator rit = my_vec.rbegin(); rit != my_vec.rend(); ++rit) {
		if (rit->is_militia) {
			if (dcount > 0) {
				rit->set_attack(dpos);
				--dcount;
			} else {
				rit->set_attack(apos);
			}
		}
	}
}
void execute_split(BWAPI::Position& dpos, BWAPI::Position& apos, std::vector<Fighter2>& my_vec, int dcount, bool is_attack)
{
	for (std::vector<Fighter2>::reverse_iterator rit = my_vec.rbegin(); rit != my_vec.rend(); ++rit) {
		if (!rit->is_raider) {
			if (dcount > 0) {
				rit->set_target(dpos, false);
				--dcount;
			} else {
				rit->set_target(apos, is_attack);
			}
		}
	}
}


//if (is_air) {
//	int g = goliaths.size();
//	float f = 0.0;
//	if (g > 0) { f = 2 * n / (float)g; }
//	d_goliath = 1 + (int)(f * a);
//	d_marine_ = 1 + (int)((a - f * a) * 0.83);
//	d_medic__ = 1 + (int)((a - f * a) * 0.17);
//	d_ghost__ = 1 + (int)((a - f * a) * 0.5); //i have no clue what i am doing here :-)
//	d_militia = 0;
//	d_firebat = 0;
//	d_sietank = 0;
//	d_vulture = 0;
//}
//else {
//	float q = 0.0;
//	if (n + m > 0) { q = a / (float)(n + m); } //20200428 found it turned around :-O
//	d_militia = 1 + (int)(q * m);
//	d_marine_ = 1 + (int)(q * marines.size());
//	d_firebat = 1 + (int)(q * firebats.size());
//	d_medic__ = 1 + (int)(q * medics.size());
//	d_ghost__ = 1 + (int)(q * ghosts.size());
//	d_sietank = 1 + (int)(q * siegetanks.size() / 2);
//	d_goliath = 1 + (int)(q * goliaths.size() / 2);
//	d_vulture = 1 + (int)(q * vultures.size() / 2);
//}
//BWAPI::Broodwar->printf("army split: %d to %d", n, a);
//BWAPI::Broodwar->printf("marines: %d of %d", d_marine_, marines.size());


//for (std::vector<SCV>::reverse_iterator it = scvs.rbegin(); it != scvs.rend(); ++it)
//	if (it->is_militia)
//		it->set_attack(decide_split(d_militia, dpos, apos));
//for (std::vector<Fighter2>::reverse_iterator it = marines.rbegin(); it != marines.rend(); ++it)
//	if (!it->is_raider)
//		it->set_target(decide_split(d_marine, dpos, apos));
//for (std::vector<Fighter2>::reverse_iterator it = firebats.rbegin(); it != firebats.rend(); ++it)
//	if (!it->is_raider)
//		it->set_target(decide_split(d_firebat, dpos, apos));
//for (std::vector<Fighter2>::reverse_iterator it = medics.rbegin(); it != medics.rend(); ++it)
//	if (!it->is_raider)
//		it->set_target(decide_split(d_medic, dpos, apos));
//for (std::vector<Fighter2>::reverse_iterator it = ghosts.rbegin(); it != ghosts.rend(); ++it)
//	if (!it->is_raider)
//		it->set_target(decide_split(d_ghost, dpos, apos));
//for (std::vector<Fighter2>::reverse_iterator it = siegetanks.rbegin(); it != siegetanks.rend(); ++it)
//	if (!it->is_raider)
//		it->set_target(decide_split(d_sietank, dpos, apos));
//for (std::vector<Fighter2>::reverse_iterator it = goliaths.rbegin(); it != goliaths.rend(); ++it)
//	if (!it->is_raider)
//		it->set_target(decide_split(d_goliath, dpos, apos));
//for (std::vector<Fighter2>::reverse_iterator it = vultures.rbegin(); it != vultures.rend(); ++it)
//	if (!it->is_raider)
//		it->set_target(decide_split(d_vulture, dpos, apos));