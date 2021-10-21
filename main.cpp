#include "offset.h"

typedef HRESULT(__stdcall* endScene)(IDirect3DDevice9* d3ddev);
endScene pendScene;

DWORD entity;
vec3 position{ 0 };
vec2 screenposition{ 0 };
matrix Cviewmarix{ 0 };
int isAlive;
int ingame;

//drawing text shit
LPD3DXFONT font;

//get the game width and heigth
byte window_width;
byte window_heigth;
// showmenu
bool active_font = true;
bool show_menu = true;
//esp
bool show_esp = false;
D3DCOLOR espcolor = red;
//recoil
bool show_norecoil = false;
D3DCOLOR recoilcolor = red;
//rapidfire
bool show_rapidfire = false;
D3DCOLOR rapidfirecolor = red;
//noammo
bool show_noammo = false;
D3DCOLOR ammocolor = red;
//fast change
bool show_fastchange = false;
D3DCOLOR fastcolor = red;
//god view
bool show_godview = false;
D3DCOLOR godviewcolor = red;
HRESULT __stdcall hookEndScene(IDirect3DDevice9* d3ddev)
{
	if (active_font)
	{
		D3DXCreateFont(d3ddev, 16, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &font);
		active_font = false;
	}
	//x position y position width heigth
	
	if (show_menu)
	{
		//our simple gui
		//draw border background
		D3DRECT borbgrect = { 45,175,205,580 };
		d3ddev->Clear(1, &borbgrect, D3DCLEAR_TARGET, white, 0, 0);
		//draw background
		D3DRECT bgrect = { 50,180,200,575 };
		d3ddev->Clear(1, &bgrect, D3DCLEAR_TARGET, black, 0, 0);
		//game name and made by 
		RECT gamename = {80,190,0,0};
		SetRect(&gamename, 80,190,0,0);
		font->DrawText(0, "COD4MW v1.0", 12, &gamename, DT_NOCLIP, blue);
		//draw Esp 
		RECT esptext = { 60, 220, 0, 0 };
		font->DrawText(0, "[NPAD1] Esp name",17, &esptext, DT_NOCLIP, espcolor);
		
		//draw norecoil 
		RECT norecoil = { 60, 265, 0, 0 };
 		font->DrawText(0, "[NPAD2] No Recoil ", 19, &norecoil, DT_NOCLIP, recoilcolor);
		
		//draw rapidfire 
		RECT rapidfire = { 60, 310, 0, 0 };
		font->DrawText(0, "[NPAD3] Rapid Fire", 19, &rapidfire, DT_NOCLIP, rapidfirecolor);
		
		//draw noammo 
		RECT noammo = { 60, 355, 0, 0 };
 
		font->DrawText(0, "[NPAD4] No Ammo", 16, &noammo, DT_NOCLIP, ammocolor);
		//draw FastCahnge
		RECT FastCahnge = { 60, 400, 0, 0 };
		font->DrawText(0, "[NPAD5] Fast Change", 20, &FastCahnge, DT_NOCLIP, fastcolor);
		
		//draw godview
		RECT GodView = { 60, 445, 0, 0 };
		font->DrawText(0, "[CTRL]  God View", 17, &GodView, DT_NOCLIP, godviewcolor);
		//strat of our esp stay the fuk way from this 
		//draw insert menu
		RECT insert = { 60, 490, 0, 0 };
		font->DrawText(0, "[INSRT] Show Menu", 18, &insert, DT_NOCLIP, white);
		//draw exit to show
		RECT exit = { 60,535,0,0 };
		font->DrawText(0, "[END] Exit Menu", 16, &exit, DT_NOCLIP, white);
		//draw exit 
	}
	if (*(int*)Eingame == 0)
		return pendScene(d3ddev);
	Cviewmarix = *(matrix*)viewmatrix;
	if (show_esp)
	{
		RECT textrect;
		espcolor = green;
		for (int i = 0; i < 100; i++)
		{
			//get the entity address
			entity = *(DWORD*)(Entitylist + entitydistance * i);
			//error verify
			if (!entity)
				break;
			position = *(vec3*)(entity + 0xD4);
			isAlive = *(int*)(entity + 0x30);
			if (isAlive != 1)
				continue;
			if (WorldToScreen(position, screenposition, Cviewmarix.Matrix, 1024, 768))
			{
				//the casting is useless just to shut the fuk up vs form whining(I hope this is the word) 
				SetRect(&textrect, (int)screenposition.x, (int)screenposition.y, 0, 0);
				font->DrawText(0, "yoass",6, &textrect, DT_NOCLIP, D3DCOLOR_XRGB(255, 0, 0));	
			}
		}
	}
	else { espcolor = red; }
	//ammo
	if (show_noammo)
		ammocolor = green;
	else 
		ammocolor = red; 
	//rapid fire
	if (show_rapidfire)
		rapidfirecolor = green;
	else
		rapidfirecolor = red;
	//noreocil
	if (show_norecoil)
		recoilcolor = green;
	else
		recoilcolor = red;
	//fastchange
	if (show_fastchange)
		fastcolor = green;
	else
		fastcolor = red;
	//glowview
	if (show_godview)
		godviewcolor = green;
	else
		godviewcolor = red;
	return pendScene(d3ddev);

}
void EndScene()
{
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
		return;

	D3DPRESENT_PARAMETERS d3dparams = { 0 };
	d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dparams.hDeviceWindow = GetForegroundWindow();
	d3dparams.Windowed = true;

	IDirect3DDevice9* d3ddev = nullptr;

	HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dparams.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dparams, &d3ddev);

	if (FAILED(result) || !d3ddev)
	{
		d3ddev->Release();
		return;
	}
	//if device create we get the vtable
	void** vtable = *reinterpret_cast<void***>(d3ddev);

	//now hooking 
	pendScene = (endScene)DetourFunction((PBYTE)vtable[42], (PBYTE)hookEndScene);

	//realsing the shit
	d3ddev->Release();
	d3d->Release();

}
 
