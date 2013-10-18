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
		public static BitmapImage DefaultImage { get; set; }

		public WTexture Data { get; private set; }

		public ATexture( String name, WTexture data )
		{
			mType	= EditorAssetType.TEXTURE;

			Data	= data;
			Name	= name;

			Header  = EditorAsset.CreateDefaultAssetPanel( name, DefaultImage );
		}
	}
}
