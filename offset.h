#pragma once
#include <Windows.h>
#include <iostream>
#include <d3d9.h>
#include <d3dx9.h>
#include "detours.h"


//address for esp
#define Entitylist 0xDCF81C
#define Eingame 0x013E3FEC 
#define viewmatrix 0x006E1804
#define entitydistance 0x74


//addresses for memory hack
#define ammo_address		0x005BFCB9 
#define rapidfire_address	0x005C161C
#define norecoil_address	0x00434611
#define noshake_address		0x005C185D 
#define godview1_address	0x005B9B46
#define godview2_address	0x005B9B85

//bg color shit like that
#define black D3DCOLOR_XRGB(36, 39, 45)
#define red D3DCOLOR_XRGB(220, 96, 105)
#define green D3DCOLOR_XRGB(141, 178, 110)
#define blue D3DCOLOR_XRGB(87, 165, 237)
#define white D3DCOLOR_XRGB(161, 169, 183)
#define cls system("cls")
//strucs
struct vec3
{
	float x, y, z;
};
struct vec2
{
	float x, y;
};
struct matrix
{
	float Matrix[16];
};
 
 
bool WorldToScreen(vec3& vecOrigin, vec2& vecscreen, float* matrix, int windowWidth, int windowHeight)
{
	 
	vecscreen.x		= vecOrigin.x * matrix[0] + vecOrigin.y * matrix[1] + vecOrigin.z * matrix[2] + matrix[3];
	vecscreen.y		= vecOrigin.x * matrix[4] + vecOrigin.y * matrix[5] + vecOrigin.z * matrix[6] + matrix[7];
	float		w	= vecOrigin.x * matrix[12] + vecOrigin.y * matrix[13] + vecOrigin.z * matrix[14] + matrix[15];

	if (w < 0.1f)
		return false;
	 
	vec2 NDC;
	NDC.x = vecscreen.x / w;
	NDC.y = vecscreen.y / w;

	vecscreen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	vecscreen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);

	//converttorange(vecscreen);
	return true;
}

void Patch(byte* desintation, byte* newbytes, byte len)
{
	DWORD oldprotect;
	VirtualProtect(desintation, len, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(desintation, newbytes, len);
	VirtualProtect(desintation, len, oldprotect, &oldprotect);
}
void Nop(byte* originalbytes, byte len)
{
	DWORD oldprotect;
	VirtualProtect(originalbytes, len, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(originalbytes, 0x90, len);
	VirtualProtect(originalbytes, len, oldprotect, &oldprotect);
}