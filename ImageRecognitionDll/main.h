#pragma once
#include <windows.h>
#include <cstdint>




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


void __stdcall  set_config(const config_t & con);
typedef void(__stdcall *result_callback_func)(const result_t &, const void *);
void __stdcall set_callback(const result_callback_func func, const void* _t);
void __stdcall install();

DWORD WINAPI ThreadProcMain(_In_ LPVOID lpParameter);
int retrunLparamDown(int key);