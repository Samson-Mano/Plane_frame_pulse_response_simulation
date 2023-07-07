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
	xy_response_data.clear();

}

void freq_analysis_result_store::add_node_response(int node_id,
	double freq_values,
	double x_ampl_values,
	double x_phase_values,
	double y_ampl_values,
	double y_phase_values,
	double xy_ampl_values,
	double xy_phase_values)
{
	// X Response




	// Y Response


	// XY Response


}
