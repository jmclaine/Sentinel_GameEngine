#include <fstream>
#include <vector>
#include <windows.h>

#include "Renderer.h"
#include "Archive.h"
#include "Debug.h"
#include "CommonGL.h"
#include "BufferGL.h"
#include "TextureGL.h"
#include "ShaderGL.h"
#include "VertexLayoutGL.h"
#include "DepthStencil.h"
#include "RenderTexture.h"
#include "WindowInfo.h"
#include "BlendState.h"

namespace Sentinel
{
	class RendererGL : public Renderer
	{
	private:
		static const UINT PRIMITIVE[(BYTE)PrimitiveFormat::COUNT];
		static const GLenum CULL_TYPE[(BYTE)CullFormat::COUNT];
		static const UINT FILL_TYPE[(BYTE)FillFormat::COUNT];

	public:
		class WindowInfoGL : public WindowInfo
		{
			friend class RendererGL;

		public:
			HDC mHDC;
			HGLRC mContext;

			GLenum mRenderMode;
		};

		//////////////////////////////////

		class RenderTextureGL : public RenderTexture
		{
		public:
			GLuint mID;

			RenderTextureGL(GLuint id, Texture* texture) :
				mID(id),
				RenderTexture(texture)
			{ }

			~RenderTextureGL()
			{
				glDeleteFramebuffers(1, &mID);
			}
		};

		//////////////////////////////////

		class DepthStencilGL : public DepthStencil
		{
		private:
			DepthFormat mCurrDepth;
			bool mDepthEnabled;

		public:
			GLuint mID;

			/////////////////////////

			DepthStencilGL(GLuint id, UINT width, UINT height) :
				mID(id),
				mCurrDepth(DepthFormat::OFF),
				mDepthEnabled(false),
				DepthStencil(width, height)
			{ }

			void SetDepth(DepthFormat depth)
			{
				if (mCurrDepth != depth)
				{
					mCurrDepth = depth;

					switch (depth)
					{
					case DepthFormat::LESS:
						glDepthFunc(GL_LESS);
						break;

					case DepthFormat::EQUAL:
						glDepthFunc(GL_EQUAL);
						break;

					case DepthFormat::GREATER:
						glDepthFunc(GL_GREATER);
						break;

					case DepthFormat::NOTEQUAL:
						glDepthFunc(GL_NOTEQUAL);
						break;

					case DepthFormat::GEQUAL:
						glDepthFunc(GL_GEQUAL);
						break;

					case DepthFormat::LEQUAL:
						glDepthFunc(GL_LEQUAL);
						break;

					case DepthFormat::ALWAYS:
						glDepthFunc(GL_ALWAYS);
						break;

					case DepthFormat::OFF:
						if (mDepthEnabled)
						{
							glDepthMask(GL_FALSE);
							mDepthEnabled = false;
						}
						return;

					default:
						_ASSERT(0);	// invalid depth
						break;
					}
				}

				if (!mDepthEnabled)
				{
					glDepthMask(GL_TRUE);
					mDepthEnabled = true;
				}
			}
		};

		//////////////////////////////////

		class BlendStateGL : public BlendState
		{
		private:
			static GLenum BLEND_TYPE[(BYTE)BlendFormat::COUNT];
			static GLenum BLEND_FUNC[(BYTE)BlendFunction::COUNT];

			static BlendStateGL CURR_STATE;

		public:
			bool mEnable;

			BlendStateGL(
				bool enable,
				BlendFormat srcBlendColor, BlendFormat dstBlendColor, BlendFunction blendFuncColor,
				BlendFormat srcBlendAlpha, BlendFormat dstBlendAlpha, BlendFunction blendFuncAlpha) :
				mEnable(enable),
				BlendState(
					srcBlendColor, dstBlendColor, blendFuncColor,
					srcBlendAlpha, dstBlendAlpha, blendFuncAlpha)
			{ }

			~BlendStateGL()
			{ }

