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

namespace Sentinel
{
	class RendererGL : public Renderer
	{
	private:

		static const UINT PRIMITIVE[PrimitiveFormat::COUNT];
		static const GLenum CULL_TYPE[CullFormat::COUNT];
		static const UINT FILL_TYPE[FillFormat::COUNT];

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
			{}

			~RenderTextureGL()
			{
				glDeleteFramebuffers(1, &mID);
			}
		};

		//////////////////////////////////

		class DepthStencilGL : public DepthStencil
		{
		private:

			DepthFormat::Type mCurrDepth;
			bool mDepthEnabled;

		public:

			GLuint mID;

			/////////////////////////

			DepthStencilGL(GLuint id, UINT width, UINT height) :
				mID(id),
				mCurrDepth(DepthFormat::OFF),
				mDepthEnabled(false),
				DepthStencil(width, height)
			{}

			void SetDepth(DepthFormat::Type depth)
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

			static GLenum BLEND_TYPE[BlendFormat::COUNT];
			static GLenum BLEND_FUNC[BlendFunction::COUNT];

			static BlendStateGL CURR_STATE;

		public:

			bool mEnable;

			BlendStateGL(
				bool enable,
				BlendFormat::Type srcBlendColor, BlendFormat::Type dstBlendColor,
				BlendFormat::Type srcBlendAlpha, BlendFormat::Type dstBlendAlpha,
				BlendFunction::Type blendFuncColor, BlendFunction::Type blendFuncAlpha) :
				mEnable(enable),
				BlendState(srcBlendColor, dstBlendColor,
				srcBlendAlpha, dstBlendAlpha,
				blendFuncColor, blendFuncAlpha)
			{}

			~BlendStateGL()
			{}

			void Apply()
			{
				if (mEnable)
				{
					//if( !CURR_STATE.mEnable )
					{
						glEnable(GL_BLEND);

						CURR_STATE.mEnable = true;
					}

					//if( CURR_STATE.mSrcBlendColor != mSrcBlendColor || CURR_STATE.mDstBlendColor != mDstBlendColor ||
					//	CURR_STATE.mSrcBlendAlpha != mSrcBlendAlpha || CURR_STATE.mDstBlendAlpha != mDstBlendAlpha )
					{
						glBlendFuncSeparate(BLEND_TYPE[mSrcBlendColor], BLEND_TYPE[mDstBlendColor], \
							BLEND_TYPE[mSrcBlendAlpha], BLEND_TYPE[mDstBlendAlpha]);

						CURR_STATE.mSrcBlendColor = mSrcBlendColor;
						CURR_STATE.mDstBlendColor = mDstBlendColor;
						CURR_STATE.mSrcBlendAlpha = mSrcBlendAlpha;
						CURR_STATE.mDstBlendAlpha = mDstBlendAlpha;
					}

					//if( CURR_STATE.mBlendFuncColor != mBlendFuncColor || CURR_STATE.mBlendFuncAlpha != mBlendFuncAlpha )
					{
						glBlendEquationSeparate(BLEND_FUNC[mBlendFuncColor], BLEND_FUNC[mBlendFuncAlpha]);

						CURR_STATE.mBlendFuncColor = mBlendFuncColor;
						CURR_STATE.mBlendFuncAlpha = mBlendFuncAlpha;
					}
				}
				else
				{
					//if( CURR_STATE.mEnable )
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
			NULL_TEXTURE = NULL;
			BASE_TEXTURE = NULL;

			mCurrWindow = NULL;
			mCurrRenderTexture = NULL;
			mCurrDepthStencil = NULL;

			mClearDepth = 1.0f;

			mCurrShader = NULL;
		}

		RendererGL::~RendererGL()
		{}

		WindowInfo* Startup(void* hWnd, bool fullscreen, UINT width, UINT height)
		{
			mCurrWindow = new WindowInfoGL();

			mCurrWindow->mHandle = hWnd;
			mCurrWindow->mFullscreen = fullscreen;
			mCurrWindow->mWidth = width;
			mCurrWindow->mHeight = height;
			mCurrWindow->mWidthRatio = (float)width / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio = (float)height / (float)WINDOW_HEIGHT_BASE;
			mCurrWindow->mHDC = GetDC((HWND)hWnd);

			if (!mCurrWindow->mHDC)
			{
				delete mCurrWindow;
				return NULL;
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
				return NULL;
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
					return NULL;
				}
			}

			glewInit();

			Debug::Log(STREAM("Created OpenGL " << glGetString(GL_VERSION) << " Renderer"));

			glEnable(GL_DEPTH_TEST);

			SetCull(CullFormat::CCW);

			// Create NULL_TEXTURE
			//
			if (!NULL_TEXTURE.get())
				NULL_TEXTURE = std::shared_ptr<Texture>(new TextureGL(0, 0, ImageFormat::RGBA, 0));

			// Create white BASE_TEXTURE
			//
			if (!BASE_TEXTURE.get())
			{
				UCHAR* newTex = new UCHAR[4];

				newTex[0] = 255;
				newTex[1] = 255;
				newTex[2] = 255;
				newTex[3] = 255;

				BASE_TEXTURE = std::shared_ptr<Texture>(CreateTexture(newTex, 1, 1, ImageFormat::RGBA, false));

				if (!BASE_TEXTURE)
				{
					delete mCurrWindow;
					return NULL;
				}

				delete newTex;
			}

			// Create non-blending state.
			//
			if (!BLEND_OFF.get())
				BLEND_OFF = std::shared_ptr<BlendState>(
					new BlendStateGL(
						false, 
						BlendFormat::ZERO, BlendFormat::ZERO,
						BlendFormat::ZERO, BlendFormat::ZERO,
						BlendFunction::ADD, BlendFunction::ADD));

			// Create default alpha-blending state
			//
			if (!BLEND_ALPHA.get())
				BLEND_ALPHA = std::shared_ptr<BlendState>(CreateBlendState());

			SetBlendState(BLEND_ALPHA.get());

			return mCurrWindow;
		}

