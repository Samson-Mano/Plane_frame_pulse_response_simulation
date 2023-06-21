#pragma once

#include <unordered_map>
#include <vector>

class modal_analysis_result_store
{
public:
	modal_analysis_result_store();
	~modal_analysis_result_store();
	void clear_data();
	void add_eigen_data(int mode_number, double eigen_value, std::vector<double> eigen_vector);
private:
	std::unordered_map<int, double> eigen_values;
	std::unordered_map<int, std::vector<double>> eigen_vectors;
};
