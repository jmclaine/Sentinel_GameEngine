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
	class GameObject_TreeViewItem : TreeViewItem
	{
		public WGameObject mObject;
		
		public GameObject_TreeViewItem( WGameObject obj )
		{
			Header		= obj.Name;
			mObject		= obj;
		}
	}
}
