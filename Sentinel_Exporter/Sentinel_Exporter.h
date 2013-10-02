#pragma once
/*
Supports keyframes and skinning animations.
*/
#include <stdio.h>

#include "FloatCompressor.h"
#include "Archive.h"

using namespace Sentinel;


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
	
	// Store object data for future writing.
	//
	struct Weight
	{
		int			mBone;
		float		mAmount;
	};

	struct Vertex
	{
		Point3		mPosition;
		Weight		mWeight[ 4 ];
		int			mNumBones;
	};

	std::vector< Vertex >			mVertex;
	std::vector< Point3 >			mNormal;
	std::vector< Point2 >			mTexCoord;
	std::vector< Point3 >			mColor;
	std::vector< IGameMaterial* >	mMaterial;

	// Stores the textures with what type they are.
	//
	struct Texture
	{
		std::string		mFilename;
		int				mType;

		Texture( const std::string& filename, int type )
		{
			this->mFilename = filename;
			this->mType		= type;
		}
	};

	// Store each face as an index to the object data.
	//
	struct FatIndex
	{
		int mVertex, mNormal, mTexCoord;
	};

	struct FatIndexWORD
	{
		WORD mVertex, mNormal, mTexCoord;
	};

	// Standard keyframe animation.
	//
	struct KeyFrame
	{
		GMatrix		mMatrixWorld;
		int			mFrame;

		KeyFrame( const GMatrix& _matWorld, int _frame )
		{
			mMatrixWorld = _matWorld;
			mFrame		 = _frame;
		}
	};

	// Store each object as a set of FatIndex for each material it contains.
	//
	struct Object
	{
		IGameNode*							  mNode;
		std::vector< int >					  mMaterialID;
		std::vector< std::vector< FatIndex >> mFatIndex;
		std::vector< KeyFrame >				  mKeyFrame;
		int									  mHierarchy;
		BYTE								  mIsSkinned;

		Object()
		{
			mHierarchy = 0;
			mIsSkinned = 0;
		}
	};

	std::vector< Object >	mObject;

	// Are the mVertex weighted for bones?
	//
	bool					mIsWeighted;

	////////////////////////////////////////////////////

	void deinit();

	MaxExporter( const MaxExporter& );
	MaxExporter& operator = ( const MaxExporter& );

	void WriteFloat( Archive& archive, IGameProperty* prop );
	void WritePoint3( Archive& archive, IGameProperty* prop, bool is32bit = false );

	void WritePoint2( Archive& archive, const Point2& point, bool is32bit = false );
	void WritePoint3( Archive& archive, const Point3& point, bool is32bit = false );
	
	void WriteFatIndex( Archive& archive, FatIndex& index, bool is32bit = false );
	void WriteMatrix( Archive& archive, GMatrix& mat );

	void WriteMaterial( Archive& archive, IGameMaterial* material );

	// Find an object within a vector.
	//
	template< typename OBJ  >
	int FindObject( const std::vector< OBJ >& obj, const OBJ& objCompare )
	{
		for( UINT x = 0; x < obj.size(); ++x )
			if( obj[ x ] == objCompare )
				return (int)x;	// found object

		return -1;	// failed to find
	};

	// Find a specific vertex.
	//
	int FindVertex( const Point3& pos )
	{
		for( UINT x = 0; x < mVertex.size(); ++x )
			if( mVertex[ x ].mPosition == pos )
				return (int)x;	// found object

		return -1;	// failed to find
	}

	// Find a specific bone.
	//
	int FindBone( const IGameNode* bone )
	{
		for( UINT x = 0; x < mObject.size(); ++x )
			if( mObject[ x ].mNode == bone )
				return (int)x;	// found object

		return -1;	// failed to find
	}

	// Create an object one face at a time.
	//
	void CreateFatIndex( IGameMesh* mesh, FaceEx* face, IGameSkin* skin, std::vector< FatIndex >& vFatIndex );

	// Get mesh data for a non-material node.
	//
	void GetNoMaterialMesh( IGameMesh* mesh, IGameSkin* skin, int currObjectIndex );

	// Get mesh data for a node.
	//
	void GetNodeMesh( IGameNode* node, int heirarchy = 0, bool doParentWT = true  );

	struct Point2S
	{
		short x, y;
	};

	struct Point3S
	{
		short x, y, z;
	};
};
