#pragma once

#include <unordered_map>
#include "CommonDX.h"
#include "Shader.h"

namespace Sentinel
{
	class ShaderDX : public Shader
	{
	private:

		enum ShaderType
		{
			VERTEX_SHADER,
			GEOMETRY_SHADER,
			PIXEL_SHADER,
		};

	public:

		class SamplerDX : public Sampler
		{
		private:

			ID3D11DeviceContext* mContext;

		public:

			ID3D11SamplerState* mSampler;

			UINT mStartSlot;

		public:

			SamplerDX();
			~SamplerDX();

			bool Create(
				ID3D11Device* device, ID3D11DeviceContext* context,
				SamplerMode::Type modeU, SamplerMode::Type modeV,
				SamplerFilter::Type minFilter,
				SamplerFilter::Type magFilter,
				SamplerFilter::Type mipFilter);

			void Apply();
		};

	private:

		ID3D11Device* mDevice;
		ID3D11DeviceContext* mContext;

		ID3D11VertexShader* mVertexShader;
		ID3D11GeometryShader* mGeometryShader;
		ID3D11PixelShader* mPixelShader;

		ID3D11Buffer* mConstantBuffer;
		BYTE* mConstantBufferData;

		std::vector<UINT> mUniformsDX; // stores offsets
		std::unordered_map<UINT, SamplerDX*> mSamplers;

		UINT mTextureLevel;

	public:

		ShaderDX(ID3D11Device* device, ID3D11DeviceContext* context);
		~ShaderDX();

		void Release();

	private:

		bool CreateUniform(char* name, ID3D11ShaderReflectionConstantBuffer* reflect);

	public:

		// Use the filename without the extension.
		// DirectX uses .fx files.
		//
		UINT CreateFromFile(std::string filename);
		UINT CreateFromMemory(char* source);

	private:

		ID3D10Blob* Compile(ShaderType type);

	public:

		void SetFloat(UINT uniform, float* data, UINT count = 1);
		void SetFloat2(UINT uniform, float* data, UINT count = 1);
		void SetFloat3(UINT uniform, float* data, UINT count = 1);
		void SetFloat4(UINT uniform, float* data, UINT count = 1);
		void SetMatrix(UINT uniform, float* data, UINT count = 1);
		void SetTexture(UINT uniform, Texture* texture);
		void SetTextureCube(UINT uniform, Texture* texture);

		void SetSampler(
			UINT index,
			SamplerMode::Type modeU, SamplerMode::Type modeV,
			SamplerFilter::Type minFilter, SamplerFilter::Type magFilter, SamplerFilter::Type mipFilter);

		void SetSamplerCube(
			UINT index,
			SamplerMode::Type modeU, SamplerMode::Type modeV, SamplerMode::Type modeW,
			SamplerFilter::Type minFilter, SamplerFilter::Type magFilter, SamplerFilter::Type mipFilter);

		///////////////////////////////////

		void Enable();
		void Disable();
	};
}
