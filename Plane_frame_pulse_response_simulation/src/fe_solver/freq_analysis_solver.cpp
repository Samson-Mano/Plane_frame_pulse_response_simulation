#include "freq_analysis_solver.h"

freq_analysis_solver::freq_analysis_solver()
{
	// Empty constructor
}

freq_analysis_solver::~freq_analysis_solver()
{
	// Empty destructor
}

void freq_analysis_solver::freq_analysis_start(const nodes_list_store& model_nodes,
	const elementline_list_store& model_lineelements,
	const nodeconstraint_list_store& model_constarints,
	const nodeload_list_store& model_loads,
	const nodepointmass_list_store& model_ptmass,
	const std::unordered_map<int, material_data>& material_list,
	const bool& is_include_consistent_mass_matrix,
	const modal_analysis_result_store& modal_results,
	const double freq_start,
	const double freq_end,
	const double freq_interval,
	const double damping_ratio,
	freq_analysis_result_store& freq_response_result,
	bool& is_freq_analysis_complete)
{
	// Main solver call
	is_freq_analysis_complete = false;

	// Check the model
	// Number of loads (Exit if no load is present)
	if (model_loads.load_count == 0)
	{
		return;
	}

	// Create global stiffness, mass and load matrices
	//___________________________________________________________________________________
	// Create a node ID map (to create a nodes as ordered and numbered from 0,1,2...n)
	int i = 0;
	for (auto& nd : model_nodes.nodeMap)
	{
		nodeid_map[nd.first] = i;
		i++;
	}

	//___________________________________________________________________________________
	// Create a file to keep track of frequency response matrices
	std::ofstream output_file;
	output_file.open("freq_analysis_results.txt");

	//____________________________________________________________________________________________________________________
	int numDOF = model_nodes.node_count * 3; // Number of degrees of freedom (3 DOFs per node (2 translation and 1 rotation)

	// Global Stiffness Matrix
	Eigen::MatrixXd globalStiffnessMatrix(numDOF, numDOF);
	globalStiffnessMatrix.setZero();

	get_global_stiffness_matrix(globalStiffnessMatrix,
		model_lineelements,
		material_list,
		model_constarints,
		output_file);


	//____________________________________________________________________________________________________________________
	// Global Point Mass Matrix
	Eigen::MatrixXd globalPointMassMatrix(numDOF, numDOF);
	globalPointMassMatrix.setZero();

	get_global_pointmass_matrix(globalPointMassMatrix,
		model_nodes,
		model_ptmass,
		output_file);

	//____________________________________________________________________________________________________________________
	// Global Consistent Mass Matrix
	Eigen::MatrixXd globalConsistentMassMatrix(numDOF, numDOF);
	globalConsistentMassMatrix.setZero();

	if (is_include_consistent_mass_matrix == true)
	{
		get_global_consistentmass_matrix(globalConsistentMassMatrix,
			model_lineelements,
			material_list,
			model_constarints,
			output_file);
	}

	//____________________________________________________________________________________________________________________
	// Global Consistent Mass Matrix
	Eigen::MatrixXd globalMassMatrix(numDOF, numDOF);
	globalMassMatrix.setZero();

	globalMassMatrix = globalPointMassMatrix + globalConsistentMassMatrix;

	//____________________________________________________________________________________________________________________
	// Global Load Matrix
	Eigen::MatrixXd globalLoadMatrix(numDOF, 1);
	globalLoadMatrix.setZero();

	get_global_load_matrix(globalLoadMatrix, model_lineelements, model_loads, output_file);

	//____________________________________________________________________________________________________________________
	// Global DOF Mass Matrix
	Eigen::MatrixXd globalDOFMatrix(numDOF, 1);
	globalDOFMatrix.setZero();

	// Determine the size of the reduced stiffness matrix based on the number of unconstrained degrees of freedom
	int reducedDOF = 0;

	get_global_dof_matrix(globalDOFMatrix,
		model_nodes,
		model_constarints,
		reducedDOF,
		output_file);

	//____________________________________________________________________________________________________________________
	// Create Reduced Global Mass and stiffness matrix
	Eigen::MatrixXd  reduced_globalStiffnessMatrix(reducedDOF, reducedDOF);
	reduced_globalStiffnessMatrix.setZero();

	// Reduced Global Mass matrix
	Eigen::MatrixXd reduced_globalMassMatrix(reducedDOF, reducedDOF);
	reduced_globalMassMatrix.setZero();

	// Reduced Load matrix
	Eigen::MatrixXd reduced_globalLoadMatrix(reducedDOF, 1);
	reduced_globalLoadMatrix.setZero();

	get_reduced_global_matrices(reduced_globalStiffnessMatrix,
		reduced_globalMassMatrix,
		reduced_globalLoadMatrix,
		globalStiffnessMatrix,
		globalMassMatrix,
		globalLoadMatrix,
		globalDOFMatrix,
		numDOF,
		output_file);

	//____________________________________________________________________________________________________________________
	// Modal Decomposition

	// Reduced Eigen Vectors matrix
	Eigen::MatrixXd reduced_eigenVectorsMatrix(reducedDOF, reducedDOF);
	reduced_eigenVectorsMatrix.setZero();

	get_reduced_modal_vector_matrix(reduced_eigenVectorsMatrix, modal_results, reducedDOF, output_file);

	//____________________________________________________________________________________________________________________
	// Create modal matrices
	std::vector<double> modalMass(reducedDOF);
	std::vector<double> modalStiff(reducedDOF);
	std::vector<double> modalForce(reducedDOF);

	get_modal_matrices(modalMass,
		modalStiff,
		modalForce,
		reduced_eigenVectorsMatrix,
		reduced_globalMassMatrix,
		reduced_globalStiffnessMatrix,
		reduced_globalLoadMatrix,
		reducedDOF);

	//____________________________________________________________________________________________________________________
	// Create the global support inclination matrix
	Eigen::MatrixXd globalSupportInclinationMatrix(numDOF, numDOF);
	globalSupportInclinationMatrix.setZero();


	get_globalSupportInclinationMatrix(globalSupportInclinationMatrix,
		model_nodes,
		model_constarints,
		numDOF,
		output_file);

	if (print_matrix == true)
	{
		output_file << "Global support inclination matrix" << std::endl;
		output_file << globalSupportInclinationMatrix << std::endl;
		output_file << std::endl;
	}


	//____________________________________________________________________________________________________________________
	// Frequency response
	freq_response_result.clear_data();

	for (double forcing_freq = freq_start; forcing_freq <= freq_end; forcing_freq += freq_interval)
	{
		Eigen::MatrixXd displ_ampl_RespMatrix(numDOF, 1);
		Eigen::MatrixXd displ_phase_RespMatrix(numDOF, 1);

		displ_ampl_RespMatrix.setZero();
		displ_phase_RespMatrix.setZero();

		get_global_response(displ_ampl_RespMatrix,
			displ_phase_RespMatrix,
			reduced_eigenVectorsMatrix,
			globalDOFMatrix,
			globalSupportInclinationMatrix,
			modalMass,
			modalStiff,
			modalForce,
			forcing_freq,
			damping_ratio,
			reducedDOF,
			numDOF,
			output_file);

		// Map the results to node
		for (auto& nd_m : model_nodes.nodeMap)
		{
			// Get the node data
			node_store nd = nd_m.second;
			int nd_map = nodeid_map[nd.node_id]; // get the ordered map of the node ID

			// Add the response
			freq_response_result.add_node_response(nd.node_id,
				forcing_freq,
				displ_ampl_RespMatrix.coeff((nd_map * 3) + 0, 0),
				displ_phase_RespMatrix.coeff((nd_map * 3) + 0, 0),
				displ_ampl_RespMatrix.coeff((nd_map * 3) + 1, 0),
				displ_phase_RespMatrix.coeff((nd_map * 3) + 1, 0),
				displ_ampl_RespMatrix.coeff((nd_map * 3) + 2, 0),
				displ_phase_RespMatrix.coeff((nd_map * 3) + 2, 0));
		}
	}

	// Create node list string
	for (auto& nd_m : model_nodes.nodeMap)
	{
		// Add the node id
		freq_response_result.node_id_str.push_back("Node: " + std::to_string(nd_m.second.node_id));
	}

	is_freq_analysis_complete = true;
	freq_response_result.is_freq_analysis_complete = true;


	//____________________________________________________________________________________________________________________
	output_file.close();
}


