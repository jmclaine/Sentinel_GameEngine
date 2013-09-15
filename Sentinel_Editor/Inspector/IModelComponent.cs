using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;
using System.ComponentModel;

using Sentinel.Components;

namespace Sentinel_Editor
{
	class IModelComponent : Inspector
	{
		public WModelComponent mData;

		public IModelComponent( WModelComponent component )
		{
			Header	= "Model";
			mData	= component;

			Grid grid = Inspector.CreateEdit( "Name: ", mData.Name );
			Items.Add( grid );
		}
	}
}
