#pragma once

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include <functional>

class Module
{
public:
	explicit Module(std::function<void(cv::Mat &)> update_img) : m_update_img(update_img) {}
	virtual ~Module() = default;
	virtual void update() = 0;

protected:
	std::function<void(cv::Mat &)> m_update_img;
};