void freq_analysis_solver::get_global_stiffness_matrix(Eigen::MatrixXd& globalStiffnessMatrix,
	const elementline_list_store& model_lineelements,
	const std::unordered_map<int, material_data>& material_list,
	const nodeconstraint_list_store& model_constarints, std::ofstream& output_file)
{
	// Create global stiffness matrix
	for (auto& ln_m : model_lineelements.elementlineMap)
	{
		// Create the element stiffness matrix
		elementline_store ln = ln_m.second;
		material_data elementline_material = material_list.at(ln.material_id);

		// Create a matrix for element stiffness matrix
		Eigen::MatrixXd elementStiffnessMatrix(6, 6);
		elementStiffnessMatrix.setZero();

		get_element_stiffness_matrix(elementStiffnessMatrix, ln, elementline_material, model_constarints, output_file);

		// Get the Node ID
		int sn_id = nodeid_map[ln.startNode->node_id]; // get the ordered map of the start node ID
		int en_id = nodeid_map[ln.endNode->node_id]; // get the ordered map of the end node ID

		globalStiffnessMatrix.block<3, 3>(sn_id * 3, sn_id * 3) += elementStiffnessMatrix.block<3, 3>(0, 0);
		globalStiffnessMatrix.block<3, 3>(sn_id * 3, en_id * 3) += elementStiffnessMatrix.block<3, 3>(0, 3);
		globalStiffnessMatrix.block<3, 3>(en_id * 3, sn_id * 3) += elementStiffnessMatrix.block<3, 3>(3, 0);
		globalStiffnessMatrix.block<3, 3>(en_id * 3, en_id * 3) += elementStiffnessMatrix.block<3, 3>(3, 3);
	}
}

