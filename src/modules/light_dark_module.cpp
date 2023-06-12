#include "light_dark_module.hpp"

#include "imgui.h"

#include <chrono>

void LightDarkModule::update()
{
    static int c = 0;

    // Create a window
    ImGui::Begin("Lighten/Darken module");

    ImGui::BeginGroup();
    ImGui::PushItemWidth(120);
    ImGui::InputInt("#", &c, 0, 255);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::SliderInt("L/D Value", &c, 0, 255);
    ImGui::EndGroup();

    // Add a button
    if (ImGui::Button("Lighten"))
    {
        // Heavy computation on another thread (e.g. performing openCV operations)
        m_future = std::async(std::launch::async, [this]
        {
            cv::Mat image, lImage;
            image = this->m_current_img->clone();
            image.convertTo(lImage, -1, 1, c);
            return lImage; });
    }

    // Add a button
    if (ImGui::Button("Darken"))
    {
        // Heavy computation on another thread (e.g. performing openCV operations)
        m_future = std::async(std::launch::async, [this]
        {
            cv::Mat image, lImage;
            image = this->m_current_img->clone();
            image.convertTo(lImage, -1, 1, -c);
            return lImage; });
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