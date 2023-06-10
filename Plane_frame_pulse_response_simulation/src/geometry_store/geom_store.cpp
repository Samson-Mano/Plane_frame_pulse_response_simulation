#include "geom_store.h"

geom_store::geom_store()
{
	// Empty constructor
}

geom_store::~geom_store()
{
	// Empty Destructor
}

void geom_store::init(options_window* op_window, material_window* mat_window, solver_window* sol_window)
{
	// Initialize
	// Initialize the geometry parameters
	geom_param.init();

	is_geometry_set = false;

	// Initialize the model nodes and lines
	 model_nodes.init(&geom_param);
	 model_lineelements.init(&geom_param);
	 model_constarints.init(&geom_param);
	 model_loads.init(&geom_param);

	// Add the window pointers
	this->op_window = op_window;
	this->mat_window = mat_window;
	this->sol_window = sol_window;

}

void geom_store::fini()
{
	// Deinitialize
	is_geometry_set = false;

}

void geom_store::read_varai2d(std::ifstream& input_file)
{
	// Read the varai2D
// Read the entire file into a string
	std::string file_contents((std::istreambuf_iterator<char>(input_file)),
		std::istreambuf_iterator<char>());

	// Split the string into lines
	std::istringstream iss(file_contents);
	std::string line;
	std::vector<std::string> lines;
	while (std::getline(iss, line))
	{
		lines.push_back(line);
	}

	int j = 0, i = 0;


	// Create a temporary variable to store the nodes
	nodes_list_store model_nodes;
	model_nodes.init(&geom_param);

	// Create a temporary variable to store the lines
	elementline_list_store model_lineelements;
	model_lineelements.init(&geom_param);

	// Process the lines
	while (j < lines.size())
	{
		std::cout << "Line: " << lines[j] << std::endl;
		// Check for the start of nodes input
		if (lines[j].find("[+] End Points") != std::string::npos)
		{
			int num_nodes;
			// Read the number of nodes
			std::stringstream ss(lines[j]);
			std::string token;
			std::getline(ss, token, ','); // Get the string "[+] End Points"
			std::getline(ss, token, ','); // Get the number of nodes as a string
			num_nodes = std::stoi(token) + j; // Convert the string to an integer

			// Read and store the nodes
			for (i = j; i < num_nodes; i++)
			{
				int node_id;
				double x, y;

				std::stringstream ss(lines[i + 1]);
				std::string token;

				std::getline(ss, token, ','); // read the node ID
				node_id = std::stoi(token);

				std::getline(ss, token, ','); // read the x-coordinate
				x = std::stod(token);

				std::getline(ss, token, ','); // read the y-coordinate
				y = std::stod(token);

				// Add to node store list
				glm::vec2 node_pt = glm::vec2(x, y);
				model_nodes.add_node(node_id,node_pt);

				j++;
			}
		}
		// Check for the start of lines input
		else if (lines[j].find("[+] Lines") != std::string::npos) {
			int num_lines;
			// Read the number of nodes
			std::stringstream ss(lines[j]);
			std::string token;
			std::getline(ss, token, ','); // Get the string "[+] Lines"
			std::getline(ss, token, ','); // Get the number of nodes as a string
			num_lines = std::stoi(token) + j; // Convert the string to an integer

			// Read and store the lines
			for (i = j; i < num_lines; i++)
			{
				int line_id, start_node_id, end_node_id;
				std::stringstream ss(lines[i + 1]);
				std::string token;

				std::getline(ss, token, ','); // read the line ID
				line_id = std::stoi(token);

				std::getline(ss, token, ','); // read the start node ID
				start_node_id = std::stoi(token);

				std::getline(ss, token, ','); // read the end node ID
				end_node_id = std::stoi(token);

				// Create lines_store object using references to startNode and endNode
				int mat_id = 0;
				model_lineelements.add_elementline(line_id, &model_nodes.nodeMap[start_node_id], &model_nodes.nodeMap[end_node_id], mat_id);

				j++;
			}
		}

		// iterate line
		j++;
	}

	if (model_nodes.node_count < 1 || model_lineelements.elementline_count < 1)
	{
		// No elements added
		return;
	}

	nodeconstraint_list_store model_constarints;
	model_constarints.init(&geom_param);

	nodeload_list_store model_loads;
	model_loads.init(&geom_param);

	// Re-instantitize geom_store object using the nodeMap and lineMap
	create_geometry(model_nodes, model_lineelements, model_constarints, model_loads);

}

void geom_store::read_rawdata(std::ifstream& input_file)
{
}

void geom_store::write_rawdata(std::ofstream& output_file)
{
}


