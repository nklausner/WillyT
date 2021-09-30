#include "WillytMap.h"

namespace wilmap
{

	int wt = 256;
	int ht = 256;
	int ww = 1024;
	int hw = 1024;
	int wp = 8192;
	int hp = 8192;
	BWAPI::Position center_pos = BWAPI::Positions::None;

	bool build_map_fix[256][256];
	bool build_map_var[256][256];
	bool walk_map[256][256];
	int space_map[256][256];

	int maindistarray[8][256][256];
	int centerdistmap[256][256];
	int mainpathmap[256][256];
	bool choke_map[256][256];
	bool main_map[256][256];

	bool walkmap[1024][1024];
	bool walkmapfixed[1024][1024];
	bool chokewalkmap[1024][1024];
	int centerdistwalkmap[1024][1024];
	int pathwalkmap[1024][1024];

	int defense_map[256][256];
	int bunker_map[256][256];
	int turret_map[256][256];
	bool siege_map[256][256];
	bool wall_map[256][256];

	bool maindefmap[256][256];
	bool natudefmap[256][256];
	bool thirddefmap[256][256];
	std::vector<BWAPI::TilePosition> my_maindefvec;
	std::vector<BWAPI::TilePosition> my_natudefvec;
	std::vector<BWAPI::TilePosition> my_thirddefvec;

	BWAPI::TilePosition my_start = BWAPI::TilePositions::None;
	BWAPI::Position my_main = BWAPI::Positions::None;
	BWAPI::Position my_natu = BWAPI::Positions::None;
	BWAPI::Position my_main_def = BWAPI::Positions::None;
	BWAPI::Position my_natu_def = BWAPI::Positions::None;
	BWAPI::TilePosition my_entrance = BWAPI::TilePositions::None;
	BWAPI::Position my_bunker_def_pos = BWAPI::Positions::None;
	int bunker_natu_sqdist = 36864;

	BWAPI::TilePosition en_start = BWAPI::TilePositions::None;
	BWAPI::Position en_main = BWAPI::Positions::None;
	BWAPI::Position en_natu = BWAPI::Positions::None;

	BWAPI::TilePosition main_tiles[8] = { BWAPI::TilePositions::None };
	BWAPI::TilePosition natu_tiles[8] = { BWAPI::TilePositions::None };
	int mn = 8;
	int mm = 0;

	std::vector<BWAPI::Position> unscouted;
	int entrance_circle_increment = -1;

	BWAPI::Position main_pos[8] = { BWAPI::Positions::None };
	BWAPI::Position natu_pos[8] = { BWAPI::Positions::None };
	BWAPI::Position main_choke_pos[8] = { BWAPI::Positions::None };
	BWAPI::Position natu_choke_pos[8] = { BWAPI::Positions::None };
	BWAPI::Position main_def_pos[8] = { BWAPI::Positions::None };
	BWAPI::Position natu_def_pos[8] = { BWAPI::Positions::None };
	BWAPI::WalkPosition main_choke_edge[16] = { BWAPI::WalkPositions::None };
	BWAPI::WalkPosition natu_choke_edge[16] = { BWAPI::WalkPositions::None };

	BWAPI::TilePosition main_def_tile[8][2] = { BWAPI::TilePositions::None };
	BWAPI::TilePosition natu_def_tile[8][2] = { BWAPI::TilePositions::None };
	BWAPI::TilePosition rush_def_tile[8] = { BWAPI::TilePositions::None };
	BWAPI::Position test_pos[8] = { BWAPI::Positions::None };
	BWAPI::TilePosition sas_tile = BWAPI::TilePositions::None;
	BWAPI::TilePosition natu_wall_tile[8][2] = { BWAPI::TilePositions::None };

	std::vector<BWAPI::TilePosition> plan_small;
	std::vector<BWAPI::TilePosition> plan_large;
	std::vector<BWAPI::TilePosition> plan_small_tech;
	std::vector<BWAPI::TilePosition> plan_large_tech;