int __stdcall MainThread(HMODULE hmodule)
{
	//this is for console
	/*AllocConsole();
	FILE* p;
	freopen_s(&p, "CONOUT$", "w", stdout);
	HWND consolehandle = GetConsoleWindow();
	MoveWindow(consolehandle, 1200, 0, 600, 400,1);*/
	EndScene();
 
	//our while loop where we get the key input and memory hacks
	while (true)
	{

		//presse insert to display menu
		if (GetAsyncKeyState(VK_INSERT) & 1)
			show_menu = !show_menu;
		//end th shit
		if (GetAsyncKeyState(VK_END) & 1)
		{
			//remove the hooking 
			DetourRemove((byte*)pendScene, (byte*)hookEndScene);
			break;
		}
		//display the esp 
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			show_esp = !show_esp;
		}
		//toggle norecoil
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			show_norecoil = !show_norecoil;
			//no recoil enabled
			if (show_norecoil)
			{
				Patch((byte*)norecoil_address, (byte*)"\xEB\x22", 2);
				Nop((byte*)noshake_address, 6);
			}
			//no recoil diabled
			else {
				Patch((byte*)norecoil_address, (byte*)"\x74\x22", 2);
				Patch((byte*)noshake_address, (byte*)"\x89\x91\x20\x06\x00\x00", 6);
			}
		}
		//toogle rapidfire
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			show_rapidfire = !show_rapidfire;
			if (show_rapidfire)
			{
				Nop((byte*)rapidfire_address, 3);
			}
			else {
				Patch((byte*)rapidfire_address, (byte*)"\x89\x4E\x3C",3);
			}
		}
		//toogle noammo
		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			show_noammo = !show_noammo;
			if (show_noammo)
			{
				Nop((byte*)ammo_address, 7);
			}
			else {
				Patch((byte*)ammo_address, (byte*)"\x89\x84\x8F\x34\x03\x00\x00", 7);
			}
		}
		//Fast change
		if (GetAsyncKeyState(VK_NUMPAD5) & 1)
			show_fastchange = !show_fastchange;
		if (show_fastchange)
			*(int*)0x00E0DA3C = 0;

		if (GetAsyncKeyState(VK_CONTROL) & 1)
		{
			show_godview = !show_godview;
			if (show_godview)
			{
				Nop((byte*)godview1_address, 6);
				Nop((byte*)godview2_address, 6);
				*(float*)0x00E0DAF4 = 200;
			}
			else {
				Patch((byte*)godview1_address, (byte*)"\xD9\x9E\xF4\x00\x00\x00",6);
				Patch((byte*)godview2_address, (byte*)"\xD9\x9E\xF4\x00\x00\x00", 6);
				*(float*)0x00E0DAF4 = 11;
			}
		}
	}
	/*if (p)
		fclose(p);
	FreeConsole()*/
	Sleep(100);
	FreeLibraryAndExitThread(hmodule, 0);

	return 0;
}
bool __stdcall DllMain(HMODULE hmodule, DWORD reason_for_call, LPVOID lpreserved)
{
	HANDLE objecthandle;
	switch (reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			objecthandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hmodule, 0, 0);
			if (objecthandle)
				CloseHandle(objecthandle);
		case DLL_PROCESS_DETACH:
			break;
	}
	return true;
}