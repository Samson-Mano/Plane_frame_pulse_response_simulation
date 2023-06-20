#include "modal_analysis_window.h"

modal_analysis_window::modal_analysis_window()
{
	// Empty Constructor
}

modal_analysis_window::~modal_analysis_window()
{
	// Empty Destructor
}

void modal_analysis_window::init()
{
	is_show_window = false;
	execute_modal_analysis = false;
	execute_open = false;
	execute_close = false;
	selected_modal_option = 0;

}

void modal_analysis_window::render_window()
{
	if (is_show_window == false)
		return;

	ImGui::Begin("Modal Analysis Solver");

	// Add a Modal Analysis button
	if (ImGui::Button("Modal Analysis"))
	{
		execute_modal_analysis = true;
	}

	// Add a log text box
	ImGui::TextWrapped("Solver Log:");
	ImGui::BeginChild("log_scroll_region", ImVec2(-1, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::TextUnformatted(log_buffer.c_str());

	// Automatic scroll to bottom
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();


	ImGui::Spacing();
	//_________________________________________________________________________________________


	// Add check boxes to show the Deformed model
	ImGui::Checkbox("Show Model", &show_undeformed_model);
	ImGui::Checkbox("Show Result values", &show_result_text_values);


	ImGui::Spacing();
	//_________________________________________________________________________________________

	// Input box to give input via text
	static bool defscale_input_mode = false;
	static char defscale_str[16] = ""; // buffer to store input deformation scale string
	static double defscale_input = 0; // buffer to store input deformation scale value

	// Button to switch to input mode
	if (!defscale_input_mode)
	{
		if (ImGui::Button("Deformation Scale"))
		{
			defscale_input_mode = true;
			snprintf(defscale_str, 16, "%.1f", deformation_scale_max); // set the buffer to current deformation scale value
		}
	}
	else // input mode
	{
		// Text box to input value
		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText("##Deformation Scale", defscale_str, IM_ARRAYSIZE(defscale_str), ImGuiInputTextFlags_CharsDecimal))
		{
			// convert the input string to int
			defscale_input = atoi(defscale_str);
			// set the load value to input value
			deformation_scale_max = defscale_input;
		}

		// Button to switch back to slider mode
		ImGui::SameLine();
		if (ImGui::Button("OK"))
		{
			defscale_input_mode = false;
		}
	}

	// Text for load value
	ImGui::SameLine();
	ImGui::Text(" %.1f", deformation_scale_max);

	// Slider for Deflection scale
	float deformation_scale_flt = static_cast<float>(deformation_scale_max);

	ImGui::Text("Deformation Scale");
	ImGui::SameLine();
	ImGui::SliderFloat(".", &deformation_scale_flt, 0.0f, 100.0f, "%.1f");
	deformation_scale_max = deformation_scale_flt;

	//Set the deformation scale
	deformation_scale = deformation_scale_max;

	ImGui::Spacing();
	//_________________________________________________________________________________________

	if (ImGui::CollapsingHeader("Animate"))
	{
		// Animate the solution
		// Start a horizontal layout
		ImGui::BeginGroup();

		// Play button active
		if (animate_play == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.8f, 0.4f, 1.0f)); // brighter green color
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // default color
		}

		// Add the Play button
		if (ImGui::Button("Play"))
		{
			// Handle Play button click
			animate_play = !animate_play;
			animate_pause = false;
		}
		ImGui::PopStyleColor(1);  // Revert back to default style

		// Add some spacing between buttons
		ImGui::SameLine();


		// Pause button active
		if (animate_pause == true)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.8f, 0.4f, 1.0f)); // brighter green color
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // default color
		}

		// Add the Pause button
		if (ImGui::Button("Pause"))
		{
			// Handle Pause button click
			animate_pause = !animate_pause;
			animate_play = false;
		}
		ImGui::PopStyleColor(1);  // Revert back to default style

		// Add some spacing between buttons
		ImGui::SameLine();

		// Add the Stop button
		if (ImGui::Button("Stop"))
		{
			// Handle Stop button click
			animate_play = false;
			animate_pause = false;
			time_val = 0.0f;
		}

		// Animation speed control
		// Input box to give input via text
		static bool animation_speed_input_mode = false;
		static char animation_speed_str[16] = ""; // buffer to store input deformation scale string
		static float animation_speed_input = 0; // buffer to store input deformation scale value

		// Button to switch to input mode
		if (!animation_speed_input_mode)
		{
			if (ImGui::Button("Animation Speed"))
			{
				animation_speed_input_mode = true;
				snprintf(animation_speed_str, 16, "%.1f", animation_speed); // set the buffer to current deformation scale value
			}
		}
		else // input mode
		{
			// Text box to input value
			ImGui::SetNextItemWidth(60.0f);
			if (ImGui::InputText("##Animation Speed", animation_speed_str, IM_ARRAYSIZE(animation_speed_str), ImGuiInputTextFlags_CharsDecimal))
			{
				// convert the input string to int
				animation_speed_input = static_cast<float>(atof(animation_speed_str));
				// set the load value to input value
				animation_speed = animation_speed_input;
			}

			// Button to switch back to slider mode
			ImGui::SameLine();
			if (ImGui::Button("OK"))
			{
				animation_speed_input_mode = false;
			}
		}

		// Text for Animation speed value
		ImGui::SameLine();
		ImGui::Text(" %.1f", animation_speed);

		// Display the frame rate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		// End the horizontal layout
		ImGui::EndGroup();
	}

	ImGui::Spacing();
	//_________________________________________________________________________________________

		// Close button
	if (ImGui::Button("Close"))
	{
		log_buffer = "";
		execute_close = true;
		execute_open = false;
		is_show_window = false; // set the flag to close the window
	}

	ImGui::End();


	// Set the animation data
	if (animate_play == true)
	{
		if (time_val > 6.283185307f)
		{
			time_val = 0.0f;
		}

		// Animation is playing 
		deformation_scale = ((std::cos(time_val * animation_speed) + 1) * 0.5f) * deformation_scale_max;
		time_val = time_val + 0.0002f;
	}
	else if (animate_pause == true)
	{
		deformation_scale = ((std::cos(time_val * animation_speed) + 1) * 0.5f) * deformation_scale_max;
	}

}
