#pragma once
#include "../geometry_buffers/gBuffers.h"
#include "../geom_parameters.h"

struct dynamic_point_store
{
	// store the individual point
	int point_id = 0;
	glm::vec2 point_loc = glm::vec2(0);
	std::vector<glm::vec2> point_offset; // Dynamic point offset
	std::vector<glm::vec3> point_color; // Dynamic point color
	int offset_pt_count = 0;
};

class dynamic_point_list_store
{
public:
	geom_parameters* geom_param_ptr = nullptr;
	unsigned int dyn_point_count = 0;
	std::vector<dynamic_point_store> dyn_pointMap;

	dynamic_point_list_store();
	~dynamic_point_list_store();
	void init(geom_parameters* geom_param_ptr);
	void add_point(int& point_id, glm::vec2& point_loc, std::vector<glm::vec2>& point_offset, std::vector<glm::vec3>& point_color);
	void set_buffer();
	void paint_points();
	void clear_points();
	void update_opengl_uniforms(bool set_modelmatrix, bool set_pantranslation, bool set_zoomtranslation, bool set_transparency, bool set_deflscale);
private:
	gBuffers dyn_point_buffer;
	Shader dyn_point_shader;

	void get_node_buffer(dynamic_point_store& pt, float* point_vertices, unsigned int& point_v_index, unsigned int* point_indices, unsigned int& point_i_index);
};