void freq_analysis_solver::get_element_stiffness_matrix(Eigen::MatrixXd& elementStiffnessMatrix,
	const elementline_store& ln,
	const material_data& elementline_material,
	const nodeconstraint_list_store& model_constarints,
	std::ofstream& output_file)
{
	// Get element stiffness matrix
	// Compute the differences in x and y coordinates
	double dx = ln.endNode->node_pt.x - ln.startNode->node_pt.x;
	double dy = -1.0 * (ln.endNode->node_pt.y - ln.startNode->node_pt.y);

	// Compute the length of the truss element
	double eLength = std::sqrt((dx * dx) + (dy * dy));

	// Compute the direction cosines
	double Lcos = (dx / eLength);
	double Msin = (dy / eLength);

	//_________________________________________________________
	// Local -> Global transformation matrix
	Eigen::MatrixXd L_transformation_matrix(6, 6);
	L_transformation_matrix.setZero();

	L_transformation_matrix.row(0) = Eigen::RowVectorXd({ {Lcos, Msin, 0.0, 0.0, 0.0, 0.0 } });
	L_transformation_matrix.row(1) = Eigen::RowVectorXd({ { -Msin, Lcos, 0.0, 0.0, 0.0, 0.0} });
	L_transformation_matrix.row(2) = Eigen::RowVectorXd({ {0.0, 0.0, 1.0, 0.0, 0.0, 0.0} });
	L_transformation_matrix.row(3) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, Lcos, Msin, 0.0} });
	L_transformation_matrix.row(4) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, -Msin, Lcos, 0.0} });
	L_transformation_matrix.row(5) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 0.0, 0.0, 1.0} });


	//_________________________________________________________
	// Local element stiffness matrix
	Eigen::MatrixXd local_element_stiffness_matrix(6, 6);
	local_element_stiffness_matrix.setZero();

	double k1 = (elementline_material.youngs_mod * elementline_material.cs_area) / eLength;
	double k2 = (elementline_material.youngs_mod * elementline_material.second_moment_of_area) / (eLength * eLength * eLength);
	double k3 = (elementline_material.youngs_mod * elementline_material.second_moment_of_area) / (eLength * eLength);
	double k4 = (elementline_material.youngs_mod * elementline_material.second_moment_of_area) / eLength;

	local_element_stiffness_matrix.row(0) = Eigen::RowVectorXd({ {k1, 0.0, 0.0, -1.0 * k1, 0.0, 0.0} });
	local_element_stiffness_matrix.row(1) = Eigen::RowVectorXd({ {0.0, 12.0 * k2, 6.0 * k3, 0.0, -12.0 * k2, 6.0 * k3} });
	local_element_stiffness_matrix.row(2) = Eigen::RowVectorXd({ {0.0, 6.0 * k3, 4.0 * k4, 0.0, -6.0 * k3, 2.0 * k4} });
	local_element_stiffness_matrix.row(3) = Eigen::RowVectorXd({ {-1.0 * k1, 0.0, 0.0, k1, 0.0, 0.0} });
	local_element_stiffness_matrix.row(4) = Eigen::RowVectorXd({ {0.0, -12.0 * k2, -6.0 * k3, 0.0, 12.0 * k2, -6.0 * k3} });
	local_element_stiffness_matrix.row(5) = Eigen::RowVectorXd({ {0.0, 6.0 * k3, 2.0 * k4, 0.0, -6.0 * k3, 4.0 * k4} });

	//_________________________________________________________
	// Transformed element stiffness matrix
	Eigen::MatrixXd e_stiffness_matrix(6, 6);
	e_stiffness_matrix.setZero();

	e_stiffness_matrix = L_transformation_matrix.transpose() * local_element_stiffness_matrix * L_transformation_matrix;
	//_________________________________________________________

	// Transformation matrices to include support inclinatation
	Eigen::MatrixXd s_transformation_matrix(6, 6);
	s_transformation_matrix.setZero(); // support inclination transformation matrix

	int constraint_type;
	double constraint_angle_rad;
	double support_Lcos;
	double support_Msin;

	// Start node support inclination
	if (model_constarints.constraintMap.find(ln.startNode->node_id) == model_constarints.constraintMap.end())
	{
		// No constraint at the start node
		s_transformation_matrix.row(0) = Eigen::RowVectorXd({ {1.0, 0.0, 0.0, 0.0, 0.0, 0.0} });
		s_transformation_matrix.row(1) = Eigen::RowVectorXd({ {0.0, 1.0, 0.0, 0.0, 0.0, 0.0} });
		s_transformation_matrix.row(2) = Eigen::RowVectorXd({ {0.0, 0.0, 1.0, 0.0, 0.0, 0.0} });
	}
	else
	{
		constraint_type = model_constarints.constraintMap.at(ln.startNode->node_id).constraint_type; // Constrint type (0 - pin support, 1 - roller support)
		constraint_angle_rad = (model_constarints.constraintMap.at(ln.startNode->node_id).constraint_angle - 90.0) * (m_pi / 180.0f); // Constrint angle in radians
		support_Lcos = std::cos(constraint_angle_rad); // cosine of support inclination
		support_Msin = std::sin(constraint_angle_rad); // sine of support inclination

		// Pin or Roller Support
		s_transformation_matrix.row(0) = Eigen::RowVectorXd({ {support_Lcos, -support_Msin, 0.0, 0.0, 0.0, 0.0} });
		s_transformation_matrix.row(1) = Eigen::RowVectorXd({ {support_Msin, support_Lcos, 0.0, 0.0, 0.0, 0.0} });
		s_transformation_matrix.row(2) = Eigen::RowVectorXd({ {0.0, 0.0, 1.0, 0.0, 0.0, 0.0} });
	}

	// End node support inclination
	if (model_constarints.constraintMap.find(ln.endNode->node_id) == model_constarints.constraintMap.end())
	{
		// No constraint at the end node
		s_transformation_matrix.row(3) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 1.0, 0.0, 0.0} });
		s_transformation_matrix.row(4) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 0.0, 1.0, 0.0} });
		s_transformation_matrix.row(5) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 0.0, 0.0, 1.0} });
	}
	else
	{
		constraint_type = model_constarints.constraintMap.at(ln.endNode->node_id).constraint_type; // Constrint type (0 - pin support, 1 - roller support)
		constraint_angle_rad = (model_constarints.constraintMap.at(ln.endNode->node_id).constraint_angle - 90.0) * (m_pi / 180.0f); // Constrint angle in radians
		support_Lcos = std::cos(constraint_angle_rad); // cosine of support inclination
		support_Msin = std::sin(constraint_angle_rad); // sine of support inclination

		// Pin or Roller Support
		s_transformation_matrix.row(3) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, support_Lcos, -support_Msin, 0.0} });
		s_transformation_matrix.row(4) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, support_Msin, support_Lcos, 0.0} });
		s_transformation_matrix.row(5) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 0.0, 0.0, 1.0} });
	}

	// Calculate the element stiffness matrix
	elementStiffnessMatrix = s_transformation_matrix.transpose() * e_stiffness_matrix * s_transformation_matrix;

}

void freq_analysis_solver::get_global_pointmass_matrix(Eigen::MatrixXd& globalPointMassMatrix,
	const nodes_list_store& model_nodes,
	const nodepointmass_list_store& model_ptmass,
	std::ofstream& output_file)
{
	// Create a global point mass matrix
	for (auto& nd_m : model_nodes.nodeMap)
	{
		// Get the node data
		node_store nd = nd_m.second;
		int nd_map = nodeid_map[nd.node_id]; // get the ordered map of the node ID

		if (model_ptmass.ptmassMap.find(nd.node_id) != model_ptmass.ptmassMap.end())
		{
			// Nodes have point mass
			nodepointmass_data ptm = model_ptmass.ptmassMap.at(nd.node_id);

			globalPointMassMatrix((nd_map * 3) + 0, (nd_map * 3) + 0) = ptm.ptmass_x;
			globalPointMassMatrix((nd_map * 3) + 1, (nd_map * 3) + 1) = ptm.ptmass_y;
			globalPointMassMatrix((nd_map * 3) + 2, (nd_map * 3) + 2) = ptm.ptmass_xy;
		}
		else
		{
			// Nodes doesnt have point mass
			globalPointMassMatrix((nd_map * 3) + 0, (nd_map * 3) + 0) = 0.0;
			globalPointMassMatrix((nd_map * 3) + 1, (nd_map * 3) + 1) = 0.0;
			globalPointMassMatrix((nd_map * 3) + 2, (nd_map * 3) + 2) = 0.0;
		}
	}
}


