#include "dynamic_point_list_store.h"

dynamic_point_list_store::dynamic_point_list_store()
{
	// Empty constructor
}

dynamic_point_list_store::~dynamic_point_list_store()
{
	// Empty destructor
}

void dynamic_point_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Create the point shader
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	dyn_point_shader.create_shader((shadersPath.string() + "/resources/shaders/point_vert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/point_frag_shader.frag").c_str());

	// Delete all the labels
	dyn_point_count = 0;
	dyn_pointMap.clear();
}

void dynamic_point_list_store::add_point(int& point_id, glm::vec2& point_loc, std::vector<glm::vec2>& point_offset, std::vector<glm::vec3>& point_color)
{
	dynamic_point_store dyn_temp_pt;
	dyn_temp_pt.point_id = point_id;
	dyn_temp_pt.point_loc = point_loc;
	dyn_temp_pt.point_offset = point_offset; // Dynamic point offset
	dyn_temp_pt.point_color = point_color; // Dynamic point color
	dyn_temp_pt.offset_pt_count = static_cast<int>(point_offset.size());

	//___________________________________________________________________
	// Reserve space for the new element
	dyn_pointMap.reserve(dyn_pointMap.size() + 1);

	// Add to the list
	dyn_pointMap.push_back(dyn_temp_pt);

	// Iterate the point count
	dyn_point_count++;
}

void dynamic_point_list_store::set_buffer()
{
}

void dynamic_point_list_store::paint_points()
{
}

void dynamic_point_list_store::clear_points()
{
	// Delete all the points
	dyn_point_count = 0;
	dyn_pointMap.clear();
}

void dynamic_point_list_store::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	if (set_modelmatrix == true)
	{
		// set the model matrix
		dyn_point_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		dyn_point_shader.setUniform("transparency", 1.0f);

		dyn_point_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		dyn_point_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_zoomtranslation == true)
	{
		// set the zoom translation
		dyn_point_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		dyn_point_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}

	if (set_deflscale == true)
	{
		// set the deflection scale
		dyn_point_shader.setUniform("normalized_deflscale", static_cast<float>(geom_param_ptr->normalized_defl_scale));
		dyn_point_shader.setUniform("deflscale", static_cast<float>(geom_param_ptr->defl_scale));
	}
}

void dynamic_point_list_store::get_node_buffer(dynamic_point_store& pt, float* point_vertices, unsigned int& point_v_index, unsigned int* point_indices, unsigned int& point_i_index)
{


}
