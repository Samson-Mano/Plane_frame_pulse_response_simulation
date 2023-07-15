#include "dynamic_line_list_store.h"

dynamic_line_list_store::dynamic_line_list_store()
{
	// Empty constructor
}

dynamic_line_list_store::~dynamic_line_list_store()
{
	// Empty destructor
}

void dynamic_line_list_store::init(geom_parameters* geom_param_ptr)
{
	// Set the geometry parameters
	this->geom_param_ptr = geom_param_ptr;

	// Create the point shader
	std::filesystem::path shadersPath = geom_param_ptr->resourcePath;

	dyn_line_shader.create_shader((shadersPath.string() + "/resources/shaders/point_vert_shader.vert").c_str(),
		(shadersPath.string() + "/resources/shaders/point_frag_shader.frag").c_str());

	// Delete all the labels
	dyn_line_count = 0;
	dyn_lineMap.clear();
}

void dynamic_line_list_store::add_line(int& line_id, 
	glm::vec2& line_startpt_loc, glm::vec2& line_endpt_loc,
	std::vector<glm::vec2>& line_startpt_offset, std::vector<glm::vec2>& line_endpt_offset,
	std::vector<glm::vec3>& line_startpt_color, std::vector<glm::vec3>& line_endpt_color)
{
	// Create a temporary points
	dynamic_line_store dyn_temp_ln;
	dyn_temp_ln.line_id = line_id;

	// Line points
	dyn_temp_ln.line_startpt_loc = line_startpt_loc;
	dyn_temp_ln.line_endpt_loc = line_endpt_loc;

	// Line offsets
	dyn_temp_ln.line_startpt_offset = line_startpt_offset;
	dyn_temp_ln.line_endpt_offset = line_endpt_offset;

	// Line Color
	dyn_temp_ln.line_startpt_color = line_startpt_color;
	dyn_temp_ln.line_endpt_color = line_endpt_color;

	// Reserve space for the new element
	dyn_lineMap.reserve(dyn_lineMap.size() + 1);

	// Add to the list
	dyn_lineMap.push_back(dyn_temp_ln);

	// Iterate the point count
	dyn_line_count++;
}

void dynamic_line_list_store::set_buffer()
{
}

void dynamic_line_list_store::paint_lines()
{
}

void dynamic_line_list_store::clear_lines()
{
	// Delete all the lines
	dyn_line_count = 0;
	dyn_lineMap.clear();
}

void dynamic_line_list_store::update_opengl_uniforms(bool set_modelmatrix, 
	bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
	if (set_modelmatrix == true)
	{
		// set the model matrix
		dyn_line_shader.setUniform("geom_scale", static_cast<float>(geom_param_ptr->geom_scale));
		dyn_line_shader.setUniform("transparency", 1.0f);

		dyn_line_shader.setUniform("modelMatrix", geom_param_ptr->modelMatrix, false);
	}

	if (set_pantranslation == true)
	{
		// set the pan translation
		dyn_line_shader.setUniform("panTranslation", geom_param_ptr->panTranslation, false);
	}

	if (set_zoomtranslation == true)
	{
		// set the zoom translation
		dyn_line_shader.setUniform("zoomscale", static_cast<float>(geom_param_ptr->zoom_scale));
	}

	if (set_transparency == true)
	{
		// set the alpha transparency
		dyn_line_shader.setUniform("transparency", static_cast<float>(geom_param_ptr->geom_transparency));
	}

	if (set_deflscale == true)
	{
		// set the deflection scale
		dyn_line_shader.setUniform("normalized_deflscale", static_cast<float>(geom_param_ptr->normalized_defl_scale));
		dyn_line_shader.setUniform("deflscale", static_cast<float>(geom_param_ptr->defl_scale));
	}
}

void dynamic_line_list_store::get_line_buffer(dynamic_line_store& ln, float* line_vertices, unsigned int& line_v_index, 
	unsigned int* line_vertex_indices, unsigned int& line_i_index)
{


}