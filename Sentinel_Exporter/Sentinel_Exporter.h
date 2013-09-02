#pragma once
/*
Supports keyframes and skinning animations.
*/

////////////////////////////////////////

struct BoundingSphere
{
	Point3	center;
	float	radius;

	BoundingSphere( const Point3& _center = Point3( 0, 0, 0 ), float _radius = 0 );
	BoundingSphere( const Point3& v0, const Point3& v1 );
	BoundingSphere( const Point3& v0, const Point3& v1, const Point3& v2 );
	BoundingSphere( const Point3& v0, const Point3& v1, const Point3& v2, const Point3& v3 );
};

// Find the area of a triangle.
//
float TriangleArea( const Point3& pos0, const Point3& pos1, const Point3& pos2 );

// Check for a point in a sphere.
//
bool CheckPointInSphere( const Point3& v, const BoundingSphere& sphere );

////////////////////////////////////////

/**
 * 3dsmax exporter plugin.
 */
class MaxExporter : public SceneExport
{
public:
	// Texture types from 3DStudio Max 2012.
	//
	enum TextureTypes
	{
		TEXTURE_DIFFUSE  = 1,
		TEXTURE_DIFFUSE2 = 2,
		TEXTURE_DIFFUSE3 = 3,
		TEXTURE_DIFFUSE4 = 4,
		TEXTURE_DIFFUSE5 = 5,
		TEXTURE_DIFFUSE6 = 6,
		TEXTURE_DIFFUSE7 = 7,
		TEXTURE_BUMP     = 8,
		TEXTURE_PARALLAX = 9,	// Refraction

		NUM_TEXTURE_TYPES,
	};

	MaxExporter();
	~MaxExporter();

	/** Exports the scene. */
	int 			DoExport( const TCHAR* name, ExpInterface *ei, Interface* i, 
							  BOOL suppressprompts = FALSE, DWORD options = 0 );

	/** Show DLL's "About..." box */
	void			ShowAbout( HWND hwnd );

	/** Number of extensions supported */
	int				ExtCount();
	
	/** Extension #n (i.e. "3DS") */
	const TCHAR*	Ext( int n );					
	
	/** Long ASCII description (i.e. "Autodesk 3D Studio File") */
	const TCHAR*	LongDesc();
	
	/** Short ASCII description (i.e. "3D Studio") */
	const TCHAR*	ShortDesc();
	
	/** ASCII Author name */
	const TCHAR*	AuthorName();
	
	/** ASCII Copyright message */
	const TCHAR*	CopyrightMessage();
	
	/** Other message #1 */
	const TCHAR*	OtherMessage1();
	
	/** Other message #2 */
	const TCHAR*	OtherMessage2();
	
	/** Version number * 100 (i.e. v3.01 = 301) */
	unsigned int	Version();
	
	/** Returns TRUE if option supported. */
	BOOL			SupportsOptions( int ext, DWORD options );

private:

	IGameScene*		mGameScene;
	FILE*			mFile;

	// Store object data for future writing.
	//
	struct Weight
	{
		int			bone;
		float		weight;
	};

	struct Vertex
	{
		Point3		pos;
		std::vector< Weight > weights;
	};

	std::vector< Vertex >			mVertex;
	std::vector< Point3 >			mNormal;
	std::vector< Point2 >			mTexCoord;
	std::vector< Point3 >			mColor;
	std::vector< IGameMaterial* >	mMaterial;

	// Store the smallest bounding sphere.
	//
	BoundingSphere mMinSphere;

	// Stores the textures with what type they are.
	//
	struct Texture
	{
		std::string filename;
		int			type;

		Texture( std::string filename, int type )
		{
			this->filename = filename;
			this->type = type;
		}
	};

	// Store each face as an index to the object data.
	//
	struct FatIndex
	{
		int vertex, normal, texCoord;
	};

	struct FatIndexWORD
	{
		WORD vertex, normal, texCoord;
	};

	// Standard keyframe animation.
	//
	struct KeyFrame
	{
		GMatrix		matWorld;
		int			frame;

		KeyFrame( const GMatrix& _matWorld, int _frame )
		{
			matWorld = _matWorld;
			frame	 = _frame;
		}
	};

	// Store each object as a set of FatIndex for each material it contains.
	//
	struct Object
	{
		IGameNode*							  node;
		std::vector< int >					  matID;
		std::vector< std::vector< FatIndex >> indices;
		std::vector< KeyFrame >				  keyframe;
		int									  hierarchy;
		BYTE								  isSkinned;

		Object()
		{
			hierarchy = 0;
			isSkinned = 0;
		}
	};

	std::vector< Object >	mObject;

	// Are the mVertex weighted for bones?
	//
	bool					mIsWeighted;

	////////////////////////////////////////////////////

	void deinit();

	MaxExporter( const MaxExporter& );
	MaxExporter& operator=( const MaxExporter& );

