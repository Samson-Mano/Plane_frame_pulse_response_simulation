#include "geom_store.h"

geom_store::geom_store()
{
	// Empty constructor
}

geom_store::~geom_store()
{
	// Empty Destructor
}

void geom_store::init()
{
	// Initialize
	// Initialize the geometry parameters
	geom_param.init();


}

void geom_store::fini()
{
	// Deinitialize
	is_geometry_set = false;

}

void geom_store::updateWindowDimension(const int& window_width, const int& window_height)
{
	// Update the window dimension
	this->geom_param.window_width = window_width;
	this->geom_param.window_height = window_height;

	if (is_geometry_set == true)
	{
		// Update the model matrix
		// set_model_matrix();
		// !! Zoom to fit operation during window resize is handled in mouse event class !!
	}
}