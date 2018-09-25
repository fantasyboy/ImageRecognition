#pragma once
#include <memory>
#include <thread>

#include "detours.h"
typedef HANDLE (WINAPI *pCreateFileW)(_In_ LPCWSTR lpFileName,_In_ DWORD dwDesiredAccess,_In_ DWORD dwShareMode,_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,_In_ DWORD dwCreationDisposition,_In_ DWORD dwFlagsAndAttributes,_In_opt_ HANDLE hTemplateFile);
typedef BOOL (WINAPI *pWriteFile)(_In_ HANDLE hFile,_In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,_In_ DWORD nNumberOfBytesToWrite,_Out_opt_ LPDWORD lpNumberOfBytesWritten,_Inout_opt_ LPOVERLAPPED lpOverlapped);
typedef BOOL (WINAPI *pReadFile)(_In_ HANDLE hFile, _Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer, _In_ DWORD nNumberOfBytesToRead, _Out_opt_ LPDWORD lpNumberOfBytesRead, _Inout_opt_ LPOVERLAPPED lpOverlapped);
typedef BOOL (WINAPI *pCloseHandle)(_In_ HANDLE hObject);
class SteamManager
{
	SteamManager();
	SteamManager(const SteamManager&) = delete;
	void operator=(const SteamManager&) = delete;

	static std::unique_ptr<SteamManager> m_pInstance;
public:
	static SteamManager* GetInstance();
	~SteamManager();

	bool Initialize(); //初始化HOOK
	static HANDLE WINAPI MyCreateFileW(_In_ LPCWSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode, _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile);
	static BOOL WINAPI MyWriteFile(_In_ HANDLE hFile, _In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer, _In_ DWORD nNumberOfBytesToWrite, _Out_opt_ LPDWORD lpNumberOfBytesWritten, _Inout_opt_ LPOVERLAPPED lpOverlapped);
	static BOOL WINAPI MyReadFile(_In_ HANDLE hFile, _Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer,_In_ DWORD nNumberOfBytesToRead,_Out_opt_ LPDWORD lpNumberOfBytesRead,_Inout_opt_ LPOVERLAPPED lpOverlapped);
	static BOOL WINAPI MyCloseHandle(_In_ HANDLE hObject);


	bool RestoreGameSet(); //重置游戏设置

	PVOID GetCreateFileWPtr();
	PVOID GetWriteFilePtr();
	PVOID GetReadFilePtr();
	PVOID GetCloseHandlePtr();

	void SetFileHandle(HANDLE hFile);
	HANDLE GetFileHandle();

	HANDLE GetEventHandle(); 

	void SetLocalConfigHandle(HANDLE hlocal);
	HANDLE GetLocalConfigHandle();
private:
	PVOID m_CreateFilePtr; //创建文件函数指针
	PVOID m_WriteFilePtr;  //写文件函数指针
	PVOID m_ReadFilePtr;   //读文件函数指针
	PVOID m_CloseHandlePtr; //关闭句柄函数指针

	HANDLE m_hFile2Image;  //游戏图片文件句柄
	HANDLE m_EvntHandle;   //命名事件句柄
	HANDLE m_hLocalConfigHandle; //本地句柄
};
