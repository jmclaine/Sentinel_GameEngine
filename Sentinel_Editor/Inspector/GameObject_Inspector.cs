using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;
using System.ComponentModel;
using System.Collections.ObjectModel;

using Sentinel.Components;

namespace Sentinel_Editor
{
	class GameObject_Inspector : Inspector_TreeViewItem
	{
		public WGameObject mData;
		
		public GameObject_Inspector( WGameObject obj )
		{
			Header	= obj.Name;
			mData	= obj;

			WTransformComponent transform = WTransformComponent.Cast( obj.FindComponent( ComponentType.TRANSFORM ));

			if( transform != null )
				Items.Add( new TransformComponent_Inspector( transform ));

			ExpandSubtree();
		}
	}
}
