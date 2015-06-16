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

using System.ComponentModel;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Basic : Page, INotifyPropertyChanged
    {
        private MainPage rootPage;

        public Scenario1_Basic()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // The SizeChanged event is raised when the user interaction mode changes.
            Window.Current.SizeChanged += OnWindowResize;
            UpdateContent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            Window.Current.SizeChanged -= OnWindowResize;
        }

        void OnWindowResize(object sender, WindowSizeChangedEventArgs e)
        {
            UpdateContent();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        #region InteractionMode data binding
        private UserInteractionMode interactionMode;

        public UserInteractionMode InteractionMode
        {
            get { return interactionMode; }
            set
            {
                if (interactionMode != value)
                {
                    interactionMode = value;
                    if (PropertyChanged != null)
                    {
                        PropertyChanged(this, new PropertyChangedEventArgs("InteractionMode"));
                    }
                }
            }
        }
        #endregion

        #region CheckBoxStyle data binding
        private Style checkBoxStyle;

        public Style CheckBoxStyle
        {
            get { return checkBoxStyle; }
            set
            {
                if (checkBoxStyle != value)
                {
                    checkBoxStyle = value;
                    if (PropertyChanged != null)
                    {
                        PropertyChanged(this, new PropertyChangedEventArgs("CheckBoxStyle"));
                    }
                }
            }
        }
        #endregion

        void UpdateContent()
        {
            InteractionMode = UIViewSettings.GetForCurrentView().UserInteractionMode;

            // Update styles
            CheckBoxStyle = InteractionMode == UserInteractionMode.Mouse ? MouseCheckBoxStyle : TouchCheckBoxStyle;
        }
    }
}
