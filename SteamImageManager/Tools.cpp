#include "stdafx.h"
#include "Tools.h"
#include <string>


ULONG Tools::GetCurrentProcessModuleBase(const WCHAR* ModeName)
{
	HMODULE hMods[1024];
	HANDLE hProcess = (HANDLE)-1;
	DWORD cbNeeded;
	unsigned int i;
	WCHAR szModName[MAX_PATH] = { 0 };

	// Get a list of all the modules in this process.
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded) == 0)
	{
		CloseHandle(hProcess);
		return 0;
	}
	for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
	{// Get the full path to the module's file.
		if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(WCHAR)))
		{
			if (wcsstr(szModName, ModeName) != NULL)
			{
				CloseHandle(hProcess);
				return (ULONG)hMods[i];
			}
		}
	}
	// Release the handle to the process.
	CloseHandle(hProcess);
	return 0;
}

void Tools::message(char* _format, ...)
{
	std::string temp;
	va_list marker = { 0 };
	va_start(marker, _format);
	size_t num_of_chars = _vscprintf(_format, marker);
	if (num_of_chars > temp.capacity())
	{
		temp.reserve(num_of_chars + 1);
	}
	vsprintf_s(const_cast<char*>(temp.c_str()), num_of_chars + 1, _format, marker);

	OutputDebugStringA(temp.c_str());
}


int Tools::retrunLparamDown(int key)
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

DWORD Tools::findPattern(DWORD startAddress, DWORD fileSize, unsigned char* pattern, char mask[])
{
	DWORD pos = 0;
	int searchLen = strlen(mask) - 1;
	//从内存内逐个字节进行比较
	for (DWORD retAddress = startAddress; retAddress < startAddress + fileSize; retAddress++)
	{
		//判断当前地址是否有效
		if (IsBadReadPtr((const void *)retAddress, 1))
		{
			pos = 0;
			continue;
		}
		if (*(PBYTE)retAddress == pattern[pos] || mask[pos] == '?')
		{
			if (mask[pos + 1] == '\0')
			{
				return (retAddress - searchLen);
			}

			pos++;
		}
		else
		{
			pos = 0;
		}
	}
	return NULL;
}

MODULEINFO Tools::getModuleInfo(const char* moduleName)
{
	MODULEINFO moudleInfo = { NULL };
	HMODULE hMoudle = GetModuleHandleA(moduleName);
	if (hMoudle)
	{
		GetModuleInformation(GetCurrentProcess(), hMoudle, &moudleInfo, sizeof(MODULEINFO));
	}
	return moudleInfo;
}
