#pragma once

#include "CommonDX.h"
#include "Buffer.h"

namespace Sentinel
{
	////////////////////////////////////////////////////////////////////////////////////
	// The DirectX 11 Buffer cannot be read for saving of data.
	// The reason is that the Buffer must be created as a DYNAMIC resource. The solution
	// is to store and modify the buffer data using the CPU, and then transfer that
	// data over to the DYNAMIC buffer.
	//
	class BufferDX : public Buffer
	{
	private:

		void*					mData;

		ID3D11DeviceContext*	mContext;

	public:

		ID3D11Buffer*			mBuffer;

		///////////////////////////////////

		BufferDX( ID3D11Device* device, ID3D11DeviceContext* context, void* data, UINT size, UINT stride, BufferType type, BufferAccess access );
		~BufferDX();

		void	Release();

		///////////////////////////////////

		void*	Lock();
		void	Unlock();
	};
}
