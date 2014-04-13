#pragma once

#include "EnumClass.h"

namespace Sentinel
{
	ENUM_CLASS( PrimitiveFormat )

		POINTS,
		LINES,
		TRIANGLES,

		COUNT

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( ImageFormat )

		R,
		RG,
		RGB,
		RGBA,
		HDR,	// R32G32B32F
		DEPTH,

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( CullFormat )

		NONE,
		CCW,
		CW,

		COUNT

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( FillFormat )
		
		SOLID,
		WIREFRAME,

		COUNT
	
	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( BlendFormat )
	
		ZERO,
		ONE,

		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,

		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,

		COUNT,
		UNKNOWN

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( BlendFunction )
	
		ADD,
		SUBTRACT,
		REVERSE_SUBTRACT,
		MIN,
		MAX,

		COUNT,
		UNKNOWN

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( DepthFormat )

		OFF,
		LESS,
		EQUAL,
		LEQUAL,
		GREATER,
		NOTEQUAL,
		GEQUAL,
		ALWAYS,

		COUNT,
		UNKNOWN

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( BufferFormat )

		UNKNOWN,

		VERTEX,
		INDEX,

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( BufferAccess )
	
		READ_WRITE,
		WRITE,

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( TextureIndex )

		DIFFUSE,
		NORMAL,
		DEPTH,

		COUNT

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( VertexAttribute )
	
		POSITION,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		QUADCOORD0,
		QUADCOORD1,
		QUADCOORD2,
		NORMAL,
		COLOR,
		TANGENT,
		BONE_COUNT,
		BONE_INDEX,
		BONE_WEIGHT,
		MATRIX,

		COUNT

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( ShaderUniform )

		WVP,
		WORLD,
		INV_WORLD,
		VIEW,
		INV_VIEW,
		PROJ,
		INV_PROJ,
		TEXTURE,
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		SPEC_COMP,
		LIGHT_POS,
		LIGHT_DIR,
		LIGHT_COLOR,
		LIGHT_ATTN,
		LIGHT_TEXTURE_CUBE,
		LIGHT_MATRIX,
		LIGHT_CUBE_MATRIX,
		SHADOW_BLEND,
		SHADOW_QUALITY,
		CAMERA_POS,
		BONES,
		DELTA_TIME,
	
	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( RenderQueue )

		BACKGROUND		= 10000,
		GEOMETRY		= 20000,
		ALPHA_BLEND		= 30000,
		FOREGROUND		= 40000,
	
	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( SamplerMode )

		WRAP,
		CLAMP,
		CLAMP_TO_EDGE,

		COUNT,
		UNKNOWN

	ENUM_END

	/////////////////////////////////

	ENUM_CLASS( SamplerFilter )

		POINT,
		LINEAR,

		UNKNOWN
	
	ENUM_END
}
