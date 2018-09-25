// ImageRecognition.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <cstdint>
#include <iostream>

typedef struct _config_t
{
	std::uint32_t duration_seconds_;	// 间隔时长(秒)
}config_t;

typedef struct _result_t
{
	bool is_team_;						// 是否组队
	char user_name_[128];				// 玩家名(UTF8编码)
	std::uint32_t order_;				// 排名
	std::uint32_t kill_;				// 杀人数
	std::uint32_t scores_;				// 分数
	char currPath[MAX_PATH];            //图片全路径
}result_t;


void __stdcall callback_func(const result_t& _t, const void* _s);
typedef void(__stdcall *set_config)(const config_t & con);
typedef void(__stdcall *result_callback_func)(const result_t &, const void*);
typedef void(__stdcall *set_callback)(result_callback_func func, const void*);
typedef void(__stdcall *install)();
int _tmain(int argc, _TCHAR* argv[])
{

	wchar_t _currPath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, _currPath, MAX_PATH);

	auto hModule = LoadLibrary("ImageRecognitionDll.dll");
	if (!hModule){
		return -1;
	}

	//获取到设置配置文件函数
	set_config pSetConfig = (set_config)GetProcAddress(hModule, "set_config");
	if (!pSetConfig){
		std::cout << "获取模块函数set_config失败！" << std::endl;
		return -1;
	}


	config_t _config;
	_config.duration_seconds_ = 1000;
	pSetConfig(_config);

	//获取到设置回调函数
	set_callback pSetCallback = (set_callback)GetProcAddress(hModule, "set_callback");
	if (!pSetCallback){
		std::cout << "获取模块函数set_callback失败！" << std::endl;
		return -1;
	}

	pSetCallback(callback_func, nullptr);

	//开始执行
	install _inst = (install)GetProcAddress(hModule, "install");
	if (!_inst){
		std::cout << "获取模块函数install失败！" << std::endl;
		return -1;
	}

	_inst();

	getchar();
	FreeLibrary(hModule);


	return 0;
}

void __stdcall callback_func(const result_t& _t, const void* _s)
{
	std::cout << "名字：" << _t.user_name_ << " 是否组队：" << _t.is_team_ << " 排名：" << _t.order_ << " 杀人数：" << _t.kill_ << " 分数：" << _t.scores_  << "图片路径: "<< _t.currPath << std::endl;
}

