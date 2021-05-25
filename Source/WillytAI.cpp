#include "WillytAI.h"

void WillytAI::onStart()
{
	willyt::test_strategy = 0;		//zero for no testing
	willyt::avoid_weak_strategies = true;
	willyt::use_hardcoded_strategies = false;
	willyt::strategy = choose_strategy( read_logfile() );
	willyt::orig_strategy = willyt::strategy;

	willyt::is_vs_human = false;
	willyt::ums_to_pratice_micro = true;

	mapanalysis.init();				//independent
	BaseFinderJaj22::Init();		//independent
	expomanager.init();				//requires distances
	mapanalysis.init_advanced();	//requires naturals
	calculate_army_positions();
	trainer.init();
	buildmanager.init();

	BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
	BWAPI::Broodwar->setCommandOptimizationLevel(2);
	BWAPI::Broodwar->setLocalSpeed(24);
	BWAPI::Broodwar->setFrameSkip(8);

	//BWAPI::Broodwar->printf("console?");
	//call AllocConsole, windows.h, freopen("CONOUT$", "w", stdout);
	//$(SolutionDir)$(Configuration)\BWAPILIB.lib

	return;
}



void WillytAI::onEnd(bool isWinner)
{
	BWAPI::Broodwar->sendText( write_logfile(wilenemy::name, wilenemy::race.toString(), willyt::orig_strategy, isWinner) );
	BWAPI::Broodwar->sendText("gg");
	return;
}



void WillytAI::onFrame()
{
	using namespace wilbuild;
	using namespace wilunits;

	// Return if the game is a replay or is paused
	if (BWAPI::Broodwar->isReplay() ||
		BWAPI::Broodwar->isPaused() ||
		!BWAPI::Broodwar->self()) {
		return;
	}

	//execute bot sections in diffent intervals
	int n = BWAPI::Broodwar->getFrameCount();
	willyt::my_time = n * 42 / 1000;
	//write_frame(n); //catch crash frames (debugging)
	//write_string(std::to_string(n) + " update start");

	if (n % 512 == 256)			//rarely
	{
		statemanager.update(dropships.size()); //calculate work/army treshholds
		expofinder.check();			//expand decision
		clean();					//corrections for miscellaneous and misterious behavior
		unitmanager.correct();		//ghost scvs and dropships
		enemymanager.correct();		//vacant enemy building positions
		expomanager.correct();		//minerals gone or geysers lost
	}
	else if (n % 64 == 32)		//regularly
	{
		comsatmanager.check();		//scanning detection, scouting, random
		upgrader.upgrade();			//tech and upgrades
		expomanager.update(scvs);	//count workers on gas/min per base
		miningmanager.update();		//count containers, redistribute workers
		update_repairing();			//building repair, continue, cancel
		update_transport();			//catch workers that want to islands
	}
	else if (n % 8 == 4)		//very often
	{
		unitmanager.count_supplies();
		update_army_groups();		//army groups
		check_all_reactions();		//specific reactions, mostly to cheese
		enemymanager.update();		//unittype percentages, incomplete defense
		trainer.train();			//training and addons
		buildmanager.build();		//follow buildorders for base building
		threatmngr.update();		//watch out for area threads: siege, lurk, storm, ...
		bunkermanager.update();		//load'n'unload
		armymanager.update();		//find targets for defense, attack (given supply)
		dropmanager.update();
		floatcontrol.update();
		threatmngr.ignore_stray_lurkers(willyt::available_scans);
		unitmanager.update_eight();	//check scvs being stuck
	}
	else						//almost every frame (439 of 512)
	{
		comsatmanager.check_unscouted();
		update_all_targets();		//sort enemy units
		unitmanager.update_all();	//micro all units
	}
	if (willyt::show_info)		//constantly
	{
		mapinfo.display();			//display expansion info on map
		info.display();				//display general info on screen
		dropmanager.show_info(540, 30);
	}
	return;
}



void WillytAI::onSendText(std::string text)
{
	//BWAPI::Broodwar->sendText("%s", text.c_str());
}

void WillytAI::onReceiveText(BWAPI::Player player, std::string text) {}

