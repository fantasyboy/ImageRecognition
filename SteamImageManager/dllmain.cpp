// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include "SteamManager.h"
#include "Tools.h"
#include "ShareMemoryClient.h"


#include <atlbase.h>

HANDLE g_hThread = { 0 };
DWORD WINAPI ThreadProc(
	_In_ LPVOID lpParameter
	);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		g_hThread = ::CreateThread(NULL, NULL, ThreadProc, NULL, NULL, NULL);

	}
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	case DLL_PROCESS_DETACH:
	{
		if (g_hThread)
		{
			CloseHandle(g_hThread);
		}
		break;
	}
	}
	return TRUE;
}

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	//1. 判断steamclient.dll是否加载，确定是否打开stream
	ULONG steamclient_Base = 0;
	while (TRUE)
	{
		steamclient_Base = Tools::GetCurrentProcessModuleBase(L"steamclient.dll");
		if (steamclient_Base != 0)
		{
			Tools::message("HXL: steam界面打开！\n");
			break;
		}
		Tools::message("HXL: 等待steam界面。\n");
		Sleep(1000);
	}

	//2. 等待用户完成登录
	CRegKey creg;
	while (TRUE)
	{
		Tools::message("HXL: 等待登录steam!\n");

		HKEY hOpen = { 0 };
		if (ERROR_SUCCESS == creg.Open(HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\ActiveProcess"))
		{
			DWORD account = { 0 };
			creg.QueryDWORDValue(L"ActiveUser", account);
			if (account > 0)
			{
				Tools::message("HXL: 登录完成，等待下面操作！\n");
				break;
			}
		}

		Sleep(1000);
	}

	if (!SteamManager::GetInstance()->RestoreGameSet())
	{
		Tools::message("HXL: 修改游戏配置失败！ \n");
		return false;
	}


	if (!ShareMemoryClient::GetInstance()->Initialize())
	{
		Tools::message("HXL: 初始化共享内存客户端失败！ \n");
		return false;
	}

	if (!SteamManager::GetInstance()->Initialize())
	{
		Tools::message("HXL: 初始化Steam截图相关操作失败！ \n");
		return false;
	}

	return 1;
}

