#pragma once

namespace Tools
{
	ULONG GetCurrentProcessModuleBase(const WCHAR* ModeName);
	void message(char* _format, ...);
	int retrunLparamDown(int key);
	DWORD findPattern(DWORD startAddress, DWORD fileSize, unsigned char* pattern, char mask[]);
	MODULEINFO getModuleInfo(const char* moduleName);

	template<class T> T Read(DWORD _addr);
	template<class T> bool Write(DWORD _addr, T value);
}

template<class T>
bool Tools::Write(DWORD _addr, T value)
{
	__try
	{
		DWORD OldProtect = { 0 };
		VirtualProtect((LPVOID)_addr, sizeof(T), PAGE_READWRITE, &OldProtect);
		*(T*)_addr = value;
		VirtualProtect((LPVOID)_addr, sizeof(T), OldProtect, &OldProtect);
	}
	__except (1)
	{
		return false;
	}

	return true;
}

template<class T>
T Tools::Read(DWORD _addr)
{
	__try
	{
		return *(T*)(_addr);
	}
	__except (1)
	{
		return T();
	}
}
