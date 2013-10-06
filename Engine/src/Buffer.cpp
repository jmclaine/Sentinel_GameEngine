#include "Buffer.h"
#include "Renderer.h"
#include "zlib.h"

namespace Sentinel
{
	Buffer::Buffer() :
		mType( NULL_BUFFER ), mSize( 0 ), mStride( 0 ), mCount( 0 )
	{}

	Buffer::~Buffer()
	{
		mType	= NULL_BUFFER;
		mSize	= 0;
		mStride = 0;
		mCount	= 0;
	}

	//////////////////////////////

	BufferType Buffer::Type()
	{
		return mType;
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

	void Buffer::Save( Archive& archive )
	{
		BYTE type = (BYTE)mType;
		archive.Write( &type );
		archive.Write( &mSize, 1, true );
		archive.Write( &mStride );
		archive.Write( &mCount );

		BYTE* data = (BYTE*)Lock();

		ULONG bound = compressBound( mSize );
		BYTE* comp_data = (BYTE*)malloc( bound );

		compress( comp_data, &bound, data, mSize );

		archive.Write( &bound, 1, true );
		archive.Write( comp_data, bound );

		Unlock();

		free( comp_data );
	}

	Buffer* Buffer::Load( Archive& archive )
	{
		BYTE type;
		archive.Read( &type );
		
		UINT size;
		archive.Read( &size, 1, true );

		UINT stride;
		archive.Read( &stride );

		UINT count;
		archive.Read( &count );

		ULONG bound;
		archive.Read( &bound, 1, true );

		BYTE* comp_data = (BYTE*)malloc( bound );
		archive.Read( comp_data, bound );

		BYTE* data = (BYTE*)malloc( size );
		uncompress( data, (ULONG*)size, comp_data, bound );

		Buffer* buffer = Renderer::Inst()->CreateBuffer( data, size, stride, (BufferType)type );

		free( comp_data );
		free( data );

		return buffer;
	}
}
