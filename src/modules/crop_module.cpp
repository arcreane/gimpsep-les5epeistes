#include "crop_module.hpp"

#include "imgui.h"

#include <chrono>
#include <iostream>

void CropModule::update()
{
	static int w = m_current_img->cols;
	static int h = m_current_img->rows;
	static int x = 0;
	static int y = 0;

	if (w > m_current_img->cols)
		w = m_current_img->cols;
	if (h > m_current_img->rows)
		h = m_current_img->rows;
	if (x > m_current_img->cols)
		x = m_current_img->cols;
	if (y > m_current_img->rows)
		y = m_current_img->rows;
	if (w < 0)
		w = 0;
	if (h < 0)
		h = 0;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;

	// Create a window
	ImGui::Begin("Crop window");

	ImGui::BeginGroup();
	ImGui::PushItemWidth(120);
	ImGui::InputInt("##", &w, 0, m_current_img->cols - x);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderInt("Width", &w, 0, m_current_img->cols - x);
	ImGui::EndGroup();

	ImGui::BeginGroup();
	ImGui::PushItemWidth(120);
	ImGui::InputInt("###", &h, 0, m_current_img->rows - y);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderInt("Height", &h, 0, m_current_img->rows - y);
	ImGui::EndGroup();

	
	ImGui::BeginGroup();
	ImGui::PushItemWidth(120);
	ImGui::InputInt("####", &x, 0, m_current_img->cols);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderInt("X", &x, 0, m_current_img->cols);
	ImGui::EndGroup();

	ImGui::BeginGroup();
	ImGui::PushItemWidth(120);
	ImGui::InputInt("#####", &y, 0, m_current_img->rows);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderInt("Y", &y, 0, m_current_img->rows);
	ImGui::EndGroup();

	ImGui::Spacing();

	if (ImGui::Button("Crop !"))
	{
		// Heavy computation on another thread (e.g. performing openCV operations)
		m_future = std::async(std::launch::async, [this]
							  {
			cv::Mat dest;
			cv::Mat src = m_current_img->clone();
			cv::Mat cropped = src(cv::Rect(x, y, w, h));
			return cropped; });
	}

	/// When the heavy computation is done, update the image
	if (m_future.valid())
	{
		auto status = m_future.wait_for(std::chrono::milliseconds(0));
		if (status == std::future_status::ready)
		{
			cv::Mat result = m_future.get();
			m_update_img(result);
		}
	}
	// End the window
	ImGui::End();

}