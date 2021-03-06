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
#include "IDraw7.h"
#include "ExtraTypes.h"
#include "OpenDrawSurface.h"

class OpenDraw : public IDraw7
{
protected:
	ULONG refCount;

public:
	OpenDrawSurface* attachedSurface;

	HDC hDc;
	HWND hDraw;

	const DisplayMode* mode;
	DWORD pitch;
	DWORD textureWidth;

	BOOL isFinish;

	HANDLE hDrawThread;
	HANDLE hDrawEvent;

	Viewport viewport;
	WindowState windowState;

	FilterState filterState;
	BOOL isTakeSnapshot;
	BOOL isFpsChanged;

	RenderBuffer temp;

	OpenDraw(IDraw7**);
	~OpenDraw();

	BOOL CheckView();
	VOID ScaleMouse(LPPOINT);

	VOID RenderStart();
	VOID RenderStop();

	VOID RenderOld();
	VOID RenderMid();
	VOID RenderNew();
	VOID LoadFilterState();

	VOID ResetDisplayMode(DWORD, DWORD);

	// Inherited via IDraw7
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall CreateClipper(DWORD, LPDIRECTDRAWCLIPPER*, IUnknown*);
	HRESULT __stdcall CreateSurface(LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE7*, IUnknown*);
	HRESULT __stdcall EnumDisplayModes(DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2);
	HRESULT __stdcall SetCooperativeLevel(HWND, DWORD);
	HRESULT __stdcall SetDisplayMode(DWORD, DWORD, DWORD, DWORD, DWORD);
};