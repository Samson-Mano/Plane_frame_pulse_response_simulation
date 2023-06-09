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
	ImGui::Text("Start Mode 1 Frequency = %.3f Hz", static_cast<float>(modal_first_frequency));
	ImGui::Text("End Mode %i Frequency = %.3f Hz",number_of_modes ,static_cast<float>(modal_end_frequency));

	ImGui::Spacing();
	// Frequency range
	// Text for load start frequency
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
	// Input box to give input via text
	static bool frequencyinterval_input_mode = false;
	static char frequencyinterval_str[16] = ""; // buffer to store input load string
	static float frequencyinterval_input = static_cast<float>(frequency_interval); // buffer to store input load End Time

	// Button to switch to input mode
	if (!frequencyinterval_input_mode)
	{
		if (ImGui::Button("Frequency Interval"))
		{
			frequencyinterval_input_mode = true;
			snprintf(frequencyinterval_str, 16, "%.3f", frequencyinterval_input); // set the buffer to current load End Time
		}
	}
	else // input mode
	{
		// Text box to input load End Time
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##InputFrequencyInterval", frequencyinterval_str, IM_ARRAYSIZE(frequencyinterval_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			frequencyinterval_input = static_cast<float>(atof(frequencyinterval_str));
			// set the load End Time to input value
			if (frequencyinterval_input < (frequency_end_val - frequency_start_val))
			{
				frequency_interval = frequencyinterval_input;
			}
			else
			{
				frequencyinterval_input = static_cast<float>(frequency_interval);
			}
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			frequencyinterval_input_mode = false;
		}
	}

	// Text for load End Time
	ImGui::SameLine();
	ImGui::Text("Frequency interval = %.3f", frequency_interval);
	//_________________________________________________________________________________________


	// Add a Frequency Analysis button
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
		// Create the node numbers combo box
		// Convert the vector of strings to a vector of const char*
		std::vector<const char*> node_id_list;
		int node_count = static_cast<int>(freq_response_result.node_id_str.size());
		node_id_list.reserve(node_count);
		for (const auto& item : freq_response_result.node_id_str)
		{
			node_id_list.push_back(item.c_str());
		}

		// ImGui::Text("Selected Node: %s", std::to_string(freq_response_result.node_id_values[selected_node_id]).c_str());
		// Render the drop-down list box
		ImGui::Combo("Selected Node", &selected_node_id, node_id_list.data(), static_cast<int> (node_id_list.size()));

		// Render the response drop down list
		const char* resp_str[] = { "Displacement resultant", "X Response", "Y Reponse" ,"XY Reponse" };

		ImGui::Combo("Selected Response Type", &selected_resp, resp_str, 4);

		response_data r;
		
		// Type of response
		if (selected_resp == 0)
		{
			// Displacement resultant
			r = freq_response_result.displ_response_data[selected_node_id];
		}
		else if (selected_resp == 1)
		{
			// X response
			r = freq_response_result.x_response_data[selected_node_id];
		}
		else if (selected_resp == 2)
		{
			// Y response
			r = freq_response_result.y_response_data[selected_node_id];
		}
		else if (selected_resp == 2)
		{
			// XY response
			r = freq_response_result.xy_response_data[selected_node_id];
		}

		// Post the responses
		std::vector<float> freq_xValues(static_cast<int>(r.ampl_values.size()));
		std::vector<float> ampl_yValues(static_cast<int>(r.ampl_values.size()));
		std::vector<float> phase_yValues(static_cast<int>(r.ampl_values.size()));

		for (int i = 0; i < static_cast<int>(r.ampl_values.size()); i++) 
		{
			freq_xValues[i] = static_cast<float>(r.freq_values[i]);
			ampl_yValues[i] = static_cast<float>(r.ampl_values[i]);
			phase_yValues[i] =  static_cast<float>(r.phase_values[i]);
		}


		ImPlot::CreateContext();

		// Plot the amplitude response
		if (ImPlot::BeginPlot("Amplitude Plot"))
		{
			ImPlot::SetupAxes("Frequency (Hz)", "Amplitude");
			ImPlot::SetupAxesLimits(static_cast<float>(r.min_freq) - ((static_cast<float>(r.max_freq) - static_cast<float>(r.min_freq))*0.1), 
				static_cast<float>(r.max_freq) + ((static_cast<float>(r.max_freq) - static_cast<float>(r.min_freq)) * 0.1),
				static_cast<float>(r.min_ampl) - ((static_cast<float>(r.max_ampl) - static_cast<float>(r.min_ampl)) * 0.1),
				static_cast<float>(r.max_ampl) + ((static_cast<float>(r.max_ampl) - static_cast<float>(r.min_ampl)) * 0.1));

			ImPlot::SetNextLineStyle(ImVec4(1, 0, 0, 1), 2.0f); // Set line color to red and width to 2.0f

			// Plot the amplitude
			ImPlot::PlotLine("Amplitude", freq_xValues.data(), ampl_yValues.data(), static_cast<int>(freq_xValues.size()));
			ImPlot::EndPlot();
		}

		// Plot the amplitude response
		if (ImPlot::BeginPlot("Phase Plot"))
		{
			ImPlot::SetupAxes("Frequency (Hz)", "Phase");
			ImPlot::SetupAxesLimits(static_cast<float>(r.min_freq) - ((static_cast<float>(r.max_freq) - static_cast<float>(r.min_freq)) * 0.1),
				static_cast<float>(r.max_freq) + ((static_cast<float>(r.max_freq) - static_cast<float>(r.min_freq)) * 0.1),
				static_cast<float>(r.min_phase) - ((static_cast<float>(r.max_phase) - static_cast<float>(r.min_phase)) * 0.1),
				static_cast<float>(r.max_phase) + ((static_cast<float>(r.max_phase) - static_cast<float>(r.min_phase)) * 0.1));

			ImPlot::SetNextLineStyle(ImVec4(0, 0, 1, 1), 2.0f); // Set line color to blue and width to 2.0f

			// Plot the amplitude
			ImPlot::PlotLine("Phase", freq_xValues.data(), phase_yValues.data(), static_cast<int>(freq_xValues.size()));
			ImPlot::EndPlot();
		}

		ImPlot::DestroyContext();
	}



	ImGui::End();

}
