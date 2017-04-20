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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Input;
using Windows.Storage.Streams;
using System;
using System.Linq;
using System.Collections.Generic;
using System.Windows.Input;

namespace SDKTemplate
{
    public sealed partial class Scenario1_EventAndMenuHookup : Page
    {
        private MainPage rootPage;
        private RadialController Controller;

        private int activeItemIndex;
        private List<RadialControllerMenuItem> menuItems;
        private List<Slider> sliders;
        private List<ToggleSwitch> toggles;

        public Scenario1_EventAndMenuHookup()
        {
            this.InitializeComponent();

            InitializeController();
            CreateMenuItems();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (Controller != null)
            {
                Controller.Menu.Items.Clear();
            }
        }

        private void InitializeController()
        {
            Controller = RadialController.CreateForCurrentView();
            Controller.RotationResolutionInDegrees = 1;

            // Wire events
            Controller.RotationChanged += Controller_RotationChanged;
            Controller.ButtonClicked += Controller_ButtonClicked;
            Controller.ScreenContactStarted += Controller_ScreenContactStarted;
            Controller.ScreenContactContinued += Controller_ScreenContactContinued;
            Controller.ScreenContactEnded += Controller_ScreenContactEnded;
            Controller.ControlAcquired += Controller_ControlAcquired;
            Controller.ControlLost += Controller_ControlLost;
        }

        private void CreateMenuItems()
        {
            menuItems = new List<RadialControllerMenuItem>
            {
                RadialControllerMenuItem.CreateFromKnownIcon("Item0", RadialControllerMenuKnownIcon.InkColor),
                RadialControllerMenuItem.CreateFromKnownIcon("Item1", RadialControllerMenuKnownIcon.NextPreviousTrack),
                RadialControllerMenuItem.CreateFromKnownIcon("Item2", RadialControllerMenuKnownIcon.Volume),
                RadialControllerMenuItem.CreateFromIcon("Item3", RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/Item3.png"))),
                RadialControllerMenuItem.CreateFromIcon("Item4", RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/Item4.png"))),
                RadialControllerMenuItem.CreateFromIcon("Item5", RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/Item5.png")))
            };
            sliders = new List<Slider> { Slider0, Slider1, Slider2, Slider3, Slider4, Slider5 };
            toggles = new List<ToggleSwitch> { Toggle0, Toggle1, Toggle2, Toggle3, Toggle4, Toggle5 };

            for (int i = 0; i < menuItems.Count; ++i)
            {
                RadialControllerMenuItem radialControllerItem = menuItems[i];
                int index = i;

                radialControllerItem.Invoked += (sender, args) => { OnItemInvoked(index); };
            }
        }

        private void OnItemInvoked(int selectedItemIndex)
        {
            activeItemIndex = selectedItemIndex;
        }

        private void AddItem(object sender, RoutedEventArgs e)
        {
            RadialControllerMenuItem radialControllerMenuItem = GetRadialControllerMenuItemFromSender(sender);

            if (!Controller.Menu.Items.Contains(radialControllerMenuItem))
            {
                Controller.Menu.Items.Add(radialControllerMenuItem);
            }
        }

        private void RemoveItem(object sender, RoutedEventArgs e)
        {
            RadialControllerMenuItem radialControllerMenuItem = GetRadialControllerMenuItemFromSender(sender);

            if (Controller.Menu.Items.Contains(radialControllerMenuItem))
            {
                Controller.Menu.Items.Remove(radialControllerMenuItem);
            }
        }

        private void SelectItem(object sender, RoutedEventArgs e)
        {
            RadialControllerMenuItem radialControllerMenuItem = GetRadialControllerMenuItemFromSender(sender);

            if (Controller.Menu.Items.Contains(radialControllerMenuItem))
            {
                Controller.Menu.SelectMenuItem(radialControllerMenuItem);
                PrintSelectedItem();
            }
        }

        private void SelectPreviouslySelectedItem(object sender, RoutedEventArgs e)
        {
            if (Controller.Menu.TrySelectPreviouslySelectedMenuItem())
            {
                PrintSelectedItem();
            }
        }

        private RadialControllerMenuItem GetRadialControllerMenuItemFromSender(object sender)
        {
            Button button = sender as Button;
            int index = Convert.ToInt32(button.CommandParameter);

            return menuItems[index];
        }

        private void PrintSelectedItem(object sender, RoutedEventArgs e)
        {
            PrintSelectedItem();
        }

        private void PrintSelectedItem()
        {
            log.Text += "\n Selected : " + GetSelectedMenuItemName();
        }

        private string GetSelectedMenuItemName()
        {
            RadialControllerMenuItem selectedMenuItem = Controller.Menu.GetSelectedMenuItem();

            if (selectedMenuItem == menuItems[activeItemIndex])
            {
                return selectedMenuItem.DisplayText;
            }
            else
            {
                return "System Item";
            }
        }
        private void OnLogSizeChanged(object sender, object e)
        {
            logViewer.ChangeView(null, logViewer.ExtentHeight, null);
        }

        private void Controller_ControlAcquired(RadialController sender, RadialControllerControlAcquiredEventArgs args)
        {
            log.Text += "\nControl Acquired";
            LogContactInfo(args.Contact);
        }

        private void Controller_ControlLost(RadialController sender, object args)
        {
            log.Text += "\nControl Lost";
        }

        private void Controller_ScreenContactStarted(RadialController sender, RadialControllerScreenContactStartedEventArgs args)
        {
            log.Text += "\nContact Started ";
            LogContactInfo(args.Contact);
        }

        private void Controller_ScreenContactContinued(RadialController sender, RadialControllerScreenContactContinuedEventArgs args)
        {

            log.Text += "\nContact Continued ";
            LogContactInfo(args.Contact);
        }

        private void Controller_ScreenContactEnded(RadialController sender, object args)
        {
            log.Text += "\nContact Ended";
        }

        private void Controller_ButtonClicked(RadialController sender, RadialControllerButtonClickedEventArgs args)
        {
            log.Text += "\nButton Clicked ";
            LogContactInfo(args.Contact);

            ToggleSwitch toggle = toggles[activeItemIndex];
            toggle.IsOn = !toggle.IsOn;
        }

        private void Controller_RotationChanged(RadialController sender, RadialControllerRotationChangedEventArgs args)
        {
            log.Text += "\nRotation Changed Delta = " + args.RotationDeltaInDegrees;
            LogContactInfo(args.Contact);

            sliders[activeItemIndex].Value += args.RotationDeltaInDegrees;
        }

        private void LogContactInfo(RadialControllerScreenContact contact)
        {
            if (contact != null)
            {
                log.Text += "\nBounds = " + contact.Bounds.ToString();
                log.Text += "\nPosition = " + contact.Position.ToString();
            }
        }
    }
}
