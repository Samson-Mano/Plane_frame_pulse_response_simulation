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

class pulse_analysis_solver
{
public:	
	const double m_pi = 3.14159265358979323846;
	bool print_matrix = true;

	pulse_analysis_solver();
	~pulse_analysis_solver();
	void pulse_analysis_start(const nodes_list_store& model_nodes,
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

};
