﻿using System;
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
	public class IGameObject : Inspector
	{
		public WGameObject Data { get; set; }
		
		public IGameObject( WGameObject obj )
		{
			Header	= obj.Name;
			Data	= obj;

			WTransformComponent transform = WTransformComponent.Cast( obj.FindComponent( ComponentType.TRANSFORM ));

			if( transform != null )
				Items.Add( new ITransformComponent( transform ));

			//////////////////////////////

			WGameComponent drawable = obj.FindComponent( ComponentType.DRAWABLE );

			if( drawable != null )
			{
				WMeshComponent mesh = WMeshComponent.Cast( drawable );

				if( mesh != null )
				{
					Items.Add( new IMeshComponent( mesh ));
				}
				else
				{
					WModelComponent model = WModelComponent.Cast( drawable );

					if( model != null )
						Items.Add( new IModelComponent( model ));
				}
			}

			//////////////////////////////

			Separator separator = new Separator();
			separator.Focusable = false;
			Items.Add( separator );

			Button button = new Button();
			button.Content = "Add Component";
			button.HorizontalAlignment		  = HorizontalAlignment.Center;
			button.MinWidth					  = 300;
			button.HorizontalContentAlignment = HorizontalAlignment.Center;
			button.VerticalContentAlignment   = VerticalAlignment.Center;
			button.Click += AddComponent;

			Items.Add( button );

			//////////////////////////////

			ExpandSubtree();
		}

		private void AddComponent( Object sender, RoutedEventArgs e )
		{
			MessageBox.Show( "Attempting to AddComponent..." );
		}
	}
}