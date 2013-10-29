using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;
using System.ComponentModel;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	class IModelComponent : Inspector
	{
		public WModelComponent Data { get; set; }

		private IString mName;

		public IModelComponent( WModelComponent component )
		{
			Header	= "Model";
			Data	= component;

			mName = new IString( "Name: ", Data.Name );
			mName.ChangedValue = ChangedName;
			Items.Add( mName );
		}

		private void ChangedName()
		{
			Data.Name = mName.Data;
		}
	}
}
