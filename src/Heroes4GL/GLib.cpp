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
#include "GLib.h"
#include "Config.h"
#include "Resource.h"

#define PREFIX_GL "gl"
#define PREFIX_WGL "wgl"

WGLCREATECONTEXTATTRIBS WGLCreateContextAttribs;
WGLCHOOSEPIXELFORMAT WGLChoosePixelFormat;
WGLGETEXTENSIONSSTRING WGLGetExtensionsString;
WGLSWAPINTERVAL WGLSwapInterval;

GLGETSTRING GLGetString;
GLVERTEX2S GLVertex2s;
GLTEXCOORD2F GLTexCoord2f;
GLBEGIN GLBegin;
GLEND GLEnd;
GLVIEWPORT GLViewport;
GLMATRIXMODE GLMatrixMode;
GLLOADIDENTITY GLLoadIdentity;
GLORTHO GLOrtho;
GLFINISH GLFinish;
GLENABLE GLEnable;
GLBINDTEXTURE GLBindTexture;
GLDELETETEXTURES GLDeleteTextures;
GLTEXPARAMETERI GLTexParameteri;
GLTEXENVI GLTexEnvi;
GLGETTEXIMAGE GLGetTexImage;
GLTEXIMAGE2D GLTexImage2D;
GLTEXSUBIMAGE2D GLTexSubImage2D;
GLGENTEXTURES GLGenTextures;
GLGETINTEGERV GLGetIntegerv;
GLCLEAR GLClear;
GLCLEARCOLOR GLClearColor;
GLPIXELSTOREI GLPixelStorei;

#ifdef _DEBUG
GLGETERROR GLGetError;
#endif

GLACTIVETEXTURE GLActiveTexture;
GLGENBUFFERS GLGenBuffers;
GLDELETEBUFFERS GLDeleteBuffers;
GLBINDBUFFER GLBindBuffer;
GLBUFFERDATA GLBufferData;
GLBUFFERSUBDATA GLBufferSubData;
GLDRAWARRAYS GLDrawArrays;

GLENABLEVERTEXATTRIBARRAY GLEnableVertexAttribArray;
GLVERTEXATTRIBPOINTER GLVertexAttribPointer;

GLCREATESHADER GLCreateShader;
GLDELETESHADER GLDeleteShader;
GLDELETEPROGRAM GLDeleteProgram;
GLCREATEPROGRAM GLCreateProgram;
GLSHADERSOURCE GLShaderSource;
GLCOMPILESHADER GLCompileShader;
GLATTACHSHADER GLAttachShader;
GLDETACHSHADER GLDetachShader;
GLLINKPROGRAM GLLinkProgram;
GLUSEPROGRAM GLUseProgram;
GLGETSHADERIV GLGetShaderiv;
GLGETSHADERINFOLOG GLGetShaderInfoLog;

GLBINDATTRIBLOCATION GLBindAttribLocation;
GLGETUNIFORMLOCATION GLGetUniformLocation;

GLUNIFORM1I GLUniform1i;
GLUNIFORM2F GLUniform2f;
GLUNIFORM4F GLUniform4f;

GLGENVERTEXARRAYS GLGenVertexArrays;
GLBINDVERTEXARRAY GLBindVertexArray;
GLDELETEVERTEXARRAYS GLDeleteVertexArrays;

GLGENFRAMEBUFFERS GLGenFramebuffers;
GLDELETEFRAMEBUFFERS GLDeleteFramebuffers;
GLBINDFRAMEBUFFER GLBindFramebuffer;
GLFRAMEBUFFERTEXTURE2D GLFramebufferTexture2D;

HMODULE hGLModule;

