#include "ImageUtils.h"

bool ImageUtils::CImage2cvMat(CImage& cimage, cv::Mat &mat)
{
	if (true == cimage.IsNull())
	{
		return false;
	}


	int nChannels = cimage.GetBPP() / 8;
	if ((1 != nChannels) && (3 != nChannels))
	{
		return false;
	}


	int nWidth = cimage.GetWidth();
	int nHeight = cimage.GetHeight();


	//重建mat  
	if (1 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC1);
	}
	else if (3 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC3);
	}


	//拷贝数据  
	uchar* pucRow;                                  //指向数据区的行指针  
	uchar* pucImage = (uchar*)cimage.GetBits();     //指向数据区的指针  
	int nStep = cimage.GetPitch();                  //每行的字节数,注意这个返回值有正有负  


	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
				}
			}
		}
	}

	return true;
}