	std::vector<BWAPI::Position> grdcircle;
	std::vector<BWAPI::Position> aircircle;
	std::vector<BWAPI::Position> flycircle;

	BWAPI::TilePosition my_sneaky_tile = BWAPI::TilePositions::None;
	int my_sneak_direction = 0;

	int cdx8[8] = { +1,+1,+0,-1,-1,-1,+0,+1 };
	int cdy8[8] = { +0,+1,+1,+1,+0,-1,-1,-1 };
	int cdz8[8] = {  2, 3, 2, 3, 2, 3, 2, 3 };
	int cdx24[24] = { 0 };
	int cdy24[24] = { 0 };
	int cr4dx[24] = { 0 };
	int cr4dy[24] = { 0 };

	int circ_area_r1[16] = { 6,7,8,8,8,8,8,8,8,8,8,8,8,8,7,6 };
	//int circ_area_r6[56] = { 9,12,14,16,18,19,20,21,22,23,24,24,25,25,26,26,27,27,27,28,28,28,28,28,28,28,28,28,
	//						28,28,28,28,28,28,28,28,28,27,27,27,26,26,25,25,24,24,23,22,21,20,19,18,16,14,12,9 };
	int circ_area_r6[60] = { 10,13,15,17,18,20,21,22,23,24,25,25,26,27,27,28,28,29,29,29,30,30,30,30,30,30,30,30,30,30,
							30,30,30,30,30,30,30,30,30,30,29,29,29,28,28,27,27,26,25,25,24,23,22,21,20,18,17,15,13,10 };
	int circ_area_r8[72] = { 11,14,16,18,20,22,23,24,25,26,27,28,29,30,31,31,32,32,33,33,34,34,35,35,35,36,36,36,36,36,36,36,36,36,36,36,
							36,36,36,36,36,36,36,36,36,36,36,35,35,35,34,34,33,33,32,32,31,31,30,29,28,27,26,25,24,23,22,20,18,16,14,11 };
	int circ_area_r9[80] = { 11,14,17,19,21,23,25,26,27,28,29,30,31,32,33,34,34,35,35,36,36,37,37,38,38,38,39,39,39,40,40,40,40,40,40,40,40,40,40,40,
							40,40,40,40,40,40,40,40,40,40,40,39,39,39,38,38,38,37,37,36,36,35,35,34,34,33,32,31,30,29,28,27,26,25,23,21,19,17,14,11 };
	int circ_area_r11[96] = { 16,18,21,23,25,28,30,32,35,37,38,39,39,40,40,40,41,41,42,42,43,43,43,44,44,45,45,46,46,46,47,47,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,
								48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,47,47,46,46,46,45,45,44,44,43,43,43,42,42,41,41,40,40,40,39,39,38,37,35,32,30,28,25,23,21,18,16 };
	int circ_area_r12[104] = { 13,17,20,22,24,26,28,30,31,33,34,35,36,37,38,39,40,41,42,43,43,44,45,45,46,46,47,47,48,48,49,49,50,50,50,51,51,51,51,52,52,52,52,52,52,52,52,52,52,52,52,52,
								52,52,52,52,52,52,52,52,52,52,52,52,52,51,51,51,51,50,50,50,49,49,48,48,47,47,46,46,45,45,44,43,43,42,41,40,39,38,37,36,35,34,33,31,30,28,26,24,22,20,17,13 };

	int circ_area_def6[60] = { 13,16,18,20,22,23,24,25,26,27,28,28,29,29,30,30,31,31,31,32,32,32,32,32,32,32,32,32,32,32,
								32,32,32,32,32,32,32,32,32,32,32,31,31,31,30,30,29,29,28,28,27,26,25,24,23,22,20,18,16,13 };
	int circ_area_def7[68] = { 14,16,19,21,24,26,28,28,29,29,30,30,30,31,31,32,32,32,33,33,34,34,34,34,34,34,34,34,34,34,34,34,34,34,
								34,34,34,34,34,34,34,34,34,34,34,34,34,34,33,33,32,32,32,31,31,30,30,30,29,29,28,28,26,24,21,19,16,14 };

}