namespace GL
{
	VOID LoadFunction(CHAR* buffer, const CHAR* prefix, const CHAR* name, VOID* func, const CHAR* sufix = NULL)
	{
		StrCopy(buffer, prefix);
		StrCat(buffer, name);

		if (sufix)
			StrCat(buffer, sufix);

		PROC* fn = (PROC*)func;
		*fn = wglGetProcAddress(buffer);
		if (*(INT*)fn >= -1 && *(INT*)fn <= 3)
		{
			if (!hGLModule)
				hGLModule = GetModuleHandle("OPENGL32.dll");
			*fn = GetProcAddress(hGLModule, buffer);

			if (!*fn && !sufix)
			{
				LoadFunction(buffer, prefix, name, fn, "EXT");
				if (!*fn)
					LoadFunction(buffer, prefix, name, fn, "ARB");
			}
		}
	}

	BOOL GetContext(HDC hDc, HGLRC* lpHRc, DWORD major, DWORD minor, BOOL showError)
	{
		DWORD wglAttributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, major,
			WGL_CONTEXT_MINOR_VERSION_ARB, minor,
			0
		};

		HGLRC hRc = WGLCreateContextAttribs(hDc, NULL, wglAttributes);
		if (hRc)
		{
			wglMakeCurrent(hDc, hRc);
			wglDeleteContext(*lpHRc);
			*lpHRc = hRc;

			return TRUE;
		}
		else if (showError)
		{
			DWORD errorCode = GetLastError();
			if (errorCode == ERROR_INVALID_VERSION_ARB)
				Main::ShowError(IDS_ERROR_ARB_VERSION, "GLib.cpp", __LINE__);
			else if (errorCode == ERROR_INVALID_PROFILE_ARB)
				Main::ShowError(IDS_ERROR_ARB_PROFILE, "GLib.cpp", __LINE__);
		}

