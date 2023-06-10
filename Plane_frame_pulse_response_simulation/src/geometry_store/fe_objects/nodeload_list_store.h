#pragma once
#include "nodes_list_store.h"

struct load_data
{
	int load_id = 0;
	glm::vec2 load_loc = glm::vec2(0);
	double load_value = 0.0;
	double load_angle = 0.0;
};

class nodeload_list_store
{
public:
	int load_count = 0;

	nodeload_list_store();
	~nodeload_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_load(int& load_id, glm::vec2 load_loc, double& load_value, double& load_angle);
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:

};
