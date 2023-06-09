#include "nodes_list_store.h"

nodes_list_store::nodes_list_store()
{
	// Empty constructor
}

nodes_list_store::~nodes_list_store()
{
	// Empty destructor
}

void nodes_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the labels (and clear the labels)
	node_points.init(geom_param_ptr);
	node_id_labels.init(geom_param_ptr);
	node_coord_labels.init(geom_param_ptr);
	// node_displ_labels.init(geom_param_ptr);

	// Clear the nodes
	node_count = 0;
	nodeMap.clear();
}

void nodes_list_store::add_node(const int& node_id, const glm::vec2& node_pt)
{
	// Add the node to the list
	nodes_store temp_node;
	temp_node.node_id = node_id;
	temp_node.node_pt = node_pt;
	temp_node.node_color = geom_param_ptr->geom_colors.node_color;
	temp_node.nodal_displ = glm::vec2(0);
	temp_node.nodal_reaction = glm::vec2(0);

	// Check whether the node_id is already there
	if (nodeMap.find(node_id) != nodeMap.end())
	{
		// Node ID already exist (do not add)
		return;
	}

	// Insert to the nodes
	nodeMap.insert({ node_id, temp_node });
	node_count++;

	//__________________________ Add the node points
	glm::vec3 temp_color = geom_param_ptr->geom_colors.node_color;
	glm::vec2 node_pt_offset = glm::vec2(0);

	node_points.add_point(node_id, node_pt, node_pt_offset, temp_color,false);

	//__________________________ Add the node labels
	std::string temp_str = std::to_string(node_id);

	node_id_labels.add_text(temp_str.c_str(), node_pt, glm::vec2(0), temp_color, 0.0f, true,false);

	// Add the node coordinate label
	temp_str = "(" + std::to_string(node_pt.x) + ", " + std::to_string(node_pt.y) + ")";

	node_coord_labels.add_text(temp_str.c_str(), node_pt, glm::vec2(0), temp_color, 0.0f, false,false);
}

void nodes_list_store::set_buffer()
{
	// Set the buffers for the Model
	node_points.set_buffer();
	node_id_labels.set_buffer();
	node_coord_labels.set_buffer();
}


void nodes_list_store::paint_model_nodes()
{
	// Paint the model nodes
	node_points.paint_nodes();
}

void nodes_list_store::paint_label_node_ids()
{
	// Paint the node id labels
	node_id_labels.paint_text();
}

void nodes_list_store::paint_label_node_coords()
{
	// Paint the node coordinate labels
	node_coord_labels.paint_text();
}

void nodes_list_store::paint_nodes_defl()
{
}

void nodes_list_store::paint_label_nodes_defl_values()
{
}

int nodes_list_store::is_node_hit(glm::vec2& loc)
{
	return 0;
}

void nodes_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	// Update model openGL uniforms
	node_points.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_zoomtranslation, set_transparency, set_deflscale);
	node_id_labels.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_zoomtranslation, set_transparency, set_deflscale);
	node_coord_labels.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_zoomtranslation, set_transparency, set_deflscale);
}
