#include "elementline_list_store.h"

elementline_list_store::elementline_list_store()
{
	// Empty constructor
}

elementline_list_store::~elementline_list_store()
{
	// Empty destructor
}

void elementline_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

}

void elementline_list_store::add_line(const int& node_id, nodes_store* startNode, nodes_store* endNode, int material_id)
{
}

void elementline_list_store::set_buffer()
{
}

void elementline_list_store::paint_model_lines()
{
}

void elementline_list_store::paint_label_line_ids()
{
}

void elementline_list_store::paint_label_line_lengths()
{
}

void elementline_list_store::paint_lines_defl()
{
}

int elementline_list_store::is_line_hit(glm::vec2& loc)
{
	return 0;
}

void elementline_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
}
