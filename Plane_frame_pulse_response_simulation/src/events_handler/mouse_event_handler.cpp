#include "mouse_event_handler.h"


mouse_event_handler::mouse_event_handler()
{
	// Empty constructor
}

mouse_event_handler::~mouse_event_handler()
{
	// Empty destructor
}

void mouse_event_handler::init(geom_store* geom, load_window* ld_window,
	constraint_window* ct_window, material_window* mat_window)
{
	// Add the pointers to initialize the mouse events
	mouse_evnt.init(geom, ld_window, ct_window, mat_window);
}

void mouse_event_handler::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	// Mouse button callback function
	if (ImGui::GetIO().WantCaptureMouse) 
	{
		return;
	}

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);  // Get mouse position
	mouse_event_handler* handler = static_cast<mouse_event_handler*>(glfwGetWindowUserPointer(window));
	handler->handleMouseButton(button, action, mods, xpos, ypos);
}

void mouse_event_handler::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	// Mouse move callback function
	mouse_event_handler* handler = static_cast<mouse_event_handler*>(glfwGetWindowUserPointer(window));
	handler->handleMouseMove(xpos, ypos);
}

void mouse_event_handler::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Mouse scroll callback function
	// Retrieve the current mouse cursor position using GLFW
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	mouse_event_handler* handler = static_cast<mouse_event_handler*>(glfwGetWindowUserPointer(window));
	handler->handleMouseScroll(xoffset, yoffset, xpos, ypos);
}

void mouse_event_handler::keyDownCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Key down callback function
	mouse_event_handler* handler = static_cast<mouse_event_handler*>(glfwGetWindowUserPointer(window));
	handler->handleKeyDown(key, scancode, action, mods);
}

void mouse_event_handler::handleMouseButton(int button, int action, int mods, double xpos, double ypos)
{
}

void mouse_event_handler::handleMouseMove(double xpos, double ypos)
{
}

void mouse_event_handler::handleMouseScroll(double xoffset, double yoffset, double xpos, double ypos)
{
}

void mouse_event_handler::handleKeyDown(int key, int scancode, int action, int mods)
{
}

void mouse_event_handler::zoom_to_fit()
{
}

