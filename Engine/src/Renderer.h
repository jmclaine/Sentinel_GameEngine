#pragma once
/*
HOW TO USE:

All objects drawn will scale according to the values set in
the following variables for their final transformation to
screen coordinates:

WindowInfo::BASE_WIDTH  = 1920;
WindowInfo::BASE_HEIGHT = 1080;

For example:

Drawing a Sprite scaled at (512, 512) scales it down using
a ratio calculated within the Renderer on Startup.

(512 * mWindowInfo->WidthRatio(), 512 * mWindowInfo->HeightRatio())



Create a file called 'config.xml' for easy setup.

<?xml version="1.0" ?>
<!--DIRECTX or OPENGL-->
<Renderer Type="OPENGL" Fullscreen="false" Width="1920" Height="1080" />

// Initialize the Renderer values within the XML file.
WindowInfo info;
Renderer* renderer = Renderer::Load("config.xml", info);


// Alternative to creating and loading the XML file.
Renderer* renderer = BuildRendererGL();
* OR *
Renderer* renderer = BuildRendererDX();


// Initialize the Renderer.
// Use appropriate HWND for Windows applications.
WindowInfo* windowInfo = renderer->Startup(mHWND, info.mFullscreen, info.mWidth, info.mHeight);


// Now the Renderer can perform its normal functionality.
// See functions for their applicability to desired use.

// See MeshBuilder.h for ease of object creation.
// See Mesh.h for ease of object rendering.

// Although custom object creation and rendering is possible,
// it is highly recommended to use both the MeshBuilder and Mesh
// to render objects easily.
//
// If more complex objects are desired, then it is recommended
// to create either OBJ or M3D (3ds Max plugin) models, and
// import them. (see Model.h)


EXAMPLE:

// Only necessary when rendering to multiple windows.
renderer->SetWindow(windowInfo);

// Clear buffers.
renderer->Clear(ColorRGBA(0.0f, 0.2f, 0.8f, 1.0f ).Ptr());

// Materials are required.
material->Apply(renderer);

// VBOs and IBOs required.
renderer->SetVertexBuffer(vertexBuffer);
renderer->SetIndexBuffer(indexBuffer);

// Set shader variables here.
// See 'Mesh.cpp' for a complete example.

// Draw primitives.
renderer->Draw(primitive, vertexBuffer.mCount, 0);

* OR *

// Draw primitives with indices.
renderer->DrawIndexed(primitive, indexBuffer.mCount, 0, 0);

// Send pixels drawn to backbuffer.
renderer->Present();


// When finished with Renderer always call:
renderer->Shutdown();


// If more than one window / handle was created:
renderer->SetWindow(windowInfo0);
renderer->Shutdown();

renderer->SetWindow(windowInfo1);
renderer->Shutdown();

*OR*

If a GameWindow was created:

gameWindow0->Shutdown();
gameWindow1->Shutdown();



RENDER TARGET:

If a Render Target is created and it requires a
Z-Buffer attached to it, OpenGL must have the
Render Target created before the Depth Stencil.

Example:

renderer->CreateRenderTexture(...);
renderer->CreateDepthStencil(...);

For additional examples of more advanced types:

http://www.opengl.org/wiki/Framebuffer_Object_Examples
*/
#include "Sentinel.h"
#include "Memory.h"
#include "BlendState.h"
#include "VertexLayout.h"
#include "DepthStencil.h"
#include "Buffer.h"
#include "Texture.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

#include <sstream>
#include <vector>

namespace Sentinel
{
	class RenderTexture;
	class BlendState;
	class WindowInfo;
	class Shader;

	enum class PrimitiveFormat : BYTE
	{
		POINTS,
		LINES,
		TRIANGLES,

		COUNT
	};

	enum class CullFormat : BYTE
	{
		NONE,
		CCW,
		CW,

		COUNT
	};

	enum class FillFormat : BYTE
	{
		SOLID,
		WIREFRAME,

		COUNT
	};

	// Only one Renderer should be created as multiple instances
	// of this particular object would unnecessarily complicate the
	// shared context capability of the video card interface without
	// any gains.
	//
	class SENTINEL_DLL Renderer
	{
	protected:
		Shader* mCurrShader;

