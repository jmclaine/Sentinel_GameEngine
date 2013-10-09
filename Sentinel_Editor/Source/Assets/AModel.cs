using System;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Sentinel.Assets;

namespace Sentinel_Editor
{
	public class AModel : EditorAsset
	{
		///
		/// Initialize this image to get an icon
		/// before the name in the TreeView.
		///
		public static BitmapImage DefaultImage { get; set; }

		public WModel Data { get; set; }

		public AModel( String name, WModel mesh )
		{
			mType	= EditorAssetType.MODEL;

			Data	= mesh;
			Name	= name;

			Header  = EditorAsset.CreateDefaultAssetPanel( name, DefaultImage );
		}
	}
}
