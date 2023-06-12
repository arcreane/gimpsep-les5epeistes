#include "canny_edge_module.hpp"

#include "imgui.h"

#include <chrono>

void CannyEdgeModule::update()
{
    static int t1 = 100;
    static int t2 = 350;

    // Create a window
    ImGui::Begin("Canny edge module");

    ImGui::BeginGroup();
    ImGui::PushItemWidth(120);
    ImGui::InputInt("#", &t1, 0, 500);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SliderInt("Threshold 1", &t1, 0, 500);
    ImGui::EndGroup();

    ImGui::BeginGroup();
    ImGui::PushItemWidth(120);
    ImGui::InputInt("##", &t2, 0, 350);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SliderInt("Threshold 2", &t2, 0, 350);
    ImGui::EndGroup();

    ImGui::Spacing();

    // Add a button
    if (ImGui::Button("Display canny edges"))
    {
        // Heavy computation on another thread (e.g. performing openCV operations)
        m_future = std::async(std::launch::async, [this]
        {
            cv::Mat gray_image, image, edges;
            image = this->m_current_img->clone();
            cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
            cv::Canny(image, edges, t1, t2, 3, false);
            return edges; });
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