#pragma once
namespace GameCall
{
	DWORD GetObjByName(DWORD rEsi, DWORD rOffset2, char* name, DWORD inNum);
	DWORD GetObjByName2(DWORD rEcx, DWORD rCall, char* name);
}