using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Windows.Threading;
using System.ComponentModel;

using System.Runtime.InteropServices;

using Sentinel.Systems;
using Sentinel.Assets;
using Sentinel.Math;

namespace Sentinel_Editor
{
    /// <summary>
    /// Editor Window
    /// </summary>
    public partial class MainWindow : Window
    {
        private WRenderer           mRenderer;
        private static WColorRGBA   mClearColor = new WColorRGBA(0.0f, 0.2f, 0.8f, 1.0f);
        private uint                mDepthStencil;
        private uint                mViewport;
        private uint                mRenderTarget;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(Object sender, RoutedEventArgs e)
        {
            mRenderer = new WRenderer();
            
            if (!mRenderer.Load("config.xml"))
            {
                MessageBox.Show("Failed to load config.xml", "Application Failure");

                System.Environment.Exit(0);
            }

            RendererHWND0.Child = mRenderer;

            mRenderer.SetActive();

            mDepthStencil  = mRenderer.CreateDepthStencil(mRenderer.mWindowInfo.mWidth, mRenderer.mWindowInfo.mHeight);
            mViewport      = mRenderer.CreateViewport(mRenderer.mWindowInfo.mWidth, mRenderer.mWindowInfo.mHeight);
            mRenderTarget  = mRenderer.CreateBackbuffer();

            ///////////////////////////////////////

            DispatcherTimer timer = new DispatcherTimer();
            timer.Tick      += new EventHandler(Update);
            timer.Interval   = new TimeSpan(16);
            timer.Start();
        }

        private void Window_Closing(Object sender, CancelEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Save before Exiting?",
                                                      "Exit Application",
                                                      MessageBoxButton.YesNoCancel,
                                                      MessageBoxImage.Question);

            switch (result)
            {
                case MessageBoxResult.Yes:
                    MessageBox.Show("Saving...");
                    break;

                case MessageBoxResult.No:
                    MessageBox.Show("Not Saving...");
                    break;

                case MessageBoxResult.Cancel:
                    e.Cancel = true;
                    return;
            }
        }

        private void Update(Object sender, EventArgs e)
        {
            mRenderer.SetActive();

            mRenderer.SetDepthStencil(mDepthStencil);
            mRenderer.SetViewport(mViewport);
            mRenderer.SetRenderTarget(mRenderTarget);
            mRenderer.Clear(mClearColor);

            // Draw stuff...

            mRenderer.Present();
        }

        ///
        /// File
        ///
        private void Open_Click(Object sender, RoutedEventArgs e)
        {
            MessageBox.Show("You clicked 'Open...'");
        }

        private void Save_Click(Object sender, RoutedEventArgs e)
        {
            MessageBox.Show("You clicked 'Save...'");
        }

        private void SaveAs_Click(Object sender, RoutedEventArgs e)
        {
            MessageBox.Show("You clicked 'Save As...'");
        }

        private void Exit_Click(Object sender, RoutedEventArgs e)
        {
            Close();
        }

        ///
        /// Edit
        ///

        ///
        /// Help
        ///
        private void About_Click(Object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Sentinel Editor\nVersion 1.0.0.0\nCopyright © BEHOLDER Software 2013", "About...");
        }

        ///
        /// ToolBar
        ///
        private void ToolBar_Open_Click(Object sender, RoutedEventArgs e)
        {
            Open_Click(sender, e);
        }

        private void ToolBar_Save_Click(Object sender, RoutedEventArgs e)
        {
            Save_Click(sender, e);
        }
    }
}
