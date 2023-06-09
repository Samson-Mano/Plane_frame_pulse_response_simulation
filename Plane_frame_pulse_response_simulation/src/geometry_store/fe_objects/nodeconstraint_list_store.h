#pragma once
#include "nodes_list_store.h"

struct constraint_data
{
	int node_id;
	nodes_store* node;
	int constraint_type;
	float constraint_angle;
};


class nodeconstraint_list_store
{
public:
	int constraint_count = 0;

	nodeconstraint_list_store();
	~nodeconstraint_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_constraint(int& node_id, nodes_store* node, int& constraint_type, float& constraint_angle);
	void update_geometry_matrices(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	geom_parameters* geom_param_ptr;



};
