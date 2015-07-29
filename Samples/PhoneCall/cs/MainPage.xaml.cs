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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using PhoneCall.Controls;
using PhoneCall.ViewModels;
using Windows.Graphics.Display;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace PhoneCall
{
    /// <summary>
    /// A page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        StatusViewModel statusVM;
        VoicemailViewModel voicemailVM;
        LinePickerViewModel linePickerVM;
        public MainPage()
        {
            this.InitializeComponent();

            //Initialize line picker VM as its common among the views
            linePickerVM = ViewModelDispatcher.LinePickerViewModel;

            //Register a resuming event for UI refresh
            Application.Current.Resuming += new EventHandler<Object>(Current_Resuming);
        }

        /// <summary>
        /// Ensures the page is refreshed on resume
        /// as content could have changed while away.
        /// </summary>
        private void Current_Resuming(object sender, object e)
        {
            int currentTabIndex = PhoneTabs.SelectedIndex;
            UpdateTabInfo(currentTabIndex);
        }

        /// <summary>
        /// Lazy loading of pivot tabs for perf.
        /// only load a tab when a user access it
        /// Naturally all pivots are loaded when the control is created (we dont want this)
        /// </summary>
        private void UpdateTabInfoOnPivotLoading(Pivot sender, PivotItemEventArgs args)
        {
            int currentTabIndex = PhoneTabs.SelectedIndex;
            //Set device oritentation based on tab
            switch (currentTabIndex)
            {
                //For the contact and dialer tabs, we should disable landscape
                case 1:
                case 2:
                    DisplayInformation.AutoRotationPreferences = DisplayOrientations.Portrait;
                    break;
                //Other tabs can be both portrait and landscape
                default:
                    DisplayInformation.AutoRotationPreferences =
                        DisplayOrientations.Portrait | DisplayOrientations.Landscape |
                        DisplayOrientations.PortraitFlipped | DisplayOrientations.LandscapeFlipped;
                    break;
            }

            //Update content as its already loaded
            if (args.Item.Content != null)
            {
                //Content loaded already, perform an update instead and exit
                UpdateTabInfo(currentTabIndex);
                return;
            }

            //Lazy load the tabs. Only load when accessed
            UserControl CurrentPanel;
            switch (currentTabIndex)
            {
                case 0:
                    statusVM = ViewModelDispatcher.StatusViewModel;
                    CurrentPanel = new StatusPanel();
                    CurrentPanel.Height = Double.NaN; //Auto height
                    args.Item.Content = CurrentPanel;
                    break;
                case 1:
                    CurrentPanel = new ContactsPanel();
                    CurrentPanel.Height = Double.NaN; //Auto height
                    args.Item.Content = CurrentPanel;
                    break;
                case 2:
                    CurrentPanel = new DialerPanel();
                    CurrentPanel.Height = Double.NaN; //Auto height
                    args.Item.Content = CurrentPanel;
                    break;
                case 3:
                    voicemailVM = ViewModelDispatcher.VoicemailViewModel;
                    CurrentPanel = new VoicemailPanel();
                    CurrentPanel.Height = Double.NaN; //Auto height
                    args.Item.Content = CurrentPanel;
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// Updating already loaded tabs when a user navigates to the tab.
        /// Ensure content is always up to date
        /// </summary>
        private void UpdateTabInfo(int TabIndex)
        {
            //Begin view updates
            //Dont expect the contacts pivot and dialer to require constant updates
            linePickerVM.UpdateLinePickerInfo();
            switch (TabIndex)
            {
                case 0:
                    statusVM.UpdateStatusInfo();
                    break;
                case 3:
                    voicemailVM.UpdateVoicemailInfo();
                    break;
                default:
                    break;
            }
        }
    }
}
