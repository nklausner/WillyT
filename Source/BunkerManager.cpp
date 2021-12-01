#include "BunkerManager.h"

void BunkerManager::update() {
	if (wilbuild::bunkers.empty()) { return; }
	safe_sum(bunker_queue, -1);

	if (wilenemy::supply_near_bunkers > 0) {
		load_bunkers();
		bunker_queue = 16;
	}
	if (bunker_queue == 0) {
		unload_bunkers();
	}
	if (bunker_queue > 0 &&
		wilenemy::supply_near_bunkers == 0 &&
		!wilenemy::intruders.empty()) {
		unload_bunkers();
	}
	return;
}

void BunkerManager::load_bunkers()
{
	std::vector<Fighter2>::iterator im = wilunits::marines.begin();
	unsigned ib = 0;
	for (BWAPI::Unit bunker : wilbuild::bunkers) {
		if (bunker->exists() &&
			bunker->isCompleted() &&
			bunker->getSpaceRemaining() > 0)
		{
			unsigned is = bunker->getSpaceRemaining();
			while (is > 0 && im != wilunits::marines.end())
			{
				if (im->unit->exists() == true &&
					im->unit->isLoaded() == false &&
					sqdist(im->unit, bunker) <= 256)
				{
					im->unit->load(bunker);
					--is;
				}
				++im;
			}
			//BWAPI::Broodwar->printf("bunker %d LOAD %d", ib, (bunker->getSpaceRemaining() - is));
		}
		++ib;
	}
	return;
}

void BunkerManager::unload_bunkers() {
	for (BWAPI::Unit bunker : wilbuild::bunkers) {
		if (bunker->getSpaceRemaining() < 4) {
			bunker->unloadAll();
			//BWAPI::Broodwar->printf("bunker UNLOAD");
		}
	}
	return;
}

