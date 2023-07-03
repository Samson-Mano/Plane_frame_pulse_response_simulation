#include "pulse_response_window.h"

pulse_response_window::pulse_response_window()
{
	// Empty constructor
}

pulse_response_window::~pulse_response_window()
{
	// Empty destructor
}

void pulse_response_window::init()
{
	is_show_window = false; // Solver window open event flag
	execute_pulse_analysis = false; // Main solver run event flag
	execute_open = false; // Solver window execute opening event flag
	execute_close = false; // Closing of solution window event flag

	pulse_response_analysis_complete = false;
}

void pulse_response_window::render_window()
{
}
