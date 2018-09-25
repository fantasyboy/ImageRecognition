#include "main.h"
#include <sstream>
#include "ImageInterception.h"
#include "ImageUtils.h"
#include "ImageRecognition.h"
#include "ShareMemoryService.h"
#include <windows.h>


#include "opencv2/highgui.hpp"

config_t g_concig;
result_callback_func g_callback_func = nullptr;
void* g_t = nullptr;

result_t g_lastResult = { 0 };
DWORD g_lastSendTime = { 0 };
HANDLE g_evtHandle = { 0 };
bool bSendData = false;
void __stdcall install()
{
	auto _handle = CreateThread(NULL, NULL, ThreadProcMain, NULL, NULL, NULL);
	WaitForSingleObject(_handle, INFINITE);
	if (_handle){
		CloseHandle(_handle);
	}
}

bool GetGameInfo(std::string & _result, result_t& _t);

DWORD WINAPI ThreadProcMain(_In_ LPVOID lpParameter)
{

	if (!ShareMemoryService::GetInstance()->Initialize())
	{
		//std::cout << "初始化共享内存失败！" << std::endl;
		return 0;
	}
	g_evtHandle = CreateEvent(NULL, FALSE, FALSE, "FANT");
	if (!g_evtHandle)
	{
		return 0;
	}
	auto pData = ShareMemoryService::GetInstance()->GetSharedMemoryPointer();

	ImageRecognition _imgRe;
	//初始化字库
	_imgRe.InitFonts();
	while (true)
	{
		//if (!pData->bInject)
		//{
		//	Sleep(5000);
		//	continue;
		//}

		////1.查找绝地求生窗口是否存在
		auto hwnd = ::FindWindow("UnrealWindow", "绝地求生 ");
		if (!hwnd)
		{
			Sleep(1000);
			continue;
		}

		//3.发送模拟按键
		PostMessage(hwnd, WM_KEYDOWN, VK_F12, retrunLparamDown(VK_F12));
		PostMessage(hwnd, WM_KEYUP, VK_F12, (0xC0000000 | retrunLparamDown(VK_F12)));

		//4. 等待传输数据完成
		DWORD status =  WaitForSingleObject(g_evtHandle, 5000);
		switch (status)
		{
		case WAIT_OBJECT_0:
		{
			//5. 从共享内存中得到图片缓冲器

			//6. 将图片转化为 cv::Mat & 取1/4图片

			//std::cout << "size = " << pData->nNumberOfBytesToWrite << "  data = " << pData->data << std::endl;
			//7. 图片识别
			std::string image = std::string(pData->data, pData->nNumberOfBytesToWrite);
			std::vector<char> dataList;
			dataList.assign(image.begin(), image.end());
			cv::Mat jpegimage = cv::imdecode(cv::Mat(dataList), CV_LOAD_IMAGE_COLOR);

			cv::Mat _mat = jpegimage(cv::Rect(0, 0, jpegimage.cols / 2, jpegimage.rows / 2));
			std::string _result;
			_imgRe.Recognition(_mat, _result);

			result_t _res_t = { 0 };
			if (GetGameInfo(_result, _res_t))
			{
				char _currPath[MAX_PATH] = { 0 };
				::GetModuleFileName(NULL, _currPath, MAX_PATH);
				PathRemoveFileSpec(_currPath);
				std::stringstream ss;
				ss << _currPath << "\\result\\" << GetTickCount() << ".jpg";
				strcpy_s(_res_t.currPath, ss.str().c_str());
				cv::imwrite(ss.str(), _mat);
				g_callback_func(_res_t, g_t);
				bSendData = false;

			}
			break;
		}
		case WAIT_TIMEOUT:
		{				

			auto hwnd = ::FindWindow("UnrealWindow", "绝地求生 ");
			if (hwnd)
			{
				PostMessage(hwnd, WM_KEYDOWN, VK_F12, retrunLparamDown(VK_F12));
				PostMessage(hwnd, WM_KEYUP, VK_F12, (0xC0000000 | retrunLparamDown(VK_F12)));
			}
			break;
		}
		}

		Sleep(2000);
	}

	return 1;
}
//DWORD WINAPI ThreadProcMain(_In_ LPVOID lpParameter)
//{