			void Apply()
			{
				if (mEnable)
				{
					//if(!CURR_STATE.mEnable)
					{
						glEnable(GL_BLEND);

						CURR_STATE.mEnable = true;
					}

					//if (CURR_STATE.mSrcBlendColor != mSrcBlendColor || CURR_STATE.mDstBlendColor != mDstBlendColor ||
					//	CURR_STATE.mSrcBlendAlpha != mSrcBlendAlpha || CURR_STATE.mDstBlendAlpha != mDstBlendAlpha)
					{
						glBlendFuncSeparate(
							BLEND_TYPE[(BYTE)mSrcBlendColor], BLEND_TYPE[(BYTE)mDstBlendColor],
							BLEND_TYPE[(BYTE)mSrcBlendAlpha], BLEND_TYPE[(BYTE)mDstBlendAlpha]);

						CURR_STATE.mSrcBlendColor = mSrcBlendColor;
						CURR_STATE.mDstBlendColor = mDstBlendColor;
						CURR_STATE.mSrcBlendAlpha = mSrcBlendAlpha;
						CURR_STATE.mDstBlendAlpha = mDstBlendAlpha;
					}

					//if (CURR_STATE.mBlendFuncColor != mBlendFuncColor || CURR_STATE.mBlendFuncAlpha != mBlendFuncAlpha)
					{
						glBlendEquationSeparate(BLEND_FUNC[(BYTE)mBlendFuncColor], BLEND_FUNC[(BYTE)mBlendFuncAlpha]);

						CURR_STATE.mBlendFuncColor = mBlendFuncColor;
						CURR_STATE.mBlendFuncAlpha = mBlendFuncAlpha;
					}
				}
				else
				{
					//if (CURR_STATE.mEnable)
					{
						glDisable(GL_BLEND);

						CURR_STATE.mEnable = false;
					}
				}
			}
		};

	private:
		WindowInfoGL* mCurrWindow;
		RenderTexture* mCurrRenderTexture;
		DepthStencil* mCurrDepthStencil;

		Buffer* mCurrVertexBuffer;
		Buffer* mCurrIndexBuffer;

		float mClearDepth;
		float* mClearColor;

	public:
		RendererGL()
		{
			mCurrWindow = nullptr;
			mCurrRenderTexture = nullptr;
			mCurrDepthStencil = nullptr;

			mClearDepth = 1.0f;

			mCurrShader = nullptr;
		}

		RendererGL::~RendererGL()
		{ }

		WindowInfo* Startup(void* hWnd, bool fullscreen, UINT width, UINT height)
		{
			mCurrWindow = new WindowInfoGL();

			mCurrWindow->mHandle = hWnd;
			mCurrWindow->mFullscreen = fullscreen;
			mCurrWindow->mWidth = width;
			mCurrWindow->mHeight = height;
			mCurrWindow->mWidthRatio = (float)width / (float)WindowInfo::BASE_WIDTH;
			mCurrWindow->mHeightRatio = (float)height / (float)WindowInfo::BASE_HEIGHT;
			mCurrWindow->mHDC = GetDC((HWND)hWnd);

			if (!mCurrWindow->mHDC)
			{
				delete mCurrWindow;
				return nullptr;
			}

			PIXELFORMATDESCRIPTOR pixelFormatDescriptor = { 0 };
			pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
			pixelFormatDescriptor.nVersion = 1;
			pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
			pixelFormatDescriptor.dwLayerMask = PFD_MAIN_PLANE;
			pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
			pixelFormatDescriptor.cColorBits = 32;
			pixelFormatDescriptor.cDepthBits = 24;

			int pixelFormat = ChoosePixelFormat(mCurrWindow->mHDC, &pixelFormatDescriptor);
			if (!pixelFormat)
			{
				delete mCurrWindow;
				return nullptr;
			}

			SetPixelFormat(mCurrWindow->mHDC, pixelFormat, &pixelFormatDescriptor);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			mCurrWindow->mContext = wglCreateContext(mCurrWindow->mHDC);
			wglMakeCurrent(mCurrWindow->mHDC, mCurrWindow->mContext);

			if (fullscreen)
			{
				DEVMODE dmScreenSettings;
				memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

				dmScreenSettings.dmSize = sizeof(dmScreenSettings);
				dmScreenSettings.dmPelsWidth = width;
				dmScreenSettings.dmPelsHeight = height;
				dmScreenSettings.dmBitsPerPel = 32;
				dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					delete mCurrWindow;
					return nullptr;
				}
			}

			glewInit();

			Debug::Log(STREAM("Created OpenGL " << glGetString(GL_VERSION) << " Renderer"));

