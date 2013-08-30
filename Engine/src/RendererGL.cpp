#include "Renderer.h"

#include <GL/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "glew32.lib")

#include <fstream>
#include <vector>

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

#ifdef _DEBUG
inline void HANDLE_GL_ERRORS()
{
	GLenum err = glGetError();

	if( err != GL_NO_ERROR )
	{
		REPORT_ERROR( gluErrorUnicodeStringEXT(err), "OpenGL Error" );
	}
}
#else
#define HANDLE_GL_ERRORS();
#endif

namespace Sentinel
{
	class RendererGL;

	class TextureGL : public Texture
	{
		friend class RendererGL;

	private:

		GLuint	mID;

	public:

		TextureGL( const char* filename, UINT width, UINT height )
		{
			mFilename	= filename;
			mWidth		= width;
			mHeight		= height;
			mID			= 0;
		}

		GLuint ID()
		{
			return mID;
		}

		void Release()
		{
			glDeleteTextures( 1, &mID );
		}
	};

	////////////////////////////////////////////////////////////////////////////////////

	class ShaderGL : public Shader
	{
	private:
		
		GLuint		mProgramID;
		GLuint		mVertexShader;
		GLuint		mGeometryShader;
		GLuint		mFragmentShader;

		UINT		mTextureLevel;

		std::vector< GLuint > mUniformGL;

		struct Attribute
		{
			GLuint	mType;			// type of value
			UINT	mOffset;		// offset value for attribute
			UINT	mOffsetSize;	// offset count of type
			bool	mNormalize;		// normalize these values?
		};

		Attribute*  mAttributeGL;
		UINT		mAttributeSize;

	public:

		ShaderGL() :
			mProgramID( 0 ),
			mVertexShader( 0 ),
			mGeometryShader( 0 ),
			mFragmentShader( 0 ),
			mAttributeGL( NULL ),
			mAttributeSize( 0 ),
			mTextureLevel( 0 )
		{}

		~ShaderGL()
		{
			Release();
		}

	private:

		// Set a vertex attribute's offset.
		//
		void SetOffset( UINT index, UINT &vertexSize, UINT offset, GLuint type, bool normalize = false )
		{
			mAttributeGL[ index ].mType   = type;
			mAttributeGL[ index ].mOffset = vertexSize;

			switch( type )
			{
			case GL_FLOAT:
				mAttributeGL[ index ].mOffsetSize = offset >> 2;
				break;

			case GL_UNSIGNED_BYTE:
				mAttributeGL[ index ].mOffsetSize = offset;
				break;

			default:
				mAttributeGL[ index ].mOffsetSize = offset >> 2;
				break;
			}

			mAttributeGL[ index ].mNormalize = normalize;
			vertexSize += offset;
		}

	public:

