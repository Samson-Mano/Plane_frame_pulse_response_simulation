#pragma once
#include <iostream>
#include <vector>
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ImGui/stb_implement.h"

class pulse_response_window
{
public:
	bool is_show_window = false;
	bool execute_pulse_analysis = false; // Main solver run event flag
	bool execute_open = false; // Solver window open event flag
	bool execute_close = false; // Closing of solution window event flag

	// Inputs for response calculation
	double total_simulation_time = 10.0;
	double time_interval = 0.1;
	
	// Modal analysis Results
	double modal_first_frequency = 0.0;
	double modal_end_frequency = 0.0;
	int number_of_modes = 0;

	// Pulse response analysis results
	bool pulse_response_analysis_complete = false;
	
	bool show_undeformed_model = true; // show undeformed model 

	// Animation control
	bool animate_play = true;
	bool animate_pause = false;
	double time_val = 0.0;
	double deformation_scale_max = 10.0;
	double animation_speed = 10.0;
	double normailzed_defomation_scale = 0.0;
	double deformation_scale = 0.0;


	pulse_response_window();
	~pulse_response_window();
	void init();
	void render_window();
private:

};
