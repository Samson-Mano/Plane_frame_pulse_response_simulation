#include "modal_analysis_solver.h"

modal_analysis_solver::modal_analysis_solver()
{
	// Empty constructor
}

modal_analysis_solver::~modal_analysis_solver()
{
	// Empty destructor
}

void modal_analysis_solver::modal_analysis_start(const nodes_list_store& model_nodes,
	const elementline_list_store& model_lineelements,
	const nodeconstraint_list_store& model_constarints,
	const nodepointmass_list_store& model_ptmass,
	const std::unordered_map<int, material_data>& material_list,
	modal_analysis_result_store& modal_results,
	modal_nodes_list_store& modal_result_nodes,
	modal_elementline_list_store& modal_result_lineelements,
	bool& is_modal_analysis_complete)
{
	// Main solver call
	is_modal_analysis_complete = false;

	// Check the model
	// Number of nodes
	if (model_nodes.node_count == 0)
	{
		return;
	}

	// Number of elements
	if (model_lineelements.elementline_count == 0)
	{
		return;
	}

	//____________________________________________

	// Create a node ID map (to create a nodes as ordered and numbered from 0,1,2...n)
	int i = 0;
	for (auto& nd : model_nodes.nodeMap)
	{
		nodeid_map[nd.first] = i;
		i++;
	}

	// Create a file to keep track of matrices
	std::ofstream output_file;
	output_file.open("fe_matrices.txt");

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

	get_global_consistentmass_matrix(globalConsistentMassMatrix,
		model_lineelements,
		material_list,
		model_constarints,
		output_file);

	//____________________________________________________________________________________________________________________
	// Global Consistent Mass Matrix
	Eigen::MatrixXd globalMassMatrix(numDOF, numDOF);
	globalMassMatrix.setZero();

	globalMassMatrix = globalPointMassMatrix + globalConsistentMassMatrix;

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
	// Create Reduced Global Mass and Global stiffness matrix



}

void modal_analysis_solver::get_global_stiffness_matrix(Eigen::MatrixXd& globalStiffnessMatrix,
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
		elementStiffnessMatrix = Eigen::MatrixXd::Zero();

		get_element_stiffness_matrix(elementStiffnessMatrix, ln, elementline_material, model_constarints, output_file);

		// Get the Node ID
		int sn_id = nodeid_map[ln.startNode->node_id]; // get the ordered map of the start node ID
		int en_id = nodeid_map[ln.endNode->node_id]; // get the ordered map of the end node ID

		globalStiffnessMatrix.block<3, 3>(sn_id * 3, sn_id * 3) += elementStiffnessMatrix.block<3, 3>(0, 0);
		globalStiffnessMatrix.block<3, 3>(sn_id * 3, en_id * 3) += elementStiffnessMatrix.block<3, 3>(0, 3);
		globalStiffnessMatrix.block<3, 3>(en_id * 3, sn_id * 3) += elementStiffnessMatrix.block<3, 3>(3, 0);
		globalStiffnessMatrix.block<3, 3>(en_id * 3, en_id * 3) += elementStiffnessMatrix.block<3, 3>(3, 3);
	}

	if (print_matrix == true)
	{
		// Print the Global Stiffness matrix
		output_file << "Global Stiffness Matrix" << std::endl;
		output_file << globalStiffnessMatrix << std::endl;
		output_file << std::endl;
	}
}

