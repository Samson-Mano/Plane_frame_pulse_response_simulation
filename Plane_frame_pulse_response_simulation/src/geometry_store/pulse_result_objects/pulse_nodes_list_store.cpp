#include "pulse_nodes_list_store.h"

pulse_nodes_list_store::pulse_nodes_list_store()
{
	// Empty constructor
}

pulse_nodes_list_store::~pulse_nodes_list_store()
{
	// Empty destructor
}

void pulse_nodes_list_store::init(geom_parameters* geom_param_ptr)
{
	// Initialize
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Clear the results
	node_count = 0;
	pulse_nodeMap.clear();
	max_node_displ = 0.0; // Maximum nodal displacement
}

void pulse_nodes_list_store::clear_data()
{
	// Clear the results
	node_count = 0;
	pulse_nodeMap.clear();
	max_node_displ = 0.0; // Maximum nodal displacement
}

void pulse_nodes_list_store::add_result_node(int& node_id, glm::vec2& node_pt, pulse_node_result node_pulse_result, const int& number_of_time_steps)
{
	// Add the result nodes
	pulse_node_store temp_pulse_result;

	temp_pulse_result.node_id = node_id; // Add the node ID
	temp_pulse_result.node_pt = node_pt; // Add the node point
	temp_pulse_result.node_pulse_result = node_pulse_result; // Node point results
	temp_pulse_result.number_of_timesteps = number_of_time_steps; // Number of time steps

	// Add to the pulse nodeMap
	pulse_nodeMap.insert({ 0,temp_pulse_result });
}



glm::vec3 pulse_nodes_list_store::getContourColor(float value)
{
	// return the contour color based on the value (0 to 1)
	glm::vec3 color;
	float r, g, b;

	// Rainbow color map
	float hue = value * 5.0f; // Scale the value to the range of 0 to 5
	float c = 1.0f;
	float x = c * (1.0f - glm::abs(glm::mod(hue / 2.0f, 1.0f) - 1.0f));
	float m = 0.0f;

	if (hue >= 0 && hue < 1) {
		r = c;
		g = x;
		b = m;
	}
	else if (hue >= 1 && hue < 2) {
		r = x;
		g = c;
		b = m;
	}
	else if (hue >= 2 && hue < 3) {
		r = m;
		g = c;
		b = x;
	}
	else if (hue >= 3 && hue < 4) {
		r = m;
		g = x;
		b = c;
	}
	else {
		r = x;
		g = m;
		b = c;
	}

	color = glm::vec3(r, g, b);
	return color;
}