using System;
using System.Windows.Controls;

using Sentinel.Assets;

namespace Sentinel_Editor
{
	public class AShader : EditorAsset
	{
		public WShader mData;

		public AShader( WShader shader, String name )
		{
			mType	= EditorAssetType.SHADER;

			mData	= shader;
			Name	= name;
		}
	}
}
