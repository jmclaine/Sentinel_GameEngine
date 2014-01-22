#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <D3D11Shader.h>
#include <D3DCompiler.h>

#ifndef NDEBUG
	#include <D3DCommon.h>
	#pragma comment (lib, "dxguid.lib") 

	#define SET_DEBUG_NAME( device )\
	{\
		const char name[] = #device;\
		device->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( name )-1, name );\
	}
#endif

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dcompiler.lib")
