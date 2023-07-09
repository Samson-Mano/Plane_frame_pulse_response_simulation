#pragma once
#include <iostream>
#include <fstream>

// FE Objects
#include "../geometry_store/fe_objects/nodes_list_store.h"
#include "../geometry_store/fe_objects/elementline_list_store.h"
#include "../geometry_store/fe_objects/nodeconstraint_list_store.h"
#include "../geometry_store/fe_objects/nodeload_list_store.h"
#include "../geometry_store/fe_objects/nodepointmass_list_store.h"

// FE Results Modal Analysis
#include "../geometry_store/modal_result_objects/modal_analysis_result_store.h"

// FE Results Freq Analysis
#include "../geometry_store/freq_result_objects/freq_analysis_result_store.h"

#pragma warning(push)
#pragma warning (disable : 26451)
#pragma warning (disable : 26495)
#pragma warning (disable : 6255)
#pragma warning (disable : 26813)
#pragma warning (disable : 26454)

#include <Eigen/Dense>
#include <Eigen/Sparse>
// Define the sparse matrix type for the reduced global stiffness matrix
typedef Eigen::SparseMatrix<double> SparseMatrix;
#pragma warning(pop)



class freq_analysis_solver
{
public:
	const double m_pi = 3.14159265358979323846;
	bool print_matrix = true;

	freq_analysis_solver();
	~freq_analysis_solver();
	void freq_analysis_start(const nodes_list_store& model_nodes,
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
		bool& is_freq_analysis_complete);

private:
	std::unordered_map<int, int> nodeid_map;

	void get_global_stiffness_matrix(Eigen::MatrixXd& globalStiffnessMatrix,
		const elementline_list_store& model_lineelements,
		const std::unordered_map<int, material_data>& material_list,
		const nodeconstraint_list_store& model_constarints,
		std::ofstream& output_file);

	void get_element_stiffness_matrix(Eigen::MatrixXd& elementStiffnessMatrix,
		const elementline_store& ln,
		const material_data& elementline_material,
		const nodeconstraint_list_store& model_constarints,
		std::ofstream& output_file);

	void get_global_pointmass_matrix(Eigen::MatrixXd& globalPointMassMatrix,
		const nodes_list_store& model_nodes,
		const nodepointmass_list_store& model_ptmass,
		std::ofstream& output_file);

	void get_global_consistentmass_matrix(Eigen::MatrixXd& globalConsistentMassMatrix,
		const elementline_list_store& model_lineelements,
		const std::unordered_map<int, material_data>& material_list,
		const nodeconstraint_list_store& model_constarints,
		std::ofstream& output_file);

	void get_element_consistentmass_matrix(Eigen::MatrixXd& elementConsistentMassMatrix,
		const elementline_store& ln,
		const material_data& elementline_material,
		const nodeconstraint_list_store& model_constarints,
		std::ofstream& output_file);

	void get_global_load_matrix(Eigen::MatrixXd& globalLoadMatrix,
		const elementline_list_store& model_lineelements,
		const nodeload_list_store& model_loads,
		std::ofstream& output_file);

	void get_element_load_matrix(Eigen::MatrixXd& elementLoadMatrix,
		const elementline_store& ln,
		const nodeload_list_store& model_loads,
		std::ofstream& output_file);

	void get_global_dof_matrix(Eigen::MatrixXd& globalDOFMatrix,
		const nodes_list_store& model_nodes,
		const nodeconstraint_list_store& model_constarints,
		int& reducedDOF,
		std::ofstream& output_file);

	void get_reduced_global_matrices(Eigen::MatrixXd& reduced_globalStiffnessMatrix,
		Eigen::MatrixXd& reduced_globalMassMatrix,
		Eigen::MatrixXd& reduced_globalLoadMatrix,
		const Eigen::MatrixXd& globalStiffnessMatrix,
		const Eigen::MatrixXd& globalMassMatrix,
		const Eigen::MatrixXd& globalLoadMatrix,
		const Eigen::MatrixXd& globalDOFMatrix,
		const int& numDOF,
		std::ofstream& output_file);

	void get_modal_matrices(std::vector<double>& modalMass,
		std::vector<double>& modalStiff,
		std::vector<double>& modalForce,
		const Eigen::MatrixXd& reduced_eigenVectorsMatrix,
		const Eigen::MatrixXd& reduced_globalMassMatrix,
		const Eigen::MatrixXd& reduced_globalStiffnessMatrix,
		const Eigen::MatrixXd& reduced_globalLoadMatrix,
		const int& reducedDOF);

	void get_reduced_modal_vector_matrix(Eigen::MatrixXd& reduced_eigenVectorsMatrix,
		const modal_analysis_result_store& modal_results,
		int& reducedDOF,
		std::ofstream& output_file);

	void get_global_response(Eigen::MatrixXd& displ_ampl_RespMatrix,
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
		std::ofstream& output_file);

	void get_reducedmodal_resp_matrix(Eigen::MatrixXd& displ_ampl_RespMatrix_reduced,
		Eigen::MatrixXd& displ_phase_RespMatrix_reduced,
		const Eigen::MatrixXd& reduced_eigenVectorsMatrix,
		const std::vector<double>& modalMass,
		const std::vector<double>& modalStiff,
		const std::vector<double>& modalForce,
		const double& forcing_freq,
		const double& damping_ratio,
		const int& reducedDOF,
		const int& numDOF);

	void get_global_resp_matrix(Eigen::MatrixXd& displ_ampl_RespMatrix_b4supp_trans,
		Eigen::MatrixXd& displ_phase_RespMatrix_b4supp_trans,
		const Eigen::MatrixXd& displ_ampl_RespMatrix_reduced,
		const Eigen::MatrixXd& displ_phase_RespMatrix_reduced,
		const Eigen::MatrixXd& globalDOFMatrix,
		const int& numDOF,
		const int& reducedDOF,
		std::ofstream& output_file);

	void get_globalSupportInclinationMatrix(Eigen::MatrixXd& globalSupportInclinationMatrix,
		const nodes_list_store& model_nodes,
		const nodeconstraint_list_store& model_constarints,
		const int& numDOF,
		std::ofstream& output_file);

};
