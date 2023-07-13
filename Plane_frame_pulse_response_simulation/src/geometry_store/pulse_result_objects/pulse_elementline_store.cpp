#include "pulse_elementline_store.h"

pulse_elementline_list_store::pulse_elementline_list_store()
{
	// Empty constructor
}

pulse_elementline_list_store::~pulse_elementline_list_store()
{
	// Empty destructor
}

void pulse_elementline_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Clear the element lines
	pulse_elementline_count = 0;
	pulse_elementlineMap.clear();
}

void pulse_elementline_list_store::clear_data()
{
	// Clear the results
	pulse_elementline_count = 0;
	pulse_elementlineMap.clear();
}

void pulse_elementline_list_store::add_pulse_elementline(int& line_id, pulse_node_store* startNode, pulse_node_store* endNode)
{
	// Add result line element
	pulse_elementline_store temp_line;
	temp_line.line_id = line_id;
	temp_line.startNode = startNode;
	temp_line.endNode = endNode;

	// Check whether the node_id is already there
	if (pulse_elementlineMap.find(line_id) != pulse_elementlineMap.end())
	{
		// Element ID already exist (do not add)
		return;
	}

	//__________________________ Add Hermite interpolation for Beam Element
	temp_line.hermite_line_data = set_line_hermite_interpolation(interpolation_count, startNode, endNode);

	// Insert to the lines
	pulse_elementlineMap.insert({ line_id, temp_line });
	pulse_elementline_count++;
}


std::vector<pulse_line_points> pulse_elementline_list_store::set_line_hermite_interpolation(const int& interpolation_count,
	pulse_node_store* startNode, 
	pulse_node_store* endNode)
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

	// Return varible
	std::vector<pulse_line_points> hermite_line_data;

	// Create the interpolation inbetween the start and end point
	for (int i = 0; i < interpolation_count; i++)
	{
		// interpolation line id
		int e_line_id = (pulse_elementline_count * interpolation_count) + i;

		// get the end points of the split line pt1
		double t_ratio1 = static_cast<double>(i) / static_cast<double>(interpolation_count);

		glm::vec2 pt1 = glm::vec2(start_node_pt.x * (1 - t_ratio1) + end_node_pt.x * t_ratio1,
			start_node_pt.y * (1 - t_ratio1) + end_node_pt.y * t_ratio1);

		// get the end points of the split line pt2
		double t_ratio2 = static_cast<double>(i + 1) / static_cast<double>(interpolation_count);

		glm::vec2 pt2 = glm::vec2(start_node_pt.x * (1 - t_ratio2) + end_node_pt.x * t_ratio2,
			start_node_pt.y * (1 - t_ratio2) + end_node_pt.y * t_ratio2);

		//_____________________________________________________________________________________________
		//_____________________________________________________________________________________________
		int num_of_time_steps = (*startNode).number_of_timesteps;

		// Find the pt modal displacement for every individual mode
		std::unordered_map<int, glm::vec2> pt1_modal_displ;
		std::unordered_map<int, glm::vec2> pt2_modal_displ;

		// get the end displacements of every individual nodes
		for (int j = 0; j < num_of_time_steps; j++)
		{
			// Get the displacement at start point
			glm::vec3 start_node_displ = (*startNode).node_pulse_result.node_pulse_displ[j];
			glm::vec3 end_node_displ = (*endNode).node_pulse_result.node_pulse_displ[j];

			// Start point displacement at local axis
			glm::vec2 local_displ_start_node = glm::vec2(((start_node_displ.x * Lcos) + (start_node_displ.y * Msin)),
				((start_node_displ.x * (-1 * Msin)) + (start_node_displ.y * Lcos)));

			// End point displacement at local axis
			glm::vec2 local_displ_end_node = glm::vec2(((end_node_displ.x * Lcos) + (end_node_displ.y * Msin)),
				((end_node_displ.x * (-1 * Msin)) + (end_node_displ.y * Lcos)));

			//_____________________________________________________________________________________________
			// Find the interpolation of the displacements at pt1
			glm::vec2 local_displ_pt1 = glm::vec2(linear_bar_element_interpolation(local_displ_start_node.x, local_displ_end_node.x, t_ratio1),
				hermite_beam_element_interpolation(local_displ_start_node.y, start_node_displ.z, local_displ_end_node.y, end_node_displ.z, t_ratio1));

			// Find the interpolation of the displacements at pt2
			glm::vec2 local_displ_pt2 = glm::vec2(linear_bar_element_interpolation(local_displ_start_node.x, local_displ_end_node.x, t_ratio2),
				hermite_beam_element_interpolation(local_displ_start_node.y, start_node_displ.z, local_displ_end_node.y, end_node_displ.z, t_ratio2));

			// Transform from local to global
			glm::vec2 global_displ_pt1 = glm::vec2(((local_displ_pt1.x * Lcos) + (local_displ_pt1.y * (-1 * Msin))),
				((local_displ_pt1.x * Msin) + (local_displ_pt1.y * Lcos)));

			glm::vec2 global_displ_pt2 = glm::vec2(((local_displ_pt2.x * Lcos) + (local_displ_pt2.y * (-1 * Msin))),
				((local_displ_pt2.x * Msin) + (local_displ_pt2.y * Lcos)));

			//__________________________________________________________________________________________________
			// Add to the list
			pt1_modal_displ.insert({ j,global_displ_pt1 });
			pt2_modal_displ.insert({ j,global_displ_pt2 });
		}

		// Add to the line list
		pulse_line_points temp_pulse_line;
		temp_pulse_line.split_line_id = e_line_id;
		temp_pulse_line.pt1 = pt1;
		temp_pulse_line.pt2 = pt2;
		temp_pulse_line.pt1_modal_displ = pt1_modal_displ;
		temp_pulse_line.pt2_modal_displ = pt2_modal_displ;

		// Add to the return variable
		hermite_line_data.push_back(temp_pulse_line);
	}

	return hermite_line_data;
}

double pulse_elementline_list_store::linear_bar_element_interpolation(double q1, double q2, double s)
{
	// Linear bar element interpolation (based on end displacements)
	return (((1 - s) * q1) + (s * q2));
}

double pulse_elementline_list_store::hermite_beam_element_interpolation(double v1, double theta1, double v2, double theta2, double s)
{
	// Hermite beam element interpolation (based on end displacement and rotation)
	double N1 = 1 - (3 * s * s) + (2 * s * s * s);
	double N2 = s - (2 * s * s) + (s * s * s);
	double N3 = (3 * s * s) - (2 * s * s * s);
	double N4 = (-s * s) + (s * s * s);

	return ((N1 * v1) + (N2 * theta1) + (N3 * v2) + (N4 * theta2));
}