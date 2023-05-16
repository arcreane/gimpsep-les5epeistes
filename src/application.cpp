#include "application.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"

#include <cstdio>
#include <string>

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Application::Application()
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return;

	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create window with graphics context
	m_window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
	if (m_window == nullptr)
		return;
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto.ttf", 20);
}

Application::~Application()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Application::run()
{
	while (!glfwWindowShouldClose(m_window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Rendering
		update();
		render_img();
		render();
	}
}

void Application::update()
{

	// // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	// if (show_demo_window)
	// 	ImGui::ShowDemoWindow(&show_demo_window);

	for (auto &module : m_modules)
	{
		module->update();
	}
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);			   // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float *)&m_clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))
		{ // Buttons return true when clicked (most widgets return true when edited/activated)
			ImGuiFileDialog::Instance()->OpenDialog(
				"ChooseFileDlgKey",
				"Choose an image",
				"Image files (.bmp *.dib *.jpeg *.jpg *.jpe *.jp2 *.png *.webp *.pbm *.pgm *.ppm *.pxm *.pnm *.sr *.ras *.tiff *.tif){.bmp,.dib,.jpeg,.jpg,.jpe,.jp2,.png,.webp,.pbm,.pgm,.ppm,.pxm,.pnm,.sr,.ras,.tiff,.tif}",
				"/");
			counter++;
		}

		img_picker();

		static int i = 0;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGuiIO &io = ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}
}

void Application::img_picker()
{
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
			load_file(path);
			ImGuiFileDialog::Instance()->Close();
		}
		ImGuiFileDialog::Instance()->Close();
	}
}
void Application::load_file(std::string &path)
{
	m_img = cv::imread(path);

	// Convert the image to RGB
	cv::cvtColor(m_img, m_img, cv::COLOR_BGR2RGB);
	if (m_img.empty())
	{
		show_popup("Error", "Could not load image");
	}
	glDeleteTextures(1, &m_texture_id);

	load_texture();
}

void Application::load_texture()
{

	glEnable(GL_TEXTURE_2D);
	// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glGenTextures(1, &m_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D,	   // Type of texture
				 0,				   // Pyramid level (for mip-mapping) - 0 is the top level
				 GL_RGB,		   // Internal colour format to convert to
				 m_img.cols,	   // Image width  i.e. 640 for Kinect in standard mode
				 m_img.rows,	   // Image height i.e. 480 for Kinect in standard mode
				 0,				   // Border width in pixels (can either be 1 or 0)
				 GL_RGB,		   // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				 GL_UNSIGNED_BYTE, // Image data type
				 m_img.ptr());	   // The actual image data itself
}
void Application::show_popup(std::string title, std::string message)
{
	static bool show_popup = true;
	ImGui::OpenPopup(title.c_str());
	if (show_popup)
	{
		ImGui::BeginPopupModal(title.c_str(), &show_popup, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text(message.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			show_popup = false;
		}
		ImGui::EndPopup();
	}
}

void Application::render_img()
{
	ImGuiIO &io = ImGui::GetIO();
	ImGui::SetNextWindowSize(io.DisplaySize);
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
	ImGui::Begin("Image", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	// Create a texture for the image
	if (!m_img.empty())
	{
		// Computing viewport in order to center image and keep aspect ratio
		float img_aspect_ratio = m_img.cols / (float)m_img.rows;
		float viewport_aspect_ratio = io.DisplaySize.x / io.DisplaySize.y;
		float scale = img_aspect_ratio > viewport_aspect_ratio ? io.DisplaySize.x / m_img.cols : io.DisplaySize.y / m_img.rows;

		ImGui::Image((void *)(intptr_t)m_texture_id, ImVec2((float)m_img.cols, (float)m_img.rows));
	}

	ImGui::End();
}
void Application::render()
{
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(m_clear_color.x * m_clear_color.w, m_clear_color.y * m_clear_color.w, m_clear_color.z * m_clear_color.w, m_clear_color.w);
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_window);
}