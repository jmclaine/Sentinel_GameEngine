#include "BufferGL.h"
#include "Renderer.h"

namespace Sentinel
{
	BufferGL::BufferGL( Renderer* renderer, void* data, UINT size, UINT stride, BufferFormat::Type format, BufferAccess::Type access )
	{
		mRenderer = renderer;
		mAccess   = access;
		mFormat	  = format;
		mSize	  = size;
		mStride   = stride;
		mCount	  = size / stride;

		glGenBuffers( 1, &mID );

		if( format == BufferFormat::VERTEX )
		{
			renderer->SetVertexBuffer( this );
			glBufferData( GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW );
		}
		else
		{
			renderer->SetIndexBuffer( this );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW );
			glIndexPointer( GL_UNSIGNED_INT, sizeof( UINT ), 0 );
		}
	}

	BufferGL::~BufferGL()
	{
		Release();
	}

	void BufferGL::Release()
	{
		glDeleteBuffers( 1, &mID );
		mID = 0;
	}

	GLuint BufferGL::ID()
	{
		return mID;
	}

	//////////////////////////////////

	void* BufferGL::Lock()
	{
		if( mFormat == BufferFormat::VERTEX )
		{
			mRenderer->SetVertexBuffer( this );

			return glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
		}
		else
		{
			mRenderer->SetIndexBuffer( this );
			
			return glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY );
		}
	}

	void BufferGL::Unlock()
	{
		(mFormat == BufferFormat::VERTEX) ? glUnmapBuffer( GL_ARRAY_BUFFER ) : glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
	}
}
