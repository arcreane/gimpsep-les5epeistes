#include "test_module.hpp"

#include "imgui.h"

#include <chrono>

void TestModule::update()
{
	// Create a window
	ImGui::Begin("My module window");

	// Add a button
	if (ImGui::Button("This button will start heavy computation !"))
	{
		// Heavy computation on another thread (e.g. performing openCV operations)
		m_future = std::async(std::launch::async, []
							  {
			std::this_thread::sleep_for(std::chrono::seconds(5));
			return cv::Mat(1000, 1000, CV_8UC3, cv::Scalar(0, 0, 255)); });
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
