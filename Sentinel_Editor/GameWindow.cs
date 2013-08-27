using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Input;

using Sentinel.Systems;
using Sentinel.Components;
using Sentinel.Math;

namespace Sentinel_Editor
{
	public class GameWindow : WGameWindow
	{
		Point		mCurrentMousePos;
		Point		mLastMousePos;

		bool		mIsMoving;
		float		mForwardMovement;
		float		mAngularSpeed;
		WVector3f	mRotation;
		
		///////////////////////////////

		public GameWindow()
		{
			mIsMoving			= false;
			mForwardMovement	= 0.0f;
			mAngularSpeed		= 1.0f;
		}

		public void SetRotation( WVector3f rot )
		{
			mRotation = rot;
		}
		
		public override void Update()
		{
			WPerspectiveCameraComponent camera = new WPerspectiveCameraComponent( WGameWorld.GetCamera( 0 ));
			camera.Set( (float)GetInfo().Width(), (float)GetInfo().Height() );

			WTransformComponent transform = camera.GetTransform();

			if( mIsMoving )
			{
				// Rotate.
				//
				WVector3f diff = new WVector3f( (float)(mLastMousePos.Y-mCurrentMousePos.Y), (float)(mLastMousePos.X-mCurrentMousePos.X), 0.0f );
				
				if( diff.LengthSquared() > 0 )
				{
					mRotation += diff * mAngularSpeed;
	
					transform.Orientation().Set( new WQuatf( mRotation.X(), mRotation.Y(), 0 ));
				}

				WMouse.SetPosition( mLastMousePos );
			}

			// Move.
			//
			if( mForwardMovement != 0 )
			{
				WMatrix4f rot = new WMatrix4f( transform.Orientation() );
				transform.Position().Set( transform.Position() + mForwardMovement * rot.Forward() );

				mForwardMovement = 0;
			}

			WGameWorld.UpdateDrawable();
		}

		///
		/// Window for the World View 
		///
		protected override void OnLeftMouseDown()
		{}

		protected override void OnRightMouseDown()
		{}

		protected override void OnMiddleMouseDown()
		{
			mIsMoving = true;

			OnMouseMove();
			mLastMousePos = new Point( mCurrentMousePos.X, mCurrentMousePos.Y );
		}

		protected override void OnLeftMouseUp()
		{}

		protected override void OnRightMouseUp()
		{}

		protected override void OnMiddleMouseUp()
		{
			mIsMoving = false;
		}

		protected override void OnLeftMouseDoubleClick()
		{}

		protected override void OnRightMouseDoubleClick()
		{}

		protected override void OnMiddleMouseDoubleClick()
		{
			mIsMoving = false;
		}

		protected override void OnMouseScroll()
		{
			mForwardMovement = (float)WMouse.ScrollDistance() * 0.1f;
		}

		protected override void OnMouseMove()
		{
			if( mIsMoving )
			{
				Point mousePos = (Point)WMouse.GetPosition( IntPtr.Zero );
				mCurrentMousePos = new Point( mousePos.X, mousePos.Y );
			}
		}
	}
}
