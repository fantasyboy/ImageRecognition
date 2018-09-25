#pragma once
#include <atlimage.h>
#include "opencv2/opencv.hpp"

namespace ImageUtils
{
	bool CImage2cvMat(CImage& image, cv::Mat &mat);
}