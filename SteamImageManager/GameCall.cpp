#include "stdafx.h"
#include "GameCall.h"
#include "Tools.h"


DWORD GameCall::GetObjByName(DWORD rEsi, DWORD rOffset2, char* name, DWORD inNum)
{
	DWORD rEax = 0;
	__try
	{
		__asm
		{
			MOV ESI, rEsi;
			MOV EAX, DWORD PTR DS : [ESI + 0xA10];
			LEA ECX, DWORD PTR DS : [ESI + 0xA10];
			PUSH 0;
			PUSH name;
			PUSH inNum;
			MOV EAX, DWORD PTR [EAX + 028h];
			CALL EAX;
		}
	}
	__except (1)
	{
		//Tools::message("HXL: 屏蔽游戏功能异常！\n");
		return 0;
	}
	return rEax; 
}

DWORD GameCall::GetObjByName2(DWORD rEcx, DWORD rCall, char* name)
{
	__try
	{
		DWORD ControlAddr = 0;
		__asm
		{
			MOV ECX, rEcx;
			PUSH name;
			PUSH 2;
			PUSH 2;
			CALL rCall;
			MOV ControlAddr, EAX;
		}

		if (ControlAddr)
		{
			__asm
			{
				MOV ESI, ControlAddr;
				MOV DWORD PTR DS : [ESI], 0;
				MOV EAX, DWORD PTR DS : [ESI + 0x8];
				AND EAX, 0xF2FFFFFF;
				OR EAX, 0x2000000;
				MOV DWORD PTR DS : [ESI + 0x8], EAX;
			}
		}
	}
	__except (1)
	{
		//Tools::message("HXL: 屏蔽游戏功能2失败！\n");
	}

	return 0;
}
