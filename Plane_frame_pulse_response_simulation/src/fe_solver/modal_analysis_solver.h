#pragma once
#include <iostream>
#include <fstream>
#include "../tool_window/modal_analysis_window.h"
#include "../geometry_store/geom_store.h"

#pragma warning(push)
#pragma warning (disable : 26451)
#pragma warning (disable : 26495)

#include <Eigen/Dense>
#include <Eigen/Sparse>
// Define the sparse matrix type for the reduced global stiffness matrix
typedef Eigen::SparseMatrix<double> SparseMatrix;
#pragma warning(pop)

class modal_analysis_solver
{
public:
	const double m_pi = 3.14159265358979323846;
	bool print_matrix = true;

	modal_analysis_solver();
	~modal_analysis_solver();
	void modal_analysis_start();
private:

};


