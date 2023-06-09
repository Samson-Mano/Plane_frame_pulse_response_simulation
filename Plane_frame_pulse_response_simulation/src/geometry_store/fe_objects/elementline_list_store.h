#pragma once
#include "nodes_list_store.h"


struct lines_store
{
	int line_id; // ID of the line
	int material_id; // Material ID of the line member
	nodes_store* startNode; // start node
	nodes_store* endNode; // end node
};


class elementline_list_store
{
public:
	int line_element_count = 1;

	elementline_list_store();
	~elementline_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_line(const int& node_id, nodes_store* startNode, nodes_store* endNode, int material_id);
	void set_buffer();
	void paint_model_lines();
	void paint_label_line_ids();
	void paint_label_line_lengths();
	void paint_lines_defl();
	int is_line_hit(glm::vec2& loc);
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale);


private:
	geom_parameters* geom_param_ptr = nullptr;
	label_list_store line_id_labels;
	label_list_store line_length_labels;
	


};
