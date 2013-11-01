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
		public static BitmapImage DefaultImage;

		public WSound Data;

		public AAudio( String name, WSound data )
		{
			mType	= EditorAssetType.AUDIO_SOURCE;

			Data	= data;
			Name	= name;

			Header  = EditorAsset.CreatePanel( name, DefaultImage );
		}
	}
}