void freq_analysis_solver::get_global_consistentmass_matrix(Eigen::MatrixXd& globalConsistentMassMatrix,
	const elementline_list_store& model_lineelements,
	const std::unordered_map<int, material_data>& material_list,
	const nodeconstraint_list_store& model_constarints,
	std::ofstream& output_file)
{
	// Create global consistent mass matrix
	for (auto& ln_m : model_lineelements.elementlineMap)
	{
		// Create the element stiffness matrix
		elementline_store ln = ln_m.second;
		material_data elementline_material = material_list.at(ln.material_id);

		// Create a matrix for element stiffness matrix
		Eigen::MatrixXd elementConsistentMassMatrix(6, 6);
		elementConsistentMassMatrix.setZero();

		get_element_consistentmass_matrix(elementConsistentMassMatrix, ln, elementline_material, model_constarints, output_file);

		// Get the Node ID
		int sn_id = nodeid_map[ln.startNode->node_id]; // get the ordered map of the start node ID
		int en_id = nodeid_map[ln.endNode->node_id]; // get the ordered map of the end node ID

		globalConsistentMassMatrix.block<3, 3>(sn_id * 3, sn_id * 3) += elementConsistentMassMatrix.block<3, 3>(0, 0);
		globalConsistentMassMatrix.block<3, 3>(sn_id * 3, en_id * 3) += elementConsistentMassMatrix.block<3, 3>(0, 3);
		globalConsistentMassMatrix.block<3, 3>(en_id * 3, sn_id * 3) += elementConsistentMassMatrix.block<3, 3>(3, 0);
		globalConsistentMassMatrix.block<3, 3>(en_id * 3, en_id * 3) += elementConsistentMassMatrix.block<3, 3>(3, 3);
	}
}

void freq_analysis_solver::get_element_consistentmass_matrix(Eigen::MatrixXd& elementConsistentMassMatrix,
	const elementline_store& ln,
	const material_data& elementline_material,
	const nodeconstraint_list_store& model_constarints,
	std::ofstream& output_file)
{
	// Create element consistent mass matrix
	// Compute the differences in x and y coordinates
	double dx = ln.endNode->node_pt.x - ln.startNode->node_pt.x;
	double dy = -1.0 * (ln.endNode->node_pt.y - ln.startNode->node_pt.y);

	// Compute the length of the frame element
	double eLength = std::sqrt((dx * dx) + (dy * dy));

	// Compute the direction cosines
	double Lcos = (dx / eLength);
	double Msin = (dy / eLength);

	//_________________________________________________________
	// Local -> Global transformation matrix
	Eigen::MatrixXd L_transformation_matrix(6, 6);
	L_transformation_matrix.setZero();

	L_transformation_matrix.row(0) = Eigen::RowVectorXd({ {Lcos, Msin, 0.0, 0.0, 0.0, 0.0} });
	L_transformation_matrix.row(1) = Eigen::RowVectorXd({ {-Msin, Lcos, 0.0, 0.0, 0.0, 0.0} });
	L_transformation_matrix.row(2) = Eigen::RowVectorXd({ {0.0, 0.0, 1.0, 0.0, 0.0, 0.0} });
	L_transformation_matrix.row(3) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, Lcos, Msin, 0.0} });
	L_transformation_matrix.row(4) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, -Msin, Lcos, 0.0} });
	L_transformation_matrix.row(5) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 0.0, 0.0, 1.0} });

	//_________________________________________________________
	// Local element stiffness matrix
	Eigen::MatrixXd local_element_consistentmass_matrix(6, 6);
	local_element_consistentmass_matrix.setZero();

	double k1 = (elementline_material.mat_density * elementline_material.cs_area * eLength) / 420.0f;
	double k2 = k1 * eLength;
	double k3 = k1 * (eLength * eLength);

	local_element_consistentmass_matrix.row(0) = Eigen::RowVectorXd({ {140.0 * k1, 0.0, 0.0, 70.0 * k1, 0.0, 0.0} });
	local_element_consistentmass_matrix.row(1) = Eigen::RowVectorXd({ {0.0, 156.0 * k1, 22.0 * k2, 0.0, 54.0 * k1, -13.0 * k2} });
	local_element_consistentmass_matrix.row(2) = Eigen::RowVectorXd({ {0.0, 22.0 * k2, 4.0 * k3, 0.0, 13.0 * k2, -3.0 * k3} });
	local_element_consistentmass_matrix.row(3) = Eigen::RowVectorXd({ {70.0 * k1, 0.0, 0.0, 140.0 * k1, 0.0, 0.0} });
	local_element_consistentmass_matrix.row(4) = Eigen::RowVectorXd({ {0.0, 54.0 * k1, 13.0 * k2, 0.0, 156.0 * k1, -22.0 * k2} });
	local_element_consistentmass_matrix.row(5) = Eigen::RowVectorXd({ {0.0, -13.0 * k2, -3.0 * k3, 0.0, -22.0 * k2, 4.0 * k3} });

	//_________________________________________________________
	// Transformed element stiffness matrix
	Eigen::MatrixXd e_consistentMass_matrix(6, 6);
	e_consistentMass_matrix.setZero();

	e_consistentMass_matrix = L_transformation_matrix.transpose() * local_element_consistentmass_matrix * L_transformation_matrix;
	//_________________________________________________________

	// Transformation matrices to include support inclinatation
	Eigen::MatrixXd s_transformation_matrix(6, 6);
	s_transformation_matrix.setZero(); // support inclination transformation matrix

	int constraint_type;
	double constraint_angle_rad;
	double support_Lcos;
	double support_Msin;

	// Start node support inclination
	if (model_constarints.constraintMap.find(ln.startNode->node_id) == model_constarints.constraintMap.end())
	{
		// No constraint at the start node
		s_transformation_matrix.row(0) = Eigen::RowVectorXd({ {1.0, 0.0, 0.0, 0.0, 0.0, 0.0} });
		s_transformation_matrix.row(1) = Eigen::RowVectorXd({ {0.0, 1.0, 0.0, 0.0, 0.0, 0.0} });
		s_transformation_matrix.row(2) = Eigen::RowVectorXd({ {0.0, 0.0, 1.0, 0.0, 0.0, 0.0} });
	}
	else
	{
		constraint_type = model_constarints.constraintMap.at(ln.startNode->node_id).constraint_type; // Constrint type (0 - pin support, 1 - roller support)
		constraint_angle_rad = (model_constarints.constraintMap.at(ln.startNode->node_id).constraint_angle - 90.0) * (m_pi / 180.0f); // Constrint angle in radians
		support_Lcos = std::cos(constraint_angle_rad); // cosine of support inclination
		support_Msin = std::sin(constraint_angle_rad); // sine of support inclination

		// Pin or Roller Support
		s_transformation_matrix.row(0) = Eigen::RowVectorXd({ {support_Lcos, -support_Msin, 0.0, 0.0, 0.0, 0.0} });
		s_transformation_matrix.row(1) = Eigen::RowVectorXd({ {support_Msin, support_Lcos, 0.0, 0.0, 0.0, 0.0} });
		s_transformation_matrix.row(2) = Eigen::RowVectorXd({ {0.0, 0.0, 1.0, 0.0, 0.0, 0.0} });
	}

	// End node support inclination
	if (model_constarints.constraintMap.find(ln.endNode->node_id) == model_constarints.constraintMap.end())
	{
		// No constraint at the end node
		s_transformation_matrix.row(3) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 1.0, 0.0, 0.0} });
		s_transformation_matrix.row(4) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 0.0, 1.0, 0.0} });
		s_transformation_matrix.row(5) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 0.0, 0.0, 1.0} });
	}
	else
	{
		constraint_type = model_constarints.constraintMap.at(ln.endNode->node_id).constraint_type; // Constrint type (0 - pin support, 1 - roller support)
		constraint_angle_rad = (model_constarints.constraintMap.at(ln.endNode->node_id).constraint_angle - 90.0) * (m_pi / 180.0f); // Constrint angle in radians
		support_Lcos = std::cos(constraint_angle_rad); // cosine of support inclination
		support_Msin = std::sin(constraint_angle_rad); // sine of support inclination

		// Pin or Roller Support
		s_transformation_matrix.row(3) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, support_Lcos, -support_Msin, 0.0} });
		s_transformation_matrix.row(4) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, support_Msin, support_Lcos, 0.0 } });
		s_transformation_matrix.row(5) = Eigen::RowVectorXd({ {0.0, 0.0, 0.0, 0.0, 0.0, 1.0} });
	}

	// Calculate the element stiffness matrix
	elementConsistentMassMatrix = s_transformation_matrix.transpose() * e_consistentMass_matrix * s_transformation_matrix;
}

