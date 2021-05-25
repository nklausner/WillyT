#pragma once
#include <BWAPI.h>
#include "WillytState.h"
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "WillytEnemy.h"
#include "WillytMap.h"
#include "WillytExpo.h"
#include "Utilities.h"

//2018-03-18: detection
//2020-02-16: reorganize to comsat manager

struct ComsatManager
{
public:
	void check();
	void check_unscouted();

private:
	void detect(BWAPI::Unit my_coms, BWAPI::Unit my_unit);
	void scan_starts(BWAPI::Unit my_coms);
	void scan_blocks(BWAPI::Unit my_coms);
	bool scan_random_area(BWAPI::Unit my_coms);
	bool scan_random_expo(BWAPI::Unit my_coms);

	unsigned get_scan_count();
	BWAPI::Unit get_max_comsat();
	BWAPI::Unit get_invis_unit();
	bool check_units_near_grd(BWAPI::Unit my_unit);
	bool check_units_near_air(BWAPI::Unit my_unit);
	bool print_debug = false;
};