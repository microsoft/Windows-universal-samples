//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.System;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Documents;

namespace SDKTemplateCS
{
    partial class MainPage : Page
    {

        private Frame _scenariosFrame;

        public Frame ScenariosFrame
        {
            get { return _scenariosFrame; }
            set { _scenariosFrame = value; }
        }

        private Frame _inputFrame;

        public Frame InputFrame
        {
            get { return _inputFrame; }
            set { _inputFrame = value; }
        }

        private Frame _outputFrame;

        public Frame OutputFrame
        {
            get { return _outputFrame; }
            set { _outputFrame = value; }
        }

        private string _rootNamespace;

        public string RootNamespace
        {
            get { return _rootNamespace; }
            set { _rootNamespace = value; }
        }




        public event System.EventHandler InputFrameLoaded;
        public event System.EventHandler OutputFrameLoaded;


        public MainPage()
        {
            InitializeComponent();

            _scenariosFrame = ScenarioList;
            _inputFrame = ScenarioInput;
            _outputFrame = ScenarioOutput;

            SetFeatureName(FEATURE_NAME);

            Loaded += new RoutedEventHandler(MainPage_Loaded);
            Window.Current.SizeChanged += new WindowSizeChangedEventHandler(MainPage_SizeChanged);

            NavigationCacheMode = Windows.UI.Xaml.Navigation.NavigationCacheMode.Required;
        }

        void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Figure out what resolution and orientation we are in and respond appropriately
            CheckLayout();

            // Load the ScenarioList page into the proper frame
            ScenarioList.Navigate(Type.GetType(_rootNamespace + ".ScenarioList"), this);
        }




        void CheckLayout()
        {
            String visualState = Window.Current.Bounds.Width < 768 ? "Below768Layout" : "DefaultLayout";
            VisualStateManager.GoToState(this, visualState, false);
        }

        void MainPage_SizeChanged(Object sender, Windows.UI.Core.WindowSizeChangedEventArgs args)
        {
            CheckLayout();
        }


        private void SetFeatureName(string str)
        {
            FeatureName.Text = str;
        }

        async void Footer_Click(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri(((HyperlinkButton)sender).Tag.ToString()));
        }

        public void NotifyUser(string strMessage, NotifyType type)
        {
            switch (type)
            {
                case NotifyType.StatusMessage:
                    StatusBlock.Style = Resources["StatusStyle"] as Style;
                    break;
                case NotifyType.ErrorMessage:
                    StatusBlock.Style = Resources["ErrorStyle"] as Style;
                    break;
            }
            StatusBlock.Text = strMessage;
        }

        public void DoNavigation(Type inPageType, Frame inFrame, Type outPageType, Frame outFrame)
        {
            inFrame.Navigate(inPageType, this);
            outFrame.Navigate(outPageType, this);

            // Raise InputFrameLoaded so downstream pages know that the input frame content has been loaded.
            if (InputFrameLoaded != null)
            {
                InputFrameLoaded(this, new EventArgs());
            }
            // Raise OutputFrameLoaded so downstream pages know that the output frame content has been loaded.
            if (OutputFrameLoaded != null)
            {
                OutputFrameLoaded(this, new EventArgs());
            }
        }
    }

    public enum NotifyType
    {
        StatusMessage,
        ErrorMessage
    };
}
