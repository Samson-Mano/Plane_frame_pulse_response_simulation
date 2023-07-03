#include "freq_analysis_result_store.h"

freq_analysis_result_store::freq_analysis_result_store()
{
	// Empty constructor
}

freq_analysis_result_store::~freq_analysis_result_store()
{
	// Empty destructor
}

void freq_analysis_result_store::clear_data()
{
	// Clear all the values
	nodeid_map.clear();
	x_response_data.clear();
	y_response_data.clear();
	xy_rms_response_data.clear();

}

void freq_analysis_result_store::add_node_response(int node_id, 
	std::vector<double> freq_values, 
	std::vector<double> ampl_values, 
	std::vector<double> phase_values)
{



}
