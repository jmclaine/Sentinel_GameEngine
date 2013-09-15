using System.Windows.Controls;

using Sentinel.Components;

namespace Sentinel_Editor
{
	class OGameObject : TreeViewItem
	{
		public WGameObject mData;
		
		public OGameObject( WGameObject obj )
		{
			Header	= obj.Name;
			mData	= obj;
		}
	}
}
