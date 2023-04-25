#pragma once

#include "GLFW/glfw3.h"
#include "imgui.h"

class Application {
public:
	Application();
	~Application();
	void run();

private:
	void update();
	void render();

private:
	GLFWwindow *m_window;
	ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};