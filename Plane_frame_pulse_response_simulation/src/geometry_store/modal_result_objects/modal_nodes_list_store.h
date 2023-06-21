#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include "../geom_parameters.h"
#include "../geometry_buffers/gBuffers.h"
#include "../geometry_objects/label_list_store.h"
#include "../geometry_objects/point_list_store.h"

struct modal_node_store
{
	int node_id = 0;
	glm::vec2 node_pt = glm::vec2(0);
	glm::vec3 node_color = glm::vec3(0);
	glm::vec2 nodal_displ = glm::vec2(0);
};

class modal_nodes_list_store
{
public:
	unsigned int node_count = 0;
	std::unordered_map<int, modal_node_store> nodeMap; // Create an unordered_map to store nodes with ID as key
	double max_modal_displacement = 0.0;

	modal_nodes_list_store();
	~modal_nodes_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_result_node(int& node_id, glm::vec2& node_pt, glm::vec2 nodal_displ);
	void set_buffer();
	void paint_modal_nodes();
	void paint_label_node_vectors();
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale);

private:
	geom_parameters* geom_param_ptr = nullptr;
	label_list_store node_vector_labels;
	point_list_store node_points;
};
