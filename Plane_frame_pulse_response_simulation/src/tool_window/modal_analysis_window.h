#pragma once
#include <iostream>
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ImGui/stb_implement.h"

class modal_analysis_window
{
public:
	bool is_show_window = false;
	bool execute_modal_analysis = false; // Main solver run event flag
	bool execute_open = false; // Solver window open event flag
	bool execute_close = false; // Closing of solution window event flag
	int selected_modal_option = 0;
	std::string log_buffer = "";

	bool show_undeformed_model = true; // show undeformed model 
	bool show_result_text_values = true; // show the result text values

	// Animation control
	bool animate_play = true;
	bool animate_pause = false;
	double time_val = 0.0;
	double deformation_scale_max = 10.0;
	double animation_speed = 10.0;
	double deformation_scale = 0.0;

	modal_analysis_window();
	~modal_analysis_window();
	void init(); // initialize bind images
	void render_window();
private:
};