void WillytAI::onPlayerLeft(BWAPI::Player player) {}

void WillytAI::onSaveGame(std::string gameName) {}

void WillytAI::onNukeDetect(BWAPI::Position target)
{
	threatmngr.append_nuke(target);	//scream and run!!!
} 

void WillytAI::onUnitEvade(BWAPI::Unit unit) {}

void WillytAI::onUnitShow(BWAPI::Unit unit) {}

void WillytAI::onUnitHide(BWAPI::Unit unit) {}



void WillytAI::onUnitDiscover(BWAPI::Unit unit)
{
	if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self())) {
		enemymanager.append_unit(unit);

		if (unit->getType().isResourceDepot()) {
			expomanager.set_enemy_expo(unit->getTilePosition(), true);
		}
	}
	return;
}

void WillytAI::onUnitCreate(BWAPI::Unit unit)
{
	if (unit->getPlayer() == BWAPI::Broodwar->self()) {
		if (!unit->getType().isBuilding()) {
			unitmanager.tcount_unit(unit);
		}
		if (unit->getType().isBuilding()) {
			wilbuild::append_building(unit);
		}
		if (unit->getType() == BWAPI::UnitTypes::Terran_Command_Center) {
			expomanager.occupy_expo(unit);
		}
	}
	if (unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser) {
		expomanager.rebuild_geyser(unit);
	}
	return;
}

void WillytAI::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit->getPlayer() == BWAPI::Broodwar->self()) {
		if (!unit->getType().isBuilding()) {
			unitmanager.remove_unit(unit);
			dropmanager.keep_kills(unit);
		}
		if (unit->getType().isBuilding()) {
			wilbuild::remove_building(unit);
		}
		if (unit->getType() == BWAPI::UnitTypes::Terran_Command_Center) {
			expomanager.desert_expo(unit->getTilePosition());
		}
	}
	else if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self())) {
		enemymanager.remove_unit(unit);
		threatmngr.remove_destroyed(unit);

		if (unit->getType().isResourceDepot()) {
			expomanager.set_enemy_expo(unit->getTilePosition(), false);
		}
	}

	if (unit->getType().isMineralField()) {
		expomanager.destroy_mineral(unit);
		miningmanager.end_min_mining(unit);
	}
	if (unit->getType().isRefinery()) {
		expomanager.destroy_geyser(unit);
	}
	return;
}

void WillytAI::onUnitMorph(BWAPI::Unit unit)
{
	if (unit->getPlayer() == BWAPI::Broodwar->self() &&
		unit->getType() == BWAPI::UnitTypes::Terran_Refinery) {
		wilbuild::append_building(unit);
	}
	else if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self())) {
		enemymanager.change_unit(unit);
	}
	return;
}

void WillytAI::onUnitRenegade(BWAPI::Unit unit)
{
	if (unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser ||
		unit->getType().isAddon()) {
		wilbuild::remove_building(unit);
		enemymanager.remove_unit(unit);
	}
	return;
}

void WillytAI::onUnitComplete(BWAPI::Unit unit)
{
	using namespace BWAPI::UnitTypes;
	if (unit->getPlayer() == BWAPI::Broodwar->self()) {
		if (!unit->getType().isBuilding()) {
			unitmanager.append_unit(unit);
			if (unit->getType().isWorker() && !wilbuild::depots.empty()) {
				unitmanager.assign_scout(unit);
			}
		}
		else {
			switch (unit->getType()) {
			case Terran_Bunker:			wilbuild::change_inf_bunker(unit, +1); break;
			case Terran_Missile_Turret:	wilbuild::change_inf_turret(unit, +1); break;
			case Terran_Refinery:		miningmanager.start_gas_mining(unit); break;
			case Terran_Command_Center:	miningmanager.start_min_mining(unit); break;
			}
			if (willyt::fast_expand) { buildmanager.build_consecutive(unit); }
		}
	}
	else if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self())) {
		if (unit->getType().isBuilding()) {
			enemymanager.check_time_lair(unit, true);
		}
		return;
	}
	return;
}
