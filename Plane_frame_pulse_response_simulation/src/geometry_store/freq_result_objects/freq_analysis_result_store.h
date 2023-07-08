#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>

struct response_data
{
	std::vector<double> freq_values; // Frequency values
	std::vector<double> ampl_values; // Amplitude values
	std::vector<double> phase_values; // Phase values
};


class freq_analysis_result_store
{
public:
	std::vector<int> node_id_values; // Node ID values
	std::unordered_map<int, response_data> x_response_data; // X response
	std::unordered_map<int, response_data> y_response_data; // Y response
	std::unordered_map<int, response_data> xy_response_data; // XY response
	std::unordered_map<int, response_data> displ_response_data; // Displ response
	bool is_freq_analysis_complete = false;

	freq_analysis_result_store();
	~freq_analysis_result_store();
	void clear_data();
	void add_node_response(const int& node_id, 
		const double& freq_values, 
		const double& x_ampl_values,
		const double& x_phase_values,
		const double& y_ampl_values,
		const double& y_phase_values,
		const double& xy_ampl_values,
		const double& xy_phase_values);
private:
};
