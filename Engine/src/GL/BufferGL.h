#include "CommonGL.h"
#include "Buffer.h"

namespace Sentinel
{
	class BufferGL : public Buffer
	{
	private:

		GLuint	mID;

	public:

		BufferGL( void* data, UINT size, UINT stride, BufferType type, BufferAccess access );
		~BufferGL();

		void	Release();

		GLuint	ID();

		//////////////////////////////////

		void*	Lock();
		void	Unlock();
	};
}