void freq_analysis_solver::get_global_load_matrix(Eigen::MatrixXd& globalLoadMatrix,
	const elementline_list_store& model_lineelements,
	const nodeload_list_store& model_loads,
	std::ofstream& output_file)
{
	// Create a global Load matrix
	for (auto& ln_m : model_lineelements.elementlineMap)
	{
		// Get the line element
		elementline_store ln = ln_m.second;

		// Create a matrix for element load matrix
		Eigen::MatrixXd elementLoadMatrix(6, 1);
		elementLoadMatrix.setZero();

		// Get the element load matrix
		get_element_load_matrix(elementLoadMatrix, ln, model_loads, output_file);

		// Get the Node ID
		int sn_id = nodeid_map[ln.startNode->node_id]; // get the ordered map of the start node ID
		int en_id = nodeid_map[ln.endNode->node_id]; // get the ordered map of the end node ID

		globalLoadMatrix.block<3, 1>(sn_id * 3, 0) += elementLoadMatrix.block<3, 1>(0, 0);
		globalLoadMatrix.block<3, 1>(en_id * 3, 0) += elementLoadMatrix.block<3, 1>(3, 0);
	}

	if (print_matrix == true)
	{
		// Print the Reduced Global Stiffness, Global Mass and Global Force matrix
		output_file << "Global Load Matrix" << std::endl;
		output_file << globalLoadMatrix << std::endl;
		output_file << std::endl;
	}
}

void freq_analysis_solver::get_element_load_matrix(Eigen::MatrixXd& elementLoadMatrix,
	const elementline_store& ln,
	const nodeload_list_store& model_loads,
	std::ofstream& output_file)
{
	// Compute the differences in x and y coordinates
	double dx = ln.endNode->node_pt.x - ln.startNode->node_pt.x;
	double dy = -1.0 * (ln.endNode->node_pt.y - ln.startNode->node_pt.y);

	// Compute the length of the frame element
	double eLength = std::sqrt((dx * dx) + (dy * dy));

	// Compute the direction cosines
	double Lcos = (dx / eLength);
	double Msin = (dy / eLength);

	// Create element load mass matrix
	for (auto& ld_m : model_loads.loadMap)
	{
		// Get the load data
		load_data ld = ld_m.second;

		if (ld.line_id == ln.line_id)
		{
			// Load's line id is equal to this line id
			double load_val = ld.load_value; // Load value
			double load_angle_rad = ld.load_angle * (m_pi / 180.0f);

			double f_x = load_val * std::cos(load_angle_rad);
			double f_y = load_val * std::sin(load_angle_rad);

			double loadHorizontal = (-f_x * Lcos) + (f_y * Msin);
			double loadVertical = (f_x * Msin) + (f_y * Lcos);

			double a = eLength * ld.load_loc_param;
			double b = eLength * (1.0 - ld.load_loc_param);

			// Load parameter
			double phi_i = (loadVertical * b * (std::pow(eLength, 2) - std::pow(b, 2)) / (6 * eLength));
			double phi_j = (loadVertical * a * (std::pow(eLength, 2) - std::pow(a, 2)) / (6 * eLength));

			// Axial Load
			double Tai = loadHorizontal * ld.load_loc_param;
			double Taj = loadHorizontal * (1 - ld.load_loc_param);

			// End Moments
			double Tmi = (((4 * phi_i) - (2 * phi_j)) / eLength);
			double Tmj = (((2 * phi_i) - (4 * phi_j)) / eLength);

			// Vertical load
			double Tfi = (((loadVertical * b) + Tmi + Tmj) / eLength);
			double Tfj = (((loadVertical * a) - Tmi - Tmj) / eLength);

			// Add to the element load matrix
			elementLoadMatrix.coeffRef(0, 0) += Tai;
			elementLoadMatrix.coeffRef(1, 0) += (-Tfi);
			elementLoadMatrix.coeffRef(2, 0) += (-Tmi);

			elementLoadMatrix.coeffRef(3, 0) += Taj;
			elementLoadMatrix.coeffRef(4, 0) += (-Tfj);
			elementLoadMatrix.coeffRef(5, 0) += (-Tmj);
		}
	}
}

