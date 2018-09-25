#include "ImageRecognition.h"
#include <memory>
#include <sstream>
#include <windows.h>
#include <Shlwapi.h>
ImageRecognition::ImageRecognition() : m_bltPixel(32)
{

}

ImageRecognition::~ImageRecognition()
{

}

bool ImageRecognition::InitFonts()
{
	char _currPath[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, _currPath, MAX_PATH);
	PathRemoveFileSpec(_currPath);

	for (int i = 0; i != 10; i++)
	{
		std::stringstream ss;
		ss << _currPath << "\\img\\ÌÔÌ­Íæ¼ÒÊý×Ö_" << i << ".png";
		EM_FONTS_PRO _temp;
		auto _mat  = cv::imread(ss.str());
		auto _destMat = ConvertImgTo32Pix(_mat);

		std::stringstream _text;
		_text << i;
		_temp._mat = _destMat;
		_temp._text = _text.str();
		m_fontsList.push_back(_temp);
	}

	for (int i = 0; i != 10; i++)
	{
		std::stringstream ss;
		ss << _currPath<< "\\img\\êÇ³ÆÊý×Ö_" << i << ".png";
		EM_FONTS_PRO _temp;
		auto _mat = cv::imread(ss.str());
		auto _destMat = ConvertImgTo32Pix(_mat);

		std::stringstream _text;
		_text << i;
		_temp._mat = _destMat;
		_temp._text = _text.str();
		m_fontsList.push_back(_temp);
	}


	for (int i = 0; i != 26; i++)
	{
		std::stringstream ss;
		ss << _currPath << "\\img\\Ð¡Ð´×ÖÄ¸_" << i + 1 << ".png";

		auto _mat = cv::imread(ss.str());
		auto _destMat = ConvertImgTo32Pix(_mat);

		std::stringstream _text;
		_text << (char)('a'+i);

		EM_FONTS_PRO _temp;
		_temp._mat = _destMat;
		_temp._text = _text.str();
		m_fontsList.push_back(_temp);
	}

	for (int i = 0; i != 26; i++)
	{
		std::stringstream ss;
		ss << _currPath << "\\img\\´óÐ´×ÖÄ¸_" << i + 1 << ".png";

		auto _mat = cv::imread(ss.str());
		auto _destMat = ConvertImgTo32Pix(_mat);

		std::stringstream _text;
		_text << (char)('A' + i);

		EM_FONTS_PRO _temp;
		_temp._mat = _destMat;
		_temp._text = _text.str();
		m_fontsList.push_back(_temp);
	}

	{
		std::stringstream ss;
		ss << _currPath << "\\img\\ÏÂ»®Ïß_" << 1 << ".png";

		std::string _aa = ss.str();
		auto _mat = cv::imread(ss.str());
		auto _destMat = ConvertImgTo32Pix(_mat);

		EM_FONTS_PRO _temp;
		_temp._mat = _destMat;
		_temp._text = "-";
		m_fontsList.push_back(_temp);
	}

	{
		std::stringstream ss;
		ss << _currPath << "\\img\\ÏÂ»®Ïß_" << 2 << ".png";

		auto _mat = cv::imread(ss.str());
		auto _destMat = ConvertImgTo32Pix(_mat);

		EM_FONTS_PRO _temp;
		_temp._mat = _destMat;
		_temp._text = "_";
		m_fontsList.push_back(_temp);
	}


	char *szName[7] = { "¶Ó", "Îé", "µÚ", "ÌÔ", "Ì­", "½±", "Àø"};

	for (int i = 0; i != 7; i++)
	{
		std::stringstream ss;
		ss << _currPath << "\\img\\ºº×Ö_" << i + 1 << ".png";

		auto _mat = cv::imread(ss.str());
		auto _destMat = ConvertImgTo32Pix(_mat);

		EM_FONTS_PRO _temp;
		_temp._mat = _destMat;
		_temp._text = szName[i];
		m_fontsList.push_back(_temp);
	}

	return true;
}

