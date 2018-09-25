#include "stdafx.h"
#include "SteamManager.h"
#include "Tools.h"
#include "ShareMemoryClient.h"
#include "GameCall.h"


#include <string>

unsigned char szEcxPattern[37] = { 0x8B, 0x43, 0x10, 0x8D, 0x0C, 0xF0, 0x8D, 0x04, 0xD5, 0x00, 0x00, 0x00, 0x00, 0x50, 0x51, 0x8D, 0x41, 0x08, 0x50, 0xE8, 0x2A, 0x68, 0x6D, 0x00, 0x83, 0xC4, 0x0C, 0x8B, 0x43, 0x10, 0xC7, 0x04, 0xF0, 0x00, 0x00, 0x00, 0x00 };
unsigned char szEcxPattern1[25] = { 0x75, 0x04, 0x33, 0xC9, 0xEB, 0x0A, 0x8B, 0x87, 0x68, 0x07, 0x00, 0x00, 0x8B, 0x4C, 0xC8, 0x04, 0x8B, 0x01, 0x56, 0xFF, 0x90, 0x8C, 0x00, 0x00, 0x00 };
unsigned char szEcxPattern2[54] = { 0xFF, 0x75, 0x0C, 0x8B, 0x55, 0x08, 0x52, 0xFF, 0xB3, 0xDC, 0x12, 0x00, 0x00, 0xE8, 0x86, 0x7A, 0xF7, 0xFF, 0x83, 0xC4, 0x0C, 0xC7, 0x83, 0x84, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x01, 0x5F, 0x5E, 0x5B, 0x8B, 0xE5, 0x5D, 0xC2, 0x08, 0x00, 0xFF, 0x75, 0x0C, 0x8B, 0x55, 0x08, 0x8D, 0x86, 0x10, 0x0A, 0x00, 0x00 };
unsigned char szGetCurrentThreadIdCrc[61] = { 0x55, 0x8B, 0xEC, 0x81, 0xEC, 0x40, 0x02, 0x00, 0x00, 0x53, 0x56, 0x8B, 0x35, 0x90, 0x12, 0xEC, 0x37, 0x57, 0x8B, 0xF9, 0xFF, 0x15, 0x98, 0x23, 0xC6, 0x37, 0x3B, 0x86, 0xEC, 0x12, 0x00, 0x00, 0x8B, 0x35, 0x68, 0x27, 0xC6, 0x37, 0x74, 0x18, 0x6A, 0x00, 0x68, 0x6C, 0x01, 0x00, 0x00, 0x68, 0xC8, 0x7C, 0xCC, 0x37, 0x6A, 0x00, 0x68, 0x60, 0x23, 0xCA, 0x37, 0xFF, 0xD6 };
unsigned char szCallPattern[48] = { 0x55, 0x8B, 0xEC, 0x81, 0xEC, 0x04, 0x02, 0x00, 0x00, 0x53, 0x57, 0x8B, 0x7D, 0x08, 0x8B, 0xD9, 0x57, 0xE8, 0x9A, 0xF1, 0xFF, 0xFF, 0x84, 0xC0, 0x0F, 0x84, 0xB0, 0x01, 0x00, 0x00, 0x8B, 0xC7, 0x83, 0xE8, 0x01, 0x74, 0x48, 0x83, 0xE8, 0x01, 0x74, 0x4C, 0x83, 0xE8, 0x01, 0x74, 0x47, 0x57 };


std::unique_ptr<SteamManager> SteamManager::m_pInstance(new SteamManager);
SteamManager::SteamManager() : m_CreateFilePtr(nullptr), m_WriteFilePtr(nullptr), m_hFile2Image(NULL), m_ReadFilePtr(nullptr)
{
}


SteamManager::~SteamManager()
{
}

SteamManager* SteamManager::GetInstance()
{
	return m_pInstance.get();
}

