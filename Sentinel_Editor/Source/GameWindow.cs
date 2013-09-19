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
		private WGameObject						mObject;
		private WPerspectiveCameraComponent		mCamera;
		private WTransformComponent				mTransform;

		private Point							mCurrentMousePos;
		private Point							mLastMousePos;

		private bool							mIsMoving;

		private float							mMovementDirection;	// forward (+) or backward (-)
		private float							mMovementSpeed;

		private float							mAngularSpeed;
		
		///////////////////////////////

		public GameWindow()
		{
			mIsMoving			= false;
			mMovementDirection	= 0.0f;
			mMovementSpeed		= 0.1f;
			mAngularSpeed		= 1.0f;
		}

		public override void Startup( String title, String windowClass, WWindowInfo info )
		{
			base.Startup( title, windowClass, info );

			// Create default viewing camera.
			//
			mObject = new WGameObject();
			mObject.Name = "~Camera~";

			mTransform	= (WTransformComponent)mObject.AttachComponent( new WTransformComponent(), "Transform" );
			mCamera		= (WPerspectiveCameraComponent)mObject.AttachComponent( new WPerspectiveCameraComponent( (float)GetInfo().Width(), (float)GetInfo().Height() ), "Camera" );
			
			mObject.Startup();

			WGameWorld.SetCamera( mCamera );
		}

		public override void Update()
		{
			base.Update();

			mCamera.Set( (float)GetInfo().Width(), (float)GetInfo().Height() );

			if( mIsMoving )
			{
				// Rotate.
				//
				WVector3f diff = new WVector3f( (float)(mLastMousePos.Y-mCurrentMousePos.Y), (float)(mLastMousePos.X-mCurrentMousePos.X), 0.0f );
				
				if( diff.LengthSquared() > 0 )
				{
					mTransform.Rotation += diff * mAngularSpeed;
				}

				WMouse.SetPosition( mLastMousePos );
			}

			// Move.
			//
			if( mMovementDirection != 0 )
			{
				mTransform.Position = mTransform.Position + mMovementDirection * new WMatrix4f( new WQuatf( mTransform.Rotation )).Forward();

				mMovementDirection = 0;
			}

			mCamera.Update();

			WGameWorld.UpdateTransform();
			WGameWorld.UpdateDrawable();
		}

		public override void Shutdown()
		{
			base.Shutdown();

			mObject.Shutdown();
			mObject.Delete();
		}

		public WCameraComponent GetCamera()
		{
			return mCamera;
		}

		public void SetCameraPosition( WVector3f pos )
		{
			mTransform.Position = pos;
		}

		public void SetCameraRotation( WVector3f rot )
		{
			mTransform.Rotation = rot;
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
			mMovementDirection = (float)WMouse.ScrollDistance() * mMovementSpeed;
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
