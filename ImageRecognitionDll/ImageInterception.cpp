#include "ImageInterception.h"


bool ImageInterception::ImageCapture(CImage& image)
{
	//1.查找绝地求生窗口是否存在
	auto hwnd = ::FindWindow("UnrealWindow", "绝地求生 ");
	if (!hwnd){
		return false;
	}

	//2. 判断绝地求生是否是顶层窗口
	if (GetForegroundWindow() != hwnd){
		return false;
	}

	RECT rc;
	GetWindowRect(hwnd, &rc);

	HDC hdcWindow = GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hdcWindow, BITSPIXEL);

	// 创建图像，设置宽高，像素
	image.Create((rc.right - rc.left) / 2, (rc.bottom - rc.top) / 3 -40, 24);

	//判断窗口是否是全屏大小的窗口，如果是窗口模式，则减去窗口边框大小。
	int with = GetSystemMetrics(SM_CXFULLSCREEN);
	int heigh = GetSystemMetrics(SM_CYFULLSCREEN);

	LONG _x = 0;
	LONG _y = 0;
	if (with > (rc.right - rc.left)){
		_x = rc.left + 20;
		_y = rc.top + 40;
	}
	else{
		_x = rc.left;
		_y = rc.top;
	}


	// 对指定的源设备环境区域中的像素进行位块（bit_block）转换
	BitBlt(
		image.GetDC(),  // 保存到的目标 图片对象 上下文
		0, 0,     // 起始 x, y 坐标
		(rc.right - rc.left) / 2, (rc.bottom - rc.top) /3 - 40,  // 截图宽高
		hdcWindow,      // 截取对象的 上下文句柄
		_x, _y,           // 指定源矩形区域左上角的 X, y 逻辑坐标
		SRCCOPY);

	// 释放 DC句柄
	ReleaseDC(NULL, hdcWindow);
	// 释放图片上下文
	image.ReleaseDC();

	return true;
}
