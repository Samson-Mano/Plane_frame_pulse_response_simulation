#include "modal_elementline_store.h"

modal_elementline_list_store::modal_elementline_list_store()
{
	// Empty constructor
}

modal_elementline_list_store::~modal_elementline_list_store()
{
	// Empty destructor
}

void modal_elementline_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Set the geometry parameters for the labels (and clear the labels)
	element_lines.init(geom_param_ptr);

	// Clear the element lines
	modal_elementline_count = 0;
	modal_elementlineMap.clear();
}

void  modal_elementline_list_store::clear_data()
{
	// Clear the element lines
	modal_elementline_count = 0;
	modal_elementlineMap.clear();
}

void modal_elementline_list_store::add_modal_elementline(int& line_id, modal_node_store* startNode, modal_node_store* endNode)
{
	// Add result line element
	modal_elementline_store temp_line;
	temp_line.line_id = line_id;
	temp_line.startNode = startNode;
	temp_line.endNode = endNode;


	// Check whether the node_id is already there
	if (modal_elementlineMap.find(line_id) != modal_elementlineMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	//__________________________ Add Hermite interpolation for Beam Element
	int interpolation_count = 10;
	temp_line.hermite_line_data = set_line_hermite_interpolation(interpolation_count, startNode, endNode);


	// Insert to the lines
	modal_elementlineMap.insert({ line_id, temp_line });
	modal_elementline_count++;


	//glm::vec3 temp_color = geom_param_ptr->geom_colors.line_color;


	//glm::vec2 start_node_pt = (*startNode).node_pt;
	//glm::vec2 end_node_pt = (*endNode).node_pt;

	////__________________________ Add the lines
	//element_lines.add_line(line_id, start_node_pt, end_node_pt,
	//	glm::vec2(0), glm::vec2(0), temp_color, temp_color, false);



}

std::vector<modal_line_points> modal_elementline_list_store::set_line_hermite_interpolation(const int& interpolation_count, modal_node_store* startNode, modal_node_store* endNode)
{
	// get the start and end point
	glm::vec2 start_node_pt = (*startNode).node_pt;
	glm::vec2 end_node_pt = (*endNode).node_pt;

	// Prepare the transformation matrix
		// Compute the differences in x and y coordinates
	double dx = end_node_pt.x - start_node_pt.x;
	double dy = -1.0 * (end_node_pt.y - start_node_pt.y);

	// Compute the length of the frame element
	double eLength = std::sqrt((dx * dx) + (dy * dy));

	// Compute the direction cosines
	double Lcos = (dx / eLength);
	double Msin = (dy / eLength);



	// Create the interpolation inbetween the start and end point
	for (int i = 0; i < (interpolation_count - 1); i++)
	{
		// interpolation line id
		int e_line_id = (modal_elementline_count * interpolation_count) + i;

		// get the end points of the split line pt1
		double t_ratio1 = static_cast<double>(i) / static_cast<double>(interpolation_count);

		glm::vec2 pt1 = glm::vec2(start_node_pt.x * (1 - t_ratio1) + end_node_pt.x * t_ratio1,
			start_node_pt.y * (1 - t_ratio1) + end_node_pt.y * t_ratio1);

		// get the end points of the split line pt2
		double t_ratio2 = static_cast<double>(i + 1) / static_cast<double>(interpolation_count);

		glm::vec2 pt2 = glm::vec2(start_node_pt.x * (1 - t_ratio2) + end_node_pt.x * t_ratio2,
			start_node_pt.y * (1 - t_ratio2) + end_node_pt.y * t_ratio2);

		//_____________________________________________________________________________________________
		int num_of_modes = static_cast<int>((*startNode).node_modal_displ.size());

		// get the end displacements of every individual nodes
		for (int j = 0; j < num_of_modes; j++)
		{
			// Get the displacement at start point
			glm::vec3 start_node_displ = (*startNode).node_modal_displ[j];
			glm::vec3 end_node_displ = (*endNode).node_modal_displ[j];



		}




	}

}


void modal_elementline_list_store::set_buffer()
{
}

void modal_elementline_list_store::paint_modal_elementlines()
{
}

void modal_elementline_list_store::update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
}