void modal_analysis_solver::get_element_stiffness_matrix(Eigen::MatrixXd& elementStiffnessMatrix,
	const elementline_store& ln,
	const material_data& elementline_material,
	const nodeconstraint_list_store& model_constarints, std::ofstream& output_file)
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
	L_transformation_matrix = Eigen::MatrixXd::Zero();

	L_transformation_matrix.row(0) = Eigen::RowVectorXd(Lcos, Msin, 0.0, 0.0, 0.0, 0.0);
	L_transformation_matrix.row(1) = Eigen::RowVectorXd(-Msin, Lcos, 0.0, 0.0, 0.0, 0.0);
	L_transformation_matrix.row(2) = Eigen::RowVectorXd(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
	L_transformation_matrix.row(3) = Eigen::RowVectorXd(0.0, 0.0, 0.0, Lcos, Msin, 0.0);
	L_transformation_matrix.row(4) = Eigen::RowVectorXd(0.0, 0.0, 0.0, -Msin, Lcos, 0.0);
	L_transformation_matrix.row(5) = Eigen::RowVectorXd(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	//_________________________________________________________
	// Local element stiffness matrix
	Eigen::MatrixXd local_element_stiffness_matrix(6, 6);
	local_element_stiffness_matrix = Eigen::MatrixXd::Zero();

	double k1 = (elementline_material.youngs_mod * elementline_material.cs_area) / eLength;
	double k2 = (elementline_material.youngs_mod * elementline_material.second_moment_of_area) / (eLength * eLength * eLength);
	double k3 = (elementline_material.youngs_mod * elementline_material.second_moment_of_area) / (eLength * eLength);
	double k4 = (elementline_material.youngs_mod * elementline_material.second_moment_of_area) / eLength;

	local_element_stiffness_matrix.row(0) = Eigen::RowVectorXd(k1, 0.0, 0.0, -1.0 * k1, 0.0, 0.0);
	local_element_stiffness_matrix.row(1) = Eigen::RowVectorXd(0.0, 12.0 * k2, 6.0 * k3, 0.0, -12.0 * k2, 6.0 * k3);
	local_element_stiffness_matrix.row(2) = Eigen::RowVectorXd(0.0, 6.0 * k3, 4.0 * k4, 0.0, -6.0 * k3, 2.0 * k4);
	local_element_stiffness_matrix.row(3) = Eigen::RowVectorXd(-1.0 * k1, 0.0, 0.0, k1, 0.0, 0.0);
	local_element_stiffness_matrix.row(4) = Eigen::RowVectorXd(0.0, -12.0 * k2, -6.0 * k3, 0.0, 12.0 * k2, -6.0 * k3);
	local_element_stiffness_matrix.row(5) = Eigen::RowVectorXd(0.0, 6.0 * k3, 2.0 * k4, 0.0, -6.0 * k3, 4.0 * k4);

	//_________________________________________________________
	// Transformed element stiffness matrix
	Eigen::MatrixXd e_stiffness_matrix(6, 6);
	e_stiffness_matrix = Eigen::MatrixXd::Zero();

	e_stiffness_matrix = L_transformation_matrix.transpose() * local_element_stiffness_matrix * L_transformation_matrix;
	//_________________________________________________________

	// Transformation matrices to include support inclinatation
	Eigen::MatrixXd s_transformation_matrix(6, 6);
	s_transformation_matrix = Eigen::MatrixXd::Zero(); // support inclination transformation matrix

	int constraint_type;
	double constraint_angle_rad;
	double support_Lcos;
	double support_Msin;

	// Start node support inclination
	if (model_constarints.constraintMap.find(ln.startNode->node_id) == model_constarints.constraintMap.end())
	{
		// No constraint at the start node
		s_transformation_matrix.row(0) = Eigen::RowVector4d(1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		s_transformation_matrix.row(1) = Eigen::RowVector4d(0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
		s_transformation_matrix.row(2) = Eigen::RowVector4d(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
	}
	else
	{
		constraint_type = model_constarints.constraintMap.at(ln.startNode->node_id).constraint_type; // Constrint type (0 - pin support, 1 - roller support)
		constraint_angle_rad = (model_constarints.constraintMap.at(ln.startNode->node_id).constraint_angle - 90.0) * (m_pi / 180.0f); // Constrint angle in radians
		support_Lcos = std::cos(constraint_angle_rad); // cosine of support inclination
		support_Msin = std::sin(constraint_angle_rad); // sine of support inclination

		// Pin or Roller Support
		s_transformation_matrix.row(0) = Eigen::RowVector4d(support_Lcos, -support_Msin, 0.0, 0.0, 0.0, 0.0);
		s_transformation_matrix.row(1) = Eigen::RowVector4d(support_Msin, support_Lcos, 0.0, 0.0, 0.0, 0.0);
		s_transformation_matrix.row(2) = Eigen::RowVector4d(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
	}

	// End node support inclination
	if (model_constarints.constraintMap.find(ln.endNode->node_id) == model_constarints.constraintMap.end())
	{
		// No constraint at the end node
		s_transformation_matrix.row(3) = Eigen::RowVector4d(0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
		s_transformation_matrix.row(4) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		s_transformation_matrix.row(5) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	}
	else
	{
		constraint_type = model_constarints.constraintMap.at(ln.endNode->node_id).constraint_type; // Constrint type (0 - pin support, 1 - roller support)
		constraint_angle_rad = (model_constarints.constraintMap.at(ln.endNode->node_id).constraint_angle - 90.0) * (m_pi / 180.0f); // Constrint angle in radians
		support_Lcos = std::cos(constraint_angle_rad); // cosine of support inclination
		support_Msin = std::sin(constraint_angle_rad); // sine of support inclination

		// Pin or Roller Support
		s_transformation_matrix.row(3) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, support_Lcos, -support_Msin);
		s_transformation_matrix.row(4) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, support_Msin, support_Lcos);
		s_transformation_matrix.row(5) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	}

	// Calculate the element stiffness matrix
	elementStiffnessMatrix = s_transformation_matrix.transpose() * e_stiffness_matrix * s_transformation_matrix;
}


void modal_analysis_solver::get_global_pointmass_matrix(Eigen::MatrixXd& globalPointMassMatrix,
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

			globalPointMassMatrix((nd_map * 3) + 0, (nd_map * 3) + 0) += ptm.ptmass_x;
			globalPointMassMatrix((nd_map * 3) + 1, (nd_map * 3) + 1) += ptm.ptmass_y;
			globalPointMassMatrix((nd_map * 3) + 2, (nd_map * 3) + 2) += ptm.ptmass_xy;
		}
		else
		{
			// Nodes doesnt have point mass
			globalPointMassMatrix((nd_map * 3) + 0, (nd_map * 3) + 0) += 0.0;
			globalPointMassMatrix((nd_map * 3) + 1, (nd_map * 3) + 1) += 0.0;
			globalPointMassMatrix((nd_map * 3) + 2, (nd_map * 3) + 2) += 0.0;
		}
	}

	if (print_matrix == true)
	{
		// Print the Global Force matrix
		output_file << "Global Point Mass Matrix" << std::endl;
		output_file << globalPointMassMatrix << std::endl;
		output_file << std::endl;
	}
}

void modal_analysis_solver::get_global_consistentmass_matrix(Eigen::MatrixXd& globalConsistentMassMatrix,
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
		elementConsistentMassMatrix = Eigen::MatrixXd::Zero();

		get_element_consistentmass_matrix(elementConsistentMassMatrix, ln, elementline_material, model_constarints, output_file);

		// Get the Node ID
		int sn_id = nodeid_map[ln.startNode->node_id]; // get the ordered map of the start node ID
		int en_id = nodeid_map[ln.endNode->node_id]; // get the ordered map of the end node ID

		globalConsistentMassMatrix.block<3, 3>(sn_id * 3, sn_id * 3) += elementConsistentMassMatrix.block<3, 3>(0, 0);
		globalConsistentMassMatrix.block<3, 3>(sn_id * 3, en_id * 3) += elementConsistentMassMatrix.block<3, 3>(0, 3);
		globalConsistentMassMatrix.block<3, 3>(en_id * 3, sn_id * 3) += elementConsistentMassMatrix.block<3, 3>(3, 0);
		globalConsistentMassMatrix.block<3, 3>(en_id * 3, en_id * 3) += elementConsistentMassMatrix.block<3, 3>(3, 3);
	}

	if (print_matrix == true)
	{
		// Print the Global Stiffness matrix
		output_file << "Global Consistent Mass Matrix" << std::endl;
		output_file << globalConsistentMassMatrix << std::endl;
		output_file << std::endl;
	}
}

void modal_analysis_solver::get_element_consistentmass_matrix(Eigen::MatrixXd& elementConsistentMassMatrix,
	const elementline_store& ln,
	const material_data& elementline_material,
	const nodeconstraint_list_store& model_constarints,
	std::ofstream& output_file)
{
	// Create element consistent mass matrix
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
	L_transformation_matrix = Eigen::MatrixXd::Zero();

	L_transformation_matrix.row(0) = Eigen::RowVectorXd(Lcos, Msin, 0.0, 0.0, 0.0, 0.0);
	L_transformation_matrix.row(1) = Eigen::RowVectorXd(-Msin, Lcos, 0.0, 0.0, 0.0, 0.0);
	L_transformation_matrix.row(2) = Eigen::RowVectorXd(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
	L_transformation_matrix.row(3) = Eigen::RowVectorXd(0.0, 0.0, 0.0, Lcos, Msin, 0.0);
	L_transformation_matrix.row(4) = Eigen::RowVectorXd(0.0, 0.0, 0.0, -Msin, Lcos, 0.0);
	L_transformation_matrix.row(5) = Eigen::RowVectorXd(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	//_________________________________________________________
	// Local element stiffness matrix
	Eigen::MatrixXd local_element_consistentmass_matrix(6, 6);
	local_element_consistentmass_matrix = Eigen::MatrixXd::Zero();

	double k1 = (elementline_material.mat_density * elementline_material.cs_area * eLength) / 420.0f;
	double k2 = k1 * eLength;
	double k3 = k1 * (eLength * eLength);

	local_element_consistentmass_matrix.row(0) = Eigen::RowVectorXd(140.0 * k1, 0.0, 0.0, 70.0 * k1, 0.0, 0.0);
	local_element_consistentmass_matrix.row(1) = Eigen::RowVectorXd(0.0, 156.0 * k1, 22.0 * k2, 0.0, 54.0 * k1, -13.0 * k2);
	local_element_consistentmass_matrix.row(2) = Eigen::RowVectorXd(0.0, 22.0 * k2, 4.0 * k3, 0.0, 13.0 * k2, -3.0 * k3);
	local_element_consistentmass_matrix.row(3) = Eigen::RowVectorXd(70.0 * k1, 0.0, 0.0, 140.0 * k1, 0.0, 0.0);
	local_element_consistentmass_matrix.row(4) = Eigen::RowVectorXd(0.0, 54.0 * k1, 13.0 * k2, 0.0, 156.0 * k1, -22.0 * k2);
	local_element_consistentmass_matrix.row(5) = Eigen::RowVectorXd(0.0, -13.0 * k2, -3.0 * k3, 0.0, -22.0 * k2, 4.0 * k3);

	//_________________________________________________________
	// Transformed element stiffness matrix
	Eigen::MatrixXd e_consistentMass_matrix(6, 6);
	e_consistentMass_matrix = Eigen::MatrixXd::Zero();

	e_consistentMass_matrix = L_transformation_matrix.transpose() * local_element_consistentmass_matrix * L_transformation_matrix;
	//_________________________________________________________

	// Transformation matrices to include support inclinatation
	Eigen::MatrixXd s_transformation_matrix(6, 6);
	s_transformation_matrix = Eigen::MatrixXd::Zero(); // support inclination transformation matrix

	int constraint_type;
	double constraint_angle_rad;
	double support_Lcos;
	double support_Msin;

	// Start node support inclination
	if (model_constarints.constraintMap.find(ln.startNode->node_id) == model_constarints.constraintMap.end())
	{
		// No constraint at the start node
		s_transformation_matrix.row(0) = Eigen::RowVector4d(1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		s_transformation_matrix.row(1) = Eigen::RowVector4d(0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
		s_transformation_matrix.row(2) = Eigen::RowVector4d(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
	}
	else
	{
		constraint_type = model_constarints.constraintMap.at(ln.startNode->node_id).constraint_type; // Constrint type (0 - pin support, 1 - roller support)
		constraint_angle_rad = (model_constarints.constraintMap.at(ln.startNode->node_id).constraint_angle - 90.0) * (m_pi / 180.0f); // Constrint angle in radians
		support_Lcos = std::cos(constraint_angle_rad); // cosine of support inclination
		support_Msin = std::sin(constraint_angle_rad); // sine of support inclination

		// Pin or Roller Support
		s_transformation_matrix.row(0) = Eigen::RowVector4d(support_Lcos, -support_Msin, 0.0, 0.0, 0.0, 0.0);
		s_transformation_matrix.row(1) = Eigen::RowVector4d(support_Msin, support_Lcos, 0.0, 0.0, 0.0, 0.0);
		s_transformation_matrix.row(2) = Eigen::RowVector4d(0.0, 0.0, 1.0, 0.0, 0.0, 0.0);
	}

	// End node support inclination
	if (model_constarints.constraintMap.find(ln.endNode->node_id) == model_constarints.constraintMap.end())
	{
		// No constraint at the end node
		s_transformation_matrix.row(3) = Eigen::RowVector4d(0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
		s_transformation_matrix.row(4) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		s_transformation_matrix.row(5) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	}
	else
	{
		constraint_type = model_constarints.constraintMap.at(ln.endNode->node_id).constraint_type; // Constrint type (0 - pin support, 1 - roller support)
		constraint_angle_rad = (model_constarints.constraintMap.at(ln.endNode->node_id).constraint_angle - 90.0) * (m_pi / 180.0f); // Constrint angle in radians
		support_Lcos = std::cos(constraint_angle_rad); // cosine of support inclination
		support_Msin = std::sin(constraint_angle_rad); // sine of support inclination

		// Pin or Roller Support
		s_transformation_matrix.row(3) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, support_Lcos, -support_Msin);
		s_transformation_matrix.row(4) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, support_Msin, support_Lcos);
		s_transformation_matrix.row(5) = Eigen::RowVector4d(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	}

	// Calculate the element stiffness matrix
	elementConsistentMassMatrix = s_transformation_matrix.transpose() * e_consistentMass_matrix * s_transformation_matrix;
}

void modal_analysis_solver::get_global_dof_matrix(Eigen::MatrixXd& globalDOFMatrix, 
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

				globalDOFMatrix((nd_map * 3) + 0, 0) = 0.0;
				globalDOFMatrix((nd_map * 3) + 1, 0) = 0.0;
				globalDOFMatrix((nd_map * 3) + 2, 0) = 0.0;
			}
			else if (cd.constraint_type == 1)
			{
				// Fixed Roller end

				globalDOFMatrix((nd_map * 3) + 0, 0) = 1.0; // X is free to move
				globalDOFMatrix((nd_map * 3) + 1, 0) = 0.0;
				globalDOFMatrix((nd_map * 3) + 2, 0) = 0.0;

				reducedDOF = reducedDOF + 1;
			}
			else if (cd.constraint_type == 2)
			{
				// Pin End

				globalDOFMatrix((nd_map * 3) + 0, 0) = 0.0; 
				globalDOFMatrix((nd_map * 3) + 1, 0) = 0.0;
				globalDOFMatrix((nd_map * 3) + 2, 0) = 1.0; // XY Rotation is free to move

				reducedDOF = reducedDOF + 1;
			}
			else if (cd.constraint_type == 3)
			{
				// Pin Roller End

				globalDOFMatrix((nd_map * 3) + 0, 0) = 1.0; // X is free to move
				globalDOFMatrix((nd_map * 3) + 1, 0) = 0.0;
				globalDOFMatrix((nd_map * 3) + 2, 0) = 1.0; // XY Rotation is free to move

				reducedDOF = reducedDOF + 2;
			}
		}
		else
		{
			// Nodes doesnt have Constraint
			globalDOFMatrix((nd_map * 3) + 0, 0) = 1.0;
			globalDOFMatrix((nd_map * 3) + 1, 0) = 1.0;
			globalDOFMatrix((nd_map * 3) + 2, 0) = 1.0;

			reducedDOF = reducedDOF + 3;
		}
	}

	if (print_matrix == true)
	{
		// Print the Global Force matrix
		output_file << "Global DOF Matrix" << std::endl;
		output_file << globalDOFMatrix << std::endl;
		output_file << std::endl;
	}
}

