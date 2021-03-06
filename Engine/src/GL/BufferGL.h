#include "CommonGL.h"
#include "Buffer.h"

namespace Sentinel
{
	class Renderer;

	class BufferGL : public Buffer
	{
	private:

		Renderer* mRenderer;
		GLuint mID;

	public:

		BufferGL(Renderer* renderer, void* data, UINT size, UINT stride, BufferFormat format, BufferAccess access);
		~BufferGL();

		void Release();

		GLuint ID();

		//////////////////////////////////

		void* Lock();
		void Unlock();
	};
}