		UINT Startup( std::string filename, std::string attrib, std::string uniform )
		{
			mAttribute  = attrib;
			mUniform	= uniform;

			// Create a new program.
			//
			mProgramID = glCreateProgram();

			if( mProgramID < 0 )
				REPORT_ERROR( "Could not create shader program.", "Shader Loader Error" );
			
			// Create the shaders.
			//
			std::string filenameVS = filename;
			filenameVS.append( ".vsh" );
			if( CreateFromFile( filenameVS.c_str(), mVertexShader, GL_VERTEX_SHADER ) != 1 )
				return S_FALSE;
			
			// Allow no geometry shader.
			//
			bool useGS = false;
			std::string filenameGS = filename;
			filenameGS.append( ".gsh" );

			int result = CreateFromFile( filenameGS.c_str(), mGeometryShader, GL_GEOMETRY_SHADER );
			if( result == -1 )
			{
				return S_FALSE;
			}
			else
			if( result == 1 )
			{
				useGS = true;

				glProgramParameteriEXT( mProgramID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS );
				glProgramParameteriEXT( mProgramID, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );
				glProgramParameteriEXT( mProgramID, GL_GEOMETRY_VERTICES_OUT_EXT, 4 );
			}

			std::string filenamePS = filename;
			filenamePS.append( ".psh" );
			if( CreateFromFile( filenamePS.c_str(), mFragmentShader, GL_FRAGMENT_SHADER ) != 1 )
				return S_FALSE;

			// Create the attributes.
			//
			mVertexSize = 0;
			GLchar name[ 32 ];
			UINT texCount = 0;

			mAttributeSize = attrib.size();

			// Increase the size for each attribute
			// that requires more than one value.
			//
			for( UINT i = 0; i < mAttributeSize; ++i )
			{
				if( attrib[ i ] == 'B' )
				{
					mAttributeSize += 2;
					break;
				}
				else
				if( attrib[ i ] == 'x' )
				{
					mAttributeSize += 3;
					break;
				}
			}

			mAttributeGL = new Attribute[ mAttributeSize ];

			UINT attribIndex = 0;
			for( UINT i = 0; i < (UINT)attrib.size(); ++i )
			{
				switch( attrib[ i ] )
				{
				// Position.
				//
				case 'P':
					sprintf_s( name, "aPosition" );
					SetOffset( attribIndex, mVertexSize, 12, GL_FLOAT );
					break;

				// Normal as float3.
				//
				case 'N':
					sprintf_s( name, "aNormal" );
					SetOffset( attribIndex, mVertexSize, 12, GL_FLOAT );
					break;

				// Color as UINT.
				//
				case 'C':
					sprintf_s( name, "aColor" );
					SetOffset( attribIndex, mVertexSize, 4, GL_UNSIGNED_BYTE, true ); 
					break;

				// Texture Coordinates.
				//
				case 'X':
					sprintf_s( name, "aTexture%d", texCount );
					SetOffset( attribIndex, mVertexSize, 8, GL_FLOAT );
					++texCount;
					break;

				// Vertex Matrix.
				//
				case 'x':
					sprintf_s( name, "aMatrix" );
					glBindAttribLocation( mProgramID, attribIndex, name );

					for( UINT x = 0; x < 4; ++x )
						SetOffset( attribIndex++, mVertexSize, 16, GL_FLOAT );
					
					continue;

				// Tangent Vector with sign.
				//
				case 'T':
					sprintf_s( name, "aTangent" );
					SetOffset( attribIndex, mVertexSize, 16, GL_FLOAT );
					break;

				// Bones.
				//
				case 'B':
					// Number of bones.
					//
					sprintf_s( name, "aNumBones" );
					SetOffset( attribIndex, mVertexSize, 4, GL_UNSIGNED_BYTE );
					glBindAttribLocation( mProgramID, attribIndex++, name );
						
					// Bone index.
					//
					sprintf_s( name, "aBoneIndex" );
					SetOffset( attribIndex, mVertexSize, 16, GL_UNSIGNED_BYTE );
					glBindAttribLocation( mProgramID, attribIndex++, name );
						
					// Weight.
					//
					sprintf_s( name, "aWeight" );
					SetOffset( attribIndex, mVertexSize, 16, GL_FLOAT );
					break;

				default:
					TRACE( attrib[ i ] << " = unknown Attribute" );
					_ASSERT(0);
				}

				glBindAttribLocation( mProgramID, attribIndex++, name );
			}
		
			// Link the shaders.
			//
			int didCompile;

			glAttachShader( mProgramID, mVertexShader );

			if( useGS )
			{
				glAttachShader( mProgramID, mGeometryShader );
			}

			glAttachShader( mProgramID, mFragmentShader );
			glLinkProgram( mProgramID );
			glGetProgramiv( mProgramID, GL_LINK_STATUS, &didCompile );

			// Report errors.
			//
			if( didCompile == GL_FALSE )
			{
				char* compileLog;
				int length;

				glGetProgramiv( mProgramID, GL_INFO_LOG_LENGTH, &length );

				compileLog = (char*)malloc( length );
 
				glGetProgramInfoLog( mProgramID, length, &length, compileLog );

				TRACE( "\nLink Shader Log" );
				TRACE( compileLog );
				free( compileLog );
				Release();

				return S_FALSE;
			}

			TRACE( "Shader Linked Successfully!" );

			ApplyPass();
		
			ProcessUniforms();

			TRACE( "Shader Uniforms Created!" );

			return S_OK;
		}

