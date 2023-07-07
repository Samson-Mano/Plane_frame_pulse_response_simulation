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
	std::unordered_map<int, int> nodeid_map;
	std::unordered_map<int, response_data> x_response_data;
	std::unordered_map<int, response_data> y_response_data;
	std::unordered_map<int, response_data> xy_response_data;


	freq_analysis_result_store();
	~freq_analysis_result_store();
	void clear_data();
	void add_node_response(int node_id, 
		double freq_values, 
		double x_ampl_values, 
		double x_phase_values,
		double y_ampl_values,
		double y_phase_values,
		double xy_ampl_values,
		double xy_phase_values);
private:
};
