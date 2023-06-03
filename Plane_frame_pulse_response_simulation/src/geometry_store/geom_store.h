#pragma once
#include "geom_parameters.h"
#include "../tool_window/constraint_window.h"
#include "../tool_window/load_window.h"
#include "../tool_window/material_window.h"
#include "../tool_window/options_window.h"
#include "../tool_window/solver_window.h"

class geom_store
{
public:
	bool is_geometry_set = false;

	// Main Variable to strore the geometry parameters
	geom_parameters geom_param;


	geom_store();
	~geom_store();
	void init();
	void fini();
	void updateWindowDimension(const int& window_width, const int& window_height);
private:
};
