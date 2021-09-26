#pragma once
#include <BWAPI.h>
#include "WillytMap.h"
#include "WillytExpo.h"
#include "MapUtilities.h"
#include "Utilities.h"

//2019-04-14: outsourcing
//2019-12-13: circle generation using arccos

//fill building map with minerals, geysers, build locations
void fill_build_map_with_expos();

//find island expansions (connected to no main)
//and continent expansion (connected to not all mains)
void determine_islands();

//determine positions of natural expansions
void determine_natu_all();
void determine_natu(int i);
void determine_my_natu();

//determine circle formation of outer expansion
void determine_expo_circle();