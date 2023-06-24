#include "modal_nodes_list_store.h"

modal_nodes_list_store::modal_nodes_list_store()
{
	// Empty constructor
}

modal_nodes_list_store::~modal_nodes_list_store()
{
	// Empty destructor
}

void modal_nodes_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the labels (and clear the labels)
	node_points.init(geom_param_ptr);
	node_vector_labels.init(geom_param_ptr);

	// Clear the nodes
	node_count = 0;
	modal_nodeMap.clear();
	max_node_displ.clear();
}

void modal_nodes_list_store::clear_data()
{
	// Clear the results
	node_count = 0;
	modal_nodeMap.clear();
	max_node_displ.clear();
}

void modal_nodes_list_store::add_result_node(int& node_id, glm::vec2& node_pt, std::unordered_map<int, glm::vec3> node_modal_displ)
{
	// Add result nodes

}

void modal_nodes_list_store::set_buffer()
{


}

void modal_nodes_list_store::paint_modal_nodes()
{
}

void modal_nodes_list_store::paint_label_node_vectors()
{
}

void modal_nodes_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
}
