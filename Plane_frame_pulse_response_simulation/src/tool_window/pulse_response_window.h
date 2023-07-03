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


	// Pulse response analysis results
	bool pulse_response_analysis_complete = false;



	pulse_response_window();
	~pulse_response_window();
	void init();
	void render_window();
private:

};