bool SteamManager::Initialize()
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	m_CreateFilePtr = DetourFindFunction("kernelbase.dll", "CreateFileW");
	if (m_CreateFilePtr)
	{
		//Tools::message("HXL: m_CreateFilePtr = %x \n", m_CreateFilePtr);
		DetourAttach(&m_CreateFilePtr, MyCreateFileW);
	}
	else{
		Tools::message("HXL: Not Find MyCreateFileW \n");
	}
	
	m_WriteFilePtr = DetourFindFunction("kernelbase.dll", "WriteFile");
	if (m_WriteFilePtr)
	{
		//Tools::message("HXL: m_WriteFilePtr = %x \n", m_WriteFilePtr);
		DetourAttach(&m_WriteFilePtr, MyWriteFile);
	}
	else
	{
		Tools::message("HXL: Not Find m_WriteFilePtr \n");
	}

	m_ReadFilePtr = DetourFindFunction("kernelbase.dll", "ReadFile");
	if (m_ReadFilePtr)
	{
		//Tools::message("HXL: m_ReadFilePtr = %x \n", m_ReadFilePtr);
		DetourAttach(&m_ReadFilePtr, MyReadFile);
	}
	else{
		Tools::message("HXL: Not Find MyReadFile \n");
	}

	m_CloseHandlePtr = DetourFindFunction("kernelbase.dll", "CloseHandle");
	if (m_CloseHandlePtr)
	{
		//Tools::message("HXL: m_ReadFilePtr = %x \n", m_CloseHandlePtr);
		DetourAttach(&m_CloseHandlePtr, MyCloseHandle);
	}
	else{
		Tools::message("HXL: Not Find MyCloseHandle \n");
	}

	if (NO_ERROR != DetourTransactionCommit())
	{
		return false;
	}

	m_EvntHandle = OpenEventA(EVENT_ALL_ACCESS, FALSE, "FANT");
	if (!m_EvntHandle)
	{
		return false;
	}

	Tools::message("HXL: ok \n");
	return true;
}

