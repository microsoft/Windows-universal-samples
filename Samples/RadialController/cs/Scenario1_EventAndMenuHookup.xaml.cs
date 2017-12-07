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
        private List<RadialControllerMenuItem> menuItems = new List<RadialControllerMenuItem>();
        private List<Slider> sliders = new List<Slider>();
        private List<ToggleSwitch> toggles = new List<ToggleSwitch>();

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
            AddKnownIconItems();
            AddCustomIconItems();
            AddFontGlyphItems();

            // Set the invoked callbacks for each menu item
            for (int i = 0; i < menuItems.Count; ++i)
            {
                RadialControllerMenuItem radialControllerItem = menuItems[i];
                int index = i;

                radialControllerItem.Invoked += (sender, args) => { OnItemInvoked(index); };
            }
        }

        private void AddKnownIconItems()
        {
            menuItems.Add(RadialControllerMenuItem.CreateFromKnownIcon("Item0", RadialControllerMenuKnownIcon.InkColor));
            sliders.Add(slider0);
            toggles.Add(toggle0);

            menuItems.Add(RadialControllerMenuItem.CreateFromKnownIcon("Item1", RadialControllerMenuKnownIcon.NextPreviousTrack));
            sliders.Add(slider1);
            toggles.Add(toggle1);
        }

        private void AddCustomIconItems()
        {
            menuItems.Add(RadialControllerMenuItem.CreateFromIcon("Item2", RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/Item2.png"))));
            sliders.Add(slider2);
            toggles.Add(toggle2);

            menuItems.Add(RadialControllerMenuItem.CreateFromIcon("Item3", RandomAccessStreamReference.CreateFromUri(new Uri("ms-appx:///Assets/Item3.png"))));
            sliders.Add(slider3);
            toggles.Add(toggle3);
        }

        private void AddFontGlyphItems()
        {
            // Using system font
            menuItems.Add(RadialControllerMenuItem.CreateFromFontGlyph("Item4", "\x2764", "Segoe UI Emoji"));
            sliders.Add(slider4);
            toggles.Add(toggle4);

            // Using custom font
            menuItems.Add(RadialControllerMenuItem.CreateFromFontGlyph("Item5", "\ue102", "Symbols", new Uri("ms-appx:///Assets/Symbols.ttf")));
            sliders.Add(slider5);
            toggles.Add(toggle5);
        }

        private void OnItemInvoked(int selectedItemIndex)
        {
            if (activeItemIndex != selectedItemIndex)
            {
                activeItemIndex = selectedItemIndex;
                PrintSelectedItem();
            }
        }

        private void AddItem(object sender, RoutedEventArgs e)
        {
            RadialControllerMenuItem radialControllerMenuItem = GetRadialControllerMenuItemFromSender(sender);

            if (!Controller.Menu.Items.Contains(radialControllerMenuItem))
            {
                Controller.Menu.Items.Add(radialControllerMenuItem);
                log.Text += "\n Added : " + radialControllerMenuItem.DisplayText;
            }
        }

        private void RemoveItem(object sender, RoutedEventArgs e)
        {
            RadialControllerMenuItem radialControllerMenuItem = GetRadialControllerMenuItemFromSender(sender);

            if (Controller.Menu.Items.Contains(radialControllerMenuItem))
            {
                Controller.Menu.Items.Remove(radialControllerMenuItem);
                log.Text += "\n Removed : " + radialControllerMenuItem.DisplayText;
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
            Controller.Menu.TrySelectPreviouslySelectedMenuItem();
            PrintSelectedItem();
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
            RadialControllerMenuItem selectedMenuItem = Controller.Menu.GetSelectedMenuItem();
            string itemName = string.Empty;

            if (selectedMenuItem == menuItems[activeItemIndex])
            {
                itemName = selectedMenuItem.DisplayText;
            }
            else if (selectedMenuItem == null)
            {
                itemName = "System Item";
            }

            if (itemName != string.Empty)
            {
                log.Text += "\n Selected : " + itemName;
            }
        }

        private void OnLogSizeChanged(object sender, object e)
        {
            logViewer.ChangeView(null, logViewer.ExtentHeight, null);
        }

        private void Controller_ControlAcquired(RadialController sender, RadialControllerControlAcquiredEventArgs args)
        {
            log.Text += "\n Control Acquired";
            LogContactInfo(args.Contact);
        }

        private void Controller_ControlLost(RadialController sender, object args)
        {
            log.Text += "\n Control Lost";
        }

        private void Controller_ScreenContactStarted(RadialController sender, RadialControllerScreenContactStartedEventArgs args)
        {
            log.Text += "\n Contact Started ";
            LogContactInfo(args.Contact);
        }

        private void Controller_ScreenContactContinued(RadialController sender, RadialControllerScreenContactContinuedEventArgs args)
        {

            log.Text += "\n Contact Continued ";
            LogContactInfo(args.Contact);
        }

        private void Controller_ScreenContactEnded(RadialController sender, object args)
        {
            log.Text += "\n Contact Ended";
        }

        private void ToggleMenuSuppression(Object sender, RoutedEventArgs args)
        {
            RadialControllerConfiguration radialControllerConfig = RadialControllerConfiguration.GetForCurrentView();

            if (MenuSuppressionToggleSwitch.IsOn)
            {
                radialControllerConfig.ActiveControllerWhenMenuIsSuppressed = Controller;
            }

            radialControllerConfig.IsMenuSuppressed = MenuSuppressionToggleSwitch.IsOn;
        }

        private void Controller_ButtonClicked(RadialController sender, RadialControllerButtonClickedEventArgs args)
        {
            log.Text += "\n Button Clicked ";
            LogContactInfo(args.Contact);

            ToggleSwitch toggle = toggles[activeItemIndex];
            toggle.IsOn = !toggle.IsOn;
        }

        private void Controller_RotationChanged(RadialController sender, RadialControllerRotationChangedEventArgs args)
        {
            log.Text += "\n Rotation Changed Delta = " + args.RotationDeltaInDegrees;
            LogContactInfo(args.Contact);

            sliders[activeItemIndex].Value += args.RotationDeltaInDegrees;
        }

        private void LogContactInfo(RadialControllerScreenContact contact)
        {
            if (contact != null)
            {
                log.Text += "\n Bounds = " + contact.Bounds.ToString();
                log.Text += "\n Position = " + contact.Position.ToString();
            }
        }
    }
}