void freq_analysis_solver::get_global_dof_matrix(Eigen::MatrixXd& globalDOFMatrix,
	const nodes_list_store& model_nodes,
	const nodeconstraint_list_store& model_constarints,
	int& reducedDOF,
	std::ofstream& output_file)
{
	// Create global DOF Matrix
	for (auto& nd_m : model_nodes.nodeMap)
	{
		// Get the node data
		node_store nd = nd_m.second;
		int nd_map = nodeid_map[nd.node_id]; // get the ordered map of the node ID

		if (model_constarints.constraintMap.find(nd.node_id) != model_constarints.constraintMap.end())
		{
			// Nodes have point mass
			constraint_data cd = model_constarints.constraintMap.at(nd.node_id);

			if (cd.constraint_type == 0)
			{
				// Fixed End

				globalDOFMatrix.coeffRef((nd_map * 3) + 0, 0) = 0.0;
				globalDOFMatrix.coeffRef((nd_map * 3) + 1, 0) = 0.0;
				globalDOFMatrix.coeffRef((nd_map * 3) + 2, 0) = 0.0;
			}
			else if (cd.constraint_type == 1)
			{
				// Fixed Roller end

				globalDOFMatrix.coeffRef((nd_map * 3) + 0, 0) = 1.0; // X is free to move
				globalDOFMatrix.coeffRef((nd_map * 3) + 1, 0) = 0.0;
				globalDOFMatrix.coeffRef((nd_map * 3) + 2, 0) = 0.0;

				reducedDOF = reducedDOF + 1;
			}
			else if (cd.constraint_type == 2)
			{
				// Pin End

				globalDOFMatrix.coeffRef((nd_map * 3) + 0, 0) = 0.0;
				globalDOFMatrix.coeffRef((nd_map * 3) + 1, 0) = 0.0;
				globalDOFMatrix.coeffRef((nd_map * 3) + 2, 0) = 1.0; // XY Rotation is free to move

				reducedDOF = reducedDOF + 1;
			}
			else if (cd.constraint_type == 3)
			{
				// Pin Roller End

				globalDOFMatrix.coeffRef((nd_map * 3) + 0, 0) = 1.0; // X is free to move
				globalDOFMatrix.coeffRef((nd_map * 3) + 1, 0) = 0.0;
				globalDOFMatrix.coeffRef((nd_map * 3) + 2, 0) = 1.0; // XY Rotation is free to move

				reducedDOF = reducedDOF + 2;
			}
		}
		else
		{
			// Nodes doesnt have Constraint
			globalDOFMatrix.coeffRef((nd_map * 3) + 0, 0) = 1.0;
			globalDOFMatrix.coeffRef((nd_map * 3) + 1, 0) = 1.0;
			globalDOFMatrix.coeffRef((nd_map * 3) + 2, 0) = 1.0;

			reducedDOF = reducedDOF + 3;
		}
	}
}

void freq_analysis_solver::get_reduced_global_matrices(Eigen::MatrixXd& reduced_globalStiffnessMatrix,
	Eigen::MatrixXd& reduced_globalMassMatrix,
	Eigen::MatrixXd& reduced_globalLoadMatrix,
	const Eigen::MatrixXd& globalStiffnessMatrix,
	const Eigen::MatrixXd& globalMassMatrix,
	const Eigen::MatrixXd& globalLoadMatrix,
	const Eigen::MatrixXd& globalDOFMatrix,
	const int& numDOF,
	std::ofstream& output_file)
{
	// Curtailment of Global stiffness and Global force matrix based on DOF
	// Get the reduced global stiffness matrix
	int r = 0;
	int s = 0;

	// Loop throug the Degree of freedom of indices
	for (int i = 0; i < numDOF; i++)
	{
		if (globalDOFMatrix(i, 0) == 0)
		{
			// constrained row index, so skip
			continue;
		}
		else
		{
			s = 0;
			for (int j = 0; j < numDOF; j++)
			{
				if (globalDOFMatrix(j, 0) == 0)
				{
					// constrained column index, so skip
					continue;
				}
				else
				{
					// Get the reduced matrices
					reduced_globalMassMatrix.coeffRef(r, s) = globalMassMatrix.coeffRef(i, j);
					reduced_globalStiffnessMatrix.coeffRef(r, s) = globalStiffnessMatrix.coeffRef(i, j);
					s++;
				}
			}

			reduced_globalLoadMatrix.coeffRef(r, 0) = globalLoadMatrix(i, 0);
			r++;
		}
	}

	if (print_matrix == true)
	{
		// Print the Reduced Global Stiffness, Global Mass and Global Force matrix
		output_file << "Reduced Global Stiffness Matrix" << std::endl;
		output_file << reduced_globalStiffnessMatrix << std::endl;
		output_file << std::endl;

		output_file << "Reduced Global Mass Matrix" << std::endl;
		output_file << reduced_globalMassMatrix << std::endl;
		output_file << std::endl;

		output_file << "Reduced Global Load Matrix" << std::endl;
		output_file << reduced_globalLoadMatrix << std::endl;
		output_file << std::endl;
	}
}