			glEnable(GL_DEPTH_TEST);

			SetCull(CullFormat::CCW);
				
			return mCurrWindow;
		}

		void Shutdown()
		{
			mCurrShader = nullptr;

			if (mCurrWindow)
			{
				wglMakeCurrent(nullptr, nullptr);
				wglDeleteContext(mCurrWindow->mContext);
				mCurrWindow->mContext = 0;

				ReleaseDC((HWND)mCurrWindow->mHandle, mCurrWindow->mHDC);
			}
		}

		#pragma region Windows

		void SetWindow(WindowInfo* info)
		{
			_ASSERT(info);

			if (mCurrWindow != info)
			{
				mCurrWindow = (WindowInfoGL*)info;

				wglMakeCurrent(mCurrWindow->mHDC, mCurrWindow->mContext);
			}
		}

		WindowInfo* GetWindow()
		{
			return mCurrWindow;
		}

		bool ShareResources(WindowInfo* info0, WindowInfo* info1)
		{
			return wglShareLists(((WindowInfoGL*)info0)->mContext, ((WindowInfoGL*)info1)->mContext) != 0;
		}

		#pragma endregion

		#pragma region Buffers

		Buffer* CreateBuffer(void* data, UINT size, UINT stride, BufferFormat type, BufferAccess access)
		{
			return new BufferGL(this, data, size, stride, type, access);
		}

		void SetVertexBuffer(Buffer* buffer)
		{
			_ASSERT(buffer);

			if (mCurrVertexBuffer != buffer)
			{
				glBindBuffer(GL_ARRAY_BUFFER, static_cast<BufferGL*>(buffer)->ID());

				mCurrVertexBuffer = buffer;
			}
		}

		void SetIndexBuffer(Buffer* buffer)
		{
			_ASSERT(buffer);

			if (mCurrIndexBuffer != buffer)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<BufferGL*>(buffer)->ID());

