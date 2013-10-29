using System;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	public class ATexture : EditorAsset
	{
		///
		/// Initialize this image to get an icon
		/// before the name in the TreeView.
		///
		public static BitmapImage DefaultImage;

		public WTexture Data;

		public ATexture( String name, WTexture data )
		{
			mType	= EditorAssetType.TEXTURE;

			Data	= data;
			Name	= name;

			Header  = EditorAsset.CreateDefaultAssetPanel( name, DefaultImage );
		}
	}
}