	private:

		int CreateFromFile( const char* filename, GLuint& shader, GLenum type )
		{
			_ASSERT( type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER || type == GL_GEOMETRY_SHADER );

			GLchar* source;
			int didCompile;

			if( !FileToBuffer( filename, source ))
			{
				shader = 0;
				return 0;
			}

			shader = glCreateShader( type );

			glShaderSource( shader, 1, (const GLchar**)&source, NULL );
			glCompileShader( shader );

			glGetShaderiv( shader, GL_COMPILE_STATUS, &didCompile );

			// Report compile errors.
			//
			if( didCompile == GL_FALSE )
			{
				int length;
				char* compileLog;

				glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &length );
 
				compileLog = (char*)malloc( length );
 
				glGetShaderInfoLog( shader, length, &length, compileLog );

				TRACE( "\n" << filename << " - Shader Compile Log" );
				TRACE( compileLog );

				free( compileLog );
				free( source );

				glDeleteShader( shader );

				return -1;
			}

			free( source );

			TRACE( filename << " Shader Created Successfully!" );

			return 1;
		}

	public:

		UINT AttributeSize()
		{
			return mAttributeSize;
		}

		void Release()
		{
			SAFE_DELETE_ARRAY( mAttributeGL );
			
			if( mProgramID != 0 )
			{
				glDeleteProgram( mProgramID );
				glDeleteShader( mVertexShader );
				glDeleteShader( mGeometryShader );
				glDeleteShader( mFragmentShader );
			}
		}

		void ApplyPass()
		{
			glUseProgram( mProgramID );
		}

		void ApplyLayout()
		{
			for( UINT i = 0; i < mAttributeSize; ++i )
				glVertexAttribPointer( i, mAttributeGL[ i ].mOffsetSize, mAttributeGL[ i ].mType, mAttributeGL[ i ].mNormalize, mVertexSize, \
									   reinterpret_cast< const GLvoid* >( mAttributeGL[ i ].mOffset ));
			
			mTextureLevel = 0;
		}

		void SetFloat( UINT uniform, float data )
		{
			glUniform1f( mUniformGL[ uniform ], data );
		}

		void SetFloat2( UINT uniform, float* data, UINT offset, UINT count )
		{
			glUniform2fv( mUniformGL[ uniform ] + offset, count, data );
		}

		void SetFloat3( UINT uniform, float* data, UINT offset, UINT count )
		{
			glUniform3fv( mUniformGL[ uniform ] + offset, count, data );
		}

		void SetFloat4( UINT uniform, float* data, UINT offset, UINT count )
		{
			glUniform4fv( mUniformGL[ uniform ] + offset, count, data );
		}

		void SetMatrix( UINT uniform, float* matrix, UINT offset, UINT count )
		{
			glUniformMatrix4fv( mUniformGL[ uniform ] + offset, count, false, matrix );
		}

		void SetTexture( UINT uniform, Texture* texture )
		{
			glUniform1i( mUniformGL[ uniform ], mTextureLevel );
			
			DWORD texID = GL_TEXTURE0 + mTextureLevel;
			glClientActiveTexture( texID );
			glActiveTexture( texID );

			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			glBindTexture( GL_TEXTURE_2D, static_cast< TextureGL* >(texture)->ID() );

			++mTextureLevel;
		}