void freq_analysis_solver::get_modal_matrices(std::vector<double>& modalMass,
	std::vector<double>& modalStiff,
	std::vector<double>& modalForce,
	const Eigen::MatrixXd& reduced_eigenVectorsMatrix,
	const Eigen::MatrixXd& reduced_globalMassMatrix,
	const Eigen::MatrixXd& reduced_globalStiffnessMatrix,
	const Eigen::MatrixXd& reduced_globalLoadMatrix,
	const int& reducedDOF)
{
	// Get the modal matrices
	Eigen::MatrixXd modalMassMatrix(reducedDOF, reducedDOF);
	modalMassMatrix.setZero();

	modalMassMatrix = reduced_eigenVectorsMatrix.transpose() * reduced_globalMassMatrix * reduced_eigenVectorsMatrix;

	// Create modal stiffness matrix
	Eigen::MatrixXd modalStiffMatrix(reducedDOF, reducedDOF);
	modalStiffMatrix.setZero();

	modalStiffMatrix = reduced_eigenVectorsMatrix.transpose() * reduced_globalStiffnessMatrix * reduced_eigenVectorsMatrix;

	// Create modal force matrix
	Eigen::MatrixXd modalForceMatrix(reducedDOF, 1);
	modalForceMatrix.setZero();

	modalForceMatrix = reduced_eigenVectorsMatrix.transpose() * reduced_globalLoadMatrix;

	// Create the modal vectors
	modalMass.clear();
	modalStiff.clear();
	modalForce.clear();

	for (int i = 0; i < reducedDOF; i++)
	{
		modalMass.push_back(modalMassMatrix(i, i));
		modalStiff.push_back(modalStiffMatrix(i, i));
		modalForce.push_back(modalForceMatrix(i, 0));
	}
}

void freq_analysis_solver::get_reduced_modal_vector_matrix(Eigen::MatrixXd& reduced_eigenVectorsMatrix,
	const modal_analysis_result_store& modal_results,
	int& reducedDOF,
	std::ofstream& output_file)
{

	// Get the global eigen vectors matrix
	for (int i = 0; i < reducedDOF; i++)
	{
		std::vector<double> eigen_vector_column = modal_results.eigen_vectors_reduced.at(i);
		for (int j = 0; j < reducedDOF; j++)
		{
			reduced_eigenVectorsMatrix.coeffRef(j, i) = eigen_vector_column[j];
		}
	}

	if (print_matrix == true)
	{
		// Print the Reduced Global Eigen Vector matrix
		output_file << "Reduced Global EigenVector Matrix" << std::endl;
		output_file << reduced_eigenVectorsMatrix << std::endl;
		output_file << std::endl;
	}
}

void freq_analysis_solver::get_global_response(Eigen::MatrixXd& displ_ampl_RespMatrix,
	Eigen::MatrixXd& displ_phase_RespMatrix,
	const Eigen::MatrixXd& reduced_eigenVectorsMatrix,
	const Eigen::MatrixXd& globalDOFMatrix,
	const Eigen::MatrixXd& globalSupportInclinationMatrix,
	const std::vector<double>& modalMass,
	const std::vector<double>& modalStiff,
	const std::vector<double>& modalForce,
	const double& forcing_freq,
	const double& damping_ratio,
	const int& reducedDOF,
	const int& numDOF,
	std::ofstream& output_file)
{
	// Function to return the response for a forcing frequency (Sine Dwell at a single frequency)
	// Eigen Matrix to store the reduced Amplitude and Phase Response
		// After transformation
	Eigen::MatrixXd displ_ampl_RespMatrix_reduced(reducedDOF, 1);
	Eigen::MatrixXd displ_phase_RespMatrix_reduced(reducedDOF, 1);

	displ_ampl_RespMatrix_reduced.setZero();
	displ_phase_RespMatrix_reduced.setZero();

	get_reducedmodal_resp_matrix(displ_ampl_RespMatrix_reduced,
		displ_phase_RespMatrix_reduced,
		reduced_eigenVectorsMatrix,
		modalMass,
		modalStiff,
		modalForce,
		forcing_freq,
		damping_ratio,
		reducedDOF,
		numDOF);

	//_______________________________________________________________________________________________________________ 
	// Convert the reduced displacement amplitude and phase response with global ( including constraint location)
	Eigen::MatrixXd displ_ampl_RespMatrix_b4supp_trans(numDOF, 1);
	Eigen::MatrixXd displ_phase_RespMatrix_b4supp_trans(numDOF, 1);

	displ_ampl_RespMatrix_b4supp_trans.setZero();
	displ_phase_RespMatrix_b4supp_trans.setZero();

	get_global_resp_matrix(displ_ampl_RespMatrix_b4supp_trans,
		displ_phase_RespMatrix_b4supp_trans,
		displ_ampl_RespMatrix_reduced,
		displ_phase_RespMatrix_reduced,
		globalDOFMatrix,
		numDOF,
		reducedDOF,
		output_file);


	//if (print_matrix == true)
	//{
	//	output_file << "Global support " << forcing_freq << std::endl;
	//	output_file << globalSupportInclinationMatrix << std::endl;
	//	output_file << std::endl;
	//}



	// Apply support inclination transformation
	displ_ampl_RespMatrix = globalSupportInclinationMatrix * displ_ampl_RespMatrix_b4supp_trans;

	displ_phase_RespMatrix = globalSupportInclinationMatrix * displ_phase_RespMatrix_b4supp_trans;

}


