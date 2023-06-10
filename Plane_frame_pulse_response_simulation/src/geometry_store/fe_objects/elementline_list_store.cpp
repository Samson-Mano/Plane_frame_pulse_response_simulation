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

	// Set the geometry parameters for the labels (and clear the labels)
	element_lines.init(geom_param_ptr);
	line_id_labels.init(geom_param_ptr);
	line_length_labels.init(geom_param_ptr);

	// Clear the lines
	elementline_count = 0;
	elementlineMap.clear();
}

void elementline_list_store::add_elementline(int& line_id, node_store* startNode, node_store* endNode,int& material_id)
{
	// Add the line to the list
	elementline_store temp_line;
	temp_line.line_id = line_id;
	temp_line.startNode = startNode;
	temp_line.endNode = endNode;
	temp_line.material_id = material_id;

	// Check whether the node_id is already there
	if (elementlineMap.find(line_id) != elementlineMap.end())
	{
		// Node ID already exist (do not add)
		return;
	}

	// Insert to the nodes
	elementlineMap.insert({ line_id, temp_line });
	elementline_count++;

	//__________________________ Add the node points
	glm::vec3 temp_color = geom_param_ptr->geom_colors.line_color;
	glm::vec2 start_node_pt = (*startNode).node_pt;
	glm::vec2 end_node_pt = (*endNode).node_pt;

	element_lines.add_line(line_id, start_node_pt, end_node_pt,
		(*startNode).nodal_displ, (*endNode).nodal_displ,temp_color,temp_color, false);

	//__________________________ Add the node labels
	std::string temp_str = "[" + std::to_string(line_id) + "]";
	glm::vec2 line_mid_pt = (start_node_pt + end_node_pt)/2.0f;

	// Calculate the angle between the line segment and the x-axis
	float line_angle = atan2(end_node_pt.y - start_node_pt.y, end_node_pt.x - start_node_pt.x);

	line_id_labels.add_text(temp_str, line_mid_pt, glm::vec2(0), temp_color, line_angle, true, false);

	// Add the node coordinate label
	float line_length = glm::distance(start_node_pt, end_node_pt);
	temp_str = std::to_string(line_length);

	line_length_labels.add_text(temp_str, line_mid_pt, glm::vec2(0), temp_color, line_angle, false, false);
}

void elementline_list_store::set_buffer()
{
	// Set the buffers for the Model
	element_lines.set_buffer();
	line_id_labels.set_buffer();
	line_length_labels.set_buffer();
}

void elementline_list_store::paint_elementlines()
{
	// Paint the model lines
	element_lines.paint_lines();
}

void elementline_list_store::paint_label_line_ids()
{
	// Paint the line id labels
	line_id_labels.paint_text();
}

void elementline_list_store::paint_label_line_lengths()
{
	// Paint the line length labels
	line_length_labels.paint_text();
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
	// Update model openGL uniforms
	element_lines.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_zoomtranslation, set_transparency, set_deflscale);
	line_id_labels.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_zoomtranslation, set_transparency, set_deflscale);
	line_length_labels.update_opengl_uniforms(set_modelmatrix, set_pantranslation, set_zoomtranslation, set_transparency, set_deflscale);

}