		void CreateUniform( const char* name )
		{
			GLuint id = glGetUniformLocation( mProgramID, name );

			if( id == -1 )
				TRACE( "Shader Warning: Uniform '" << name << "' does not match within file." );
			
			mUniformGL.push_back( id );
		}
	};

	////////////////////////////////////////////////////////////////////////////////////

	class BufferGL : public Buffer
	{
		friend class RendererGL;

	private:

		GLuint	mID;

		////////////////////////

		BufferGL()
		{
			mID = 0;
		}

	public:

		~BufferGL()
		{
			Release();
		}

		void Startup( BufferType type )
		{
			mType = type;
		}

		void* Lock()
		{
			if( mType == VERTEX_BUFFER )
			{
				Renderer::Inst()->SetVBO( this );
				return glMapBuffer( GL_ARRAY_BUFFER, GL_READ_WRITE );
			}
			else
			{
				Renderer::Inst()->SetIBO( this );
				return glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE );
			}
		}

		void Unlock()
		{
			(mType == VERTEX_BUFFER) ? glUnmapBuffer( GL_ARRAY_BUFFER ) : glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
		}

		void Release()
		{
			glDeleteBuffers( 1, &mID );
		}
	};

	////////////////////////////////////////////////////////////////////////////////////
	// OpenGL 4.1 Renderer.
	//
	class RendererGL : public Renderer
	{
	private:

		class RenderTarget
		{
		public:

			Texture*	mTexture;
			GLuint		mID;
			
			RenderTarget( Texture* texture, GLuint id )
			{
				mTexture = texture;
				mID = id;
			}
		};

		class Viewport
		{
		public:

			GLuint		mWidth;
			GLuint		mHeight;

			Viewport( GLuint width, GLuint height )
			{
				mWidth  = width;
				mHeight = height;
			}
		};

		class WindowInfoGL : public WindowInfo
		{
			friend class RendererGL;
			
		public:

			HWND		mHWND;
			HDC			mHDC;
			HGLRC		mContext;

			GLenum		mRenderMode;
		};

		WindowInfoGL*					mCurrWindow;

		std::vector< RenderTarget >		mRenderTarget;
		std::vector< GLuint >			mDepthStencil;
		std::vector< Viewport >			mViewport;
		
		UINT IMAGE_FORMAT[ NUM_IMAGE_FORMATS ];

	public:
		
		RendererGL()
		{
			PRIMITIVE[ POINT_LIST ]				= GL_POINTS;
			PRIMITIVE[ LINE_LIST ]				= GL_LINES;
			PRIMITIVE[ TRIANGLE_LIST ]			= GL_TRIANGLES;

			CULL_TYPE[ CULL_NONE ]				= GL_NONE;
			CULL_TYPE[ CULL_CCW ]				= GL_CCW;
			CULL_TYPE[ CULL_CW ]				= GL_CW;

			IMAGE_FORMAT[ IMAGE_FORMAT_RGB ]	= GL_RGB;
			IMAGE_FORMAT[ IMAGE_FORMAT_RGBA ]	= GL_RGBA;

			NULL_TEXTURE = NULL;
			BASE_TEXTURE = NULL;

			mDepthStencil.push_back( 0 );

			mCurrShader = NULL;
			mCurrWindow = NULL;
		}

		RendererGL::~RendererGL()
		{
			SAFE_DELETE( NULL_TEXTURE );
			SAFE_DELETE( BASE_TEXTURE );
		}
		
		WindowInfo* Startup( void* hWnd, bool fullscreen, UINT width, UINT height )
		{
			mCurrWindow = new WindowInfoGL();

			mCurrWindow->mFullscreen	= fullscreen;
			mCurrWindow->mWidth			= width;
			mCurrWindow->mHeight		= height;
			mCurrWindow->mWidthRatio	= (float)width  / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio	= (float)height / (float)WINDOW_HEIGHT_BASE;
			mCurrWindow->mHWND			= (HWND)hWnd;
			mCurrWindow->mHDC			= GetDC( mCurrWindow->mHWND );

			if( !mCurrWindow->mHDC )
			{
				delete mCurrWindow;
				return NULL;
			}

			PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};
			pixelFormatDescriptor.nSize			= sizeof( pixelFormatDescriptor );
			pixelFormatDescriptor.nVersion		= 1;
			pixelFormatDescriptor.dwFlags		= PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
			pixelFormatDescriptor.dwLayerMask	= PFD_MAIN_PLANE;
			pixelFormatDescriptor.iPixelType	= PFD_TYPE_RGBA;
			pixelFormatDescriptor.cColorBits	= 32;
			pixelFormatDescriptor.cDepthBits	= 16;
			
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

			glEnable( GL_DEPTH_TEST );

			glEnable( GL_CULL_FACE );
			glFrontFace( GL_CCW );

			// Create NULL_TEXTURE.
			//
			if( !NULL_TEXTURE )
			{
				NULL_TEXTURE = new TextureGL( "", 0, 0 );
			}

			// Create initial white texture as BASE_TEXTURE.
			//
			if( !BASE_TEXTURE )
			{
				UCHAR* newTex = new UCHAR[ 4 ];

				newTex[ 0 ] = 255;
				newTex[ 1 ] = 255;
				newTex[ 2 ] = 255;
				newTex[ 3 ] = 255;

				BASE_TEXTURE = CreateTextureFromMemory( newTex, 1, 1, IMAGE_FORMAT_RGBA, false );

				if( !BASE_TEXTURE )
				{
					delete mCurrWindow;
					return NULL;
				}

				delete newTex;
			}

			return mCurrWindow;
		}

		void Shutdown()
		{
			mCurrShader = NULL;

			for( UINT i = 0; i < mRenderTarget.size(); ++i )
				glDeleteFramebuffers( 1, &mRenderTarget[ i ].mID );
			mRenderTarget.clear();

			if( mCurrWindow )
			{
				wglMakeCurrent( NULL, NULL );
				wglDeleteContext( mCurrWindow->mContext );
				mCurrWindow->mContext = 0;

				ReleaseDC( mCurrWindow->mHWND, mCurrWindow->mHDC );

				delete mCurrWindow;
				mCurrWindow = NULL;
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
		Buffer* CreateBuffer( void* data, UINT size, UINT stride, BufferType type )
		{
			_ASSERT( type == VERTEX_BUFFER || type == INDEX_BUFFER );

			BufferGL* buffer = new BufferGL();
			buffer->mType	 = type;
			buffer->mSize	 = size;
			buffer->mStride  = stride;
			buffer->mCount	 = size / stride;

			UINT bufferType;
			(type == VERTEX_BUFFER) ? bufferType = GL_ARRAY_BUFFER : bufferType = GL_ELEMENT_ARRAY_BUFFER;
			
			glGenBuffers( 1, &buffer->mID );
			glBindBuffer( bufferType, buffer->mID );
			glBufferData( bufferType, size,	data, GL_STATIC_DRAW );

			return buffer;
		}

		void SetVBO( Buffer* buffer )
		{
			glBindBuffer( GL_ARRAY_BUFFER, static_cast< BufferGL* >(buffer)->mID );
		}

		void SetIBO( Buffer* buffer )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, static_cast< BufferGL* >(buffer)->mID );
			glEnableClientState( GL_INDEX_ARRAY );
			glIndexPointer( GL_UNSIGNED_INT, sizeof( UINT ), 0 );
		}

		// Textures.
		//
		Texture* CreateTextureFromFile( const char* filename )
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

			TextureGL* texture = static_cast< TextureGL* >(CreateTextureFromMemory( pixels, width, height, IMAGE_FORMAT_RGBA ));

			texture->mFilename = filename;

			stbi_image_free( pixels );

			return texture;
		}

		Texture* CreateTextureFromMemory( void* data, UINT width, UINT height, ImageFormatType format, bool createMips = true )
		{
			UINT texID;
			glGenTextures( 1, &texID );

			// Configure the texture object.
			//
			glBindTexture( GL_TEXTURE_2D, texID );

			if( createMips )
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			}
			else
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );  
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			}
			
			// Load the texture object from memory.
			//
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, IMAGE_FORMAT[ format ], GL_UNSIGNED_BYTE, data );

			if( createMips )
				glGenerateMipmap( GL_TEXTURE_2D );
			
			TextureGL* texture = new TextureGL( "~Memory~", width, height );
			texture->mID = texID;

			return texture;
		}

		// Special Rendering.
		//
		UINT CreateBackbuffer()
		{
			mRenderTarget.push_back( RenderTarget( NULL_TEXTURE, 0 ));
			return mRenderTarget.size()-1;
		}

		UINT CreateRenderTarget( Texture* texture )
		{
			UINT renderID = mRenderTarget.size();

			mRenderTarget.push_back( RenderTarget( texture, 0 ));
			
			glGenFramebuffers( 1, &mRenderTarget.back().mID );
			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mRenderTarget.back().mID );
			glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, static_cast< TextureGL* >(texture)->mID, 0 );
			
			if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
				REPORT_ERROR( "Failed to create Render Target #" << renderID, "OpenGL Render Target" );
			
			return renderID;
		}

		UINT CreateDepthStencil( UINT width, UINT height )
		{
			GLuint id;
			glGenRenderbuffers( 1, &id );
			glBindRenderbuffer( GL_RENDERBUFFER, id );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height );
			glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id );
			
			mDepthStencil.push_back( id );
			return mDepthStencil.size()-1;
		}

		UINT CreateViewport( UINT width, UINT height )
		{
			mViewport.push_back( Viewport( width, height ));

			return mViewport.size()-1;
		}

		UINT ResizeBuffers( UINT width, UINT height )
		{
			mCurrWindow->mWidth			= width;
			mCurrWindow->mHeight		= height;

			mCurrWindow->mWidthRatio	= (float)width  / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio	= (float)height / (float)WINDOW_HEIGHT_BASE;

			return 1;
		}

		void SetRenderType( PrimitiveType type )
		{
			_ASSERT( mCurrShader );
			_ASSERT( mCurrWindow );

			mCurrShader->ApplyLayout();
			mCurrWindow->mRenderMode = type;
		}

		void SetRenderTarget( UINT target )
		{
			_ASSERT( target < mRenderTarget.size() );

			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mRenderTarget[ target ].mID );
		}

		void SetDepthStencil( UINT stencil )
		{
			_ASSERT( stencil < mDepthStencil.size() );

			glBindRenderbuffer( GL_RENDERBUFFER, mDepthStencil[ stencil ] );
		}

		void SetDepthStencilState( UINT state )
		{}

		void SetViewport( UINT viewport )
		{
			glViewport( 0, 0, mViewport[ viewport ].mWidth, mViewport[ viewport ].mHeight );
		}

		UINT SetCull( CullType type )
		{
			glFrontFace( CULL_TYPE[ type ] );

			return S_OK;
		}

		void SetBlend( BlendType type )
		{
			if( type == BLEND_DEFAULT )
			{
				glDisable( GL_BLEND );
			}
			else
			{
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glEnable( GL_BLEND );
			}
		}
		
		// Shaders.
		//
		Shader* CreateShader( const char* filename, const char* attrib, const char* uniform )
		{
			ShaderGL* shader = new ShaderGL();

			if( shader->Startup( filename, attrib, uniform ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return shader;
		}

		void SetShader( Shader* shader )
		{
			if( mCurrShader != NULL )
			{
				for( UINT i = 0; i < static_cast< ShaderGL* >(mCurrShader)->AttributeSize(); ++i )
					glDisableVertexAttribArray( i );
			}

			mCurrShader = shader;
			mCurrShader->ApplyPass();
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
			_ASSERT( mCurrShader != NULL );

			for( UINT i = 0; i < static_cast< ShaderGL* >(mCurrShader)->AttributeSize(); ++i )
				glEnableVertexAttribArray( i );
			
			glDrawRangeElementsBaseVertex( PRIMITIVE[ mCurrWindow->mRenderMode ], startIndex, startIndex + count, count, GL_UNSIGNED_INT, \
										   reinterpret_cast< void* >( startIndex * sizeof( UINT )), baseVertex );
			
			for( UINT i = 0; i < static_cast< ShaderGL* >(mCurrShader)->AttributeSize(); ++i )
				glDisableVertexAttribArray( i );
				
			glFlush();
		}

		void Present()
		{
			SwapBuffers( mCurrWindow->mHDC );
		}
	};


	Renderer* BuildRendererGL()
	{
		TRACE( "Creating OpenGL 4.1 Renderer..." );

		return new RendererGL();
	}
}