void ImageRecognition::Recognition(cv::Mat _mat, std::string &_ret)
{
	cv::Mat _CvtColorMat;
	cv::cvtColor(_mat, _CvtColorMat, CV_BGR2GRAY);
	cv::threshold(_CvtColorMat, _CvtColorMat, 180, 255, cv::THRESH_BINARY);

	cv::Mat _blurMat;
	blur(_CvtColorMat, _blurMat, cv::Size(2, 7));

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(_blurMat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<EM_IMAGE_PRO> list;
	for (auto & _temp : contours)
	{
		auto _rc = cv::boundingRect(_temp);
		if (_rc.empty()){
			continue;
		}

		if (_rc.width < 10 && _rc.height < 10){
			continue;
		}

		//cv::rectangle(_mat, _rc, cv::Scalar(0, 0, 255));
		cv::Mat _tempMat = _mat(_rc);

		EM_IMAGE_FONTS_PRO _eifp;
		RecognitionFonts(_tempMat, _eifp);

		if (_eifp._text.empty()){
			continue;
		}

		EM_IMAGE_PRO eip;
		eip._rc = _rc;
		eip._eip._samilar = _eifp._samilar;
		eip._eip._text = _eifp._text;
		list.push_back(eip);
	}

	std::sort(list.begin(), list.end(), [](EM_IMAGE_PRO& a, EM_IMAGE_PRO& b){
		return a._rc.x < b._rc.x;
	});

	std::vector<std::vector<EM_IMAGE_PRO>> boxList;
	for (auto &temp : list)
	{
		auto _mid = (temp._rc.y * 2 + temp._rc.height) / 2;

		bool bInBox = false;
		for (auto &_box : boxList)
		{
			for (auto &_empTemp : _box){
				if (_mid >= _empTemp._rc.y && _mid <= (_empTemp._rc.y + _empTemp._rc.height))
				{
					bInBox = true;
					break;
				}
			}
			
			if (bInBox){
				_box.push_back(temp);
				break;
			}
		}

		if (!bInBox){
			std::vector<EM_IMAGE_PRO> _tempList;
			_tempList.push_back(temp);
			boxList.push_back(_tempList);
		}
	}


	std::sort(boxList.begin(), boxList.end(), [](std::vector<EM_IMAGE_PRO>& a, std::vector<EM_IMAGE_PRO> & b){
		return a.at(0)._rc.y < b.at(0)._rc.y;
	});

	for (auto& temp : boxList)
	{
		if (temp.size() < 3){
			continue;
		}
		
		std::sort(temp.begin(), temp.end(), [](EM_IMAGE_PRO &a, EM_IMAGE_PRO &b){
			return a._rc.x < b._rc.x;
		});

		for (auto _val: temp){
			_ret.append(_val._eip._text);
		}
		_ret.push_back('|');
	}

	//std::cout << _ret << std::endl;
}

void ImageRecognition::RecognitionFonts(cv::Mat _mat, EM_IMAGE_FONTS_PRO & _eip)
{

	cv::Mat _destMat1 = ConvertImgTo32Pix(_mat);
	if (_destMat1.empty()){
		return;
	}

	std::vector<EM_IMAGE_FONTS_PRO> _tempList;
	for (auto & _temp : m_fontsList)
	{
		auto _sem = GetImageSemblance(_destMat1, _temp._mat);
		if (_sem < 30){
			_eip._samilar = _sem;
			_eip._text = _temp._text;
			return;
		}

		EM_IMAGE_FONTS_PRO _tempEip; 
		_tempEip._samilar = _sem;
		_tempEip._text = _temp._text;
		_tempList.push_back(_tempEip);
	}

	auto _iter = std::min_element(_tempList.begin(), _tempList.end(), [](EM_IMAGE_FONTS_PRO& a, EM_IMAGE_FONTS_PRO& b)->bool{
		return a._samilar < b._samilar;
	});

	if (_iter != _tempList.end()){
		if (_iter->_samilar < 250){
			_eip._samilar = _iter->_samilar;
			_eip._text = _iter->_text;
		}
	}
}

int ImageRecognition::GetImageSemblance(cv::Mat  _mat1, cv::Mat _mat2)
{

	int *_arr1 = new int[m_bltPixel * m_bltPixel];
	int *_arr2 = new int[m_bltPixel * m_bltPixel];
	int iAvg1 = 0, iAvg2 = 0;

	for (int i = 0; i < m_bltPixel; i++)
	{
		auto _data1 = _mat1.ptr(i);
		auto _data2 = _mat2.ptr(i);

		int tmp = i * m_bltPixel;
		for (int j = 0; j < m_bltPixel; j++)
		{
			int tmp1 = tmp + j;
			_arr1[tmp1] = _data1[j];
			_arr2[tmp1] = _data2[j];

			iAvg1 += _arr1[tmp1];
			iAvg2 += _arr2[tmp1];
		}
	}

	iAvg1 = iAvg1 / (m_bltPixel* m_bltPixel);
	iAvg2 = iAvg2 / (m_bltPixel* m_bltPixel);

	for (int i = 0; i < m_bltPixel* m_bltPixel; i++)
	{
		_arr1[i] = (_arr1[i] >= iAvg1) ? 1 : 0;
		_arr2[i] = (_arr2[i] >= iAvg2) ? 1 : 0;
	}

	int iDiffNum = 0;
	for (int i = 0; i < m_bltPixel * m_bltPixel; i++)
	{
		if (_arr1[i] != _arr2[i]){
			++iDiffNum;
		}
	}

	delete _arr1;
	delete _arr2;

	return iDiffNum;
}

cv::Mat ImageRecognition::ConvertImgTo32Pix(cv::Mat& _mat)
{
	if (_mat.empty()){
		return cv::Mat();
	}

	cv::Mat _cvtMat1;
	cv::cvtColor(_mat, _cvtMat1, CV_BGR2GRAY);
	cv::threshold(_cvtMat1, _cvtMat1, 180/*255 / 2*/, 255, cv::THRESH_BINARY);
	cv::Rect _Mat1Rect = cv::boundingRect(_cvtMat1);
	if (_Mat1Rect.empty()){
		return cv::Mat();
	}

	cv::Mat _mat1New = _cvtMat1(_Mat1Rect);
	//cv::Mat _blurMat1;
	//blur(_mat1New, _blurMat1, cv::Size(2, 2));

	auto _width = _Mat1Rect.width > _Mat1Rect.height ? _Mat1Rect.width : _Mat1Rect.height;
	cv::Mat _destMat(_width, _width, CV_8UC1, cv::Scalar(0));

	cv::Mat _roi = _destMat(cv::Rect((_width - _Mat1Rect.width) / 2  , (_width - _Mat1Rect.height) / 2, _Mat1Rect.width, _Mat1Rect.height));
	_mat1New.copyTo(_roi);

	cv::Mat _destMat1;
	cv::resize(_destMat, _destMat1, cv::Size(m_bltPixel, m_bltPixel), (0, 0), (0, 0), cv::INTER_NEAREST);

	return _destMat1;
}
