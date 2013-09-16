using System;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Sentinel.Assets;

namespace Sentinel_Editor
{
	public class ATexture : EditorAsset
	{
		///
		/// Initialize this image to get an icon
		/// before the name in the TreeView.
		///
		public static BitmapImage DefaultImage { get; set; }

		public WTexture Data { get; set; }

		public ATexture( WTexture texture, String name )
		{
			mType	= EditorAssetType.TEXTURE;

			Data	= texture;
			Name	= name;

			Header  = EditorAsset.CreateDefaultAssetPanel( name, DefaultImage );
		}
	}
}
