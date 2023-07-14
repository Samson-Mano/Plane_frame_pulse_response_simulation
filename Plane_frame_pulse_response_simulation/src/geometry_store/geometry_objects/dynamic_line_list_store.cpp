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
}

void dynamic_line_list_store::add_line(int& line_id, 
	glm::vec2& line_startpt_loc, 	glm::vec2& line_endpt_loc, 
	glm::vec2 line_startpt_offset, 	glm::vec2 line_endpt_offset, 
	glm::vec3& line_startpt_color,	glm::vec3& line_endpt_color, bool is_offset)
{
}

void dynamic_line_list_store::set_buffer()
{
}

void dynamic_line_list_store::paint_lines()
{
}

void dynamic_line_list_store::clear_lines()
{
}

void dynamic_line_list_store::update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale)
{
}
