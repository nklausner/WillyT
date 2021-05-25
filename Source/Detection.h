#pragma once
#include <BWAPI.h>
#include "WillytBuild.h"
#include "WillytUnits.h"
#include "Utilities.h"

//2018-03-18: detection

struct Detection
{
public:
	void check();

private:
	BWAPI::Unit get_max_comsat();
	BWAPI::Unit get_invis_unit();
	bool check_units_near_grd(BWAPI::Unit my_unit);
	bool check_units_near_air(BWAPI::Unit my_unit);
};