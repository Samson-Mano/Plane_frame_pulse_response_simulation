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
	node_id_values.clear();
	x_response_data.clear();
	y_response_data.clear();
	xy_response_data.clear();
	displ_response_data.clear();
	is_freq_analysis_complete = false;
}

void freq_analysis_result_store::add_node_response(const int& node_id,
	const double& freq_values,
	const double& x_ampl_values,
	const double& x_phase_values,
	const double& y_ampl_values,
	const double& y_phase_values,
	const double& xy_ampl_values,
	const double& xy_phase_values)
{
	// X Response
	x_response_data[node_id].freq_values.push_back(freq_values);
	x_response_data[node_id].ampl_values.push_back(x_ampl_values);
	x_response_data[node_id].phase_values.push_back(x_phase_values);

	// Y Response
	y_response_data[node_id].freq_values.push_back(freq_values);
	y_response_data[node_id].ampl_values.push_back(y_ampl_values);
	y_response_data[node_id].phase_values.push_back(y_phase_values);

	// XY Response
	xy_response_data[node_id].freq_values.push_back(freq_values);
	xy_response_data[node_id].ampl_values.push_back(xy_ampl_values);
	xy_response_data[node_id].phase_values.push_back(xy_phase_values);

	// Resultant response
	displ_response_data[node_id].freq_values.push_back(freq_values);

	double displ_ampl = std::sqrt(std::pow(x_ampl_values, 2) + std::pow(y_ampl_values, 2));
	double displ_phase = std::atan2(y_ampl_values, x_ampl_values);

	displ_response_data[node_id].ampl_values.push_back(displ_ampl);
	displ_response_data[node_id].phase_values.push_back(displ_phase);
}
