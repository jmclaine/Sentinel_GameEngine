#include <vector>
#include <crtdbg.h>

#include "Renderer.h"
#include "Archive.h"
#include "Debug.h"
#include "CommonDX.h"
#include "BufferDX.h"
#include "TextureDX.h"
#include "ShaderDX.h"
#include "VertexLayoutDX.h"
#include "WindowInfo.h"
#include "RenderTexture.h"

#ifdef _DEBUG
#include <comdef.h>
#endif

namespace Sentinel
{
#define HV(func)\
	if ((func) != S_OK)\
		return S_FALSE;

#ifndef _DEBUG
#define HV_PTR(func)\
	if ((func) != S_OK)\
		return nullptr;
#else
#define HV_PTR(func) {\
	HRESULT result = func;\
	if ((result) != S_OK) {\
		_com_error err(result);\
		Debug::LogError(STREAM(TRACE << DXGetErrorString(result) << ": " << err.ErrorMessage()));\
		return nullptr;\
	}}
#endif

	////////////////////////////////////////////////////////////////////////////////////
	// DirectX11 Renderer.
	//
	class RendererDX : public Renderer
	{
		friend class TextureDX;
		friend class ShaderDX;

	private:
		static const D3D11_PRIMITIVE_TOPOLOGY PRIMITIVE[(BYTE)PrimitiveFormat::COUNT];
		static const D3D11_CULL_MODE CULL_TYPE[(BYTE)CullFormat::COUNT];
		static const D3D11_FILL_MODE FILL_TYPE[(BYTE)FillFormat::COUNT];
		static const D3D11_BLEND BLEND_TYPE[(BYTE)BlendFormat::COUNT];
		static const D3D11_BLEND_OP BLEND_FUNC_TYPE[(BYTE)BlendFunction::COUNT];

		class WindowInfoDX : public WindowInfo
		{
			friend class RendererDX;

		public:
			ID3D11Device* mDevice;
			ID3D11DeviceContext* mContext;
			IDXGISwapChain* mSwapChain;
			ID3D11RasterizerState* mRasterizerState;
		};

		//////////////////////////////////

		class RenderTextureDX : public RenderTexture
		{
		public:
			ID3D11RenderTargetView* mView;

			RenderTextureDX(ID3D11RenderTargetView* view, Texture* texture) :
				mView(view),
				RenderTexture(texture)
			{ }

			~RenderTextureDX()
			{
				mView->Release();
			}
		};

		//////////////////////////////////

		class DepthStencilDX : public DepthStencil
		{
		public:

			ID3D11DepthStencilView* mView;

			DepthStencilDX(ID3D11DepthStencilView* view, UINT width, UINT height) :
				mView(view),
				DepthStencil(width, height)
			{ }

			~DepthStencilDX()
			{
				mView->Release();
			}
		};

		//////////////////////////////////

		class BlendStateDX : public BlendState
		{
			friend class RendererDX;

		public:

			ID3D11BlendState* mBlendState;

			//////////////////////////////////////////////////

			BlendStateDX(
				ID3D11BlendState* blendState,
				BlendFormat srcBlendColor, BlendFormat dstBlendColor, BlendFunction blendFuncColor,
				BlendFormat srcBlendAlpha, BlendFormat dstBlendAlpha, BlendFunction blendFuncAlpha) :
				mBlendState(blendState),
				BlendState(
					srcBlendColor, dstBlendColor, blendFuncColor,
					srcBlendAlpha, dstBlendAlpha, blendFuncAlpha)
			{ }

			~BlendStateDX()
			{
				SAFE_RELEASE_PTR(mBlendState);
			}
		};

		WindowInfoDX* mCurrWindow;

		ID3D11DepthStencilView* mCurrStencil;
		ID3D11DepthStencilState** mDepthStencilState;
		ID3D11RenderTargetView* mCurrTarget;

		DXGI_SAMPLE_DESC mSampleDesc;

	public:

		RendererDX()
		{
			mSampleDesc.Count = 1;
			mSampleDesc.Quality = 0;

			mCurrWindow = nullptr;
			mCurrStencil = nullptr;
			mCurrTarget = nullptr;

			mDepthStencilState = new ID3D11DepthStencilState*[(BYTE)DepthFormat::COUNT];
		}

		~RendererDX()
		{
			for (BYTE x = 0; x < (BYTE)DepthFormat::COUNT; ++x)
			{
				SAFE_RELEASE_PTR(mDepthStencilState[x]);
			}
			delete[] mDepthStencilState;
		}

	private:

		WindowInfo* Startup(void* hWnd, bool fullscreen, UINT width, UINT height)
		{
			D3D_FEATURE_LEVEL featurelevels[] =
			{
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
			};

			DXGI_SWAP_CHAIN_DESC sd =
			{
				{ width, height,
				{ 1, 60 },
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
				DXGI_MODE_SCALING_UNSPECIFIED
				},
				mSampleDesc,
				DXGI_USAGE_RENDER_TARGET_OUTPUT,
				1,
				(HWND)hWnd,
				!fullscreen,
				DXGI_SWAP_EFFECT_DISCARD,
				DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
			};

			UINT createdeviceflags = 0;

			// Windows update now makes this fail.
			//
			//#ifdef _DEBUG
			//	createdeviceflags |= D3D11_CREATE_DEVICE_DEBUG;
			//#endif

			mCurrWindow = new WindowInfoDX();

			mCurrWindow->mHandle = hWnd;
			mCurrWindow->mFullscreen = fullscreen;
			mCurrWindow->mWidth = width;
			mCurrWindow->mHeight = height;
			mCurrWindow->mWidthRatio = (float)width / (float)WindowInfo::BASE_WIDTH;
			mCurrWindow->mHeightRatio = (float)height / (float)WindowInfo::BASE_HEIGHT;

			HV_PTR(D3D11CreateDeviceAndSwapChain(
				nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createdeviceflags,
				featurelevels, sizeof(featurelevels) / sizeof(D3D_FEATURE_LEVEL),
				D3D11_SDK_VERSION, &sd, &mCurrWindow->mSwapChain,
				&mCurrWindow->mDevice, 0, &mCurrWindow->mContext));

			Debug::Log("Created DX11 Renderer...");

			if (fullscreen)
			{
				HV_PTR(mCurrWindow->mSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
				HV_PTR(mCurrWindow->mSwapChain->SetFullscreenState(TRUE, nullptr));
			}

			D3D11_RASTERIZER_DESC rasterizerstate = { D3D11_FILL_SOLID, D3D11_CULL_BACK, true, 0, 0, 0, true, false, false, false };
			HV_PTR(mCurrWindow->mDevice->CreateRasterizerState(&rasterizerstate, &mCurrWindow->mRasterizerState));
			mCurrWindow->mContext->RSSetState(mCurrWindow->mRasterizerState);

		#ifndef NDEBUG
			SET_DEBUG_NAME(mCurrWindow->mSwapChain);
			SET_DEBUG_NAME(mCurrWindow->mDevice);
			SET_DEBUG_NAME(mCurrWindow->mContext);
			SET_DEBUG_NAME(mCurrWindow->mRasterizerState);
		#endif

			// Create BLEND_OFF.
			//
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

			for (UINT x = 0; x < 8; ++x)
			{
				blendDesc.RenderTarget[x].BlendEnable = FALSE;

				blendDesc.RenderTarget[x].SrcBlend = BLEND_TYPE[(BYTE)BlendFormat::SRC_ALPHA];
				blendDesc.RenderTarget[x].DestBlend = BLEND_TYPE[(BYTE)BlendFormat::ONE_MINUS_SRC_ALPHA];
				blendDesc.RenderTarget[x].BlendOp = BLEND_FUNC_TYPE[(BYTE)BlendFunction::ADD];

				blendDesc.RenderTarget[x].SrcBlendAlpha = BLEND_TYPE[(BYTE)BlendFormat::SRC_ALPHA];
				blendDesc.RenderTarget[x].DestBlendAlpha = BLEND_TYPE[(BYTE)BlendFormat::ONE_MINUS_SRC_ALPHA];
				blendDesc.RenderTarget[x].BlendOpAlpha = BLEND_FUNC_TYPE[(BYTE)BlendFunction::ADD];

				blendDesc.RenderTarget[x].RenderTargetWriteMask = 0x0F;
			}

			// Create depth stencils.
			//
			D3D11_DEPTH_STENCIL_DESC stencilDesc;
			stencilDesc.DepthEnable = TRUE;
			stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

			stencilDesc.StencilEnable = FALSE;
			stencilDesc.StencilReadMask = 0xFF;
			stencilDesc.StencilWriteMask = 0xFF;

			stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			ID3D11DepthStencilState* stencilState = nullptr;

			stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
			HV_PTR(mCurrWindow->mDevice->CreateDepthStencilState(&stencilDesc, &stencilState));
			mDepthStencilState[(BYTE)DepthFormat::LESS] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
			HV_PTR(mCurrWindow->mDevice->CreateDepthStencilState(&stencilDesc, &stencilState));
			mDepthStencilState[(BYTE)DepthFormat::EQUAL] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			HV_PTR(mCurrWindow->mDevice->CreateDepthStencilState(&stencilDesc, &stencilState));
			mDepthStencilState[(BYTE)DepthFormat::LEQUAL] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
			HV_PTR(mCurrWindow->mDevice->CreateDepthStencilState(&stencilDesc, &stencilState));
			mDepthStencilState[(BYTE)DepthFormat::GREATER] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
			HV_PTR(mCurrWindow->mDevice->CreateDepthStencilState(&stencilDesc, &stencilState));
			mDepthStencilState[(BYTE)DepthFormat::NOTEQUAL] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			HV_PTR(mCurrWindow->mDevice->CreateDepthStencilState(&stencilDesc, &stencilState));
			mDepthStencilState[(BYTE)DepthFormat::GEQUAL] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			HV_PTR(mCurrWindow->mDevice->CreateDepthStencilState(&stencilDesc, &stencilState));
			mDepthStencilState[(BYTE)DepthFormat::ALWAYS] = stencilState;

			stencilDesc.DepthEnable = FALSE;
			HV_PTR(mCurrWindow->mDevice->CreateDepthStencilState(&stencilDesc, &stencilState));
			mDepthStencilState[(BYTE)DepthFormat::OFF] = stencilState;

			SetDepthStencilType(DepthFormat::LEQUAL);

			return mCurrWindow;
		}

		void Shutdown()
		{
			if (mCurrWindow)
			{
				if (mCurrWindow->mSwapChain != nullptr)
				{
					mCurrWindow->mSwapChain->SetFullscreenState(FALSE, nullptr);

					SAFE_RELEASE_PTR(mCurrWindow->mSwapChain);
					SAFE_RELEASE_PTR(mCurrWindow->mRasterizerState);
					SAFE_RELEASE_PTR(mCurrWindow->mContext);
				}
			}
		}

		// Windows.
		//
		void SetWindow(WindowInfo* info)
		{
			_ASSERT(info);

			mCurrWindow = (WindowInfoDX*)info;
		}

		WindowInfo* GetWindow()
		{
			return mCurrWindow;
		}

		bool ShareResources(WindowInfo* info0, WindowInfo* info1)
		{
			//((WindowInfoDX*)info0)->
			/*IDXGIResource* res = nullptr;

			if( ((WindowInfoDX*)info0)->mContext->QueryInterface( __uuidof(IDXGIResource), (void**)&res ) != S_OK )
			return false;

			HWND hWnd;
			if( res->GetSharedHandle( &hWnd );*/
			return false;
		}

		// Buffers.
		//
		Buffer* CreateBuffer(void* data, UINT size, UINT stride, BufferFormat type, BufferAccess access)
		{
			return new BufferDX(mCurrWindow->mDevice, mCurrWindow->mContext, data, size, stride, type, access);
		}

		void SetVertexBuffer(Buffer* buffer)
		{
			_ASSERT(mCurrWindow);
			_ASSERT(mCurrWindow->mContext);

			UINT offset = 0;
			UINT stride = buffer->Stride();

			_ASSERT(stride > 0);

			mCurrWindow->mContext->IASetVertexBuffers(0, 1, &static_cast<BufferDX*>(buffer)->mBuffer, &stride, &offset);
		}

		void SetIndexBuffer(Buffer* buffer)
		{
			_ASSERT(mCurrWindow);
			_ASSERT(mCurrWindow->mContext);

			mCurrWindow->mContext->IASetIndexBuffer(static_cast<BufferDX*>(buffer)->mBuffer, DXGI_FORMAT_R32_UINT, 0);
		}

		// Textures.
		//
		Texture* CreateTextureFromFile(const char* filename, bool createMips = true)
		{
			// TODO: Check for compatible texture size.

			_ASSERT(strlen(filename) > 0);

			// DDS files are loaded from DirectX.
			// stbi does not support them.
			//
			int count = strlen(filename) - 1;
			if (toupper(filename[count - 2]) == 'D' && toupper(filename[count - 1]) == 'D' && toupper(filename[count]) == 'S')
			{
				D3DX11_IMAGE_LOAD_INFO info;
				ID3D11ShaderResourceView* image;

				if (D3DX11CreateShaderResourceViewFromFileA(mCurrWindow->mDevice, filename, &info, nullptr, &image, nullptr) == S_OK)
				{
					return new TextureDX(info.Width, info.Height, ImageFormat::RGBA, 0, image);
				}
				else
				{
					Debug::ShowError(
						STREAM("Failed to load " << filename),
						STREAM("Texture Load Failure"));

					return nullptr;
				}
			}

			// Texture must be 32-bit aligned
			//
			int width, height;
			int nChannels;
			unsigned char* pixels = stbi_load(filename, &width, &height, &nChannels, 4);

			if (pixels == nullptr)
			{
				Debug::ShowError(
					STREAM(filename << " failed to load."),
					STREAM("Texture load failure."));

				return nullptr;
			}

			Texture* texture = CreateTexture(pixels, width, height, ImageFormat::RGBA, createMips);

			stbi_image_free(pixels);

			return texture;
		}

		Texture* CreateTexture(void* data, UINT width, UINT height, ImageFormat format, bool createMips = true)
		{
			UCHAR* newData = nullptr;
			UINT size = width * height;

			DXGI_FORMAT newFormat;
			switch (format)
			{
			case ImageFormat::R:
				if (data)
				{
					newData = (UCHAR*)data;
				}
				else
				{
					newData = (UCHAR*)malloc(size << 2);
					memset(newData, 0, size << 2);
				}
				newFormat = DXGI_FORMAT_R32_FLOAT;
				break;

			case ImageFormat::RG:
				if (data)
				{
					newData = (UCHAR*)data;
				}
				else
				{
					newData = (UCHAR*)malloc(size << 2);
					memset(newData, 0, size << 2);
				}
				newFormat = DXGI_FORMAT_R16G16_FLOAT;
				break;

			case ImageFormat::RGB:
			{
				// The texture must be converted to RGBA

				newData = (UCHAR*)malloc(size << 2);

				if (data)
				{
					for (UINT i = 0; i < size; ++i)
					{
						for (UINT j = 0; j < 3; ++j)
							newData[(i << 2) + j] = reinterpret_cast<UCHAR*>(data)[i * 3 + j];

						newData[(i << 2) + 3] = (UCHAR)(255);
					}
				}
				else memset(newData, 0, size << 2);

				newFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			}
			break;

			case ImageFormat::RGBA:
				if (data)
				{
					newData = (UCHAR*)data;
				}
				else
				{
					newData = (UCHAR*)malloc(size << 2);
					memset(newData, 0, size << 2);
				}
				newFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				break;

			case ImageFormat::HDR:
				if (data)
				{
					newData = (UCHAR*)data;
				}
				else
				{
					newData = (UCHAR*)malloc(size << 8);
					memset(newData, 0, size << 8);
				}
				newFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;

			case ImageFormat::DEPTH:
				if (data)
				{
					newData = (UCHAR*)data;
				}
				else
				{
					newData = (UCHAR*)malloc(size << 2);
					memset(newData, 0, size << 2);
				}
				newFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;

			default:
				_ASSERT(false);	// Invalid image format
				break;
			}

			D3D11_TEXTURE2D_DESC desc;
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = createMips ? 0 : 1;
			desc.ArraySize = 1;
			desc.Format = newFormat;
			desc.SampleDesc = mSampleDesc;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = ((format != ImageFormat::DEPTH) ? D3D11_BIND_RENDER_TARGET : D3D11_BIND_DEPTH_STENCIL) | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = createMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

			ID3D11Texture2D* tex0;
			HV_PTR(mCurrWindow->mDevice->CreateTexture2D(&desc, nullptr, &tex0));

			D3D11_SHADER_RESOURCE_VIEW_DESC rsv =
			{
				newFormat,
				D3D_SRV_DIMENSION_TEXTURE2D
			};
			rsv.Texture2D.MipLevels = 1;
			rsv.Texture2D.MostDetailedMip = 0;

			if (createMips)
			{
				D3D11_SUBRESOURCE_DATA res =
				{
					newData,
					width << 2, 0
				};

				rsv.Texture2D.MipLevels = UINT_MAX;

				desc.MipLevels = 1;
				desc.Usage = D3D11_USAGE_STAGING;
				desc.BindFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				desc.MiscFlags = 0;

				ID3D11Texture2D* tex1;
				HV_PTR(mCurrWindow->mDevice->CreateTexture2D(&desc, &res, &tex1));

				D3D11_BOX box =
				{
					0, 0, 0,
					width, height, 1
				};

				mCurrWindow->mContext->CopySubresourceRegion(tex0, 0, 0, 0, 0, tex1, 0, &box);
				SAFE_RELEASE_PTR(tex1);
			}

			ID3D11ShaderResourceView* resource;

			HV_PTR(mCurrWindow->mDevice->CreateShaderResourceView(tex0, &rsv, &resource));
			mCurrWindow->mContext->GenerateMips(resource);

			if (!data || format == ImageFormat::RGB)
				free(newData);

		#ifndef NDEBUG
			SET_DEBUG_NAME(tex0);
			SET_DEBUG_NAME(resource);
		#endif

			return new TextureDX(width, height, format, tex0, resource);
		}

		Texture* CreateTextureCube(void* data, UINT width, UINT height, ImageFormat format)
		{
			UCHAR* newData = (UCHAR*)data;
			UINT size = width * height;

			DXGI_FORMAT newFormat;
			switch (format)
			{
			case ImageFormat::R:
				if (!newData)
				{
					UINT dataSize = (size << 2) * 6;
					newData = (UCHAR*)malloc(dataSize);
					memset(newData, 0, dataSize);
				}
				newFormat = DXGI_FORMAT_R32_FLOAT;
				break;

			case ImageFormat::RG:
				if (!newData)
				{
					UINT dataSize = (size << 2) * 6;
					newData = (UCHAR*)malloc(dataSize);
					memset(newData, 0, dataSize);
				}
				newFormat = DXGI_FORMAT_R16G16_FLOAT;
				break;

			case ImageFormat::RGB:
			{
				// The texture must be converted to RGBA

				UINT dataSize = (size << 2) * 6;
				newData = (UCHAR*)malloc(dataSize);

				if (data)
				{
					for (UINT i = 0; i < size; ++i)
					{
						for (UINT j = 0; j < 3; ++j)
							newData[(i << 2) + j] = reinterpret_cast<UCHAR*>(data)[i * 3 + j];

						newData[(i << 2) + 3] = (UCHAR)(255);
					}
				}
				else memset(newData, 0, dataSize);

				newFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			}
			break;

			case ImageFormat::RGBA:
				if (!newData)
				{
					UINT dataSize = (size << 2) * 6;
					newData = (UCHAR*)malloc(dataSize);
					memset(newData, 0, dataSize);
				}
				newFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				break;

			case ImageFormat::HDR:
				if (!newData)
				{
					UINT dataSize = (size << 8) * 6;
					newData = (UCHAR*)malloc(dataSize);
					memset(newData, 0, dataSize);
				}
				newFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;

			case ImageFormat::DEPTH:
				if (!newData)
				{
					UINT dataSize = (size << 2) * 6;
					newData = (UCHAR*)malloc(dataSize);
					memset(newData, 0, dataSize);
				}
				newFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;

			default:
				_ASSERT(false);	// Invalid image format
				break;
			}

			D3D11_TEXTURE3D_DESC desc;
			desc.Width = width;
			desc.Height = height;
			desc.Depth = 6; // cube
			desc.MipLevels = 1;
			desc.Format = newFormat;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = ((format != ImageFormat::DEPTH) ? D3D11_BIND_RENDER_TARGET : D3D11_BIND_DEPTH_STENCIL) | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
			desc.MiscFlags = 0;
			//desc.MiscFlags = createMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

			D3D11_SUBRESOURCE_DATA resData;
			resData.pSysMem = newData;
			resData.SysMemPitch = width << 2;
			resData.SysMemSlicePitch = size << 2;

			ID3D11Texture3D* texture = nullptr;
			HV_PTR(mCurrWindow->mDevice->CreateTexture3D(&desc, &resData, &texture));

			D3D11_SHADER_RESOURCE_VIEW_DESC rsv;
			memset(&rsv, 0, sizeof(rsv));
			rsv.Format = desc.Format;
			rsv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			rsv.Texture3D.MipLevels = desc.MipLevels;

			ID3D11ShaderResourceView* resource = nullptr;
			HV_PTR(mCurrWindow->mDevice->CreateShaderResourceView(texture, &rsv, &resource));

		#ifndef NDEBUG
			SET_DEBUG_NAME(texture);
			SET_DEBUG_NAME(resource);
		#endif

			free(newData);

			return new TextureCubeDX(width, height, format, texture, resource);
		}

		void* GetTexturePixels(Texture* texture)
		{
			_ASSERT(mCurrWindow);

			TextureDX* texDX = static_cast<TextureDX*>(texture);

			ID3D11Texture2D* texCopy = nullptr;

			D3D11_TEXTURE2D_DESC desc;
			texDX->mTexture->GetDesc(&desc);
			desc.MipLevels = 0;
			desc.BindFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.MiscFlags = D3D11_USAGE_DEFAULT;

			HRESULT hr = mCurrWindow->mDevice->CreateTexture2D(&desc, nullptr, &texCopy);
			if (texCopy)
			{
				mCurrWindow->mContext->CopyResource(texCopy, texDX->mTexture);

				D3D11_MAPPED_SUBRESOURCE res;
				UINT subres = D3D11CalcSubresource(0, 0, 0);

				HV_PTR(mCurrWindow->mContext->Map(texCopy, subres, D3D11_MAP_READ_WRITE, 0, &res));

				UINT  size = (texDX->Width() << 2) * texDX->Height();
				BYTE* data = new BYTE[size];

				memcpy(data, res.pData, size);

				mCurrWindow->mContext->Unmap(texCopy, 0);

				return data;
			}

			return nullptr;
		}

		// Special Rendering.
		//
		RenderTexture* CreateBackbuffer()
		{
			_ASSERT(mCurrWindow);

			ID3D11Texture2D *tex = nullptr;

			HV_PTR(mCurrWindow->mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&tex));

			return CreateRenderTexture(tex, nullptr);
		}

		RenderTexture* CreateRenderTexture(ID3D11Texture2D* backbuffer, Texture* texture)
		{
			_ASSERT(mCurrWindow);

			ID3D11RenderTargetView* view = nullptr;

			HV_PTR(mCurrWindow->mDevice->CreateRenderTargetView(backbuffer, nullptr, &view));

			return new RenderTextureDX(view, texture);
		}

		RenderTexture* CreateRenderTexture(Texture* texture)
		{
			_ASSERT(texture);

			return CreateRenderTexture(static_cast<TextureDX*>(texture)->mTexture, texture);
		}

		DepthStencil* CreateDepthStencil(UINT width, UINT height)
		{
			_ASSERT(mCurrWindow);

			D3D11_TEXTURE2D_DESC texDesc =
			{
				width, height,
				1, 1,
				DXGI_FORMAT_D32_FLOAT,
				mSampleDesc,
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_DEPTH_STENCIL,
				0, 0
			};

			ID3D11Texture2D *stencil = nullptr;
			ID3D11DepthStencilView* view = nullptr;

			HV_PTR(mCurrWindow->mDevice->CreateTexture2D(&texDesc, nullptr, &stencil));
			HV_PTR(mCurrWindow->mDevice->CreateDepthStencilView(stencil, nullptr, &view));

			SAFE_RELEASE_PTR(stencil);

			//#ifndef NDEBUG
			//	SET_DEBUG_NAME( view );
			//#endif

			return new DepthStencilDX(view, width, height);
		}

		BlendState* CreateBlendState(
			BlendFormat srcBlendColor, BlendFormat dstBlendColor, BlendFunction blendFuncColor,
			BlendFormat srcBlendAlpha, BlendFormat dstBlendAlpha, BlendFunction blendFuncAlpha)
		{
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

			for (UINT x = 0; x < 8; ++x)
			{
				blendDesc.RenderTarget[x].BlendEnable = TRUE;

				blendDesc.RenderTarget[x].SrcBlend = BLEND_TYPE[(BYTE)srcBlendColor];
				blendDesc.RenderTarget[x].DestBlend = BLEND_TYPE[(BYTE)dstBlendColor];
				blendDesc.RenderTarget[x].BlendOp = BLEND_FUNC_TYPE[(BYTE)blendFuncColor];

				blendDesc.RenderTarget[x].SrcBlendAlpha = BLEND_TYPE[(BYTE)srcBlendAlpha];
				blendDesc.RenderTarget[x].DestBlendAlpha = BLEND_TYPE[(BYTE)dstBlendAlpha];
				blendDesc.RenderTarget[x].BlendOpAlpha = BLEND_FUNC_TYPE[(BYTE)blendFuncAlpha];

				blendDesc.RenderTarget[x].RenderTargetWriteMask = 0x0F;
			}

			ID3D11BlendState* blendState = nullptr;
			HV_PTR(mCurrWindow->mDevice->CreateBlendState(&blendDesc, &blendState));

			return new BlendStateDX(
				blendState,
				srcBlendColor, dstBlendColor, blendFuncColor,
				srcBlendAlpha, dstBlendAlpha, blendFuncAlpha);
		}

		UINT ResizeBuffers(UINT width, UINT height)
		{
			mCurrWindow->mWidth = width;
			mCurrWindow->mHeight = height;

			mCurrWindow->mWidthRatio = width / static_cast<float>(WindowInfo::BASE_WIDTH);
			mCurrWindow->mHeightRatio = height / static_cast<float>(WindowInfo::BASE_HEIGHT);

			/*
			mCurrWindow->mContext->OMSetRenderTextures( 0, 0, 0 );

			mRenderTexture[ 0 ]->Release();

			mCurrWindow->mSwapChain->ResizeBuffers( 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );

			CreateBackbuffer();

			//Debug::Log( "Resized: (" << width << ", " << height << ")" );
			*/
			return S_OK;
		}

		void SetRenderTexture(RenderTexture* target)
		{
			_ASSERT(mCurrWindow);
			//_ASSERT(mCurrStencil);
			_ASSERT(target);

			//mCurrWindow->mContext->OMSetRenderTextures( 0, 0, 0 );

			//ID3D11ShaderResourceView* nullptrViews[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };
			//mCurrWindow->mContext->PSSetShaderResources( 0, 8, nullptrViews );

			mCurrTarget = static_cast<RenderTextureDX*>(target)->mView;

			mCurrWindow->mContext->OMSetRenderTargets(1, &mCurrTarget, mCurrStencil);
		}

		void SetDepthStencil(DepthStencil* stencil)
		{
			//_ASSERT(stencil);

			if (stencil)
				mCurrStencil = static_cast<DepthStencilDX*>(stencil)->mView;
			else
				mCurrStencil = nullptr;
		}

		void SetDepthStencilType(DepthFormat state)
		{
			_ASSERT(mCurrWindow);

			mCurrWindow->mContext->OMSetDepthStencilState(mDepthStencilState[(BYTE)state], 0);
		}

		void SetViewport(int x, int y, UINT width, UINT height)
		{
			_ASSERT(mCurrWindow);

			RECT rect;
			GetClientRect((HWND)mCurrWindow->mHandle, &rect);

			D3D11_VIEWPORT vp =
			{
				static_cast<FLOAT>(x), static_cast<FLOAT>(y),
				(width != WindowInfo::BASE_WIDTH) ? static_cast<FLOAT>(WindowInfo::BASE_WIDTH) * (static_cast<FLOAT>(rect.right) / static_cast<FLOAT>(width)) : static_cast<FLOAT>(WindowInfo::BASE_WIDTH),
				(height != WindowInfo::BASE_HEIGHT) ? static_cast<FLOAT>(WindowInfo::BASE_HEIGHT) * (static_cast<FLOAT>(rect.bottom) / static_cast<FLOAT>(height)) : static_cast<FLOAT>(WindowInfo::BASE_HEIGHT),
				0.0f, 1.0f
			};

			mCurrWindow->mContext->RSSetViewports(1, &vp);
		}

		UINT SetCull(CullFormat type)
		{
			_ASSERT(mCurrWindow);
			_ASSERT(type < CullFormat::COUNT);

			D3D11_RASTERIZER_DESC rasterizerstate;
			mCurrWindow->mRasterizerState->GetDesc(&rasterizerstate);

			SAFE_RELEASE_PTR(mCurrWindow->mRasterizerState);
			rasterizerstate.CullMode = CULL_TYPE[(BYTE)type];

			HV(mCurrWindow->mDevice->CreateRasterizerState(&rasterizerstate, &mCurrWindow->mRasterizerState));
			mCurrWindow->mContext->RSSetState(mCurrWindow->mRasterizerState);

			return S_OK;
		}

		UINT SetFill(FillFormat type)
		{
			_ASSERT(mCurrWindow);
			_ASSERT(type < FillFormat::COUNT);

			D3D11_RASTERIZER_DESC rasterizerstate;
			mCurrWindow->mRasterizerState->GetDesc(&rasterizerstate);

			SAFE_RELEASE_PTR(mCurrWindow->mRasterizerState);
			rasterizerstate.FillMode = FILL_TYPE[(BYTE)type];

			HV(mCurrWindow->mDevice->CreateRasterizerState(&rasterizerstate, &mCurrWindow->mRasterizerState));
			mCurrWindow->mContext->RSSetState(mCurrWindow->mRasterizerState);

			return S_OK;
		}

		void SetBlendState(BlendState* blend)
		{
			_ASSERT(mCurrWindow);
			_ASSERT(mCurrWindow->mContext);
			_ASSERT(blend);

			float blendFactor[4] = { 1, 1, 1, 1 };

			mCurrWindow->mContext->OMSetBlendState(static_cast<BlendStateDX*>(blend)->mBlendState, blendFactor, 0xFFFFFFFF);
		}

		// Shaders.
		//
		Shader* CreateShaderFromFile(const char* filename)
		{
			ShaderDX* shader = new ShaderDX(mCurrWindow->mDevice, mCurrWindow->mContext);

			if (shader->CreateFromFile(filename) != S_OK)
			{
				delete shader;
				return nullptr;
			}

			return shader;
		}

		Shader* CreateShaderFromMemory(const char* source)
		{
			ShaderDX* shader = new ShaderDX(mCurrWindow->mDevice, mCurrWindow->mContext);

			if (shader->CreateFromMemory(const_cast<char*>(source)) != S_OK)
			{
				delete shader;
				return nullptr;
			}

			return shader;
		}

		// Vertex Layout.
		//
		VertexLayout* CreateVertexLayout(const std::vector<VertexAttribute>& attrib)
		{
			VertexLayoutDX* layout = new VertexLayoutDX();

			UINT size = (UINT)attrib.size();
			_ASSERT(size > 0);

			for (UINT x = 0; x < size; ++x)
			{
				layout->AddAttribute(attrib[x]);
			}

			if (layout->Create(mCurrWindow->mDevice) == S_FALSE)
			{
				delete layout;
				return nullptr;
			}

			return layout;
		}

		void SetVertexLayout(VertexLayout* vertexLayout)
		{
			_ASSERT(vertexLayout);

			mCurrWindow->mContext->IASetInputLayout(static_cast<const VertexLayoutDX*>(vertexLayout)->mInputLayout);
		}

		// Rendering.
		//
		void Clear(float* color, float depth)
		{
			_ASSERT(mCurrWindow);
			_ASSERT(mCurrWindow->mContext);
			_ASSERT(mCurrTarget);
			_ASSERT(mCurrStencil);

			ClearColor(color);
			ClearDepth(depth);
		}

		void ClearColor(float* color)
		{
			mCurrWindow->mContext->ClearRenderTargetView(mCurrTarget, color);
		}

		void ClearDepth(float depth)
		{
			mCurrWindow->mContext->ClearDepthStencilView(mCurrStencil, D3D11_CLEAR_DEPTH, depth, 0);
		}

		void Draw(PrimitiveFormat primitive, UINT count, UINT baseVertex)
		{
			_ASSERT(mCurrWindow);
			_ASSERT(mCurrWindow->mContext);
			_ASSERT(mCurrShader);

			mCurrWindow->mContext->IASetPrimitiveTopology(PRIMITIVE[(BYTE)primitive]);
			mCurrWindow->mContext->Draw(count, baseVertex);
		}

		void DrawIndexed(PrimitiveFormat primitive, UINT count, UINT startIndex, UINT baseVertex)
		{
			_ASSERT(mCurrWindow);
			_ASSERT(mCurrWindow->mContext);
			_ASSERT(mCurrShader);

			mCurrWindow->mContext->IASetPrimitiveTopology(PRIMITIVE[(BYTE)primitive]);
			mCurrWindow->mContext->DrawIndexed(count, startIndex, baseVertex);
		}

		void Present()
		{
			_ASSERT(mCurrWindow);

			mCurrWindow->mSwapChain->Present(0, 0);
		}
	};

	const D3D11_PRIMITIVE_TOPOLOGY RendererDX::PRIMITIVE[] =
	{
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	};

	const D3D11_CULL_MODE RendererDX::CULL_TYPE[] =
	{
		D3D11_CULL_NONE,
		D3D11_CULL_BACK,
		D3D11_CULL_FRONT
	};

	const D3D11_FILL_MODE RendererDX::FILL_TYPE[] =
	{
		D3D11_FILL_SOLID,
		D3D11_FILL_WIREFRAME
	};

	const D3D11_BLEND RendererDX::BLEND_TYPE[] =
	{
		D3D11_BLEND_ZERO,
		D3D11_BLEND_ONE,

		D3D11_BLEND_SRC_COLOR,
		D3D11_BLEND_INV_SRC_COLOR,
		D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND_INV_SRC_ALPHA,

		D3D11_BLEND_DEST_COLOR,
		D3D11_BLEND_INV_DEST_COLOR,
		D3D11_BLEND_DEST_ALPHA,
		D3D11_BLEND_INV_DEST_ALPHA
	};

	const D3D11_BLEND_OP RendererDX::BLEND_FUNC_TYPE[] =
	{
		D3D11_BLEND_OP_ADD,
		D3D11_BLEND_OP_SUBTRACT,
		D3D11_BLEND_OP_REV_SUBTRACT,
		D3D11_BLEND_OP_MIN,
		D3D11_BLEND_OP_MAX
	};

	Renderer* BuildRendererDX()
	{
		return new RendererDX();
	}
}
