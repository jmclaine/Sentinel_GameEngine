using System;
using System.Windows.Controls;

using Sentinel.Assets;

namespace Sentinel_Editor
{
	public class AMesh : EditorAsset
	{
		public WMesh mData;

		public AMesh( WMesh mesh, String name )
		{
			mType	= EditorAssetType.MESH;

			mData	= mesh;
			Name	= name;
		}
	}
}
