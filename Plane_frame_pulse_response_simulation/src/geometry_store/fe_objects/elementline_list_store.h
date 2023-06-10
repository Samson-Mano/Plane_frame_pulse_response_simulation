#pragma once
#include "nodes_list_store.h"
#include "../geometry_objects/line_list_store.h"

struct elementline_store
{
	int line_id; // ID of the line
	int material_id; // Material ID of the line member
	node_store* startNode; // start node
	node_store* endNode; // end node
};


class elementline_list_store
{
public:
	unsigned int elementline_count = 0;
	std::unordered_map<int, elementline_store> elementlineMap; // Create an unordered_map to store nodes with ID as key

	elementline_list_store();
	~elementline_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_elementline(int& line_id, node_store* startNode, node_store* endNode, int& material_id);
	void set_buffer();
	void paint_elementlines();
	void paint_label_line_ids();
	void paint_label_line_lengths();
	void paint_lines_defl();
	int is_line_hit(glm::vec2& loc);
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale);


private:
	geom_parameters* geom_param_ptr = nullptr;
	line_list_store element_lines;
	label_list_store line_id_labels;
	label_list_store line_length_labels;



};
