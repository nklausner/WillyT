#include "Repair.h"

void update_repairing() {
	if (wilunits::scvs.empty()) {
		return;
	}
	int nmax = 4;
	if (willyt::strategy == 3 &&
		willyt::my_time < 600) {
		nmax = 5;
	}
	int n = count_repairers();
	for (BWAPI::Unit &u : wilbuild::buildings) {
		check_canceling(u);
		check_continuing(u);
		if (n < nmax) { check_repairing(u); }
	}
	if (willyt::hold_bunker && n < 2) {
		hold_the_bunker();
	}
	return;
}

void check_canceling(BWAPI::Unit u) {
	if (!u->isCompleted() &&
		u->exists() &&
		u->isUnderAttack() &&
		u->getHitPoints() * 8 < u->getType().maxHitPoints()) {
		u->cancelConstruction();
		//BWAPI::Broodwar->printf("cancel building");
	}
	return;
}

void check_continuing(BWAPI::Unit u) {
	if (!u->isCompleted() &&
		!u->isBeingConstructed() &&
		u->exists()) {
		auto it = choose_scv(u->getTilePosition());
		if (it != wilunits::scvs.end()) {
			it->unit->rightClick(u);
			//it->destination = u->getPosition();
			//BWAPI::Broodwar->printf("continue building");
		}
	}
	return;
}

void check_repairing(BWAPI::Unit u) {
	using namespace BWAPI::UnitTypes;
	if (BWAPI::Broodwar->self()->minerals() > 0 &&
		u->exists() && u->isCompleted())
	{
		if ((u->getType() == Terran_Bunker && u->getHitPoints() < 350) ||
			(u->getType() == Terran_Missile_Turret && u->getHitPoints() < 200) ||
			(u->getType() == Terran_Command_Center && u->getHitPoints() < 1200) ||
			u->getHitPoints() * 2 < u->getType().maxHitPoints())
		{
			auto it = choose_scv(u->getTilePosition(), false);
			if (it != wilunits::scvs.end()) {
				it->set_repair_mission(u);
			}
		}
	}
	return;
}

void hold_the_bunker() {
	BWAPI::Unit my_bunker = wilbuild::bunkers.front();
	auto it = choose_scv(my_bunker->getTilePosition(), false);
	if (it != wilunits::scvs.end()) {
		it->set_repair_mission(my_bunker);
		it->move(my_bunker->getPosition());
	}
	return;
}

int count_repairers() {
	int i = 0;
	for (SCV& scv : wilunits::scvs)
		if (scv.is_repair)
			i++;
	return i;
}



void update_transport()
{
	for (SCV& scv : wilunits::scvs) {
		if (scv.called_transport && !scv.is_trapped)
		{
			for (Flyer& ds : wilunits::dropships) {
				if (ds.transport_unit == scv.unit)
				{
					ds.transport_unit = NULL;
					ds.transport_pos = BWAPI::Positions::None;
					ds.unit->stop();
					scv.called_transport = false;
					//BWAPI::Broodwar->printf("Transport cancelled.");
					break;
				}
			}
		}
	}

	for (SCV& scv : wilunits::scvs) {
		if (scv.is_trapped && !scv.called_transport)
		{
			for (Flyer& ds : wilunits::dropships) {
				if (!ds.is_raider &&
					!ds.transport_unit &&
					ds.unit->getSpaceRemaining() == 8)
				{
					ds.transport_unit = scv.unit;
					ds.transport_pos = scv.destination;
					scv.called_transport = true;
					//BWAPI::Broodwar->printf("Calling dropship for transport.");
					break;
				}
			}
		}
	}

	//auto it_scv = scvs.end();
	//for (auto it = scvs.begin(); it != scvs.end(); ++it) {
	//	if (it->is_trapped && !it->called_transport) {
	//		it_scv = it;
	//		break;
	//	}
	//}
	//auto it_ds = dropships.end();
	//for (auto it = dropships.begin(); it != dropships.end(); ++it) {
	//	if (!it->is_raider &&
	//		it->unit->getSpaceRemaining() == 8 &&
	//		it->transport_unit == NULL) {
	//		it_ds = it; 
	//		break;
	//	}
	//}

	//if (it_ds != dropships.end() && it_scv != scvs.end()) {
	//	it_ds->transport_unit = it_scv->unit;
	//	it_ds->transport_pos = it_scv->destination;
	//	it_scv->called_transport = true;
	//	BWAPI::Broodwar->printf("Calling dropship for transport.");
	//}

	//if (it_ds != dropships.end()) {
	//	if (!it_ds->unit->getLoadedUnits().empty()) {
	//		it_ds->unit->unloadAll(it_ds->transport_pos);
	//		//BWAPI::Broodwar->printf("Send transport.");
	//		return;
	//	}
	//	if (it_scv != scvs.end()) {
	//		it_scv->unit->load(it_ds->unit);
	//		it_ds->transport_pos = it_scv->destination;
	//		//BWAPI::Broodwar->printf("Catch transport.");
	//		return;
	//	}
	//}

	//for (auto it = dropships.begin(); it != dropships.end(); ++it) {
	//	if (!it->is_raider &&
	//		!it->unit->getLoadedUnits().empty() &&
	//		has_grd_connection(it->unit->getPosition(), it->transport_pos)) {
	//		it->unit->unloadAll();
	//		it->unit->stop();
	//		//BWAPI::Broodwar->printf("Unload transport.");
	//		return;
	//	}
	//}
	return;
}