	void writeByte( BYTE& value );
	void writeInt( int& num, bool is32bit = false );
	void writeFloat( IGameProperty* prop );
	void writeFloat( float& num );
	void writeFloat32( float& num );
	void writeString( std::string& str );

	void writePoint2( Point2& point, bool is32bit = false );

	void writePoint3( IGameProperty* prop, bool is32bit = false  );
	void writePoint3( Point3& point, bool is32bit = false  );

	void writeFatIndex( FatIndex& index, bool is32bit = false );
	void writeMatrix( GMatrix& mat );

	void writeMaterial( IGameMaterial* material );

	// Find an object within a vector.
	//
	template< typename OBJ  >
	int findObject( std::vector< OBJ >& obj, OBJ& objCompare )
	{
		for( UINT x = 0; x < obj.size(); ++x )
		{
			if( obj[ x ] == objCompare )
			{
				return (int)x;	// found object
			}
		}
		return -1;	// failed to find
	};

	// Find a specific vertex.
	//
	int findVertex( Point3 pos )
	{
		for( UINT x = 0; x < mVertex.size(); ++x )
		{
			if( mVertex[ x ].pos == pos )
			{
				return (int)x;	// found object
			}
		}
		return -1;	// failed to find
	}

	// Find a specific bone.
	//
	int findBone( IGameNode* bone )
	{
		for( UINT x = 0; x < mObject.size(); ++x )
		{
			if( mObject[ x ].node == bone )
			{
				return (int)x;	// found object
			}
		}
		return -1;	// failed to find
	}

	// Get mesh data for a non-material node.
	//
	void getNoMaterialMesh( IGameMesh* mesh, IGameSkin* skin, int currObjectIndex );

	// Get mesh data for a node.
	//
	void getNodeMesh( IGameNode* node, int heirarchy = 0, bool doParentWT = true  );

	// Find the smallest bounding sphere.
	//
	BoundingSphere FindSmallestSphere( std::vector< Vertex >& verts, UINT size );

	// Floating point compressor.
	// Provided by Phernost @ stackoverflow.com
	//
	class FloatCompressor
	{
		union Bits
		{
			float f;
			__int32 si;
			__int32 ui;
		};

		static int const shift = 13;
		static int const shiftSign = 16;

		static __int32 const infN = 0x7F800000; // flt32 infinity
		static __int32 const maxN = 0x477FE000; // max flt16 normal as a flt32
		static __int32 const minN = 0x38800000; // min flt16 normal as a flt32
		static __int32 const signN = 0x80000000; // flt32 sign bit

		static __int32 const infC = infN >> shift;
		static __int32 const nanN = (infC + 1) << shift; // minimum flt16 nan as a flt32
		static __int32 const maxC = maxN >> shift;
		static __int32 const minC = minN >> shift;
		static __int32 const signC = signN >> shiftSign; // flt16 sign bit

		static __int32 const mulN = 0x52000000; // (1 << 23) / minN
		static __int32 const mulC = 0x33800000; // minN / (1 << (23 - shift))

		static __int32 const subC = 0x003FF; // max flt32 subnormal down shifted
		static __int32 const norC = 0x00400; // min flt32 normal down shifted

		static __int32 const maxD = infC - maxC - 1;
		static __int32 const minD = minC - subC - 1;

	public:

		static __int16 compress( float value )
		{
			Bits v, s;
			v.f = value;
			__int32 sign = v.si & signN;
			v.si ^= sign;
			sign >>= shiftSign; // logical shift
			s.si = mulN;
			s.si = (int)(s.f * v.f); // correct submNormal
			v.si ^= (s.si ^ v.si) & -(minN > v.si);
			v.si ^= (infN ^ v.si) & -((infN > v.si) & (v.si > maxN));
			v.si ^= (nanN ^ v.si) & -((nanN > v.si) & (v.si > infN));
			v.ui >>= shift; // logical shift
			v.si ^= ((v.si - maxD) ^ v.si) & -(v.si > maxC);
			v.si ^= ((v.si - minD) ^ v.si) & -(v.si > subC);
			return v.ui | sign;
		}

		static float decompress( __int16 value )
		{
			Bits v;
			v.ui = value;
			__int32 sign = v.si & signC;
			v.si ^= sign;
			sign <<= shiftSign;
			v.si ^= ((v.si + minD) ^ v.si) & -(v.si > subC);
			v.si ^= ((v.si + maxD) ^ v.si) & -(v.si > maxC);
			Bits s;
			s.si = mulC;
			s.f *= v.si;
			__int32 mask = -(norC > v.si);
			v.si <<= shift;
			v.si ^= (s.si ^ v.si) & mask;
			v.si |= sign;
			return v.f;
		}
	};

	struct Point2S
	{
		short x, y;
	};

	struct Point3S
	{
		short x, y, z;
	};
};
