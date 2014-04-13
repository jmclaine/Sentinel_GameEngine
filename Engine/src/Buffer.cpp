#include "Buffer.h"
#include "Archive.h"
#include "Renderer.h"
#include "zlib.h"

namespace Sentinel
{
	Buffer::Buffer() :
		mFormat( BufferFormat::UNKNOWN ), 
		mSize( 0 ), 
		mStride( 0 ), 
		mCount( 0 )
	{}

	Buffer::~Buffer()
	{
		mFormat	= BufferFormat::UNKNOWN;
		mSize	= 0;
		mStride = 0;
		mCount	= 0;
	}

	//////////////////////////////

	BufferFormat::Type Buffer::Format()
	{
		return mFormat;
	}

	UINT Buffer::Size()
	{
		return mSize;
	}

	UINT Buffer::Stride()
	{
		return mStride;
	}

	UINT Buffer::Count()
	{
		return mCount;
	}

	//////////////////////////////

	void Buffer::Save( Archive& archive, Buffer* buffer )
	{
		BYTE format = (BYTE)buffer->mFormat;
		archive.Write( &format );

		archive.Write( &buffer->mSize );
		archive.Write( &buffer->mStride );
		
		BYTE* data = (BYTE*)buffer->Lock();

		ULONG bound = compressBound( buffer->mSize );
		BYTE* comp_data = (BYTE*)malloc( bound );

		compress( comp_data, &bound, data, buffer->mSize );

		archive.Write( &bound );
		archive.Write( comp_data, bound );

		buffer->Unlock();

		free( comp_data );
	}

	Buffer* Buffer::Load( Archive& archive, Renderer* renderer )
	{
		BYTE format;
		archive.Read( &format );
		
		UINT size;
		archive.Read( &size );

		UINT stride;
		archive.Read( &stride );

		ULONG bound;
		archive.Read( &bound );

		BYTE* comp_data = (BYTE*)malloc( bound );
		archive.Read( comp_data, bound );

		BYTE* data = (BYTE*)malloc( size );
		uncompress( data, (ULONG*)(&size), comp_data, bound );

		Buffer* buffer = renderer->CreateBuffer( data, size, stride, (BufferFormat::Type)format );

		free( comp_data );
		free( data );

		return buffer;
	}
}
