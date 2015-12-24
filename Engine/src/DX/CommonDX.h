#pragma once

#if (WINVER >= _WIN32_WINNT_WIN8)
// macro redefinition because of Windows 8
#pragma warning (disable : 4005)
#endif

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#ifdef _DEBUG
#include <Windows.h>
#pragma comment (lib, "DxErr.lib")
#include <DxErr.h>
#endif

#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <D3D11Shader.h>
#include <D3DCompiler.h>
#include <D3DCommon.h>

#ifndef NDEBUG
#define SET_DEBUG_NAME(device)\
{\
	const char name[] = #device;\
	device->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(name)-1, name);\
}
#endif
