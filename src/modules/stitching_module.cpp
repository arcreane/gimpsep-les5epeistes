#include "stitching_module.hpp"

#include "imgui.h"

#include <chrono>
#include <iostream>
#include <vector>
#include "ImGuiFileDialog.h"

void StitchingModule::update()
{
	static int w = m_current_img->cols;
	static int h = m_current_img->rows;
	static std::string status = "";
	static std::vector<std::string> imgs;

	// defining img picker
	{
		if (ImGuiFileDialog::Instance()->Display("StitchImgPicker"))
		{
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
				imgs.push_back(path);
				ImGuiFileDialog::Instance()->Close();
			}
			ImGuiFileDialog::Instance()->Close();
		}
	}

	// Create a window
	ImGui::Begin("Stitching window");

	if (ImGui::Button("Add an image"))
	{
		ImGuiFileDialog::Instance()->OpenDialog(
			"StitchImgPicker",
			"Choose an image",
			"Image files (.bmp *.dib *.jpeg *.jpg *.jpe *.jp2 *.png *.webp *.pbm *.pgm *.ppm *.pxm *.pnm *.sr *.ras *.tiff *.tif){.bmp,.dib,.jpeg,.jpg,.jpe,.jp2,.png,.webp,.pbm,.pgm,.ppm,.pxm,.pnm,.sr,.ras,.tiff,.tif}",
			"/");
	}

	ImGui::Text("Images :");
	for (int i = 0; i < imgs.size(); i++)
	{
		ImGui::BeginGroup();
		ImGui::Text(imgs.at(i).c_str());
		ImGui::SameLine();
		if (ImGui::Button(" | Remove"))
		{
			imgs.erase(imgs.begin() + i);
		}
		ImGui::EndGroup();
	}

	ImGui::Spacing();
	ImGui::Text(status.c_str());

	if (ImGui::Button("Stitch !"))
	{
		// Heavy computation on another thread (e.g. performing openCV operations)
		m_future = std::async(std::launch::async, [this]
							  {
			std::vector<cv::Mat> mats;
			cv::Mat src = m_current_img->clone();
			mats.push_back(src);

			cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create();
			cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA;  // Set the stitching mode (PANORAMA or SCANS)

			status = "Resizing images...\n";
			for (auto img_name : imgs)
			{
				cv::Mat img = cv::imread(img_name);
				cv::Mat resized;
				cv::resize(img, resized, cv::Size(w, h), 0, 0, cv::INTER_NEAREST);
				mats.push_back(resized.clone());
			}

			status += "Estimating transform...\n";

			status += "Stitching images...\n";
			cv::Mat dest;
			cv::Stitcher::Status code = stitcher->stitch(mats, dest);
		
			if (code != cv::Stitcher::OK)
			{
				status = "Can't stitch images, error code = ";
				status += std::to_string((int)code) + std::string("\n");
			}

			return src; });
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
