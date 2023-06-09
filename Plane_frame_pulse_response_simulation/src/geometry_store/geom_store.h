#pragma once
#include "geom_parameters.h"
// File system
#include <fstream>
#include <sstream>

// Window includes
#include "../tool_window/constraint_window.h"
#include "../tool_window/load_window.h"
#include "../tool_window/material_window.h"
#include "../tool_window/options_window.h"
#include "../tool_window/solver_window.h"
// FE Objects
#include "fe_objects/nodes_list_store.h"
#include "fe_objects/elementline_list_store.h"
#include "fe_objects/nodeconstraint_list_store.h"
#include "fe_objects/nodeload_list_store.h"


class geom_store
{
public:
	bool is_geometry_set = false;

	// Main Variable to strore the geometry parameters
	geom_parameters geom_param;

	geom_store();
	~geom_store();
	void init(options_window* op_window, material_window* mat_window, solver_window* sol_window);
	void fini();

	// Reading and writing the geometry file
	void read_varai2d(std::ifstream& input_file);
	void read_rawdata(std::ifstream& input_file);
	void write_rawdata(std::ofstream& output_file);

	// Functions to control the origin
	void update_WindowDimension(const int& window_width, const int& window_height);
	void update_model_matrix();
	void update_model_zoomfit();
	void update_model_pan(glm::vec2& transl);
	void update_model_zoom(double& z_scale);
	void update_model_transperency(bool is_transparent);

	// Functions to paint the geometry and results
	void paint_geometry();
private:
	// main variables to store the geometry (Nodes, lines, loads, supports)
	// Geometry objects
	nodes_list_store model_nodes;
	elementline_list_store model_lineelements;
	nodeconstraint_list_store model_constarints;
	nodeload_list_store model_loads;

	// View options ptr and Material window ptr
	options_window* op_window = nullptr;
	material_window* mat_window = nullptr;
	solver_window* sol_window = nullptr;


	// Create geometry private functions
	void create_geometry(nodes_list_store& model_nodes,
		elementline_list_store& model_lineelements,
		nodeconstraint_list_store& model_constarints,
		nodeload_list_store& model_loads);

	std::pair<glm::vec2, glm::vec2> findMinMaxXY(const std::unordered_map<int, nodes_store>& model_nodes);
	glm::vec2 findGeometricCenter(const std::unordered_map<int, nodes_store>& model_nodes);

};