BOOL WINAPI SteamManager::MyWriteFile(_In_ HANDLE hFile, _In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer, _In_ DWORD nNumberOfBytesToWrite, _Out_opt_ LPDWORD lpNumberOfBytesWritten, _Inout_opt_ LPOVERLAPPED lpOverlapped)
{
	if (SteamManager::GetInstance()->GetFileHandle() == hFile)
	{
		//Tools::message("HXL: MyWriteFile hFile = %x nNumberOfBytesToWrite=%x\n", hFile, nNumberOfBytesToWrite);

		//拷贝图片数据到共享内存并设置信号
		auto pData = ShareMemoryClient::GetInstance()->GetSharedMemoryPointer();
		pData->nNumberOfBytesToWrite = nNumberOfBytesToWrite;
		memset(pData->data, 0, MAX_IAMGE_SIZE);
		memcpy(pData->data, lpBuffer, nNumberOfBytesToWrite);

		SetEvent(SteamManager::GetInstance()->GetEventHandle());

		return TRUE;
	}


	if (SteamManager::GetInstance()->GetLocalConfigHandle() == hFile)
	{
		//Tools::message("HXL: MyWriteFile GetLocalConfigHandle hFile=%x", hFile);
	}


	return pWriteFile(SteamManager::GetInstance()->GetWriteFilePtr())(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

PVOID SteamManager::GetCreateFileWPtr()
{
	return m_CreateFilePtr;
}

PVOID SteamManager::GetWriteFilePtr()
{
	return m_WriteFilePtr;
}

HANDLE WINAPI SteamManager::MyCreateFileW(_In_ LPCWSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode, _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile)
{
	//[5964] HXL: MyCreateFileW hFile = 9b0 lpFileName = \\ ? \D : \Program Files(x86)\Steam\userdata\847960779\760\remote\578080\screenshots\thumbnails\20180611154428_1.jpg dwDesiredAccess = 40000000
	std::wstring filename(lpFileName);
	if (filename.find(L"userdata") != std::string::npos &&
		filename.find(L"remote") != std::string::npos &&
		filename.find(L"screenshots") != std::string::npos &&
		filename.find(L".jpg") != std::string::npos
		)
	{
		if (filename.find(L"thumbnails") == std::string::npos)
		{
			HANDLE hFile = pCreateFileW(SteamManager::GetInstance()->GetCreateFileWPtr())(L"C:\\normal.jpg", dwDesiredAccess, dwShareMode, lpSecurityAttributes, OPEN_ALWAYS, dwFlagsAndAttributes, hTemplateFile);
			if (dwDesiredAccess == GENERIC_WRITE)
			{
				SteamManager::GetInstance()->SetFileHandle(hFile);
			}
			return hFile;
		}
		else
		{
			//return pCreateFileW(SteamManager::GetInstance()->GetCreateFileWPtr())(L"C:\\thumbnails.jpg", dwDesiredAccess, dwShareMode, lpSecurityAttributes, OPEN_EXISTING, dwFlagsAndAttributes, hTemplateFile);
			return INVALID_HANDLE_VALUE;
		}
	}

	//if (filename.find(L"\\config\\localconfig.vdf") != std::string::npos &&
	//	filename.find(L".tmp") == std::string::npos)
	//{
	//	HANDLE hFile = pCreateFileW(SteamManager::GetInstance()->GetCreateFileWPtr())(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	//	//Tools::message("HXL: MyCreateFileW---hFile=%x lpFileName=%ls dwDesiredAccess=%x", hFile, lpFileName, dwDesiredAccess);
	//	SteamManager::GetInstance()->SetLocalConfigHandle(hFile);
	//	return hFile;
	//}
	return pCreateFileW(SteamManager::GetInstance()->GetCreateFileWPtr())(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

void SteamManager::SetFileHandle(HANDLE hFile)
{
	m_hFile2Image = hFile;
}

HANDLE SteamManager::GetFileHandle()
{
	return m_hFile2Image;
}

HANDLE SteamManager::GetEventHandle()
{
	return m_EvntHandle;
}

void SteamManager::SetLocalConfigHandle(HANDLE hlocal)
{
	m_hLocalConfigHandle = hlocal;
}

HANDLE SteamManager::GetLocalConfigHandle()
{
	return m_hLocalConfigHandle;
}

BOOL WINAPI SteamManager::MyReadFile(_In_ HANDLE hFile, _Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer, _In_ DWORD nNumberOfBytesToRead, _Out_opt_ LPDWORD lpNumberOfBytesRead, _Inout_opt_ LPOVERLAPPED lpOverlapped)
{
	BOOL bRet = pReadFile(SteamManager::GetInstance()->GetReadFilePtr())(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	if (SteamManager::GetInstance()->GetLocalConfigHandle() == hFile && bRet)
	{
		//Tools::message("HXL: MyReadFile hFile=%x nNumberOfBytesToRead=%x lpNumberOfBytesRead=%x data=%s \n", hFile, nNumberOfBytesToRead, *lpNumberOfBytesRead /*, lpBuffer*/);
	}

	return bRet;
}

PVOID SteamManager::GetReadFilePtr()
{
	return m_ReadFilePtr;
}

BOOL WINAPI SteamManager::MyCloseHandle(_In_ HANDLE hObject)
{
	BOOL bRet = pCloseHandle(SteamManager::GetInstance()->GetCloseHandlePtr())(hObject);

	if (SteamManager::GetInstance()->GetFileHandle() == hObject)
	{
		SteamManager::GetInstance()->SetFileHandle(NULL);
	}

	if (SteamManager::GetInstance()->GetLocalConfigHandle() == hObject)
	{
		SteamManager::GetInstance()->SetLocalConfigHandle(NULL);
	}

	return bRet;
}

PVOID SteamManager::GetCloseHandlePtr()
{
	return m_CloseHandlePtr;
}

bool SteamManager::RestoreGameSet()
{
	DWORD time = GetTickCount();

	auto hModule = Tools::getModuleInfo("steamclient.dll");
	if ((DWORD)hModule.lpBaseOfDll < 1 || hModule.SizeOfImage < 1)
	{
		return false; 
	}
	//Tools::message("HXL: lpBaseOfDll=%x SizeOfImage=%x \n", hModule.lpBaseOfDll, hModule.SizeOfImage);
	//1. 查找特征码
	DWORD EcxPattern =  Tools::findPattern((DWORD)hModule.lpBaseOfDll, hModule.SizeOfImage, szEcxPattern, "********************????*************");
	if (0 == EcxPattern)
	{
		return false;
	}
	//Tools::message("HXL: EcxPattern=%x \n", EcxPattern);
	DWORD Base = Tools::Read<DWORD>(EcxPattern + 0x27);
	if (0 == Base)
	{
		return false;
	}
	//Tools::message("HXL: Base=%x \n", Base);
	DWORD EcxOffset1 = Tools::findPattern((DWORD)hModule.lpBaseOfDll, hModule.SizeOfImage, szEcxPattern1, "********????*********????");
	if (0 == EcxOffset1)
	{
		return false;
	}
	//Tools::message("HXL: EcxOffset1=%x \n", EcxOffset1);
	DWORD Offset_1 = Tools::Read<DWORD>(EcxOffset1 + 0x8);
	if (0 == Offset_1)
	{
		return false;
	}
	//Tools::message("HXL: Offset_1=%x \n", Offset_1);
	DWORD EcxOffset2 = Tools::findPattern((DWORD)hModule.lpBaseOfDll, hModule.SizeOfImage, szEcxPattern2, "*********????*????*****????*******************????");
	if (0 == EcxOffset2)
	{
		return false;
	}
	//Tools::message("HXL: EcxOffset2=%x \n", EcxOffset2);
	DWORD Offset_2 = Tools::Read<DWORD>(EcxOffset2 + 0x32);
	if (0 == Offset_2)
	{
		return false;
	}
	//Tools::message("HXL: Offset_2=%x \n", Offset_2);

	DWORD currentCrc =  Tools::findPattern((DWORD)hModule.lpBaseOfDll, hModule.SizeOfImage, szGetCurrentThreadIdCrc, "*****????****????***??????**????**????*****????*????***????**");
	if (0 == currentCrc)
	{
		return false;
	}
	//Tools::message("HXL: currentCrc=%x", currentCrc);
	Tools::Write(currentCrc + 0x26, (BYTE)0xEB);

	DWORD callAddr = Tools::findPattern((DWORD)hModule.lpBaseOfDll, hModule.SizeOfImage, szCallPattern, "*****????*********????**??????******************");
	if (0 == callAddr)
	{
		return false;
	}

	//Tools::message("HXL: callAddr = %x \n", callAddr);
	//Tools::message("HXL: 查找完毕，总共用时 %d 毫秒", GetTickCount() - time);

	DWORD Obj = Tools::Read<DWORD>(Base);
	if (0 == Obj)
	{
		return false;
	}

	DWORD Obj_Offset1 = Tools::Read<DWORD>(Obj + Offset_1);
	if (0 == Obj_Offset1)
	{
		return false;
	}
	//Tools::message("HXL: Obj_Offset1=%x \n", Obj_Offset1);
	DWORD Obj_Offset2 = Tools::Read<DWORD>(Obj_Offset1 + 4);
	if (0 == Obj_Offset2)
	{
		return false;
	}
	//Tools::message("HXL: Obj_Offset2=%x \n", Obj_Offset2);

	GameCall::GetObjByName(Obj_Offset2, Offset_2, "system\\InGameOverlayScreenshotNotification",3);
	Sleep(100);
	GameCall::GetObjByName(Obj_Offset2, Offset_2, "system\\InGameOverlayScreenshotPlaySound",3);
	Sleep(100);
	GameCall::GetObjByName(Obj_Offset2, Offset_2, "Software\\Valve\\Steam\\apps\\760\\cloudenabled", 2);
	Sleep(100);

	Tools::message("HXL: 还原游戏设置成功！\n");

	return true;
}



