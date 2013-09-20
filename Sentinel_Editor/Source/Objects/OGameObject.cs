using System.Windows.Controls;

using Sentinel.Components;

namespace Sentinel_Editor
{
	class OGameObject : TreeViewItem
	{
		public WGameObject Data { get; set; }
		
		public OGameObject( WGameObject obj )
		{
			Header	= obj.Name;
			Data	= obj;
		}
	}
}