//	ImageRecognition _imgRe;
//	//初始化字库
//	_imgRe.InitFonts();
//	bool bSendData = false;
//	int iEndIndex = 0;
//	while (true)
//	{
//		//1. 截图
//		int _a =  GetTickCount();
//		CImage image;
//		if (ImageInterception::ImageCapture(image))
//		{
//			//2. 将CImage转化为cv::Mat
//			cv::Mat _mat;
//			if (ImageUtils::CImage2cvMat(image, _mat))
//			{
//				//3. 图像识别
//				std::string _result;
//				_imgRe.Recognition(_mat, _result);
//
//				//4. 判断是否游戏结束，如果结束，就返回结果到回调函数中。
//				//std::cout << _result << std::endl;
//
//				//如果找到奖励和淘汰，说明游戏结束
//				if (_result.find("奖励") != std::string::npos && _result.find("淘汰") != std::string::npos)
//				{
//					iEndIndex++;
//					Sleep(200);
//
//					if (iEndIndex > 4)
//					{
//						result_t _t = { 0 };
//
//						//判断是否组队
//						if (_result.find("队伍") != std::string::npos){
//							_t.is_team_ = true;
//						}
//						else{
//							_t.is_team_ = false;
//						}
//
//						//获取玩家名字
//						auto namepos = _result.find('|');
//						if (std::string::npos != namepos){
//							std::string _name = _result.substr(0, namepos);
//							memcpy(_t.user_name_, _name.c_str(), _name.size());
//						}
//
//						//获取排名
//						auto diPos = _result.find("第") + 2;
//						auto taotaiPos = _result.find("淘汰");
//						if (diPos != std::string::npos && taotaiPos != std::string::npos)
//						{
//							auto order_ = _result.substr(diPos, taotaiPos - diPos);
//
//							std::stringstream ss;
//							ss << order_;
//							ss >> _t.order_;
//
//						}
//
//						//获取杀人书
//						auto jiangliPos = _result.find("奖励");
//						if (taotaiPos + 4 != std::string::npos && jiangliPos != std::string::npos)
//						{
//							auto _kill = _result.substr(taotaiPos + 4, jiangliPos);
//
//							std::stringstream ss;
//							ss << _kill;
//							ss >> _t.kill_;
//						}
//
//						//获取奖励
//						auto _endText = _result.find('|', jiangliPos);
//						if (jiangliPos + 4 != std::string::npos && _endText != std::string::npos)
//						{
//							auto _score = _result.substr(jiangliPos + 4, _endText);
//
//							std::stringstream ss;
//							ss << _score;
//							ss >> _t.scores_;
//						}
//
//
//						if (g_lastResult.is_team_ == _t.is_team_ 
//							&& g_lastResult.kill_ == _t.kill_ 
//							&& g_lastResult.order_ == _t.order_
//							&& g_lastResult.scores_ == _t.scores_)
//						{
//						}
//						else
//						{
//							if (!bSendData && ((GetTickCount() - g_lastSendTime) > 1000 * 60 * 5))
//							{
//								char _currPath[MAX_PATH] = { 0 };
//								::GetModuleFileName(NULL, _currPath, MAX_PATH);
//								PathRemoveFileSpec(_currPath);
//								std::stringstream ss;
//								ss << _currPath << "\\result\\" << GetTickCount() << ".jpg";
//								image.Save(ss.str().c_str());
//								strcpy_s(_t.currPath, ss.str().c_str());
//								memcpy(&g_lastResult, &_t, sizeof(result_t));
//								g_callback_func(_t, g_t);
//								bSendData = true;
//								iEndIndex = 0;
//								g_lastSendTime = GetTickCount();
//							}
//						}
//					}
//				}
//				else{
//					iEndIndex = 0;
//					bSendData = false;
//				}
//			}
//
//		}
//		Sleep(100);
//	}
//
//	return 1;
//}

