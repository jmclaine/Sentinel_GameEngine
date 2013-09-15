using System;
using System.Windows.Controls;

using Sentinel.Assets;

namespace Sentinel_Editor
{
	public class ATexture : EditorAsset
	{
		public WTexture Data { get; set; }

		public ATexture( WTexture texture, String name )
		{
			mType	= EditorAssetType.TEXTURE;

			Data	= texture;
			Name	= name;
		}
	}
}
