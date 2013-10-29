using System;
using System.Globalization;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	public class Inspector : TreeViewItem
	{
		public delegate void ChangedValueDelegate();
		public ChangedValueDelegate ChangedValue;

		///
		/// Must be initialized before creating a new object.
		///
		public static Style TreeStyle { get; set; }
		public static Style TextStyle { get; set; }

		public Inspector()
		{
			Style = TreeStyle;
		}

		public Inspector( Object obj )
		{
			Style  = TreeStyle;
			Header = obj;
		}

		protected Label CreateLabel( String name )
		{
			Label label = new Label();

			label.Content = name;
			label.Style   = TextStyle;

			return label;
		}
	}
}