				mCurrIndexBuffer = buffer;
			}
		}

		#pragma endregion

		#pragma region Textures

		Texture* CreateTextureFromFile(const char* filename, bool createMips = true)
		{
			// TODO: Check for compatible texture size.

			_ASSERT(strlen(filename) > 0);

			// Load texture for 32-bits.
			//
			int width, height;
			int nChannels;
			unsigned char *pixels = stbi_load(
				filename,
				&width, &height,
				&nChannels, 4);

			if (!pixels)
			{
				Debug::ShowError(
					STREAM(filename << " failed to load."),
					STREAM("Texture Load Error"));

				return nullptr;
			}

			Texture* texture = CreateTexture(pixels, width, height, ImageFormat::RGBA, createMips);

			stbi_image_free(pixels);

			return texture;
		}

		Texture* CreateTexture(void* data, UINT width, UINT height, ImageFormat format, bool createMips = true)
		{
			GLuint texID;
			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);

			if (createMips)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			switch (format)
			{
			case ImageFormat::R:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
				break;

			case ImageFormat::RG:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
				break;

			case ImageFormat::RGB:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				break;

			case ImageFormat::RGBA:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				break;

			case ImageFormat::HDR:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data);
				break;

			case ImageFormat::DEPTH:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, data);
				break;

			default:
				_ASSERT(0); // invalid image type
				return nullptr;
			}

			if (createMips)
				glGenerateMipmap(GL_TEXTURE_2D);

			return new TextureGL(width, height, format, texID);
		}

		Texture* CreateTextureCube(void* data, UINT width, UINT height, ImageFormat format)
		{
			GLuint texID;
			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			UCHAR* dataPtr = (UCHAR*)data;
			UINT size = width * height;

			switch (format)
			{
			case ImageFormat::R:
				if (data)
				{
					for (UINT x = 0; x < 6; ++x)
					{
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, dataPtr);
						dataPtr += (size * sizeof(float));
					}
				}
				else
				{
					for (UINT x = 0; x < 6; ++x)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
				}
				break;

			case ImageFormat::RG:
				if (data)
				{
					for (UINT x = 0; x < 6; ++x)
					{
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, dataPtr);
						dataPtr += (size * sizeof(float));
					}
				}
				else
				{
					for (UINT x = 0; x < 6; ++x)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, nullptr);
				}
				break;

			case ImageFormat::RGB:
				if (data)
				{
					for (UINT x = 0; x < 6; ++x)
					{
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, dataPtr);
						dataPtr += (size * sizeof(float));
					}
				}
				else
				{
					for (UINT x = 0; x < 6; ++x)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
				}
				break;

			case ImageFormat::RGBA:
				if (data)
				{
					for (UINT x = 0; x < 6; ++x)
					{
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataPtr);
						dataPtr += (size * sizeof(float));
					}
				}
				else
				{
					for (UINT x = 0; x < 6; ++x)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				}
				break;

			case ImageFormat::HDR:
				if (data)
				{
					for (UINT x = 0; x < 6; ++x)
					{
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, dataPtr);
						dataPtr += (size * sizeof(float));
					}
				}
				else
				{
					for (UINT x = 0; x < 6; ++x)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
				}
				break;

			case ImageFormat::DEPTH:
				if (data)
				{
					for (UINT x = 0; x < 6; ++x)
					{
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, dataPtr);
						dataPtr += (size * sizeof(float));
					}
				}
				else
				{
					for (UINT x = 0; x < 6; ++x)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
				}
				break;

			default:
				_ASSERT(0); // invalid image type
				return nullptr;
			}

			return new TextureGL(width, height, format, texID);
		}

		void* GetTexturePixels(Texture* texture)
		{
			BYTE* pixels = new BYTE[(texture->Width() << 2) * texture->Height()];

			glBindTexture(GL_TEXTURE_2D, static_cast<TextureGL*>(texture)->ID());
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

			return pixels;
		}

		#pragma endregion

		#pragma region Render Settings

		RenderTexture* CreateBackbuffer()
		{
			return new RenderTextureGL(0, nullptr);
		}

		RenderTexture* CreateRenderTexture(Texture* texture)
		{
			_ASSERT(texture);
			
			GLuint id;
			glGenFramebuffers(1, &id);
			glBindFramebuffer(GL_FRAMEBUFFER, id);

			if (texture->Format() != ImageFormat::DEPTH)
				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, static_cast<TextureGL*>(texture)->ID(), 0);
			else
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, static_cast<TextureGL*>(texture)->ID(), 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				Debug::ShowError(
					"Failed to create Render Target",
					"OpenGL Render Target");

				return nullptr;
			}

			return new RenderTextureGL(id, texture);
		}

		DepthStencil* CreateDepthStencil(UINT width, UINT height)
		{
			GLuint id;
			glGenRenderbuffers(1, &id);
			glBindRenderbuffer(GL_RENDERBUFFER, id);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id);

			return new DepthStencilGL(id, width, height);
		}

		BlendState* CreateBlendState(
			BlendFormat srcBlendColor, BlendFormat dstBlendColor, BlendFunction blendFuncColor,
			BlendFormat srcBlendAlpha, BlendFormat dstBlendAlpha, BlendFunction blendFuncAlpha)
		{
			return new BlendStateGL(true,
				srcBlendColor, dstBlendColor, blendFuncColor,
				srcBlendAlpha, dstBlendAlpha, blendFuncAlpha);
		}

		UINT ResizeBuffers(UINT width, UINT height)
		{
			mCurrWindow->mWidth = width;
			mCurrWindow->mHeight = height;

			mCurrWindow->mWidthRatio = (float)width / (float)WindowInfo::BASE_WIDTH;
			mCurrWindow->mHeightRatio = (float)height / (float)WindowInfo::BASE_HEIGHT;

			return 1;
		}

		void SetRenderTexture(RenderTexture* target)
		{
			if (mCurrRenderTexture != target)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, (target) ? static_cast<RenderTextureGL*>(target)->mID : 0);

				mCurrRenderTexture = target;
			}
		}

		void SetDepthStencil(DepthStencil* stencil)
		{
			if (mCurrDepthStencil != stencil)
			{
				glBindRenderbuffer(GL_RENDERBUFFER, (stencil) ? static_cast<DepthStencilGL*>(stencil)->mID : 0);

				mCurrDepthStencil = stencil;
			}
		}

		void SetDepthStencilType(DepthFormat depth)
		{
			//if( mCurrDepthStencil )
			//	static_cast< DepthStencilGL* >(mCurrDepthStencil)->SetDepth( depth );
			//else
			{
				static DepthFormat CURR_DEPTH = DepthFormat::UNKNOWN;
				static bool DEPTH_ENABLED = true;

				if (depth == DepthFormat::OFF)
				{
					CURR_DEPTH = depth;

					if (DEPTH_ENABLED)
					{
						glDepthMask(GL_FALSE);

						DEPTH_ENABLED = false;
					}

					return;
				}

				//if( CURR_DEPTH != depth )
				{
					CURR_DEPTH = depth;

					switch (depth)
					{
					case DepthFormat::LESS:
						glDepthFunc(GL_LESS);
						break;

					case DepthFormat::EQUAL:
						glDepthFunc(GL_EQUAL);
						break;

					case DepthFormat::GREATER:
						glDepthFunc(GL_GREATER);
						break;

					case DepthFormat::NOTEQUAL:
						glDepthFunc(GL_NOTEQUAL);
						break;

					case DepthFormat::GEQUAL:
						glDepthFunc(GL_GEQUAL);
						break;

					case DepthFormat::LEQUAL:
						glDepthFunc(GL_LEQUAL);
						break;

					case DepthFormat::ALWAYS:
						glDepthFunc(GL_ALWAYS);
						break;

					default:
						_ASSERT(0);	// invalid depth
						break;
					}
				}

				if (!DEPTH_ENABLED)
				{
					glDepthMask(GL_TRUE);

					DEPTH_ENABLED = true;
				}
			}
		}

		void SetViewport(int x, int y, UINT width, UINT height)
		{
			_ASSERT(mCurrWindow);

			static int CURR_X = INT_MIN;
			static int CURR_Y = INT_MIN;
			static UINT CURR_WIDTH = -1;
			static UINT CURR_HEIGHT = -1;

			if (CURR_X != x || CURR_Y != y || CURR_WIDTH != width || CURR_HEIGHT != height)
			{
				glViewport(x, y, width, height);

				CURR_X = x;
				CURR_Y = y;
				CURR_WIDTH = width;
				CURR_HEIGHT = height;
			}
		}

		UINT SetCull(CullFormat type)
		{
			static CullFormat CURR_CULL = CullFormat::CCW;

			if (CURR_CULL != type)
			{
				CURR_CULL = type;

				static bool ENABLE_CULL = false;

				if (type != CullFormat::NONE)
				{
					if (!ENABLE_CULL)
					{
						glEnable(GL_CULL_FACE);

						ENABLE_CULL = true;
					}

					static CullFormat CURR_FACE = CullFormat::CCW;

					if (CURR_FACE != type)
					{
						glFrontFace(CULL_TYPE[(BYTE)type]);

						CURR_FACE = type;
					}
				}
				else if (ENABLE_CULL)
				{
					glDisable(GL_CULL_FACE);

					ENABLE_CULL = false;
				}
			}

			return S_OK;
		}

		UINT SetFill(FillFormat type)
		{
			glPolygonMode(GL_FRONT_AND_BACK, FILL_TYPE[(BYTE)type]);

			return S_OK;
		}

		void SetBlendState(BlendState* blend)
		{
			_ASSERT(blend);

			static_cast<BlendStateGL*>(blend)->Apply();
		}

		#pragma endregion

		#pragma region Shaders

		Shader* CreateShaderFromFile(const char* filename)
		{
			ShaderGL* shader = new ShaderGL();

			if (shader->CreateFromFile(filename) != S_OK)
			{
				delete shader;
				return nullptr;
			}

			return shader;
		}

		Shader* CreateShaderFromMemory(const char* source)
		{
			ShaderGL* shader = new ShaderGL();

			if (shader->CreateFromMemory(const_cast<char*>(source)) != S_OK)
			{
				delete shader;
				return nullptr;
			}

			return shader;
		}

		#pragma endregion

		#pragma region Vertex Layout

		VertexLayout* CreateVertexLayout(const std::vector<VertexAttribute>& attrib)
		{
			VertexLayoutGL* layout = new VertexLayoutGL();

			UINT size = (UINT)attrib.size();
			for (UINT x = 0; x < size; ++x)
			{
				layout->AddAttribute(attrib[x]);
			}

			return layout;
		}

		void SetVertexLayout(VertexLayout* vertexLayout)
		{
			const VertexLayoutGL* vertexLayoutGL = static_cast<const VertexLayoutGL*>(vertexLayout);

			ShaderGL* shaderGL = static_cast<ShaderGL*>(mCurrShader);

			const std::vector<VertexAttribute>& layout = vertexLayout->Layout();
			UINT size = (UINT)layout.size();
			UINT index = 0;

			VertexAttribute type;
			GLint loc;

			for (UINT x = 0; x < size; ++x)
			{
				type = layout[x];

				loc = (GLint)shaderGL->mAttributeGL[(BYTE)type];

				if (type != VertexAttribute::MATRIX)
				{
					const VertexLayoutGL::AttributeGL& attrib = vertexLayoutGL->GetAttribute(index++);

					glVertexAttribPointer(loc,
						attrib.mCount, attrib.mType, attrib.mNormalize, vertexLayout->VertexSize(),
						reinterpret_cast<const GLvoid*>(attrib.mOffset));
				}
				else
				{
					for (UINT y = 0; y < 4; ++y)
					{
						const VertexLayoutGL::AttributeGL& attrib = vertexLayoutGL->GetAttribute(index++);

						glVertexAttribPointer(loc++,
							attrib.mCount, attrib.mType, attrib.mNormalize, vertexLayout->VertexSize(),
							reinterpret_cast<const GLvoid*>(attrib.mOffset));
					}
				}
			}
		}

		#pragma endregion

		#pragma region Rendering

		void Clear(float* color, float depth)
		{
			if (mClearColor != color)
			{
				glClearColor(color[0], color[1], color[2], color[3]);

				mClearColor = color;
			}

			if (mClearDepth != depth)
			{
				glClearDepth(depth);

				mClearDepth = depth;
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void ClearColor(float* color)
		{
			if (mClearColor != color)
			{
				glClearColor(color[0], color[1], color[2], color[3]);

				mClearColor = color;
			}

			glClear(GL_COLOR_BUFFER_BIT);
		}

		void ClearDepth(float depth)
		{
			if (mClearDepth != depth)
			{
				glClearDepth(depth);

				mClearDepth = depth;
			}

			glClear(GL_DEPTH_BUFFER_BIT);
		}

		void Draw(PrimitiveFormat primitive, UINT count, UINT baseVertex)
		{
			glDrawArrays(PRIMITIVE[(BYTE)primitive], baseVertex, count);
		}

		void DrawIndexed(PrimitiveFormat primitive, UINT count, UINT startIndex, UINT baseVertex)
		{
			//glMultiDrawElementsBaseVertex
			glDrawRangeElementsBaseVertex(PRIMITIVE[(BYTE)primitive], startIndex, startIndex + count, count, GL_UNSIGNED_INT,
				reinterpret_cast<void*>(startIndex * sizeof(UINT)), baseVertex);
		}

		void Present()
		{
			SwapBuffers(mCurrWindow->mHDC);

			glFlush();
			glFinish();
		}

		#pragma endregion
	};

	///////////////////////////////////////////////////

	const UINT RendererGL::PRIMITIVE[] =
	{ 
		GL_POINTS,
		GL_LINES,
		GL_TRIANGLES
	};

	const GLenum RendererGL::CULL_TYPE[] =
	{ 
		GL_NONE,
		GL_CCW,
		GL_CW
	};

	const UINT RendererGL::FILL_TYPE[] =
	{
		GL_FILL,
		GL_LINE
	};

	GLenum RendererGL::BlendStateGL::BLEND_TYPE[] =
	{
		GL_ZERO,
		GL_ONE,

		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,

		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA
	};

	GLenum RendererGL::BlendStateGL::BLEND_FUNC[] =
	{
		GL_FUNC_ADD,
		GL_FUNC_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT,
		GL_MIN,
		GL_MAX
	};

	///////////////////////////////////////////////////

	RendererGL::BlendStateGL RendererGL::BlendStateGL::CURR_STATE(
		false,
		BlendFormat::UNKNOWN, BlendFormat::UNKNOWN, BlendFunction::ADD,
		BlendFormat::UNKNOWN, BlendFormat::UNKNOWN, BlendFunction::ADD);

	Renderer* BuildRendererGL()
	{
		return new RendererGL();
	}
}