	protected:
		Renderer();

	public:
		virtual ~Renderer() { }

		/////////////////////////////////

		// Initializes the Renderer as either DirectX11 or OpenGL from an XML file.
		//
		static Renderer* Create(const char* filename, WindowInfo& info);

		virtual WindowInfo* Startup(void* hWnd, bool fullscreen, UINT width, UINT height) = 0;

		virtual void Shutdown() = 0;

		#pragma region Windows

		virtual void SetWindow(WindowInfo* info) = 0;
		virtual WindowInfo* GetWindow() = 0;

		virtual bool ShareResources(WindowInfo* info0, WindowInfo* info1) = 0;

		#pragma endregion

		#pragma region Buffers

		virtual Buffer* CreateBuffer(void* data, UINT size, UINT stride, BufferFormat type, BufferAccess access = BufferAccess::READ_WRITE) = 0;

		virtual void SetVertexBuffer(Buffer* buffer) = 0;
		virtual void SetIndexBuffer(Buffer* buffer) = 0;

		#pragma endregion

		#pragma region Textures

		Texture* CreateTextureFromResource(void* data, UINT length); // used for deserialization

		virtual Texture* CreateTextureFromFile(const char* filename, bool createMips = true) = 0;
		virtual Texture* CreateTexture(void* data, UINT width, UINT height, ImageFormat format, bool createMips = true) = 0;
		virtual Texture* CreateTextureCube(void* data, UINT width, UINT height, ImageFormat format) = 0;

		virtual void* GetTexturePixels(Texture* texture) = 0;

		#pragma endregion

		#pragma region Render Settings

		virtual RenderTexture* CreateBackbuffer() = 0;
		virtual RenderTexture* CreateRenderTexture(Texture* texture) = 0;
		virtual DepthStencil* CreateDepthStencil(UINT width, UINT height) = 0;
		virtual BlendState* CreateBlendState(
			BlendFormat srcBlendColor, BlendFormat dstBlendColor, BlendFunction blendFuncColor,
			BlendFormat srcBlendAlpha, BlendFormat dstBlendAlpha, BlendFunction blendFuncAlpha) = 0;

		virtual UINT ResizeBuffers(UINT width, UINT height) = 0;

		virtual void SetRenderTexture(RenderTexture* target) = 0;
		virtual void SetDepthStencil(DepthStencil* stencil) = 0;
		virtual void SetBlendState(BlendState* blend) = 0;

		virtual void SetDepthStencilType(DepthFormat depth) = 0;
		virtual void SetViewport(int x, int y, UINT width, UINT height) = 0;
		virtual UINT SetCull(CullFormat type) = 0;
		virtual UINT SetFill(FillFormat type) = 0;

		#pragma endregion

		#pragma region Shaders

		virtual Shader* CreateShaderFromFile(const char* filename) = 0;
		virtual Shader* CreateShaderFromMemory(const char* source) = 0;

		void SetShader(Shader* shader);
		Shader* GetShader();

		#pragma endregion

		#pragma region Vertex Layout

		virtual VertexLayout* CreateVertexLayout(const std::vector<VertexAttribute>& attrib) = 0;
		virtual void SetVertexLayout(VertexLayout* vertexLayout) = 0;

		#pragma endregion

		#pragma region Rendering

		virtual void Clear(float* color, float depth = 1.0f) = 0;
		virtual void ClearColor(float* color) = 0;
		virtual void ClearDepth(float depth) = 0;

		virtual void Draw(PrimitiveFormat primitive, UINT count, UINT baseVertex) = 0;
		virtual void DrawIndexed(PrimitiveFormat primitive, UINT count, UINT startIndex, UINT baseVertex) = 0;

		virtual void Present() = 0;

		#pragma endregion
	};

	////////////////////////////////////////////////////////////////////
	// Renderer-type Selection
	//
	extern SENTINEL_DLL Renderer* BuildRendererDX();
	extern SENTINEL_DLL Renderer* BuildRendererGL();
}
