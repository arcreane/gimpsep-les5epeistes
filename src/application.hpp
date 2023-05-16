#pragma once

#include "GLFW/glfw3.h"
#include "imgui.h"

#include <string>
#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include "module.hpp"

// Modules
#include "modules/real_module.hpp"

class Application
{
public:
	Application();
	~Application();
	void run();

private:
	void update();

	void render_img();
	void render();

	void img_picker();
	void load_file(std::string &path);
	void load_texture();
	void show_popup(std::string title, std::string message);

private:
	GLFWwindow *m_window;
	ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// This image should always be in BGR format
	cv::Mat m_img;

	unsigned int m_texture_id = 2;

	std::array<std::unique_ptr<Module>, 1> m_modules = {
		std::make_unique<RealModule>()
	};
};