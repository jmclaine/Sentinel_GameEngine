#include "CommonDX.h"
#include "VertexLayout.h"

namespace Sentinel
{
	class VertexLayoutDX : public VertexLayout
	{
	private:

		std::vector< D3D11_INPUT_ELEMENT_DESC > mInputDesc;

	public:

		ID3D11InputLayout* mInputLayout;

		////////////////////////////////////

		VertexLayoutDX();
		~VertexLayoutDX();

	private:

		void SetAttribute( LPCSTR name, DXGI_FORMAT format, UINT size, UINT index = 0 );

	public:

		void AddAttribute( VertexAttribute type );

		HRESULT Create( ID3D11Device* device );
		HRESULT Create( ID3D11Device* device, ID3D10Blob* shaderBlob );
	};
}
