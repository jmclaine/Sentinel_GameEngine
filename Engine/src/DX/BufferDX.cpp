#include "BufferDX.h"
#include "Util.h"

namespace Sentinel
{
	BufferDX::BufferDX( ID3D11Device* device, ID3D11DeviceContext* context, void* data, UINT size, UINT stride, BufferType type, BufferAccessType access )
	{
		_ASSERT( device );
		_ASSERT( context );
		_ASSERT( type == VERTEX_BUFFER || type == INDEX_BUFFER );
		_ASSERT( size > 0 );
		_ASSERT( stride > 0 );

		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage				= D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth			= size;
		bufferDesc.BindFlags			= (type == VERTEX_BUFFER) ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags			= 0;
		bufferDesc.StructureByteStride	= 0;

		mAccess  = access;
		mContext = context;
		mType	 = type;
		mSize	 = size;
		mStride  = stride;
		mCount	 = size / stride;

		D3D11_SUBRESOURCE_DATA resourceData;

		if( access == BUFFER_READ_WRITE )
		{
			mData = malloc( size );
			memcpy( mData, data, size );

			resourceData.pSysMem = mData;
		}
		else
		{
			resourceData.pSysMem = data;
		}

		resourceData.SysMemPitch		= 0;
		resourceData.SysMemSlicePitch	= 0;
			
		if( device->CreateBuffer( &bufferDesc, &resourceData, &mBuffer ) == S_FALSE )
		{
			Release();
				
			throw AppException( "Failed to create BufferDX" );
		}
	}

	BufferDX::~BufferDX()
	{
		Release();
	}

	void BufferDX::Release()
	{
		free( mData );
		SAFE_RELEASE_PTR( mBuffer );
		mStride = 0;
	}

	///////////////////////////////////

	void* BufferDX::Lock()
	{
		if( mAccess == BUFFER_READ_WRITE )
		{
			return mData;
		}
		else
		//if( mAccess == Buffer::WRITE )
		{
			D3D11_MAPPED_SUBRESOURCE mapRes;
				
			mContext->Map( mBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapRes );
			
			return mapRes.pData;
		}
	}

	void BufferDX::Unlock()
	{
		if( mAccess == BUFFER_READ_WRITE )
		{
			D3D11_MAPPED_SUBRESOURCE mapRes;
				
			mContext->Map( mBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapRes );

			memcpy( mapRes.pData, mData, mSize );
		}
			
		mContext->Unmap( mBuffer, 0 );
	}
}