void geom_store::update_WindowDimension(const int& window_width, const int& window_height)
{
	// Update the window dimension
	this->geom_param.window_width = window_width;
	this->geom_param.window_height = window_height;

	if (is_geometry_set == true)
	{
		// Update the model matrix
		update_model_matrix();
		// !! Zoom to fit operation during window resize is handled in mouse event class !!
	}
}

void geom_store::update_model_matrix()
{
	// Set the model matrix for the model shader
	// Find the scale of the model (with 0.9 being the maximum used)
	int max_dim = geom_param.window_width > geom_param.window_height ? geom_param.window_width : geom_param.window_height;

	double normalized_screen_width = 1.8f * (static_cast<double>(geom_param.window_width) / static_cast<double>(max_dim));
	double normalized_screen_height = 1.8f * (static_cast<double>(geom_param.window_height) / static_cast<double>(max_dim));


	geom_param.geom_scale = std::min(normalized_screen_width / geom_param.geom_bound.x,
		normalized_screen_height / geom_param.geom_bound.y);

	// Translation
	glm::vec3 geom_translation = glm::vec3(-1.0f * (geom_param.max_b.x + geom_param.min_b.x) * 0.5f * geom_param.geom_scale,
		-1.0f * (geom_param.max_b.y + geom_param.min_b.y) * 0.5f * geom_param.geom_scale,
		0.0f);

	glm::mat4 g_transl = glm::translate(glm::mat4(1.0f), geom_translation);

	geom_param.modelMatrix = g_transl * glm::scale(glm::mat4(1.0f), glm::vec3(geom_param.geom_scale));

	// Update the model matrix
	model_nodes.update_geometry_matrices(true, false, false, false, false);
	model_lineelements.update_geometry_matrices(true, false, false, false, false);
	model_constarints.update_geometry_matrices(true, false, false, false, false);
}

void geom_store::update_model_zoomfit()
{
	if (is_geometry_set == false)
		return;

	// Set the pan translation matrix
	geom_param.panTranslation = glm::mat4(1.0f);

	// Set the zoom scale
	geom_param.zoom_scale = 1.0f;

	// Update the zoom scale and pan translation
	model_nodes.update_geometry_matrices(false, true, true, false, false);
	model_lineelements.update_geometry_matrices(false, true, true, false, false);
	model_constarints.update_geometry_matrices(false, true, true, false, false);
}

void geom_store::update_model_pan(glm::vec2& transl)
{
	if (is_geometry_set == false)
		return;

	// Pan the geometry
	geom_param.panTranslation = glm::mat4(1.0f);

	geom_param.panTranslation[0][3] = -1.0f * transl.x;
	geom_param.panTranslation[1][3] = transl.y;

	// Update the pan translation
	model_nodes.update_geometry_matrices(false, true, false, false, false);
	model_lineelements.update_geometry_matrices(false, true, false, false, false);
	model_constarints.update_geometry_matrices(false, true, false, false, false);
}

void geom_store::update_model_zoom(double& z_scale)
{
	if (is_geometry_set == false)
		return;

	// Zoom the geometry
	geom_param.zoom_scale = z_scale;

	// Update the Zoom
	model_nodes.update_geometry_matrices(false, false, true, false, false);
	model_lineelements.update_geometry_matrices(false, false, true, false, false);
	model_constarints.update_geometry_matrices(false, false, true, false, false);
}

void geom_store::update_model_transperency(bool is_transparent)
{
	if (is_geometry_set == false)
		return;

	if (is_transparent == true)
	{
		// Set the transparency value
		geom_param.geom_transparency = 0.2f;
	}
	else
	{
		// remove transparency
		geom_param.geom_transparency = 1.0f;
	}

	// Update the model transparency
	model_nodes.update_geometry_matrices(false, false, false, true, false);
	model_lineelements.update_geometry_matrices(false, false, false, true, false);
	model_constarints.update_geometry_matrices(false, false, false, true, false);
}

void geom_store::set_nodal_constraint(glm::vec2 mouse_click_loc, int& constraint_type, double& constraint_angle, bool is_add)
{
	if (is_geometry_set == false)
		return;

	// geometry is set so check whether node is hit
	int node_hit_id = model_nodes.is_node_hit(mouse_click_loc);;

	if (node_hit_id != -1)
	{
		// Node is hit
		if (is_add == true)
		{
			// Add constraints
			model_constarints.add_constraint(node_hit_id, model_nodes.nodeMap[node_hit_id].node_pt, constraint_type, constraint_angle);
			model_constarints.set_buffer();
		}
		else
		{
			// remove constraint
			model_constarints.delete_constraint(node_hit_id);
			model_constarints.set_buffer();
		}
	}
}

