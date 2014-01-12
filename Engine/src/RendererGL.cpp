#include <GL/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "glew32.lib")

#include <fstream>
#include <vector>

#include "Renderer.h"
#include "Archive.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"

#ifdef _DEBUG
inline void HANDLE_GL_ERRORS()
{
	GLenum err = glGetError();

	if( err != GL_NO_ERROR )
		REPORT_ERROR( gluErrorUnicodeStringEXT(err), "OpenGL Error" );
}
#else
#define HANDLE_GL_ERRORS();
#endif

namespace Sentinel
{
	class RendererGL;

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
				glBindBuffer( GL_ARRAY_BUFFER, mID );

				return glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
			}
			else
			{
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mID );

				glEnableClientState( GL_INDEX_ARRAY );
				glIndexPointer( GL_UNSIGNED_INT, sizeof( UINT ), 0 );

				return glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY );
			}
		}

		void Unlock()
		{
			(mType == VERTEX_BUFFER) ? glUnmapBuffer( GL_ARRAY_BUFFER ) : glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
		}

		void Release()
		{
			glDeleteBuffers( 1, &mID );
			mID = 0;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////

	class TextureGL : public Texture
	{
		friend class RendererGL;

	private:

		GLuint	mID;

	public:

		TextureGL( UINT width, UINT height, GLuint id )
		{
			mWidth	= width;
			mHeight	= height;
			mID		= id;
		}

		~TextureGL()
		{
			Release();
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
	public:

		class SamplerGL : public Sampler
		{
		private:

			GLint	mWrapS;
			GLint	mWrapT;

			GLint	mMinFilter;
			GLint	mMagFilter;

		public:

			SamplerGL() :
				mWrapS( GL_REPEAT ),
				mWrapT( GL_REPEAT ),
				mMinFilter( GL_LINEAR ),
				mMagFilter( GL_LINEAR )
			{}

			void Create( SamplerMode modeU, SamplerMode modeV, SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter )
			{
				mWrapS = (modeU == MODE_WRAP) ? GL_REPEAT : GL_CLAMP;
				mWrapT = (modeV == MODE_WRAP) ? GL_REPEAT : GL_CLAMP;

				if( minFilter == FILTER_LINEAR )
				{
					if( mipFilter == FILTER_LINEAR )
					{
						mMinFilter = GL_LINEAR_MIPMAP_LINEAR;
					}
					else
					if( mipFilter == FILTER_POINT )
					{
						mMinFilter = GL_LINEAR_MIPMAP_NEAREST;
					}
					else
					{
						mMinFilter = GL_LINEAR;
					}
				}
				else
				{
					if( mipFilter == FILTER_LINEAR )
					{
						mMinFilter = GL_NEAREST_MIPMAP_LINEAR;
					}
					else
					if( mipFilter == FILTER_POINT )
					{
						mMinFilter = GL_NEAREST_MIPMAP_NEAREST;
					}
					else
					{
						mMinFilter = GL_NEAREST;
					}
				}

				if( magFilter == FILTER_LINEAR )
				{
					mMagFilter = GL_LINEAR;
				}
				else
				{
					mMagFilter = GL_NEAREST;
				}
			}

			void Apply()
			{
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrapS );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrapT );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mMinFilter );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mMagFilter );
			}
		};

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

		UINT CreateFromFile( std::string filename, const std::string& attrib, const std::string& uniform )
		{
			filename.append( ".xsh" );

			if( Archive::ToBuffer( filename.c_str(), mShaderSource ) == 0 )
			{
				REPORT_ERROR( "Could not open '" << filename << "'", "Shader Loader Error" );

				return S_FALSE;
			}
			
			TRACE( "Compiling '" << filename << "'..." );

			return CreateFromMemory( mShaderSource, attrib, uniform );
		}

		UINT CreateFromMemory( char* source, const std::string& attrib, const std::string& uniform )
		{
			mShaderSource	= source;
			mAttribute		= attrib;
			mUniform		= uniform;

			// Create a new program.
			//
			mProgramID = glCreateProgram();

			if( mProgramID < 0 )
				REPORT_ERROR( "Could not create shader program.", "Shader Loader Error" );

			if( strstr( mShaderSource, "VERTEX_SHADER" ) != NULL )
			{
				const char *vshader[2] = { "#define VERSION_GL\n#define VERTEX_SHADER\n\0", mShaderSource };
				
				if( !Compile( vshader, mVertexShader, GL_VERTEX_SHADER, 2 ))
					return S_FALSE;

				glAttachShader( mProgramID, mVertexShader );
			}
			
			if( strstr( mShaderSource, "GEOMETRY_SHADER" ) != NULL )
			{
				const char *gshader[2] = { "#define VERSION_GL\n#define GEOMETRY_SHADER\n\0", mShaderSource };

				if( !Compile( gshader, mGeometryShader, GL_GEOMETRY_SHADER, 2 ))
					return S_FALSE;

				// Figure out a way to allow customization of this:
				//
				glProgramParameteriEXT( mProgramID, GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS );
				glProgramParameteriEXT( mProgramID, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP );
				glProgramParameteriEXT( mProgramID, GL_GEOMETRY_VERTICES_OUT_EXT, 4 );

				glAttachShader( mProgramID, mGeometryShader );
			}

			if( strstr( mShaderSource, "FRAGMENT_SHADER" ) != NULL )
			{
				const char *fshader[2] = { "#define VERSION_GL\n#define FRAGMENT_SHADER\n\0", mShaderSource };

				if( !Compile( fshader, mFragmentShader, GL_FRAGMENT_SHADER, 2 ))
					return S_FALSE;

				glAttachShader( mProgramID, mFragmentShader );
			}
			
			TRACE( "Shader Compiled Successfully!" );

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

				// Quad Texture Coordinates.
				//
				case 'x':
					sprintf_s( name, "aTexture%d", texCount );
					SetOffset( attribIndex, mVertexSize, 16, GL_FLOAT );
					++texCount;
					break;

				// Vertex Matrix.
				//
				case 'M':
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

			// Create texture samplers.
			//
			if( texCount > 0 )
			{
				mNumSamplers = texCount;

				mSampler = new Sampler*[ texCount ];

				for( UINT i = 0; i < texCount; ++i )
					mSampler[ i ] = new SamplerGL();
			}
		
			// Link the shaders.
			//
			int didCompile;

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

			ApplyPass();
		
			ProcessUniforms();

			TRACE( "Shader Created Sucessfully!" );

			return S_OK;
		}

	private:

		int Compile( const GLchar** source, GLuint& shader, GLenum type, GLsizei count )
		{
			_ASSERT( type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER || type == GL_GEOMETRY_SHADER );

			shader = glCreateShader( type );

			glShaderSource( shader, count, source, NULL );
			glCompileShader( shader );

			int didCompile = 0;
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

				TRACE( "\nShader Compile Log" );
				TRACE( compileLog );

				free( compileLog );
				
				glDeleteShader( shader );

				return 0;
			}

			return 1;
		}

		void CreateUniform( const char* name )
		{
			GLuint id = glGetUniformLocation( mProgramID, name );

			if( id == -1 )
				TRACE( "Shader Warning: Uniform '" << name << "' does not match within file." );
			
			mUniformGL.push_back( id );
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
				mProgramID = 0;

				glDeleteShader( mVertexShader );
				mVertexShader = 0;

				glDeleteShader( mGeometryShader );
				mGeometryShader = 0;

				glDeleteShader( mFragmentShader );
				mFragmentShader = 0;
			}
		}

		void ApplyPass()
		{
			glUseProgram( mProgramID );

			mTextureLevel = 0;
		}

		void ApplyLayout()
		{
			ApplyLayout( mVertexSize );
		}

		void ApplyLayout( UINT stride )
		{
			for( UINT i = 0; i < mAttributeSize; ++i )
				glVertexAttribPointer( i, mAttributeGL[ i ].mOffsetSize, mAttributeGL[ i ].mType, mAttributeGL[ i ].mNormalize, stride, \
									   reinterpret_cast< const GLvoid* >( mAttributeGL[ i ].mOffset ));
		}

		void SetFloat( UINT uniform, float data )
		{
			glUniform1f( mUniformGL[ uniform ], data );
		}

		void SetFloat2( UINT uniform, float* data )
		{
			glUniform2fv( mUniformGL[ uniform ], 1, data );
		}

		void SetFloat3( UINT uniform, float* data )
		{
			glUniform3fv( mUniformGL[ uniform ], 1, data );
		}

		void SetFloat4( UINT uniform, float* data )
		{
			glUniform4fv( mUniformGL[ uniform ], 1, data );
		}

		void SetMatrix( UINT uniform, float* matrix )
		{
			glUniformMatrix4fv( mUniformGL[ uniform ], 1, false, matrix );
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

			static_cast< SamplerGL* >(mSampler[ mTextureLevel ])->Apply();
			
			++mTextureLevel;
		}

		void SetSampler( UINT index, SamplerMode modeU, SamplerMode modeV, 
						 SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter )
		{
			_ASSERT( index < mNumSamplers );

			static_cast< SamplerGL* >(mSampler[ index ])->Create( modeU, modeV, minFilter, magFilter, mipFilter );
		}
	};

	////////////////////////////////////////////////////////////////////////////////////
	// OpenGL 4.1 Renderer.
	//
	class RendererGL : public Renderer
	{
	private:

		static const UINT PRIMITIVE[ NUM_PRIMITIVES ];
		static const UINT CULL_TYPE[ NUM_CULL_TYPES ];
		static const UINT FILL_TYPE[ NUM_FILL_TYPES ];

		class RenderTarget
		{
		public:

			std::shared_ptr< Texture >	mTexture;
			GLuint		mID;
			
			RenderTarget( const std::shared_ptr< Texture >& texture, GLuint id )
			{
				mTexture = texture;
				mID		 = id;
			}
		};

		class WindowInfoGL : public WindowInfo
		{
			friend class RendererGL;
			
		public:

			HDC			mHDC;
			HGLRC		mContext;

			GLenum		mRenderMode;
		};

		WindowInfoGL*					mCurrWindow;

		std::vector< RenderTarget >		mRenderTarget;
		std::vector< GLuint >			mDepthStencil;
		
		static const UINT IMAGE_FORMAT[ NUM_IMAGE_FORMATS ];

		BufferGL*						mCurrVBO;

	public:
		
		RendererGL()
		{
			NULL_TEXTURE = NULL;
			BASE_TEXTURE = NULL;

			mDepthStencil.push_back( 0 );

			mCurrShader = NULL;
			mCurrWindow = NULL;

			mCurrVBO    = NULL;
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
				NULL_TEXTURE = std::shared_ptr< Texture >( new TextureGL( 0, 0, 0 ));

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
		Buffer* CreateBuffer( void* data, UINT size, UINT stride, BufferType type, BufferAccess access )
		{
			_ASSERT( type == VERTEX_BUFFER || type == INDEX_BUFFER );

			BufferGL* buffer = new BufferGL();
			buffer->mAccess  = access;
			buffer->mType	 = type;
			buffer->mSize	 = size;
			buffer->mStride  = stride;
			buffer->mCount	 = size / stride;

			UINT bufferType;
			(type == VERTEX_BUFFER) ? bufferType = GL_ARRAY_BUFFER : bufferType = GL_ELEMENT_ARRAY_BUFFER;
			
			glGenBuffers( 1, &buffer->mID );
			glBindBuffer( bufferType, buffer->mID );
			glBufferData( bufferType, size,	data, GL_DYNAMIC_DRAW );

			return buffer;
		}

		void SetVBO( Buffer* buffer )
		{
			BufferGL* b = static_cast< BufferGL* >(buffer);
			glBindBuffer( GL_ARRAY_BUFFER, b->mID );
			mCurrVBO = b;
		}

		void SetIBO( Buffer* buffer )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, static_cast< BufferGL* >(buffer)->mID );
			glEnableClientState( GL_INDEX_ARRAY );
			glIndexPointer( GL_UNSIGNED_INT, sizeof( UINT ), 0 );
		}

		// Textures.
		//
		std::shared_ptr< Texture > CreateTextureFromFile( const char* filename, bool createMips = true )
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

			std::shared_ptr< Texture > texture = CreateTextureFromMemory( pixels, width, height, IMAGE_FORMAT_RGBA, createMips );

			stbi_image_free( pixels );

			return texture;
		}

		std::shared_ptr< Texture > CreateTextureFromMemory( void* data, UINT width, UINT height, ImageFormatType format, bool createMips = true )
		{
			UINT texID;
			glGenTextures( 1, &texID );

			// Configure the texture object.
			//
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
			
			// Load the texture object from memory.
			//
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, IMAGE_FORMAT[ format ], GL_UNSIGNED_BYTE, data );

			if( createMips )
				glGenerateMipmap( GL_TEXTURE_2D );
			
			return std::shared_ptr< Texture >( new TextureGL( width, height, texID ));
		}

		void* GetTexturePixels( std::shared_ptr< Texture > texture )
		{
			TextureGL* texGL = static_cast< TextureGL* >(texture.get());

			BYTE* pixels = new BYTE[ (texGL->mWidth << 2) * texGL->mHeight ];

			glBindTexture( GL_TEXTURE_2D, texGL->mID );
			glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

			return pixels;
		}

		// Special Rendering.
		//
		UINT CreateBackbuffer()
		{
			mRenderTarget.push_back( RenderTarget( NULL_TEXTURE, 0 ));
			return mRenderTarget.size()-1;
		}

		UINT CreateRenderTarget( std::shared_ptr< Texture > texture )
		{
			UINT renderID = mRenderTarget.size();

			mRenderTarget.push_back( RenderTarget( texture, 0 ));
			
			glGenFramebuffers( 1, &mRenderTarget.back().mID );
			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mRenderTarget.back().mID );
			glFramebufferTexture( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, static_cast< TextureGL* >(texture.get())->mID, 0 );
			
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
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id );
			
			mDepthStencil.push_back( id );
			return mDepthStencil.size()-1;
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

			static_cast< ShaderGL* >(mCurrShader.get())->ApplyLayout( mCurrVBO->Stride() );
			mCurrWindow->mRenderMode = type;
		}

		void SetRenderTarget( UINT target )
		{
			_ASSERT( target < mRenderTarget.size() );

			glBindFramebuffer( GL_FRAMEBUFFER, mRenderTarget[ target ].mID );
		}

		void SetDepthStencil( UINT stencil )
		{
			_ASSERT( stencil < mDepthStencil.size() );

			glBindRenderbuffer( GL_RENDERBUFFER, mDepthStencil[ stencil ] );
		}

		void SetDepthStencilState( StencilType state )
		{
			switch( state )
			{
				case STENCIL_DEFAULT:
					glDepthFunc( GL_LEQUAL );
					glDepthMask( GL_TRUE );
					break;

				case STENCIL_NO_ZBUFFER:
					glDepthFunc( GL_ALWAYS );
					glDepthMask( GL_TRUE );
					break;

				case STENCIL_PARTICLE:
					glDepthMask( GL_FALSE );
					break;
			}
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

		void SetBlend( BlendType type )
		{
			switch( type )
			{
				case BLEND_ALPHA:
					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
					glEnable( GL_BLEND );
					break;

				case BLEND_PARTICLE:
					//glBlendFunc( GL_SRC_ALPHA, GL_ZERO );
					glEnable( GL_BLEND );
					break;

				default:
					glDisable( GL_BLEND );
					break;
			}
		}
		
		// Shaders.
		//
		std::shared_ptr< Shader > CreateShaderFromFile( const char* filename, const char* attrib, const char* uniform )
		{
			ShaderGL* shader = new ShaderGL();

			if( shader->CreateFromFile( filename, attrib, uniform ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return std::shared_ptr< Shader >( shader );
		}

		std::shared_ptr< Shader > CreateShaderFromMemory( const char* source, const char* attrib, const char* uniform )
		{
			ShaderGL* shader = new ShaderGL();

			if( shader->CreateFromMemory( const_cast< char* >(source), attrib, uniform ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return std::shared_ptr< Shader >( shader );
		}

		void SetShader( const std::shared_ptr< Shader >& shader )
		{
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

			for( UINT i = 0; i < static_cast< ShaderGL* >(mCurrShader.get())->AttributeSize(); ++i )
				glEnableVertexAttribArray( i );
			
			glDrawRangeElementsBaseVertex( PRIMITIVE[ mCurrWindow->mRenderMode ], startIndex, startIndex + count, count, GL_UNSIGNED_INT, \
										   reinterpret_cast< void* >( startIndex * sizeof( UINT )), baseVertex );
			
			for( UINT i = 0; i < static_cast< ShaderGL* >(mCurrShader.get())->AttributeSize(); ++i )
				glDisableVertexAttribArray( i );
				
			glFlush();
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

	const UINT RendererGL::IMAGE_FORMAT[] =
		{ GL_R, 
		  GL_RGB,
		  GL_RGBA,
		  GL_RGBA32F };

	Renderer* BuildRendererGL()
	{
		TRACE( "Creating OpenGL 4.1 Renderer..." );

		return new RendererGL();
	}
}