void __stdcall set_config(const config_t & con)
{
	memcpy(&g_concig, &con, sizeof(config_t));
}

void  __stdcall set_callback(const result_callback_func func,const void* _t)
{
	g_callback_func = func;
	g_t = const_cast<void*>(_t);
}

bool GetGameInfo(std::string & _result, result_t& _t)
{

	static int iEndIndex = 0;

	if (_result.empty())
	{
		return false;
	}

	if (_result.find("奖励") != std::string::npos && _result.find("淘汰") != std::string::npos)
	{
		iEndIndex++;
		if (iEndIndex > 2)
		{
			if (_result.find("队伍") != std::string::npos){
				_t.is_team_ = true;
			}
			else{
				_t.is_team_ = false;
			}

			//获取玩家名字
			auto namepos = _result.find('|');
			if (std::string::npos != namepos){
				std::string _name = _result.substr(0, namepos);
				memcpy(_t.user_name_, _name.c_str(), _name.size());
			}

			//获取排名
			auto diPos = _result.find("第") + 2;
			auto taotaiPos = _result.find("淘汰");
			if (diPos != std::string::npos && taotaiPos != std::string::npos)
			{
				auto order_ = _result.substr(diPos, taotaiPos - diPos);

				std::stringstream ss;
				ss << order_;
				ss >> _t.order_;

			}

			//获取杀人书
			auto jiangliPos = _result.find("奖励");
			if (taotaiPos + 4 != std::string::npos && jiangliPos != std::string::npos)
			{
				auto _kill = _result.substr(taotaiPos + 4, jiangliPos);

				std::stringstream ss;
				ss << _kill;
				ss >> _t.kill_;
			}

			//获取奖励
			auto _endText = _result.find('|', jiangliPos);
			if (jiangliPos + 4 != std::string::npos && _endText != std::string::npos)
			{
				auto _score = _result.substr(jiangliPos + 4, _endText);

				std::stringstream ss;
				ss << _score;
				ss >> _t.scores_;
			}


			if (g_lastResult.is_team_ == _t.is_team_
				&& g_lastResult.kill_ == _t.kill_
				&& g_lastResult.order_ == _t.order_
				&& g_lastResult.scores_ == _t.scores_)
			{
				bSendData = false;
			}
			else
			{
				if (!bSendData && ((GetTickCount() - g_lastSendTime) > 1000 * 60 * 5))
				{
					memcpy(&g_lastResult, &_t, sizeof(result_t));
					//g_callback_func(_t, g_t);
					bSendData = true;
					//iEndIndex = 0;
					//g_lastSendTime = GetTickCount();
				}
			}
		}
	}
	else{
		iEndIndex = 0;
		bSendData = false;
	}

	return bSendData;
}

