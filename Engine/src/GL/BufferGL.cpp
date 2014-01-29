#include "BufferGL.h"

namespace Sentinel
{
	BufferGL::BufferGL( void* data, UINT size, UINT stride, BufferType type, BufferAccessType access )
	{
		_ASSERT( type == VERTEX_BUFFER || type == INDEX_BUFFER );

		mAccess  = access;
		mType	 = type;
		mSize	 = size;
		mStride  = stride;
		mCount	 = size / stride;

		UINT bufferType;
		(type == VERTEX_BUFFER) ? bufferType = GL_ARRAY_BUFFER : bufferType = GL_ELEMENT_ARRAY_BUFFER;
			
		glGenBuffers( 1, &mID );
		glBindBuffer( bufferType, mID );
		glBufferData( bufferType, size,	data, GL_DYNAMIC_DRAW );
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

	void BufferGL::Unlock()
	{
		(mType == VERTEX_BUFFER) ? glUnmapBuffer( GL_ARRAY_BUFFER ) : glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
	}
}
