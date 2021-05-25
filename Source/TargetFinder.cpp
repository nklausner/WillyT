#include "TargetFinder.h"


void update_all_targets() {
	using namespace wilenemy;
	using namespace BWAPI::UnitTypes;

	tlg.clear();
	tlgg.clear();
	tlga.clear();
	tlgs.clear();
	tlgc.clear();
	tlgw.clear();
	tlgm.clear();
	tlgl.clear();
	tla.clear();
	tlag.clear();
	tlaa.clear();
	tlas.clear();
	tlac.clear();

	tleggla.clear();
	tlflare.clear();
	tllockd.clear();
	tlyama.clear();
	tldefg.clear();
	tldefa.clear();

	target_count = 0;

	for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
		for (BWAPI::Unit u : p->getUnits()) {
			if (!u->exists()) { continue; }
			BWAPI::UnitType t = u->getType();

			//handle non targetable units
			if ((u->isCloaked() && !u->isDetected()) ||
				(u->isBurrowed() && !u->isDetected()) ||
				u->isLoaded() ||
				u->isStasised() ||
				u->isInvincible()) {
				if (u->isFlying()) {
					tlac.push_back(u);
				} else {
					tlgc.push_back(u);
				}
			}
			else {
				//handle flying units
				if (u->isFlying()) {
					if (t.groundWeapon() != 130) {
						tlag.push_back(u);
					}
					if (t.airWeapon() != 130) {
						tlaa.push_back(u);
					}
					if (t == Protoss_Carrier) {
						tlag.push_back(u);
						tlaa.push_back(u);
						if (!u->isBlind()) {
							tlflare.push_back(u);
						}
					}
					if (t == Zerg_Overlord ||
						t == Zerg_Queen ||
						t == Zerg_Cocoon ||
						t == Protoss_Observer ||
						t == Protoss_Shuttle ||
						t == Terran_Science_Vessel ||
						t == Terran_Dropship) {
						tlas.push_back(u);
						if (!u->isBlind()) {
							tlflare.push_back(u);
						}
					}
					if (t == Zerg_Guardian ||
						t == Zerg_Devourer ||
						t == Terran_Battlecruiser) {
						if (!u->isBlind()) {
							tlflare.push_back(u);
						}
					}
					if (t.isMechanical() &&
						!t.isBuilding() &&
						t != Protoss_Interceptor &&
						!u->isLockedDown()) {
						tllockd.push_back(u);
					}
					tla.push_back(u);
				}
				//handle ground units
				else {
					if (t.airWeapon() != 130) {
						tlga.push_back(u);
					}
					if (t.groundWeapon() != 130) {
						tlgg.push_back(u);
					}
					else if (t == Zerg_Defiler ||
						t == Zerg_Infested_Terran ||
						t == Protoss_High_Templar ||
						t == Protoss_Dark_Archon ||
						t == Protoss_Reaver ||
						t == Terran_Medic) {
						tlgs.push_back(u);
					}
					if ((t.size() == BWAPI::UnitSizeTypes::Small ||
						t.size() == BWAPI::UnitSizeTypes::Medium) &&
						t != Zerg_Larva &&
						t != Zerg_Egg &&
						t != Zerg_Lurker_Egg &&
						t != Protoss_Scarab) {
						tlgm.push_back(u);
					}
					else if (t.size() == BWAPI::UnitSizeTypes::Large &&
						!t.isBuilding()) {
						tlgl.push_back(u);
					}
					if (t.isMechanical() &&
						!t.isBuilding() &&
						!t.isWorker() &&
						!u->isLockedDown()) {
						tllockd.push_back(u);
					}
					if (t.isWorker()) {
						tlgw.push_back(u);
					}
					if (t.isBuilding()) {
						if (t == Terran_Bunker ||
							t == Protoss_Photon_Cannon ||
							t == Zerg_Sunken_Colony) {
							tldefg.push_back(u);
						}
						if (t == Terran_Bunker ||
							t == Terran_Missile_Turret ||
							t == Protoss_Photon_Cannon ||
							t == Zerg_Spore_Colony) {
							tldefa.push_back(u);
						}
					}
					if (t == Zerg_Larva ||
						t == Zerg_Egg) {
						tleggla.push_back(u);
					} else {
						tlg.push_back(u);
					}
				}
				++target_count;
			}
		}
	}

	if (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Yamato_Gun)) {
		for (BWAPI::Player p : BWAPI::Broodwar->enemies()) {
			for (BWAPI::Unit u : p->getUnits()) {
				BWAPI::UnitType t = u->getType();

				if (t.isBuilding()) {
					if (t == Zerg_Spore_Colony ||
						t == Terran_Missile_Turret ||
						t == Terran_Bunker ||
						t == Protoss_Photon_Cannon) {
						tlyama.push_back(u);
					}
				}
				else if (t.isFlyer()) {
					if (t == Zerg_Defiler ||
						t == Zerg_Queen ||
						t == Zerg_Devourer ||
						t == Terran_Wraith ||
						t == Terran_Valkyrie ||
						t == Terran_Science_Vessel ||
						t == Terran_Battlecruiser ||
						t == Protoss_Scout ||
						t == Protoss_Corsair ||
						t == Protoss_Arbiter ||
						t == Protoss_Carrier) {
						tlyama.push_back(u);
					}
				}
				else {
					if (t == Terran_Goliath ||
						t == Protoss_Dragoon) {
						tlyama.push_back(u);
					}
				}
			}
		}
	}

	return;
}