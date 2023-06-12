#include "dilatation_module.hpp"

#include "imgui.h"

#include <chrono>
#include <iostream>

void DilatationModule::update()
{
    static int w = m_current_img->cols;
    static int h = m_current_img->rows;
    static int x = -1;
    static int y = -1;

    if (w > m_current_img->cols)
        w = m_current_img->cols;
    if (h > m_current_img->rows)
        h = m_current_img->rows;

    // Anchor pos cannot be outside of kernel
    if (x > w)
        x = w;
    if (y > h)
        y = h;

    if (w < 1)
        w = 1;
    if (h < 1)
        h = 1;
    if (x < -1)
        x = -1;
    if (y < -1)
        y = -1;


    // Create a window
    ImGui::Begin("Dilate window");

    ImGui::BeginGroup();
    ImGui::PushItemWidth(120);
    ImGui::InputInt("##", &w, 1, m_current_img->cols);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SliderInt("Kernel Width", &w, 1, m_current_img->cols);
    ImGui::EndGroup();

    ImGui::BeginGroup();
    ImGui::PushItemWidth(120);
    ImGui::InputInt("###", &h, 1, m_current_img->rows);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SliderInt("Kernel Height", &h, 1, m_current_img->rows);
    ImGui::EndGroup();

    ImGui::BeginGroup();
    ImGui::PushItemWidth(120);
    ImGui::InputInt("##", &x, -1, w);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SliderInt("Anchor X", &x, -1, w);
    ImGui::EndGroup();

    ImGui::BeginGroup();
    ImGui::PushItemWidth(120);
    ImGui::InputInt("###", &y, -1, h);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SliderInt("Anchor Y", &y, -1, h);
    ImGui::EndGroup();

    ImGui::Spacing();

    if (ImGui::Button("Erosion !"))
    {
        // Heavy computation on another thread (e.g. performing openCV operations)
        m_future = std::async(std::launch::async, [this]
        {
            cv::Mat dest;
            cv::Mat src = m_current_img->clone();
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(w, h));
            cv::dilate(src, dest, kernel, cv::Point(x, y), 1, 1, 1);
            return dest; });
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