		return FALSE;
	}

	VOID CreateContextAttribs(HDC hDc, HGLRC* hRc)
	{
		CHAR buffer[256];
		LoadFunction(buffer, PREFIX_WGL, "CreateContextAttribs", &WGLCreateContextAttribs, "ARB");

		if (WGLCreateContextAttribs)
		{
			if (!GetContext(hDc, hRc, 3, 0, FALSE) && !GetContext(hDc, hRc, 2, 0, FALSE))
				GetContext(hDc, hRc, 1, 4, TRUE);
		}

		LoadFunction(buffer, PREFIX_WGL, "GetExtensionsString", &WGLGetExtensionsString, "EXT");
		if (WGLGetExtensionsString)
		{
			CHAR* extensions = (CHAR*)WGLGetExtensionsString();
			if (StrStr(extensions, "WGL_EXT_swap_control"))
				LoadFunction(buffer, PREFIX_WGL, "SwapInterval", &WGLSwapInterval, "EXT");
		}

		LoadFunction(buffer, PREFIX_GL, "GetString", &GLGetString);
		LoadFunction(buffer, PREFIX_GL, "TexCoord2f", &GLTexCoord2f);
		LoadFunction(buffer, PREFIX_GL, "Vertex2s", &GLVertex2s);
		LoadFunction(buffer, PREFIX_GL, "Begin", &GLBegin);
		LoadFunction(buffer, PREFIX_GL, "End", &GLEnd);
		LoadFunction(buffer, PREFIX_GL, "Viewport", &GLViewport);
		LoadFunction(buffer, PREFIX_GL, "MatrixMode", &GLMatrixMode);
		LoadFunction(buffer, PREFIX_GL, "LoadIdentity", &GLLoadIdentity);
		LoadFunction(buffer, PREFIX_GL, "Ortho", &GLOrtho);
		LoadFunction(buffer, PREFIX_GL, "Finish", &GLFinish);
		LoadFunction(buffer, PREFIX_GL, "Enable", &GLEnable);
		LoadFunction(buffer, PREFIX_GL, "BindTexture", &GLBindTexture);
		LoadFunction(buffer, PREFIX_GL, "DeleteTextures", &GLDeleteTextures);
		LoadFunction(buffer, PREFIX_GL, "TexParameteri", &GLTexParameteri);
		LoadFunction(buffer, PREFIX_GL, "TexEnvi", &GLTexEnvi);
		LoadFunction(buffer, PREFIX_GL, "GetTexImage", &GLGetTexImage);
		LoadFunction(buffer, PREFIX_GL, "TexImage2D", &GLTexImage2D);
		LoadFunction(buffer, PREFIX_GL, "TexSubImage2D", &GLTexSubImage2D);
		LoadFunction(buffer, PREFIX_GL, "GenTextures", &GLGenTextures);
		LoadFunction(buffer, PREFIX_GL, "GetIntegerv", &GLGetIntegerv);
		LoadFunction(buffer, PREFIX_GL, "Clear", &GLClear);
		LoadFunction(buffer, PREFIX_GL, "ClearColor", &GLClearColor);
		LoadFunction(buffer, PREFIX_GL, "PixelStorei", &GLPixelStorei);

#ifdef _DEBUG
		LoadFunction(buffer, PREFIX_GL, "GetError", &GLGetError);
#endif

		LoadFunction(buffer, PREFIX_GL, "ActiveTexture", &GLActiveTexture);
		LoadFunction(buffer, PREFIX_GL, "GenBuffers", &GLGenBuffers);
		LoadFunction(buffer, PREFIX_GL, "DeleteBuffers", &GLDeleteBuffers);
		LoadFunction(buffer, PREFIX_GL, "BindBuffer", &GLBindBuffer);
		LoadFunction(buffer, PREFIX_GL, "BufferData", &GLBufferData);
		LoadFunction(buffer, PREFIX_GL, "BufferSubData", &GLBufferSubData);
		LoadFunction(buffer, PREFIX_GL, "DrawArrays", &GLDrawArrays);

		LoadFunction(buffer, PREFIX_GL, "EnableVertexAttribArray", &GLEnableVertexAttribArray);
		LoadFunction(buffer, PREFIX_GL, "VertexAttribPointer", &GLVertexAttribPointer);

		LoadFunction(buffer, PREFIX_GL, "CreateShader", &GLCreateShader);
		LoadFunction(buffer, PREFIX_GL, "DeleteShader", &GLDeleteShader);
		LoadFunction(buffer, PREFIX_GL, "CreateProgram", &GLCreateProgram);
		LoadFunction(buffer, PREFIX_GL, "DeleteProgram", &GLDeleteProgram);
		LoadFunction(buffer, PREFIX_GL, "ShaderSource", &GLShaderSource);
		LoadFunction(buffer, PREFIX_GL, "CompileShader", &GLCompileShader);
		LoadFunction(buffer, PREFIX_GL, "AttachShader", &GLAttachShader);
		LoadFunction(buffer, PREFIX_GL, "DetachShader", &GLDetachShader);
		LoadFunction(buffer, PREFIX_GL, "LinkProgram", &GLLinkProgram);
		LoadFunction(buffer, PREFIX_GL, "UseProgram", &GLUseProgram);
		LoadFunction(buffer, PREFIX_GL, "GetShaderiv", &GLGetShaderiv);
		LoadFunction(buffer, PREFIX_GL, "GetShaderInfoLog", &GLGetShaderInfoLog);

		LoadFunction(buffer, PREFIX_GL, "BindAttribLocation", &GLBindAttribLocation);
		LoadFunction(buffer, PREFIX_GL, "GetUniformLocation", &GLGetUniformLocation);

		LoadFunction(buffer, PREFIX_GL, "Uniform1i", &GLUniform1i);
		LoadFunction(buffer, PREFIX_GL, "Uniform2f", &GLUniform2f);
		LoadFunction(buffer, PREFIX_GL, "Uniform4f", &GLUniform4f);

		LoadFunction(buffer, PREFIX_GL, "GenVertexArrays", &GLGenVertexArrays);
		LoadFunction(buffer, PREFIX_GL, "BindVertexArray", &GLBindVertexArray);
		LoadFunction(buffer, PREFIX_GL, "DeleteVertexArrays", &GLDeleteVertexArrays);

		LoadFunction(buffer, PREFIX_GL, "GenFramebuffers", &GLGenFramebuffers);
		LoadFunction(buffer, PREFIX_GL, "DeleteFramebuffers", &GLDeleteFramebuffers);
		LoadFunction(buffer, PREFIX_GL, "BindFramebuffer", &GLBindFramebuffer);
		LoadFunction(buffer, PREFIX_GL, "FramebufferTexture2D", &GLFramebufferTexture2D);

		config.gl.version.value = NULL;
		if (GLGetString)
		{
			config.gl.version.value = 0;
			CHAR* strVer = (CHAR*)GLGetString(GL_VERSION);
			if (strVer && *strVer >= '0' && *strVer <= '9')
			{
				BYTE* ver = (BYTE*)&config.gl.version.value;

				BOOL appears = FALSE;
				CHAR* p = strVer;
				for (DWORD charIdx = 0, byteIdx = 0; byteIdx < 4; ++p)
				{
					if (*p >= '0' && *p <= '9')
					{
						appears = FALSE;

						*ver = *ver * 10 + (*p - '0');
					}
					else
					{
						if (*p != '.' || appears)
						{
							if (config.gl.version.value)
							{
								BYTE* ver = (BYTE*)&config.gl.version.value + 3;
								while (!*ver)
									config.gl.version.value <<= 8;
							}

							break;
						}

						appears = TRUE;
						config.gl.version.value <<= 8;
						++byteIdx;
						charIdx = 0;
					}
				}
			}
			else
				config.gl.version.value = GL_VER_1_1;

			if (config.gl.version.value < GL_VER_1_2)
			{
				CHAR* extensions = (CHAR*)GLGetString(GL_EXTENSIONS);
				if (extensions)
					config.gl.caps.clampToEdge = (StrStr(extensions, "GL_EXT_texture_edge_clamp") || StrStr(extensions, "GL_SGIS_texture_edge_clamp")) ? GL_CLAMP_TO_EDGE : GL_CLAMP;
			}
			else
				config.gl.caps.clampToEdge = GL_CLAMP_TO_EDGE;
		}

		if (!config.gl.version.value)
			config.gl.version.value = GL_VER_1_1;

		config.gl.version.real = config.gl.version.value;
	}

	VOID ResetPixelFormatDescription(PIXELFORMATDESCRIPTOR* pfd)
	{
		*pfd = {};
		pfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd->nVersion = 1;
	}

	VOID PreparePixelFormatDescription(PIXELFORMATDESCRIPTOR* pfd)
	{
		ResetPixelFormatDescription(pfd);

		INT bpp = 0;
		HDC hDc = GetDC(NULL);
		if (hDc)
		{
			bpp = GetDeviceCaps(hDc, BITSPIXEL);
			ReleaseDC(NULL, hDc);
		}

		pfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DEPTH_DONTCARE | PFD_SWAP_EXCHANGE;
		pfd->cColorBits = LOBYTE(bpp);
	}

	INT PreparePixelFormat(PIXELFORMATDESCRIPTOR* pfd)
	{
		PreparePixelFormatDescription(pfd);

		INT res = 0;

		HWND hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			WC_DRAW,
			NULL,
			WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0,
			1, 1,
			NULL,
			NULL,
			hDllModule,
			NULL);

		if (hWnd)
		{
			HDC hDc = GetDC(hWnd);
			if (hDc)
			{
				res = ::ChoosePixelFormat(hDc, pfd);
				if (res && ::SetPixelFormat(hDc, res, pfd))
				{
					HGLRC hRc = wglCreateContext(hDc);
					if (hRc)
					{
						if (wglMakeCurrent(hDc, hRc))
						{
							CHAR buffer[32];
							LoadFunction(buffer, PREFIX_WGL, "ChoosePixelFormat", &WGLChoosePixelFormat, "ARB");
							if (WGLChoosePixelFormat)
							{
								INT glAttributes[] = {
									WGL_DRAW_TO_WINDOW_ARB, (pfd->dwFlags & PFD_DRAW_TO_WINDOW) ? GL_TRUE : GL_FALSE,
									WGL_SUPPORT_OPENGL_ARB, (pfd->dwFlags & PFD_SUPPORT_OPENGL) ? GL_TRUE : GL_FALSE,
									WGL_DOUBLE_BUFFER_ARB, (pfd->dwFlags & PFD_DOUBLEBUFFER) ? GL_TRUE : GL_FALSE,
									WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
									WGL_COLOR_BITS_ARB, pfd->cColorBits,
									WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
									WGL_SWAP_METHOD_ARB, (pfd->dwFlags & PFD_SWAP_EXCHANGE) ? WGL_SWAP_EXCHANGE_ARB : WGL_SWAP_COPY_ARB,
									0
								};

								INT piFormat;
								UINT nNumFormats;
								if (WGLChoosePixelFormat(hDc, glAttributes, NULL, 1, &piFormat, &nNumFormats) && nNumFormats)
									res = piFormat;
							}

							wglMakeCurrent(hDc, NULL);
						}

						wglDeleteContext(hRc);
					}
				}

				ReleaseDC(hWnd, hDc);
			}

			DestroyWindow(hWnd);
		}

		return res;
	}

	VOID ResetPixelFormat(HWND hParent)
	{
		RECT rc;
		GetClientRect(hParent, &rc);

		HWND hWnd = CreateWindowEx(
			WS_EX_CONTROLPARENT,
			WC_DRAW,
			NULL,
			WS_VISIBLE | WS_CHILD,
			0, 0,
			rc.right, rc.bottom,
			hParent,
			NULL,
			hDllModule,
			NULL);

		if (hWnd)
		{
			HDC hDc = GetDC(hWnd);
			if (hDc)
			{
				PIXELFORMATDESCRIPTOR pfd;
				PreparePixelFormatDescription(&pfd);

				INT res = ::ChoosePixelFormat(hDc, &pfd);
				if (res)
					::SetPixelFormat(hDc, res, &pfd);

				ReleaseDC(hWnd, hDc);
			}

			DestroyWindow(hWnd);
		}
	}

	GLuint CompileShaderSource(DWORD name, CHAR* prefix, GLenum type)
	{
		HGLOBAL hResourceData;
		LPVOID pData = NULL;
		HRSRC hResource = FindResource(hDllModule, MAKEINTRESOURCE(name), RT_RCDATA);
		if (hResource)
		{
			hResourceData = LoadResource(hDllModule, hResource);
			if (hResourceData)
				pData = LockResource(hResourceData);
		}

		if (!pData)
			Main::ShowError(IDS_ERROR_LOAD_RESOURCE, "GLib.cpp", __LINE__);

		GLuint shader = GLCreateShader(type);

		DWORD pre = StrLength(prefix);
		DWORD length = SizeofResource(hDllModule, hResource);
		DWORD size = length + pre;
		CHAR* source = (CHAR*)MemoryAlloc(size + 1);
		const GLchar* srcData[] = { source };
		{
			MemoryCopy(source, prefix, pre);
			MemoryCopy(source + pre, pData, length);
			*(source + size) = NULL;

			GLShaderSource(shader, 1, srcData, NULL);
		}
		MemoryFree(source);

		GLint result;
		GLCompileShader(shader);

		GLGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			GLGetShaderiv(shader, GL_INFO_LOG_LENGTH, &result);

			if (!result)
				Main::ShowError(IDS_ERROR_COMPILE_SHADER, "GLib.cpp", __LINE__);
			else
			{
				CHAR data[1024];
				GLGetShaderInfoLog(shader, sizeof(data), &result, data);
				Main::ShowError(data, "GLib.cpp", __LINE__);
			}
		}

		return shader;
	}
}