#pragma once

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>

class Module
{
public:
	void init(cv::Mat *img)
	{
		m_img = img;
	}
	virtual ~Module() = default;
	virtual void update() = 0;

protected:
	cv::Mat *m_img;
};