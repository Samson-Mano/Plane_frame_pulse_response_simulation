#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ImGui/stb_implement.h"

#pragma warning(push)
#pragma warning (disable : 26495)
#pragma warning (disable : 26451)

#include "../ImGui/implot.h"
#pragma warning(pop)

// Frequency Result Objects
#include "../geometry_store/freq_result_objects/freq_analysis_result_store.h"

class frequency_response_window
{
public:
	bool is_show_window = false;
	bool is_include_consistent_mass_matrix = true; // flag to include consistent mass matrix
	bool execute_freq_analysis = false; // Main solver run event flag
	bool execute_open = false; // Solver window execute opening event flag
	bool execute_close = false; // Closing of solution window event flag

	// Inputs for response calculation
	double frequency_start_val = 0.1;
	double frequency_end_val = 10.0;
	double frequency_interval = 0.1;
	double damping_ratio = 0.1;

	// Frequency analysis results
	bool freq_response_analysis_complete = false;
	freq_analysis_result_store freq_response_result;

	// Frequency response dropdown selected index
	int selected_node_id = 0;
	int selected_resp = 0;

	frequency_response_window();
	~frequency_response_window();
	void init();
	void render_window();
private:

};
