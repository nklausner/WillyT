#include "MapInfluence.h"

void set_influence_a(int(&my_map)[256][256], BWAPI::TilePosition my_tile, int dz) {
	int my_arr[16] = { 3,6,7,7,7,8,8,8,8,8,8,7,7,7,6,3 };
	int ymin = my_tile.y - 7;
	for (int i = 0; i < 16; ++i) {
		if (ymin + i >= 0 && ymin + i < 256) {
			set_influence_line_even(my_map, ymin + i, my_tile.x, my_arr[i], dz);
		}
	}
}
void set_influence_m(int(&my_map)[256][256], BWAPI::TilePosition my_tile, int dz) {
	int my_arr[18] = { 3,5,6,7,8,8,9,9,9,9,9,9,8,8,7,6,5,3 };
	int ymin = my_tile.y - 8;
	for (int i = 0; i < 18; ++i) {
		if (ymin + i >= 0 && ymin + i < 256) {
			set_influence_line_even(my_map, ymin + i, my_tile.x, my_arr[i], dz);
		}
	}
}
void set_influence_l(int(&my_map)[256][256], BWAPI::TilePosition my_tile, int dz) {
	int my_arr[19] = { 4,6,7,8,9,9,10,10,10,10,10,10,10,9,9,8,7,6,4 };
	int ymin = my_tile.y - 10;
	for (int i = 0; i < 19; ++i) {
		if (ymin + i >= 0 && ymin + i < 256) {
			set_influence_line_even(my_map, ymin + i, my_tile.x + 1, my_arr[i], dz);
		}
	}
}
void set_influence_t(bool(&my_map)[256][256], BWAPI::TilePosition my_tile) {
	int my_arr[25] = { 5,7,9,10,10,11,11,12,12,12,12,12,12,12,12,12,12,12,11,11,10,10,9,7,5 };
	int ymin = my_tile.y - 12;
	for (int i = 0; i < 25; ++i) {
		if (ymin + i >= 0 && ymin + i < 256) {
			set_influence_line_odd(my_map, ymin + i, my_tile.x, my_arr[i], true);
		}
	}
}


void set_influence_line_even(int(&my_map)[256][256], int y, int x0, int dx, int dz) {
	for (int x = x0 - dx + 1; x < x0 + dx + 1; x++) {
		if (x >= 0 && x < 256) {
			my_map[y][x] = my_map[y][x] + dz;
		}
	}
}
void set_influence_line_odd(bool(&my_map)[256][256], int y, int x0, int dx, bool b) {
	for (int x = x0 - dx; x < x0 + dx + 1; x++) {
		if (x >= 0 && x < 256) {
			my_map[y][x] = b;
		}
	}
}


//int my_arr[23] = { 4,6,7,8,9,10,11,11,12,12,12,12,12,12,12,11,11,10,9,8,7,6,4 };
//int my_arr[22] = { 3,5,7,8,9,9,10,10,11,11,11,11,11,11,10,10,9,9,8,7,5,3 };


void set_influence(int(&my_map)[1024][1024], int x0, int y0, int size, int dz) {
	if (x0 < 0 || x0 >= wilmap::wp ||
		y0 < 0 || x0 >= wilmap::hp) { return; }

	switch(size)
	{
	case 16: set_influence(my_map, x0, y0, 16, wilmap::circ_area_r1, dz); break;	//storm
	case 56: set_influence(my_map, x0, y0, 60, wilmap::circ_area_r6, dz); break;	//lurk
	case 72: set_influence(my_map, x0, y0, 72, wilmap::circ_area_r8, dz); break;	//nuke
	case 104: set_influence(my_map, x0, y0, 104, wilmap::circ_area_r12, dz); break;	//siege
	case 60: set_influence(my_map, x0, y0, 60, wilmap::circ_area_def6, dz); break;	//bunker
	case 68: set_influence(my_map, x0, y0, 68, wilmap::circ_area_def7, dz); break;	//other static defense
	case 96: set_influence(my_map, x0, y0, 96, wilmap::circ_area_r11, dz); break;	//detector sight range
	default: break;
	}
	return;
}
void set_influence(int(&my_map)[1024][1024], int x0, int y0, int size, int my_arr[], int dz) {
	int xmid = x0 / 8;
	int ymid = y0 / 8;
	int ymin = ymid - size / 2;
	int ymax = ymid + size / 2;
	int i = 0;
	if (ymin < 0) { i = -ymin; ymin = 0; }
	if (ymax > 1024) { ymax = 1024; }

	for (int y = ymin; y < ymax; ++y) {
		set_influence_line_even(my_map, y, xmid, my_arr[i], dz);
		i++;
	}
}
void set_influence(int(&my_map)[1024][1024], BWAPI::Position p, int size, int my_arr[], int dz) {
	set_influence(my_map, p.x, p.y, size, my_arr, dz);
}


void set_influence_line_even(int(&my_map)[1024][1024], int y, int x0, int dx, int dz) {
	int xmin = x0 - dx;
	int xmax = x0 + dx;
	if (xmin < 0)	{ xmin = 0; }
	if (xmax > 1024) { xmax = 1024; }

	for (int x = xmin; x < xmax; x++) { my_map[y][x] += dz; }
}
void set_influence_line_odd(int(&my_map)[1024][1024], int y, int x0, int dx, int dz) {
	int xmin = x0 - dx;
	int xmax = x0 + dx + 1;
	if (xmin < 0) { xmin = 0; }
	if (xmax > 1024) { xmax = 1024; }

	for (int x = xmin; x < xmax; x++) { my_map[y][x] += dz; }
}