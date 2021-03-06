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

#include "stdafx.h"
#include "Main.h"
#include "Config.h"
#include "Hooks.h"
#include "Resource.h"
#include "DirectDraw.h"

IDraw7* drawList;
const DisplayMode modesList[11] = {
	// 4x3
	800, 600, 16, // (4x3)
	1024, 768, 16, // (4x3)
	1280, 1024, 16, // (5x4)

	// 16x10
	960, 600, 16, // (16x9)
	1280, 800, 16, // (16x10)
	1680, 1050, 16, // (16x10)

	// 16x9
	1024, 600, 16, // (16x9)
	1366, 768, 16, // (16x9)
	1920, 1080, 16, // (16x9)

	// 16x9
	2560, 1080, 16, // (16x9)

	// 16x9
	3840, 1080, 16, // (16x9)
};

namespace Main
{
	HRESULT __stdcall DirectDrawCreateEx(GUID* lpGuid, LPVOID* lplpDD, REFIID iid, IUnknown* pUnkOuter)
	{
		if (config.isDDraw)
		{
			LoadDDraw();
			HRESULT res = DIRECTDRAWCREATEEX(pDirectDrawCreateEx)(lpGuid, lplpDD, iid, pUnkOuter);
			if (res == DD_OK)
				*lplpDD = new DirectDraw(&drawList, (IDirectDraw7*)*lplpDD);
			return res;
		}
		else
		{
			*lplpDD = new OpenDraw(&drawList);
			return DD_OK;
		}
	}

	OpenDraw* FindOpenDrawByWindow(HWND hWnd)
	{
		IDraw7* ddraw = drawList;
		while (ddraw)
		{
			if (((OpenDraw*)ddraw)->hWnd == hWnd || ((OpenDraw*)ddraw)->hDraw == hWnd)
				return (OpenDraw*)ddraw;

			ddraw = ddraw->last;
		}

		return NULL;
	}

	VOID ShowError(UINT id, CHAR* file, DWORD line)
	{
		CHAR message[256];
		LoadString(hDllModule, id, message, sizeof(message));
		ShowError(message, file, line);
	}

	VOID ShowError(CHAR* message, CHAR* file, DWORD line)
	{
		CHAR title[64];
		LoadString(hDllModule, IDS_ERROR, title, sizeof(title));

		CHAR dest[400];
		StrPrint(dest, "%s\n\n\nFILE %s\nLINE %d", message, file, line);

		Hooks::MessageBoxHook(NULL, dest, title, MB_OK | MB_ICONERROR | MB_TASKMODAL);

		Exit(EXIT_FAILURE);
	}

	VOID ShowInfo(UINT id)
	{
		CHAR message[256];
		LoadString(hDllModule, id, message, sizeof(message));
		ShowInfo(message);
	}

	VOID ShowInfo(CHAR* message)
	{
		CHAR title[64];
		LoadString(hDllModule, IDS_INFO, title, sizeof(title));

		Hooks::MessageBoxHook(NULL, message, title, MB_OK | MB_ICONASTERISK | MB_TASKMODAL);
	}

#ifdef _DEBUG
	VOID CheckError(CHAR* file, DWORD line)
	{
		DWORD statusCode = GLGetError();

		CHAR* message;

		if (statusCode != GL_NO_ERROR)
		{
			switch (statusCode)
			{
			case GL_INVALID_ENUM:
				message = "GL_INVALID_ENUM";
				break;

			case GL_INVALID_VALUE:
				message = "GL_INVALID_VALUE";
				break;

			case GL_INVALID_OPERATION:
				message = "GL_INVALID_OPERATION";
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				message = "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;

			case GL_OUT_OF_MEMORY:
				message = "GL_OUT_OF_MEMORY";
				break;

			case GL_STACK_UNDERFLOW:
				message = "GL_STACK_UNDERFLOW";
				break;

			case GL_STACK_OVERFLOW:
				message = "GL_STACK_OVERFLOW";
				break;

			default:
				message = "GL_UNDEFINED";
				break;
			}

			ShowError(message, file, line);
		}
	}
#endif
}