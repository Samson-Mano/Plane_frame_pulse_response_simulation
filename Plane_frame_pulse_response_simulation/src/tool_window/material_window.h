#pragma once
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"

struct material_data
{
	unsigned int material_id;
	std::string material_name;
	double youngs_mod;
	double second_moment_of_area;
	double mat_density;
	double cs_area;
};

class material_window
{
public:
	bool is_show_window = false;
	bool is_assign_material = false;
	int execute_delete_materialid = -1;

	int selected_material_option = 0;
	std::unordered_map<int, material_data> material_list;

	material_window();
	~material_window();
	void init();
	void render_window();
	static glm::vec3 get_standard_color(int color_index);
private:
	int get_unique_material_id();
};
