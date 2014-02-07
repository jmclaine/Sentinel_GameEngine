#include <fstream>
#include <vector>

#include "Renderer.h"
#include "Archive.h"

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

		static const UINT PRIMITIVE[ NUM_PRIMITIVES ];
		static const UINT CULL_TYPE[ NUM_CULL_TYPES ];
		static const UINT FILL_TYPE[ NUM_FILL_TYPES ];

		//////////////////////////////////

		class WindowInfoGL : public WindowInfo
		{
			friend class RendererGL;
			
		public:

			HDC			mHDC;
			HGLRC		mContext;

			GLenum		mRenderMode;
		};

		//////////////////////////////////

		class RenderTextureGL : public RenderTexture
		{
		public:

			Texture*	mTexture;
			GLuint		mID;
			
			RenderTextureGL( Texture* texture, GLuint id ) :
				mTexture( texture ),
				mID( id )
			{}

			~RenderTextureGL()
			{
				glDeleteFramebuffers( 1, &mID );
			}
		};

		//////////////////////////////////

		class DepthStencilGL : public DepthStencil
		{
		public:

			GLuint		mID;

			DepthStencilGL( GLuint id ) :
				mID( id )
			{}
		};

		//////////////////////////////////

		class BlendStateGL : public BlendState
		{
		public:

			bool		mEnable;

			GLenum		mSrcBlendColor;
			GLenum		mDstBlendColor;

			GLenum		mSrcBlendAlpha;
			GLenum		mDstBlendAlpha;

			GLenum		mBlendFuncColor;
			GLenum		mBlendFuncAlpha;

			BlendStateGL( bool enable,
						  BlendType srcBlendColor, BlendType dstBlendColor,
						  BlendType srcBlendAlpha, BlendType dstBlendAlpha,
						  BlendFuncType blendFuncColor, BlendFuncType blendFuncAlpha ) :
				mEnable( enable ),
				mSrcBlendColor( BLENDtoENUM( srcBlendColor )), mDstBlendColor( BLENDtoENUM( dstBlendColor )),
				mSrcBlendAlpha( BLENDtoENUM( srcBlendAlpha )), mDstBlendAlpha( BLENDtoENUM( dstBlendAlpha )),
				mBlendFuncColor( FUNCtoENUM( blendFuncColor )), mBlendFuncAlpha( FUNCtoENUM( blendFuncAlpha ))
			{}

			~BlendStateGL()
			{}

			void Apply()
			{
				if( mEnable )
				{
					glEnable( GL_BLEND );
					glBlendFuncSeparate( mSrcBlendColor, mDstBlendColor, mSrcBlendAlpha, mDstBlendAlpha );
					glBlendEquationSeparate( mBlendFuncColor, mBlendFuncAlpha );
				}
				else
				{
					glDisable( GL_BLEND );
				}
			}

		private:

			static GLenum BLENDtoENUM( BlendType type )
			{
				switch( type )
				{
				case BLEND_ZERO:
					return GL_ZERO;

				case BLEND_ONE:
					return GL_ONE;

				case BLEND_SRC_COLOR:
					return GL_SRC_COLOR;

				case BLEND_ONE_MINUS_SRC_COLOR:
					return GL_ONE_MINUS_SRC_COLOR;

				case BLEND_SRC_ALPHA:
					return GL_SRC_ALPHA;

				case BLEND_ONE_MINUS_SRC_ALPHA:
					return GL_ONE_MINUS_SRC_ALPHA;

				case BLEND_DST_COLOR:
					return GL_DST_COLOR;

				case BLEND_ONE_MINUS_DST_COLOR:
					return GL_ONE_MINUS_DST_COLOR;

				case BLEND_DST_ALPHA:
					return GL_DST_ALPHA;

				case BLEND_ONE_MINUS_DST_ALPHA:
					return GL_ONE_MINUS_DST_ALPHA;

				default:
					return GL_INVALID_ENUM;
				}
			}

			static GLenum FUNCtoENUM( BlendFuncType type )
			{
				switch( type )
				{
				case BLEND_FUNC_ADD:
					return GL_FUNC_ADD;

				case BLEND_FUNC_SUBTRACT:
					return GL_FUNC_SUBTRACT;

				case BLEND_FUNC_REVERSE_SUBTRACT:
					return GL_FUNC_REVERSE_SUBTRACT;

				case BLEND_FUNC_MIN:
					return GL_MIN;

				case BLEND_FUNC_MAX:
					return GL_MAX;

				default:
					return GL_INVALID_ENUM;
				}
			}
		};

		//////////////////////////////////

		WindowInfoGL* mCurrWindow;

	public:
		
		RendererGL()
		{
			NULL_TEXTURE = NULL;
			BASE_TEXTURE = NULL;

			mCurrWindow  = NULL;
			mCurrShader  = NULL;
		}

		RendererGL::~RendererGL()
		{}
		
		WindowInfo* Startup( void* hWnd, bool fullscreen, UINT width, UINT height )
		{
			mCurrWindow = new WindowInfoGL();

			mCurrWindow->mHandle		= hWnd;
			mCurrWindow->mFullscreen	= fullscreen;
			mCurrWindow->mWidth			= width;
			mCurrWindow->mHeight		= height;
			mCurrWindow->mWidthRatio	= (float)width  / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio	= (float)height / (float)WINDOW_HEIGHT_BASE;
			mCurrWindow->mHDC			= GetDC( (HWND)hWnd );

			if( !mCurrWindow->mHDC )
			{
				delete mCurrWindow;
				return NULL;
			}

			PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};
			pixelFormatDescriptor.nSize			= sizeof( pixelFormatDescriptor );
			pixelFormatDescriptor.nVersion		= 1;
			pixelFormatDescriptor.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
			pixelFormatDescriptor.dwLayerMask	= PFD_MAIN_PLANE;
			pixelFormatDescriptor.iPixelType	= PFD_TYPE_RGBA;
			pixelFormatDescriptor.cColorBits	= 32;
			pixelFormatDescriptor.cDepthBits	= 24;
			
			int pixelFormat = ChoosePixelFormat( mCurrWindow->mHDC, &pixelFormatDescriptor );
			if( !pixelFormat )
			{
				delete mCurrWindow;
				return NULL;
			}

			SetPixelFormat( mCurrWindow->mHDC, pixelFormat, &pixelFormatDescriptor );

			mCurrWindow->mContext = wglCreateContext( mCurrWindow->mHDC );
			wglMakeCurrent( mCurrWindow->mHDC, mCurrWindow->mContext );

			if( fullscreen )
			{
				DEVMODE dmScreenSettings;
				memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ));

				dmScreenSettings.dmSize			= sizeof( dmScreenSettings );
				dmScreenSettings.dmPelsWidth	= width;
				dmScreenSettings.dmPelsHeight	= height;
				dmScreenSettings.dmBitsPerPel	= 32;
				dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				if( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
				{
					delete mCurrWindow;
					return NULL;
				}
			}

			glewInit();

			TRACE( "Created OpenGL " << glGetString( GL_VERSION ) << " Renderer" );

			glEnable( GL_DEPTH_TEST );

			glEnable( GL_CULL_FACE );
			glFrontFace( GL_CCW );

			// Create NULL_TEXTURE.
			//
			if( !NULL_TEXTURE.get() )
				NULL_TEXTURE = std::shared_ptr< Texture >( new TextureGL( 0, 0, IMAGE_FORMAT_RGBA, 0 ));

			// Create initial white texture as BASE_TEXTURE.
			//
			if( !BASE_TEXTURE.get() )
			{
				UCHAR* newTex = new UCHAR[ 4 ];

				newTex[ 0 ] = 255;
				newTex[ 1 ] = 255;
				newTex[ 2 ] = 255;
				newTex[ 3 ] = 255;

				BASE_TEXTURE = std::shared_ptr< Texture >(CreateTextureFromMemory( newTex, 1, 1, IMAGE_FORMAT_RGBA, false ));

				if( !BASE_TEXTURE )
				{
					delete mCurrWindow;
					return NULL;
				}

				delete newTex;
			}

			// Create non-blending state.
			//
			if( !BLEND_OFF.get() )
				BLEND_OFF = std::shared_ptr< BlendState >(new BlendStateGL( false, BLEND_ZERO, BLEND_ZERO, BLEND_ZERO, BLEND_ZERO, BLEND_FUNC_ADD, BLEND_FUNC_ADD ));

			// Create default alpha-blending state.
			//
			if( !BLEND_ALPHA.get() )
				BLEND_ALPHA = std::shared_ptr< BlendState >(CreateBlendState());

			SetBlendState( BLEND_ALPHA.get() );

			return mCurrWindow;
		}

		void Shutdown()
		{
			mCurrShader = NULL;

			if( mCurrWindow )
			{
				wglMakeCurrent( NULL, NULL );
				wglDeleteContext( mCurrWindow->mContext );
				mCurrWindow->mContext = 0;

				ReleaseDC( (HWND)mCurrWindow->mHandle, mCurrWindow->mHDC );
			}
		}

		// Windows.
		//
		void SetWindow( WindowInfo* info )
		{
			_ASSERT( info );

			mCurrWindow = (WindowInfoGL*)info;

			glFinish();
			glFlush();

			wglMakeCurrent( mCurrWindow->mHDC, mCurrWindow->mContext );
		}

		WindowInfo* GetWindow()
		{
			return mCurrWindow;
		}

		bool ShareResources( WindowInfo* info0, WindowInfo* info1 )
		{
			return wglShareLists( ((WindowInfoGL*)info0)->mContext, ((WindowInfoGL*)info1)->mContext ) != 0;
		}

		// Buffers.
		//
		Buffer* CreateBuffer( void* data, UINT size, UINT stride, BufferType type, BufferAccessType access )
		{
			return new BufferGL( data, size, stride, type, access );
		}

		void SetVBO( Buffer* buffer )
		{
			glBindBuffer( GL_ARRAY_BUFFER, static_cast< BufferGL* >(buffer)->ID() );
		}

		void SetIBO( Buffer* buffer )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, static_cast< BufferGL* >(buffer)->ID() );
			glEnableClientState( GL_INDEX_ARRAY );
			glIndexPointer( GL_UNSIGNED_INT, sizeof( UINT ), 0 );
		}

		// Textures.
		//
		Texture* CreateTextureFromFile( const char* filename, bool createMips = true )
		{
			// TODO: Check for compatible texture size.

			_ASSERT( strlen( filename ) > 0 );

			// Load texture for 32-bits.
			//
			int width, height;
			int nChannels;
			unsigned char *pixels = stbi_load( filename,
											   &width, &height,
											   &nChannels, 4 );

			if ( !pixels )
			{
				REPORT_ERROR( filename << TEXT(" failed to load."), TEXT("Texture Load Error"));
				return NULL;
			}

			Texture* texture = CreateTextureFromMemory( pixels, width, height, IMAGE_FORMAT_RGBA, createMips );

			stbi_image_free( pixels );

			return texture;
		}

		Texture* CreateTextureFromMemory( void* data, UINT width, UINT height, ImageFormatType format, bool createMips = true )
		{
			GLuint texID;
			glGenTextures( 1, &texID );
			glBindTexture( GL_TEXTURE_2D, texID );

			if( createMips )
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			}
			else
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			}
			
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

			switch( format )
			{
			case IMAGE_FORMAT_R:
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_R, GL_UNSIGNED_BYTE, data );
				break;

			case IMAGE_FORMAT_RGB:
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
				break;

			case IMAGE_FORMAT_RGBA:
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
				break;

			case IMAGE_FORMAT_HDR:
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA32F, GL_UNSIGNED_BYTE, data );
				break;

			case IMAGE_FORMAT_DEPTH:
				glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, data );
				break;

			default:
				_ASSERT(0); // invalid image type
				return NULL;
			}

			if( createMips )
				glGenerateMipmap( GL_TEXTURE_2D );
			
			return new TextureGL( width, height, format, texID );
		}

		Texture* CreateTextureCube( UINT width, UINT height, ImageFormatType format )
		{
			GLuint texID;
			glGenTextures( 1, &texID );
			glBindTexture( GL_TEXTURE_CUBE_MAP, texID );
			
			glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

			glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

			switch( format )
			{
			case IMAGE_FORMAT_R:
				for( UINT x = 0; x < 6; ++x )
					glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, width, height, 0, GL_R, GL_UNSIGNED_BYTE, NULL );
				break;

			case IMAGE_FORMAT_RGB:
				for( UINT x = 0; x < 6; ++x )
					glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
				break;

			case IMAGE_FORMAT_RGBA:
				for( UINT x = 0; x < 6; ++x )
					glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
				break;

			case IMAGE_FORMAT_HDR:
				for( UINT x = 0; x < 6; ++x )
					glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_RGBA, width, height, 0, GL_RGBA32F, GL_UNSIGNED_BYTE, NULL );
				break;

			case IMAGE_FORMAT_DEPTH:
				for( UINT x = 0; x < 6; ++x )
					glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL );
				break;

			default:
				_ASSERT(0); // invalid image type
				return NULL;
			}

			return new TextureGL( width, height, format, texID );
		}

		void* GetTexturePixels( Texture* texture )
		{
			BYTE* pixels = new BYTE[ (texture->Width() << 2) * texture->Height() ];

			glBindTexture( GL_TEXTURE_2D, static_cast< TextureGL* >(texture)->ID() );
			glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

			return pixels;
		}

		// Special Rendering.
		//
		RenderTexture* CreateBackbuffer()
		{
			return new RenderTextureGL( NULL_TEXTURE.get(), 0 );
		}

		RenderTexture* CreateRenderTexture( Texture* texture )
		{
			GLuint id;
			glGenFramebuffers( 1, &id );
			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, id );

			switch( texture->Format() )
			{
			case IMAGE_FORMAT_DEPTH:
				glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, static_cast< TextureGL* >(texture)->ID(), 0 );
				break;

			default:
				glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, static_cast< TextureGL* >(texture)->ID(), 0 );
				break;
			}
			
			if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			{
				REPORT_ERROR( "Failed to create Render Target", "OpenGL Render Target" );

				return NULL;
			}
			
			return new RenderTextureGL( texture, id );
		}

		DepthStencil* CreateDepthStencil( UINT width, UINT height )
		{
			GLuint id;
			glGenRenderbuffers( 1, &id );
			glBindRenderbuffer( GL_RENDERBUFFER, id );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id );
			
			return new DepthStencilGL( id );
		}

		BlendState* CreateBlendState( BlendType srcBlendColor = BLEND_SRC_ALPHA, BlendType dstBlendColor = BLEND_ONE_MINUS_SRC_ALPHA, 
									  BlendType srcBlendAlpha = BLEND_SRC_ALPHA, BlendType dstBlendAlpha = BLEND_ONE_MINUS_SRC_ALPHA,
									  BlendFuncType blendFuncColor = BLEND_FUNC_ADD, BlendFuncType blendFuncAlpha = BLEND_FUNC_ADD )
		{
			return new BlendStateGL( true, 
									 srcBlendColor, dstBlendColor, 
									 srcBlendAlpha, dstBlendAlpha,
									 blendFuncColor, blendFuncAlpha );
		}

		UINT ResizeBuffers( UINT width, UINT height )
		{
			mCurrWindow->mWidth			= width;
			mCurrWindow->mHeight		= height;

			mCurrWindow->mWidthRatio	= (float)width  / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio	= (float)height / (float)WINDOW_HEIGHT_BASE;

			return 1;
		}

		void SetRenderType( RenderType type )
		{
			_ASSERT( mCurrWindow );

			mCurrWindow->mRenderMode = type;
		}

		void SetRenderTexture( RenderTexture* target )
		{
			_ASSERT( target );

			RenderTextureGL* targetGL = static_cast< RenderTextureGL* >(target);

			glBindFramebuffer( GL_FRAMEBUFFER, targetGL->mID );

			if( targetGL->mTexture->Format() == IMAGE_FORMAT_DEPTH )
			{
				glDrawBuffer( GL_NONE );
				glReadBuffer( GL_NONE );
			}
		}

		void SetDepthStencil( DepthStencil* stencil )
		{
			_ASSERT( stencil );

			glBindRenderbuffer( GL_RENDERBUFFER, static_cast< DepthStencilGL* >(stencil)->mID );
		}

		void SetDepthStencilType( DepthType state )
		{
			switch( state )
			{
			case DEPTH_LESS:
				glDepthFunc( GL_LESS );
				break;

			case DEPTH_EQUAL:
				glDepthFunc( GL_EQUAL );
				break;

			case DEPTH_GREATER:
				glDepthFunc( GL_GREATER );
				break;

			case DEPTH_NOTEQUAL:
				glDepthFunc( GL_NOTEQUAL );
				break;

			case DEPTH_GEQUAL:
				glDepthFunc( GL_GEQUAL );
				break;
				
			case DEPTH_LEQUAL:
				glDepthFunc( GL_LEQUAL );
				break;

			case DEPTH_ALWAYS:
				glDepthFunc( GL_ALWAYS );
				break;

			case DEPTH_OFF:
				glDepthMask( GL_FALSE );
				return;
			}

			glDepthMask( GL_TRUE );
		}

		void SetViewport( int x, int y, UINT width, UINT height )
		{
			_ASSERT( mCurrWindow );

			glViewport( x, y, width, height );
		}

		UINT SetCull( CullType type )
		{
			if( type != CULL_NONE )
			{
				glEnable( GL_CULL_FACE );
				glFrontFace( CULL_TYPE[ type ] );
			}
			else
			{
				glDisable( GL_CULL_FACE );
			}

			return S_OK;
		}

		UINT SetFill( FillType type )
		{
			glPolygonMode( GL_FRONT_AND_BACK, FILL_TYPE[ type ] );

			return S_OK;
		}

		void SetBlendState( BlendState* blend )
		{
			_ASSERT( blend );

			static_cast< BlendStateGL* >(blend)->Apply();
		}
		
		// Shaders.
		//
		Shader* CreateShaderFromFile( const char* filename )
		{
			ShaderGL* shader = new ShaderGL();

			if( shader->CreateFromFile( filename ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return shader;
		}

		Shader* CreateShaderFromMemory( const char* source )
		{
			ShaderGL* shader = new ShaderGL();

			if( shader->CreateFromMemory( const_cast< char* >(source)) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return shader;
		}

		// Vertex Layout.
		//
		VertexLayout* CreateVertexLayout( const std::vector< AttributeType >& attrib )
		{
			VertexLayoutGL* layout = new VertexLayoutGL();

			UINT size = (UINT)attrib.size();
			for( UINT x = 0; x < size; ++x )
			{
				layout->AddAttribute( attrib[ x ] );
			}

			return layout;
		}

		void SetVertexLayout( VertexLayout* vertexLayout )
		{
			const VertexLayoutGL* vertexLayoutGL = static_cast< const VertexLayoutGL* >(vertexLayout);

			ShaderGL* shaderGL = static_cast< ShaderGL* >(mCurrShader);

			const std::vector< AttributeType >& layout = vertexLayout->Layout();
			UINT size = (UINT)layout.size();
			UINT index = 0;

			AttributeType type;
			GLint loc;
			
			for( UINT x = 0; x < size; ++x )
			{
				type = layout[ x ];

				loc = (GLint)shaderGL->mAttributeGL[ type ];

				if( type != ATTRIB_MATRIX )
				{
					const VertexLayoutGL::AttributeGL& attrib = vertexLayoutGL->GetAttribute( index++ );

					glVertexAttribPointer( loc, \
									   attrib.mCount, attrib.mType, attrib.mNormalize, vertexLayout->VertexSize(), \
									   reinterpret_cast< const GLvoid* >( attrib.mOffset ));
				}
				else
				{
					for( UINT y = 0; y < 4; ++y )
					{
						const VertexLayoutGL::AttributeGL& attrib = vertexLayoutGL->GetAttribute( index++ );
						
						glVertexAttribPointer( loc++, \
									   attrib.mCount, attrib.mType, attrib.mNormalize, vertexLayout->VertexSize(), \
									   reinterpret_cast< const GLvoid* >( attrib.mOffset ));
					}
				}
			}
		}

		// Rendering.
		//
		void Clear( float* color )
		{
			glClearColor( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
			glClearDepth( 1 );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		}

		void DrawIndexed( UINT count, UINT startIndex, UINT baseVertex )
		{
			glDrawRangeElementsBaseVertex( PRIMITIVE[ mCurrWindow->mRenderMode ], startIndex, startIndex + count, count, GL_UNSIGNED_INT, \
										   reinterpret_cast< void* >( startIndex * sizeof( UINT )), baseVertex );
		}

		void Present()
		{
			SwapBuffers( mCurrWindow->mHDC );
		}
	};

	const UINT RendererGL::PRIMITIVE[] = 
		{ GL_POINTS, 
		  GL_LINES, 
		  GL_TRIANGLES };

	const UINT RendererGL::CULL_TYPE[] = 
		{ GL_NONE, 
		  GL_CCW,
		  GL_CW };

	const UINT RendererGL::FILL_TYPE[] = 
		{ GL_FILL,
		  GL_LINE };

	Renderer* BuildRendererGL()
	{
		return new RendererGL();
	}
}
