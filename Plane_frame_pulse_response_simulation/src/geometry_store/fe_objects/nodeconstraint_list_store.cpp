#include "nodeconstraint_list_store.h"

nodeconstraint_list_store::nodeconstraint_list_store()
{
	// Empty constructor
}

nodeconstraint_list_store::~nodeconstraint_list_store()
{
	// Empty destructor
}

void nodeconstraint_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

}

void nodeconstraint_list_store::add_constraint(int& node_id, nodes_store* node, int& constraint_type, float& constraint_angle)
{


}