		void Shutdown()
		{
			mCurrShader = NULL;

			if (mCurrWindow)
			{
				wglMakeCurrent(NULL, NULL);
				wglDeleteContext(mCurrWindow->mContext);
				mCurrWindow->mContext = 0;

				ReleaseDC((HWND)mCurrWindow->mHandle, mCurrWindow->mHDC);
			}
		}

		//
		// Windows
		//
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

		//
		// Buffers
		//
		Buffer* CreateBuffer(void* data, UINT size, UINT stride, BufferFormat::Type type, BufferAccess::Type access)
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

		//
		// Textures
		//
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

				return NULL;
			}

			Texture* texture = CreateTexture(pixels, width, height, ImageFormat::RGBA, createMips);

			stbi_image_free(pixels);

			return texture;
		}

		Texture* CreateTexture(void* data, UINT width, UINT height, ImageFormat::Type format, bool createMips = true)
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
				return NULL;
			}

			if (createMips)
				glGenerateMipmap(GL_TEXTURE_2D);

			return new TextureGL(width, height, format, texID);
		}

		Texture* CreateTextureCube(void* data, UINT width, UINT height, ImageFormat::Type format)
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
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
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
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, NULL);
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
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
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
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
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
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				}
				break;

			default:
				_ASSERT(0); // invalid image type
				return NULL;
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

		// Special Rendering
		//
		RenderTexture* CreateBackbuffer()
		{
			return new RenderTextureGL(0, NULL);
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

				return NULL;
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
			BlendFormat::Type srcBlendColor = BlendFormat::SRC_ALPHA, BlendFormat::Type dstBlendColor = BlendFormat::ONE_MINUS_SRC_ALPHA,
			BlendFormat::Type srcBlendAlpha = BlendFormat::SRC_ALPHA, BlendFormat::Type dstBlendAlpha = BlendFormat::ONE_MINUS_SRC_ALPHA,
			BlendFunction::Type blendFuncColor = BlendFunction::ADD, BlendFunction::Type blendFuncAlpha = BlendFunction::ADD)
		{
			return new BlendStateGL(true,
				srcBlendColor, dstBlendColor,
				srcBlendAlpha, dstBlendAlpha,
				blendFuncColor, blendFuncAlpha);
		}

		UINT ResizeBuffers(UINT width, UINT height)
		{
			mCurrWindow->mWidth = width;
			mCurrWindow->mHeight = height;

			mCurrWindow->mWidthRatio = (float)width / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio = (float)height / (float)WINDOW_HEIGHT_BASE;

			return 1;
		}

		void SetRenderTexture(RenderTexture* target)
		{
			_ASSERT(target);

			if (mCurrRenderTexture != target)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, static_cast<RenderTextureGL*>(target)->mID);

				mCurrRenderTexture = target;
			}
		}

		void SetDepthStencil(DepthStencil* stencil)
		{
			_ASSERT(stencil);

			if (mCurrDepthStencil != stencil)
			{
				glBindRenderbuffer(GL_RENDERBUFFER, static_cast<DepthStencilGL*>(stencil)->mID);

				mCurrDepthStencil = stencil;
			}
		}

		void SetDepthStencilType(DepthFormat::Type depth)
		{
			//if( mCurrDepthStencil )
			//	static_cast< DepthStencilGL* >(mCurrDepthStencil)->SetDepth( depth );
			//else
			{
				static DepthFormat::Type CURR_DEPTH = DepthFormat::UNKNOWN;
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

		UINT SetCull(CullFormat::Type type)
		{
			static CullFormat::Type CURR_CULL = CullFormat::CCW;

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

					static CullFormat::Type CURR_FACE = CullFormat::CCW;

					if (CURR_FACE != type)
					{
						glFrontFace(CULL_TYPE[type]);

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

		UINT SetFill(FillFormat::Type type)
		{
			glPolygonMode(GL_FRONT_AND_BACK, FILL_TYPE[type]);

			return S_OK;
		}

		void SetBlendState(BlendState* blend)
		{
			_ASSERT(blend);

			static_cast<BlendStateGL*>(blend)->Apply();
		}

		// Shaders.
		//
		Shader* CreateShaderFromFile(const char* filename)
		{
			ShaderGL* shader = new ShaderGL();

			if (shader->CreateFromFile(filename) != S_OK)
			{
				delete shader;
				return NULL;
			}

			return shader;
		}

		Shader* CreateShaderFromMemory(const char* source)
		{
			ShaderGL* shader = new ShaderGL();

			if (shader->CreateFromMemory(const_cast<char*>(source)) != S_OK)
			{
				delete shader;
				return NULL;
			}

			return shader;
		}

		// Vertex Layout.
		//
		VertexLayout* CreateVertexLayout(const std::vector<VertexAttribute::Type>& attrib)
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

			const std::vector<VertexAttribute::Type>& layout = vertexLayout->Layout();
			UINT size = (UINT)layout.size();
			UINT index = 0;

			VertexAttribute::Type type;
			GLint loc;

			for (UINT x = 0; x < size; ++x)
			{
				type = layout[x];

				loc = (GLint)shaderGL->mAttributeGL[type];

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

		// Rendering.
		//
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

		void Draw(PrimitiveFormat::Type primitive, UINT count, UINT baseVertex)
		{
			glDrawArrays(PRIMITIVE[primitive], baseVertex, count);
		}

		void DrawIndexed(PrimitiveFormat::Type primitive, UINT count, UINT startIndex, UINT baseVertex)
		{
			glDrawRangeElementsBaseVertex(PRIMITIVE[primitive], startIndex, startIndex + count, count, GL_UNSIGNED_INT,
				reinterpret_cast<void*>(startIndex * sizeof(UINT)), baseVertex);
		}

		void Present()
		{
			SwapBuffers(mCurrWindow->mHDC);

			glFlush();
			glFinish();
		}
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
		BlendFormat::UNKNOWN, BlendFormat::UNKNOWN, BlendFormat::UNKNOWN, BlendFormat::UNKNOWN,
		BlendFunction::ADD, BlendFunction::ADD);

	Renderer* BuildRendererGL()
	{
		return new RendererGL();
	}
}
