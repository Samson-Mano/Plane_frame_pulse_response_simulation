#pragma once
#include "modal_nodes_list_store.h"
#include "../geometry_objects/line_list_store.h"


struct modal_line_points
{
	int split_line_id = 0; // line id of the individual hermitian interpolation line 
	// Point coordinate
	glm::vec2 pt1 = glm::vec2(0);
	glm::vec2 pt2 = glm::vec2(0);

	// Point displacements
	std::unordered_map<int, glm::vec2> pt1_modal_displ;
	std::unordered_map<int, glm::vec2> pt2_modal_displ;
};


struct modal_elementline_store
{
	int line_id = 0; // ID of the line
	modal_node_store* startNode = nullptr; // start node
	modal_node_store* endNode = nullptr; // end node

	// Line modal displacement data
	std::vector<modal_line_points> hermite_line_data;
};

class modal_elementline_list_store
{
public:
	unsigned int modal_elementline_count = 0;
	std::unordered_map<int, modal_elementline_store> modal_elementlineMap; // Create an unordered_map to store nodes with ID as key

	modal_elementline_list_store();
	~modal_elementline_list_store();
	void init(geom_parameters* geom_param_ptr);
	void clear_data();
	void add_modal_elementline(int& line_id, modal_node_store* startNode, modal_node_store* endNode);
	std::vector<modal_line_points> set_line_hermite_interpolation(const int& interpolation_count, modal_node_store* startNode, modal_node_store* endNode);
	void set_buffer();
	void paint_modal_elementlines();
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	geom_parameters* geom_param_ptr = nullptr;
	line_list_store element_lines;
};
