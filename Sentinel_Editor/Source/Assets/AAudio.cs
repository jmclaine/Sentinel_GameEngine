using System;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	public class AAudio : EditorAsset
	{
		///
		/// Initialize this image to get an icon
		/// before the name in the TreeView.
		///
		public static BitmapImage DefaultImage { get; set; }

		public WAudioSource Data { get; set; }

		public AAudio( String name, WAudioSource data )
		{
			mType	= EditorAssetType.AUDIO_SOURCE;

			Data	= data;
			Name	= name;

			Header  = EditorAsset.CreateDefaultAssetPanel( name, DefaultImage );
		}
	}
}
