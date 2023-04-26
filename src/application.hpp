#pragma once

#include "GLFW/glfw3.h"
#include "imgui.h"

#include <string>
#include <opencv2/opencv.hpp>


class Application {
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

	unsigned int m_texture_id = 1;
};