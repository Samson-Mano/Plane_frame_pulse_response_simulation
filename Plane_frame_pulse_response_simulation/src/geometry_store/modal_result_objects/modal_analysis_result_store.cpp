#include "modal_analysis_result_store.h"

modal_analysis_result_store::modal_analysis_result_store()
{
	// Empty constructor
}

modal_analysis_result_store::~modal_analysis_result_store()
{
	// Empty destructor
}

void modal_analysis_result_store::clear_data()
{
	// Clear the eigen values and eigen vectors
	eigen_values.clear();
	eigen_vectors.clear();
}

void modal_analysis_result_store::add_eigen_data(int mode_number, double eigen_value, std::vector<double> eigen_vector)
{
	// insert the eigen value
	eigen_values.insert({ mode_number,eigen_value });

	// insert the eigen vectors of this particular eigen value
	eigen_vectors.insert({ mode_number,eigen_vector });

}
