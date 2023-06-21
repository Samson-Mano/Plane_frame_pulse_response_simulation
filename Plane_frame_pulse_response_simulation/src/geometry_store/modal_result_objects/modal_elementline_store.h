#pragma once
#include "modal_nodes_list_store.h"
#include "../geometry_objects/line_list_store.h"


struct modal_elementline_store
{
	int line_id; // ID of the line
	int material_id; // Material ID of the line member
	modal_node_store* startNode; // start node
	modal_node_store* endNode; // end node
};

class modal_elementline_list_store
{
public:
	unsigned int elementline_count = 0;
	std::unordered_map<int, modal_elementline_store> elementlineMap; // Create an unordered_map to store nodes with ID as key

	modal_elementline_list_store();
	~modal_elementline_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_modal_elementline(int& line_id, modal_node_store* startNode, modal_node_store* endNode, int& material_id);
	void set_buffer();
	void paint_modal_elementlines();
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	geom_parameters* geom_param_ptr = nullptr;
	line_list_store element_lines;
};
