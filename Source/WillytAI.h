#pragma once
#include <BWAPI.h>
#include "BaseFinderJaj22.h"
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytEnemy.h"
#include "WillyThreat.h"
#include "MapAnalysis.h"
#include "ArmyPositions.h"
#include "ExpoManager.h"
#include "ExpoFinder.h"
#include "Trainer.h"
#include "Upgrader.h"
#include "BuildManager.h"
#include "ArmyManager.h"
#include "BunkerManager.h"
#include "MiningManager.h"
#include "ComsatManager.h"
#include "TargetFinder.h"
#include "Reaction.h"
#include "Repair.h"
#include "Cleaner.h"
#include "Grouper.h"
#include "DropManager.h"
#include "FloatControl.h"
#include "Info.h"
#include "MapInfo.h"
#include "ReadWrite.h"
#include "Learner.h"


//willyt history:
//2018-03-06: start with renaming the example-ai-module
//2018-04-02: later april fools upload
//2018-04-07: division by zero :D (average medic energy)
//2018-06-24: added many new features: namespace, bunker, +1/+1, expanding, mining distribution
//2018-06-24: first win vs 2 standard AIs, still potato & crash bugs lurking
//2018-10-05: finished preparations for the submission to AIIDE 2018
//2019-01-28: finally a hint to the recurring crashes, they appear at n % 64 == 32
//			  crash frames: 30176, 32544, 26720, 28576, 28960
//2019-01-31: found it: remove_gas_miner()
//2019-03-07: map analysis: dist, path, choke maps now for all start locations
//2019-04-06: new fighter class replacing multiple classes
//2019-04-18: starport/science building spot => +3/+3 upgrades and cloak/lockdown ghost
//2019-08-09: update to bwapi 4.4.0
//2020-01-28: random spider mines and clearing mine/burrow blocks
//			  now with third build/strat: 1Fac expand + mech only
//2020-04_19: implemented fighter2 with targeting for marines
//2020-04-24: added threat areas for siege, lurk, storm, nuke
//2020-04-25: added fighter2 for ghosts, now crashes
//2021-03-05: added mineral locking


//written by Nico Klausner (WillBotSC) 2018
//modified ExampleAIModule
//inspired by Oleg Ostroumovs bio+scv rush
//using BWAPI 4.4.0
//using BaseFinder by Jaj22
//thanks for the help in SSCAIT discord chat: N00byEdge and others


class WillytAI : public BWAPI::AIModule
{
public:

	// Virtual functions for callbacks, leave these as they are.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);
	// Everything below this line is safe to modify.

	StateManager statemanager;
	UnitManager unitmanager;
	ExpoManager expomanager;
	ExpoFinder expofinder;
	MapAnalysis mapanalysis;
	EnemyManager enemymanager;
	ThreatManager threatmngr;
	Trainer trainer;
	Upgrader upgrader;
	BuildManager buildmanager;
	ArmyManager armymanager;
	BunkerManager bunkermanager;
	MiningManager miningmanager;
	ComsatManager comsatmanager;
	DropManager dropmanager;
	FloatControl floatcontrol;
	Info info;
	MapInfo mapinfo;

};