int retrunLparamDown(int key)
{
	int lparamdown = 0X00000000;
	switch (key)
	{
	case VK_RETURN:lparamdown = 0X001C0001; break;
	case VK_ESCAPE:lparamdown = 0X00010001; break;

		//按下大写键
	case VK_CAPITAL:lparamdown = 0X003A0001; break;
	case VK_BACK:lparamdown = 0X000E0001; break;
	case VK_TAB:lparamdown = 0X000F0001; break;
	case VK_SPACE:lparamdown = 0X00390001; break;
	case VK_CONTROL:lparamdown = 0X001D0001; break;

	case 'A':lparamdown = 0X001E0001; break;
	case 'D':lparamdown = 0X00200001; break;
	case 'S':lparamdown = 0X001F0001; break;
	case 'Q':lparamdown = 0X00100001; break;
	case 'W':lparamdown = 0X00110001; break;
	case 'E':lparamdown = 0X00120001; break;
	case 'V':lparamdown = 0X002F0001; break;
	case 'R':lparamdown = 0X00130001; break;
	case 'Y':lparamdown = 0X00150001; break;
	case 'U':lparamdown = 0X00160001; break;
	case 'I':lparamdown = 0X00170001; break;
	case 'O':lparamdown = 0X00180001; break;
	case 'P':lparamdown = 0X00190001; break;
	case 'T':lparamdown = 0X00140001; break;
	case 'F':lparamdown = 0X00210001; break;
	case 'H':lparamdown = 0X00230001; break;
	case 'J':lparamdown = 0X00240001; break;
	case 'C':lparamdown = 0X002E0001; break;
	case 'K':lparamdown = 0X00250001; break;
	case 'B':lparamdown = 0X00300001; break;
	case 'L':lparamdown = 0X00260001; break;
	case 'M':lparamdown = 0X00320001; break;
	case 'Z':lparamdown = 0X002C0001; break;
	case 'X':lparamdown = 0X002D0001; break;

	case '1':lparamdown = 0X00020001; break;
	case '2':lparamdown = 0X00030001; break;
	case '3':lparamdown = 0X00040001; break;
	case '4':lparamdown = 0X00050001; break;
	case '5':lparamdown = 0X00060001; break;
	case '6':lparamdown = 0X00070001; break;
	case '7':lparamdown = 0X00080001; break;
	case '8':lparamdown = 0X00090001; break;
	case '9':lparamdown = 0X000A0001; break;
	case '0':lparamdown = 0X000B0001; break;

	case VK_F1:lparamdown = 0X003B0001; break;
	case VK_F2:lparamdown = 0X003C0001; break;
	case VK_F3:lparamdown = 0X003D0001; break;
	case VK_F4:lparamdown = 0X003E0001; break;
	case VK_F5:lparamdown = 0X003F0001; break;
	case VK_F6:lparamdown = 0X00400001; break;
	case VK_F7:lparamdown = 0X00410001; break;
	case VK_F8:lparamdown = 0X00420001; break;
	case VK_F9:lparamdown = 0X00430001; break;
	case VK_F10:lparamdown = 0X00440001; break;
	case VK_F11:lparamdown = 0X00570001; break;
	case VK_F12:lparamdown = 0X00580001; break;

	case VK_NUMPAD1:lparamdown = 0X004F0001; break;
	case VK_NUMPAD2:lparamdown = 0X00500001; break;
	case VK_NUMPAD3:lparamdown = 0X00510001; break;
	case VK_NUMPAD4:lparamdown = 0X004B0001; break;
	case VK_NUMPAD5:lparamdown = 0X004C0001; break;
	case VK_NUMPAD6:lparamdown = 0X004D0001; break;
	case VK_NUMPAD7:lparamdown = 0X00470001; break;
	case VK_NUMPAD8:lparamdown = 0X00480001; break;
	case VK_NUMPAD9:lparamdown = 0X00490001; break;
	case VK_NUMPAD0:lparamdown = 0X00520001; break;

	case VK_UP:lparamdown = 0X01480001; break;
	case VK_DOWN:lparamdown = 0X01500001; break;
	case VK_LEFT:lparamdown = 0X014B0001; break;
	case VK_RIGHT:lparamdown = 0X014D0001; break;
	case VK_SHIFT:lparamdown = 0X002A0001; break;
	case VK_MENU:lparamdown = 0X20380001; break;

	case VK_NUMLOCK:lparamdown = 0X01450001; break;
	case VK_DIVIDE:lparamdown = 0X01350001; break;
	case VK_MULTIPLY:lparamdown = 0X00370001; break;
	case VK_SUBTRACT:lparamdown = 0X004A0001; break;

	case VK_ADD:lparamdown = 0X004E0001; break;
	case VK_DECIMAL:lparamdown = 0X00110001; break;
	case VK_OEM_MINUS:lparamdown = 0X000C0001; break;
	case VK_OEM_PLUS:lparamdown = 0X000D0001; break;
	case VK_OEM_3:lparamdown = 0X00290001; break;
	case VK_OEM_5:lparamdown = 0X002B0001; break;

	}
	return lparamdown;
}
