#pragma once

#include "Common.h"
#include "Texture.h"

namespace Sentinel
{
	#define MAX_BONE_MATRICES		100
	#define MAX_TEXTURES			4
	#define MAX_LIGHTS				1

	class SENTINEL_DLL Shader
	{
	protected:
		
		UINT				mVertexSize;

		std::string			mAttribute;
		std::string			mUniform;

		////////////////////////////////////

		Shader();

	public:

		virtual ~Shader();

		const std::string&	AttributeDecl();
		const std::string&	UniformDecl();

		UINT  				VertexSize();

		virtual void		Release() = 0;

		virtual void		ApplyPass() = 0;
		virtual void		ApplyLayout() = 0;

		virtual void		SetFloat( UINT uniform, float data ) = 0;
		virtual void		SetFloat2( UINT uniform, float* data, UINT offset = 0, UINT count = 1 ) = 0;
		virtual void		SetFloat3( UINT uniform, float* data, UINT offset = 0, UINT count = 1 ) = 0;
		virtual void		SetFloat4( UINT uniform, float* data, UINT offset = 0, UINT count = 1 ) = 0;
		virtual void		SetMatrix( UINT uniform, float* matrix, UINT offset = 0, UINT count = 1 ) = 0;
		virtual void		SetTexture( UINT uniform, Texture* texture ) = 0;

	protected:

		virtual void		CreateUniform( const char* name ) = 0;

		void  				ProcessUniforms();
	};
}