void freq_analysis_solver::get_reducedmodal_resp_matrix(Eigen::MatrixXd& displ_ampl_RespMatrix_reduced,
	Eigen::MatrixXd& displ_phase_RespMatrix_reduced,
	const Eigen::MatrixXd& reduced_eigenVectorsMatrix,
	const std::vector<double>& modalMass,
	const std::vector<double>& modalStiff,
	const std::vector<double>& modalForce,
	const double& forcing_freq,
	const double& damping_ratio,
	const int& reducedDOF,
	const int& numDOF)
{
	// Eigen Matrix to store the reduced Amplitude and Phase Response
	// before the eigen vector tranformation
	Eigen::MatrixXd displ_ampl_RespMatrix_reduced_b4eig_trans(reducedDOF, 1);
	Eigen::MatrixXd displ_phase_RespMatrix_reduced_b4eig_trans(reducedDOF, 1);

	// Reduced Amplitude and Phase response set zero
	displ_ampl_RespMatrix_reduced_b4eig_trans.setZero();
	displ_phase_RespMatrix_reduced_b4eig_trans.setZero();

	// Displacement matrix column 
	for (int i = 0; i < reducedDOF; i++)
	{
		double modalomega_n = std::sqrt(modalStiff[i] / modalMass[i]);

		// Angular frequency ratio
		double omega_ratio = (2 * m_pi * forcing_freq) / modalomega_n;
		double omega_ratio_sq = omega_ratio * omega_ratio;

		// Maximum displacement response
		double max_displ_ampl_resp = (modalForce[i] / modalStiff[i]) *
			(1 / std::sqrt(std::pow((1 - omega_ratio_sq), 2) + std::pow((2 * damping_ratio * omega_ratio), 2)));

		displ_ampl_RespMatrix_reduced_b4eig_trans.coeffRef(i, 0) = max_displ_ampl_resp;

		// Phase of the maximum displacement response
		double max_displ_phase_resp = std::atan2((2 * damping_ratio * omega_ratio), (1 - omega_ratio_sq));

		displ_phase_RespMatrix_reduced_b4eig_trans.coeffRef(i, 0) = max_displ_phase_resp;
	}

	// Apply modal de-transformation
	displ_ampl_RespMatrix_reduced = reduced_eigenVectorsMatrix * displ_ampl_RespMatrix_reduced_b4eig_trans;

	displ_phase_RespMatrix_reduced = reduced_eigenVectorsMatrix * displ_phase_RespMatrix_reduced_b4eig_trans;
}

void freq_analysis_solver::get_global_resp_matrix(Eigen::MatrixXd& displ_ampl_RespMatrix_b4supp_trans,
	Eigen::MatrixXd& displ_phase_RespMatrix_b4supp_trans,
	const Eigen::MatrixXd& displ_ampl_RespMatrix_reduced,
	const Eigen::MatrixXd& displ_phase_RespMatrix_reduced,
	const Eigen::MatrixXd& globalDOFMatrix,
	const int& numDOF,
	const int& reducedDOF,
	std::ofstream& output_file)
{
	// Get global response matrix from the reduced matrices
	// Loop throug the Degree of freedom of indices
	int r = 0;

	// Loop throug the Degree of freedom of indices
	for (int i = 0; i < numDOF; i++)
	{
		if (globalDOFMatrix(i, 0) == 0)
		{
			// constrained row index, so skip
			continue;
		}
		else
		{
			// Get the reduced matrices
			displ_ampl_RespMatrix_b4supp_trans.coeffRef(i, 0) = displ_ampl_RespMatrix_reduced(r, 0);
			displ_phase_RespMatrix_b4supp_trans.coeffRef(i, 0) = displ_phase_RespMatrix_reduced(r, 0);
			r++;
		}
	}
}

void freq_analysis_solver::get_globalSupportInclinationMatrix(Eigen::MatrixXd& globalSupportInclinationMatrix,
	const nodes_list_store& model_nodes,
	const nodeconstraint_list_store& model_constarints,
	const int& numDOF,
	std::ofstream& output_file)
{
	// Create the global support inclination matrix
	int node_id = 0;

	// Transform the Nodal results with support inclination
	double constraint_angle_rad = 0.0;
	double support_Lcos = 0.0;
	double support_Msin = 0.0;

	for (auto& nd_m : model_nodes.nodeMap)
	{
		node_id = nd_m.first;
		int matrix_index = nodeid_map[node_id];


		if (model_constarints.constraintMap.find(node_id) == model_constarints.constraintMap.end())
		{
			// No constraint is in this node
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 0, (matrix_index * 3) + 0) = 1.0;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 0, (matrix_index * 3) + 1) = 0.0;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 0, (matrix_index * 3) + 2) = 0.0;

			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 1, (matrix_index * 3) + 0) = 0.0;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 1, (matrix_index * 3) + 1) = 1.0;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 1, (matrix_index * 3) + 2) = 0.0;

			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 2, (matrix_index * 3) + 0) = 0.0;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 2, (matrix_index * 3) + 1) = 0.0;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 2, (matrix_index * 3) + 2) = 1.0;
		}
		else
		{
			// Constraint present in this node
			constraint_angle_rad = (model_constarints.constraintMap.at(node_id).constraint_angle - 90.0f) * (m_pi / 180.0f); // Constrint angle in radians
			support_Lcos = std::cos(constraint_angle_rad); // cosine of support inclination
			support_Msin = std::sin(constraint_angle_rad); // sine of support inclination

			// Pin or Roller Support
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 0, (matrix_index * 3) + 0) = support_Lcos;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 0, (matrix_index * 3) + 1) = -1.0 * support_Msin;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 0, (matrix_index * 3) + 2) = 0.0;

			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 1, (matrix_index * 3) + 0) = support_Msin;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 1, (matrix_index * 3) + 1) = support_Lcos;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 1, (matrix_index * 3) + 2) = 0.0;

			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 2, (matrix_index * 3) + 0) = 0.0;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 2, (matrix_index * 3) + 1) = 0.0;
			globalSupportInclinationMatrix.coeffRef((matrix_index * 3) + 2, (matrix_index * 3) + 2) = 1.0;
		}
	}

	if (print_matrix == true)
	{
		// Print the Global support inclination matrix
		output_file << "Global Support Inclination Matrix" << std::endl;
		output_file << globalSupportInclinationMatrix << std::endl;
		output_file << std::endl;
	}
}