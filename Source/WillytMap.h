#pragma once
#include <BWAPI.h>

//2019-03-08: new namespace for all maps and positions
//centralising and collecting
//in future unit agents can have access to it 

namespace wilmap
{
	//map dimensions

	extern int wt;	//map width  in tiles
	extern int ht;	//map height in tiles
	extern int ww;	//map width  in walkpos
	extern int hw;	//map height in walkpos
	extern int wp;	//map width  in pixel
	extern int hp;	//map height in pixel
	extern BWAPI::Position center_pos;	//center position

	//primary maps

	extern bool build_map_fix[256][256];
	extern bool build_map_var[256][256];
	extern bool walk_map[256][256];
	extern int space_map[256][256];

	//secondary/tertiary maps

	extern int maindistarray[8][256][256];
	extern int centerdistmap[256][256];
	extern int mainpathmap[256][256];
	extern bool choke_map[256][256];
	extern bool main_map[256][256];

	extern bool walkmap[1024][1024];
	extern bool walkmapfixed[1024][1024];
	extern bool chokewalkmap[1024][1024];
	extern int centerdistwalkmap[1024][1024];
	extern int pathwalkmap[1024][1024];

	//influence area maps

	extern int defense_map[256][256];		//own defense area
	extern int bunker_map[256][256];	//own bunker area
	extern int turret_map[256][256];	//own turret area
	extern bool siege_map[256][256];	//own siege area
	extern bool wall_map[256][256];		//wallins

	//defense maps

	extern bool maindefmap[256][256];
	extern bool natudefmap[256][256];
	extern std::vector<BWAPI::TilePosition> my_maindefvec;
	extern std::vector<BWAPI::TilePosition> my_natudefvec;

	//player special locations

	extern BWAPI::TilePosition my_start;
	extern BWAPI::Position my_main;
	extern BWAPI::Position my_natu;
	extern BWAPI::Position my_main_def;
	extern BWAPI::Position my_natu_def;
	extern BWAPI::TilePosition my_entrance;
	extern BWAPI::Position my_bunker_def_pos;
	extern int bunker_natu_sqdist;

	//enemy special locations

	extern BWAPI::TilePosition en_start;
	extern BWAPI::Position en_main;
	extern BWAPI::Position en_natu;

	//base locations

	extern BWAPI::TilePosition main_tiles[8];	//all main base tiles
	extern BWAPI::TilePosition natu_tiles[8];	//all natural expo tiles
	extern int mn;			//total main number
	extern int mm;			//my main number

	//scouting and enemy

	extern std::vector<BWAPI::Position> unscouted;
	extern int entrance_circle_increment;

	//main and natural positions

	extern BWAPI::Position main_pos[8];
	extern BWAPI::Position natu_pos[8];
	extern BWAPI::Position main_choke_pos[8];
	extern BWAPI::Position natu_choke_pos[8];
	extern BWAPI::Position main_def_pos[8];
	extern BWAPI::Position natu_def_pos[8];
	extern BWAPI::WalkPosition main_choke_edge[16];
	extern BWAPI::WalkPosition natu_choke_edge[16];

	//defense tiles

	extern BWAPI::TilePosition main_def_tile[8][2];
	extern BWAPI::TilePosition natu_def_tile[8][2];
	extern BWAPI::TilePosition rush_def_tile[8];
	extern BWAPI::Position test_pos[8];
	extern BWAPI::TilePosition sas_tile;
	extern BWAPI::TilePosition natu_wall_tile[8][2];

	//build plan tile

	extern std::vector<BWAPI::TilePosition> plan_small;
	extern std::vector<BWAPI::TilePosition> plan_large;
	extern std::vector<BWAPI::TilePosition> plan_small_tech;
	extern std::vector<BWAPI::TilePosition> plan_large_tech;

	//circles of outside expansions

	extern std::vector<BWAPI::Position> grdcircle;
	extern std::vector<BWAPI::Position> aircircle;
	extern std::vector<BWAPI::Position> flycircle;

	//army positions

	extern std::vector<BWAPI::Position> armycircle;

	//circles with increments

	extern int cdx8[8];		//dx for circle8
	extern int cdy8[8];		//dy for circle8
	extern int cdz8[8];
	extern int cdx24[24];	//dx pixel per 2 tiles for circle24
	extern int cdy24[24];	//dy pixel per 2 tiles for circle24
	extern int cr4dx[24];	//dx walkpos for circle24 with 4 tiles range
	extern int cr4dy[24];	//dy walkpos for circle24 with 4 tiles range

	//predesigned arrays for circle areas in walkpositions

	extern int circ_area_r1[16];
	//extern int circ_area_r6[56];
	extern int circ_area_r6[60];
	extern int circ_area_r8[72];
	extern int circ_area_r9[80];
	extern int circ_area_r11[96];
	extern int circ_area_r12[104];

	extern int circ_area_def6[60];
	extern int circ_area_def7[68];

}

//void clear_build_map_var(BWAPI::Unit u);