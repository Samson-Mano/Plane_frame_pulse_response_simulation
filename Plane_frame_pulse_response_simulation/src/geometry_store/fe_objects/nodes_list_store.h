#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include "../geom_parameters.h"
#include "../geometry_buffers/gBuffers.h"
#include "../geometry_objects/label_list_store.h"
#include "../geometry_objects/point_list_store.h"

struct nodes_store
{
	int node_id = 0;
	glm::vec2 node_pt = glm::vec2(0);
	glm::vec3 node_color = glm::vec3(0);
	glm::vec2 nodal_displ = glm::vec2(0);
	glm::vec2 nodal_reaction = glm::vec2(0);
	double nodal_reaction_angle = 0;
};

class nodes_list_store
{
public:
	unsigned int node_count = 0;
	std::unordered_map<int, nodes_store> nodeMap; // Create an unordered_map to store nodes with ID as key
	double max_displacement = 0.0;
	double max_reaction_force = 0.0;

	nodes_list_store();
	~nodes_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_node(const int& node_id, const glm::vec2& node_pt);
	void set_buffer();
	void paint_model_nodes();
	void paint_label_node_ids();
	void paint_label_node_coords();
	void paint_nodes_defl();
	void paint_label_nodes_defl_values();
	int is_node_hit(glm::vec2& loc);
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale);

private:
	geom_parameters* geom_param_ptr = nullptr;
	label_list_store node_id_labels;
	label_list_store node_coord_labels;
	// label_list_store node_displ_labels;
	point_list_store node_points;
};
