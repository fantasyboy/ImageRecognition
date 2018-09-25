#pragma once

/*
// 识别库
*/
#include <vector>
#include "opencv2/opencv.hpp"

//字库结构体
typedef struct _EM_FONTS_PRO
{
	cv::Mat _mat;  //图片
	std::string _text; //图片对应文字
}EM_FONTS_PRO, *PEM_FONTS_PRO;


//相似度结构体
typedef struct _EM_IMAGE_FONTS_PRO
{
	int _samilar; //相似度
	std::string _text; //文字
}EM_IMAGE_FONTS_PRO,*PEM_IMAGE_FONTS_PRO;

//图片结构体
typedef struct _EM_IMAGE_PRO
{
	cv::Rect _rc;
	EM_IMAGE_FONTS_PRO _eip;
}EM_IMAGE_PRO, *PEM_IMAGE_PRO;


class ImageRecognition
{

public:
	ImageRecognition();
	~ImageRecognition();
	//初始化字库
	bool InitFonts();
	//识别图像，并返回字符串数组
	void Recognition(cv::Mat _mat, std::string &_ret);
private:
	//返回和字库中最相思的字
	void RecognitionFonts(cv::Mat _mat, EM_IMAGE_FONTS_PRO & _eip);
	//判断两张图片是否相似
	int GetImageSemblance(cv::Mat _mat1, cv::Mat _mat2);
	//将图片转化为32*32的图片
	cv::Mat ConvertImgTo32Pix(cv::Mat& _mat);
private:
	std::vector<EM_FONTS_PRO> m_fontsList; //字体库
	int m_bltPixel;
};
