#include "canny_edge_module.hpp"

#include "imgui.h"

#include <chrono>

void CannyEdgeModule::update()
{
    // Create a window
    ImGui::Begin("Canny edge module");

    // Add a button
    if (ImGui::Button("This button will display canny edges !"))
    {
        // Heavy computation on another thread (e.g. performing openCV operations)
        m_future = std::async(std::launch::async, [this]
        {
            cv::Mat gray_image, image, edges;
            image = this->m_current_img->clone();
            cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);
            cv::Canny(image, edges, 100, 350, 3, false);
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
        } else {
            ImGui::Text("Computing...");
        }
    }


    // End the window
    ImGui::End();
}