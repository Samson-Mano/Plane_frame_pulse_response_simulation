#include "frequency_response_window.h"

frequency_response_window::frequency_response_window()
{
	// Empty constructor
}

frequency_response_window::~frequency_response_window()
{
	// Empty destructor
}

void frequency_response_window::init()
{
	is_show_window = false; // Solver window open event flag
	execute_freq_analysis = false; // Main solver run event flag
	execute_open = false; // Solver window execute opening event flag
	execute_close = false; // Closing of solution window event flag

	freq_response_analysis_complete = false;
}

void frequency_response_window::render_window()
{
	if (is_show_window == false)
		return;

	ImGui::Begin("Frequency Response Analysis Solver");

	// Show Modal analysis results
	//________________________________________________________________________________________





	ImGui::Spacing();
	// Frequency range
	// Text for load start time
	//_________________________________________________________________________________________
	// Input box to give input via text
	static bool frequencystart_input_mode = false;
	static char frequencystart_str[16] = ""; // buffer to store input load string
	static float frequencystart_input = static_cast<float>(frequency_start_val); // buffer to store input load End Time

	// Button to switch to input mode
	if (!frequencystart_input_mode)
	{
		if (ImGui::Button("Frequency Start"))
		{
			frequencystart_input_mode = true;
			snprintf(frequencystart_str, 16, "%.3f", frequencystart_input); // set the buffer to current load End Time
		}
	}
	else // input mode
	{
		// Text box to input load start time
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##InputFrequencyStart", frequencystart_str, IM_ARRAYSIZE(frequencystart_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			frequencystart_input = static_cast<float>(atof(frequencystart_str));
			// set the load start time to input value
			frequency_start_val = frequencystart_input;
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			frequencystart_input_mode = false;
		}
	}

	// Text for load Start Time
	ImGui::SameLine();
	ImGui::Text("Start Frequency = %.3f", frequency_start_val);

	//_________________________________________________________________________________________
	// Input box to give input via text
	static bool frequencyend_input_mode = false;
	static char frequencyend_str[16] = ""; // buffer to store input load string
	static float frequencyend_input = static_cast<float>(frequency_end_val); // buffer to store input load End Time

	// Button to switch to input mode
	if (!frequencyend_input_mode)
	{
		if (ImGui::Button("Frequency End"))
		{
			frequencyend_input_mode = true;
			snprintf(frequencyend_str, 16, "%.3f", frequencyend_input); // set the buffer to current load End Time
		}
	}
	else // input mode
	{
		// Text box to input load End Time
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##InputFrequencyEnd", frequencyend_str, IM_ARRAYSIZE(frequencyend_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			frequencyend_input = static_cast<float>(atof(frequencyend_str));
			// set the load End Time to input value
			if (frequencyend_input > frequency_start_val)
			{
				frequency_end_val = frequencyend_input;
			}
			else
			{
				frequencyend_input = static_cast<float>(frequency_end_val);
			}
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			frequencyend_input_mode = false;
		}
	}

	// Text for load End Time
	ImGui::SameLine();
	ImGui::Text("End Frequency = %.3f", frequency_end_val);
	//_________________________________________________________________________________________

	// Add a Modal Analysis button
	if (ImGui::Button("Frequency Response Analysis"))
	{
		execute_freq_analysis = true;
	}


	ImGui::Spacing();

	//_________________________________________________________________________________________
	// Close button
	if (ImGui::Button("Close"))
	{
		execute_close = true;
		execute_open = false;
		is_show_window = false; // set the flag to close the window
	}

	//_________________________________________________________________________________________
	if (freq_response_analysis_complete == true)
	{
		// Create the node numbers and response


		// Post the responses



	}



	ImGui::End();

}