void geom_store::set_member_load(glm::vec2 mouse_click_loc, double& load_value, double& load_angle, bool is_add)
{

}


void geom_store::paint_geometry()
{
	if (is_geometry_set == false)
		return;

	// Clean the back buffer and assign the new color to it
	 glClear(GL_COLOR_BUFFER_BIT);


	model_constarints.paint_constraints();
	model_lineelements.paint_elementlines();
	model_nodes.paint_model_nodes();

	if (op_window->is_show_nodenumber == true)
	{
		// Show model node number
		model_nodes.paint_label_node_ids();
	}
	
	if (op_window->is_show_nodecoord == true)
	{
		// Show model node coordinate
		model_nodes.paint_label_node_coords();
	}

	if (op_window->is_show_linenumber == true)
	{
		// Show line ID label
		model_lineelements.paint_label_line_ids();
	}

	if (op_window->is_show_linelength == true)
	{
		// Show line length label
		model_lineelements.paint_label_line_lengths();
	}
}

void geom_store::create_geometry(nodes_list_store& model_nodes, elementline_list_store& model_lineelements,
	nodeconstraint_list_store& model_constarints, nodeload_list_store& model_loads)
{
	// Reinitialize the model geometry
	is_geometry_set = false;

	this->model_nodes.init(&geom_param);
	this->model_lineelements.init(&geom_param);
	this->model_constarints.init(&geom_param);
	this->model_loads.init(&geom_param);

	//________________________________________________

	// Add to model nodes
	for (auto& nd : model_nodes.nodeMap)
	{
		// create a temporary node
		node_store temp_node;
		temp_node = nd.second;

		// Add to the node list
		this->model_nodes.add_node(temp_node.node_id, temp_node.node_pt);
	}

	// Add to model lines
	for (auto& ln : model_lineelements.elementlineMap)
	{
		// create a temporary node
		elementline_store temp_line;
		temp_line = ln.second;

		// Add to the element list
		this->model_lineelements.add_elementline(temp_line.line_id, &this->model_nodes.nodeMap[temp_line.startNode->node_id],
			&this->model_nodes.nodeMap[temp_line.endNode->node_id], temp_line.material_id);
	}

	// Add to model constraints
	for (auto& cnst : model_constarints.constraintMap)
	{
		// create a temporary node
		constraint_data temp_cnst;
		temp_cnst = cnst.second;

		// Add to the line list
		this->model_constarints.add_constraint(temp_cnst.node_id, temp_cnst.constraint_loc,
			temp_cnst.constraint_type, temp_cnst.constraint_angle);
	}

	// Add to model loads



	// Geometry is loaded
	is_geometry_set = true;

	// Set the boundary of the geometry
	std::pair<glm::vec2, glm::vec2> result = findMinMaxXY(model_nodes.nodeMap);
	this->geom_param.min_b = result.first;
	this->geom_param.max_b = result.second;
	this->geom_param.geom_bound = geom_param.max_b - geom_param.min_b;

	// Set the center of the geometry
	this->geom_param.center = findGeometricCenter(model_nodes.nodeMap);

	// Set the geometry
	update_model_matrix();
	update_model_zoomfit();

	// Set the geometry buffers
	this->model_nodes.set_buffer();
	this->model_lineelements.set_buffer();
	this->model_constarints.set_buffer();

}

std::pair<glm::vec2, glm::vec2> geom_store::findMinMaxXY(const std::unordered_map<int, node_store>& model_nodes)
{
	// Initialize min and max values to first node in map
	glm::vec2 firstNode = model_nodes.begin()->second.node_pt;
	glm::vec2 minXY = glm::vec2(firstNode.x, firstNode.y);
	glm::vec2 maxXY = minXY;

	// Loop through all nodes in map and update min and max values
	for (auto it = model_nodes.begin(); it != model_nodes.end(); ++it)
	{
		const auto& node = it->second.node_pt;
		if (node.x < minXY.x)
		{
			minXY.x = node.x;
		}
		if (node.y < minXY.y)
		{
			minXY.y = node.y;
		}
		if (node.x > maxXY.x)
		{
			maxXY.x = node.x;
		}
		if (node.y > maxXY.y)
		{
			maxXY.y = node.y;
		}
	}

	// Return pair of min and max values
	return { minXY, maxXY };
}

glm::vec2 geom_store::findGeometricCenter(const std::unordered_map<int, node_store>& model_nodes)
{
	// Function returns the geometric center of the nodes
		// Initialize the sum with zero
	glm::vec2 sum(0);

	// Sum the points
	for (auto it = model_nodes.begin(); it != model_nodes.end(); ++it)
	{
		sum += it->second.node_pt;
	}
	return sum / static_cast<float>(model_nodes.size());
}


