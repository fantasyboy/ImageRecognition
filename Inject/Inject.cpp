// Inject.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Inject.h"
#include <Tlhelp32.h>

DWORD gPID = 0;

VOID OutInfo(WCHAR *formatstring, ...)
{
	WCHAR buff[MAX_PATH] = { 0 };
	WCHAR buffA[MAX_PATH] = { 0 };
	va_list arglist;
	va_start(arglist, formatstring);
	_vsnwprintf_s(buff, MAX_PATH, _TRUNCATE, formatstring, arglist);//注意第二个参数不能用sizeof
	va_end(arglist);
	swprintf_s(buffA, MAX_PATH, L"YY-%s\n", buff);//注意第二个参数不能用sizeof
	OutputDebugStringW(buffA);
}

DWORD GetProcessID(WCHAR* ProcessName)
{
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);//进程快照
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		if (Process32First(hSnapshot, &pe))
		{
			while (Process32Next(hSnapshot, &pe))
			{
				if (wcscmp(ProcessName, pe.szExeFile) == 0)
				{
					CloseHandle(hSnapshot);
					return pe.th32ProcessID;
				}
			}
		}
		CloseHandle(hSnapshot);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL GetSelfPath(WCHAR* OutBuffer)//获取程序自身路径不包括程序名，最后没有\符合
{
	size_t len;
	size_t i;

	if (GetModuleFileNameW(
		NULL,//句柄，如果为NULL就是自己程序
		OutBuffer,//指定一个字串缓冲区，用于存放这个路径
		MAX_PATH//前面指定的这个缓冲区的大小
		)//获取一个模块的运行路径，包括模块名，失败返回0
		== 0)
	{
		return FALSE;
	}

	len = wcslen(OutBuffer);
	for (i = len; i >= 0; i -- )
	{
		if (OutBuffer[i] == '\\')
		{
			OutBuffer[i+1] = 0;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL RemoteThreadInject(
	DWORD PID,
	WCHAR* DLLPath
	)
{
	BOOL bRet = FALSE;
	HANDLE hProcess = NULL;
	HMODULE hMoude = NULL;
	HANDLE hThread = NULL;
	PVOID pDllName = NULL;
	size_t dwStrLen = 0;

	//打开进程
	hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		OutInfo(L"打开远程进程失败");
		bRet = FALSE;
		goto Exit;
	}
	//在远程进程中申请空间
	pDllName = VirtualAllocEx(
		hProcess, //远程进程句柄
		NULL,		//建议开始地址
		0x1000,		//分配空间大小
		MEM_COMMIT,	//空间初始化全0
		PAGE_EXECUTE_READWRITE
		);	//空间权限
	if (pDllName == NULL)
	{
		OutInfo(L"在远程进程中申请内存失败");
		bRet = FALSE;
		goto Exit;
	}
	dwStrLen = wcslen(DLLPath) * 2 + 2;
	//写DLL路径
	if (WriteProcessMemory(hProcess, pDllName, DLLPath, dwStrLen, NULL) == 0)
	{
		OutInfo(L"在远程进程中写入DLL路径失败");
		bRet = FALSE;
		goto Exit;
	}
	//获取LoadLibraryW地址
	hMoude = GetModuleHandleW(L"Kernel32.dll");
	LPTHREAD_START_ROUTINE pfnLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hMoude, "LoadLibraryW");
	if (pfnLoadLibrary == NULL)
	{
		OutInfo(L"获取LoadLibraryW地址失败");
		bRet = FALSE;
		goto Exit;
	}
	//创建远程线程
	hThread = CreateRemoteThread(hProcess, NULL, 0, pfnLoadLibrary, pDllName, 0, NULL);
	if (hThread == NULL)
	{
		OutInfo(L"创建远程线程失败");
		//释放远程空间
		VirtualFreeEx(hProcess, pDllName, dwStrLen, MEM_DECOMMIT);
		bRet = FALSE;
		goto Exit;
	}
	WaitForSingleObject(hThread, INFINITE);
	VirtualFreeEx(hProcess, pDllName, dwStrLen, MEM_DECOMMIT);
	bRet = TRUE;
Exit:
	if (hThread != NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}
	if (hMoude != NULL)
	{
		FreeLibrary(hMoude);
		hMoude = NULL;
	}
	if (hProcess > NULL)
	{
		CloseHandle(hProcess);
		hProcess = NULL;
	}

	return bRet;
}
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	DWORD PID = 0;
	WCHAR DLLPath[MAX_PATH] = { 0 };
	HWND GameWndHandle = NULL;

	if (GetSelfPath(DLLPath) == FALSE)
	{
		OutInfo(L"获取自身路径失败");
		return 0;
	}
	wcscat_s(DLLPath, MAX_PATH, L"SteamImageManager.dll");
	//OutInfo(L"%s", DLLPath);
	
	while (TRUE)
	{
		PID = GetProcessID(L"Steam.exe");
		GameWndHandle = FindWindow(
			TEXT("vguiPopupWindow"),
			NULL
			);

		//auto hwnd = ::FindWindow(L"UnrealWindow", L"绝地求生 ");

		if (/*hwnd &&*/
			GameWndHandle != NULL &&
			PID > 4 &&
			gPID != PID
			)
		{
			//注入
			OutInfo(L"开始注入");
			if (RemoteThreadInject(PID, DLLPath) == FALSE)
			{
				OutInfo(L"注入失败");
			}
			else
			{
				OutInfo(L"注入成功");
				gPID = PID;
			}
		}

		Sleep(5000);
	}

	return 0;
}