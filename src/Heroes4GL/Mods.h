/*
	MIT License

	Copyright (c) 2020 Oleksiy Ryabchun

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once

#define MENU_OFFSET 10000
#define MENU_STEP 1000
#define MENU_RESERVED 32000
#define MENU_EQULIBRIS 36000

typedef VOID(__cdecl* LOADPACKAGE)(const CHAR* name);

typedef BOOL(__stdcall* ISSUPPORTED)();
typedef const CHAR*(__stdcall* GETNAME)();
typedef HMENU(__stdcall* GETMENU)(DWORD);
typedef VOID(__stdcall* SETHWND)(HWND);
typedef VOID(__stdcall* LOADPACKAGES)(LOADPACKAGE callback);

struct Mod {
	Mod* last;
	HWND hWnd;
	BOOL added;
	CHAR name[256];

	GETNAME GetName;
	GETMENU GetMenu;
	SETHWND SetHWND;
	LOADPACKAGES LoadPackages;
};

extern Mod* mods;

namespace Mods
{
	VOID Load();
	VOID SetHWND(HWND);
	VOID SetMenu(HMENU);
	VOID LoadPackages(LOADPACKAGE);
}