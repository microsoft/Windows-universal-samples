//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using AppUIBasics.Common;
using AppUIBasics.Data;
using System;
using System.Diagnostics;
using System.Linq;
using Windows.Devices.Input;
using Windows.Gaming.Input;
using Windows.System.Profile;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace AppUIBasics
{
    public sealed partial class NavigationRootPage : Page
    {
        public static NavigationRootPage Current;
        public static Frame RootFrame = null;

        private RootFrameNavigationHelper _navHelper;
        private PageHeader _header;
        private bool _isGamePadConnected;
        private bool _isKeyboardConnected;
        private NavigationViewItem _allControlsMenuItem;
        private NavigationViewItem _newControlsMenuItem;

        public NavigationView NavigationView
        {
            get { return NavigationViewControl; }
        }
        
        public DeviceType DeviceFamily { get; set; }

        public bool IsFocusSupported
        {
            get
            {
                return DeviceFamily == DeviceType.Xbox || _isGamePadConnected || _isKeyboardConnected;
            }
        }

        public PageHeader PageHeader
        {
            get
            {
                return _header ?? (_header = UIHelper.GetDescendantsOfType<PageHeader>(NavigationViewControl).FirstOrDefault());
            }
        }
                
        public NavigationRootPage()
        {
            this.InitializeComponent();

            _navHelper = new RootFrameNavigationHelper(rootFrame);

            SetDeviceFamily();
            AddNavigationMenuItems();
            Current = this;
            RootFrame = rootFrame;

            this.KeyDown += OnNavigationRootPageKeyDown;
            this.GotFocus += (object sender, RoutedEventArgs e) =>
            {
                // helpful for debugging focus problems w/ keyboard & gamepad
                if (FocusManager.GetFocusedElement() is FrameworkElement focus)
                {
                    Debug.WriteLine("got focus: " + focus.Name + " (" + focus.GetType().ToString() + ")");
                }
            };
            
            this.Loaded += (s, e) =>
            {
                // This is to work around a bug in the NavigationView header that hard-codes the header content to a 48 pixel height.
                var headerContentControl = NavigationViewControl.GetDescendantsOfType<ContentControl>().Where(c => c.Name == "HeaderContent").FirstOrDefault();

                if (headerContentControl != null)
                {
                    headerContentControl.Height = double.NaN;
                }
            };
            
            Gamepad.GamepadAdded += OnGamepadAdded;
            Gamepad.GamepadRemoved += OnGamepadRemoved;

            _isKeyboardConnected = Convert.ToBoolean(new KeyboardCapabilities().KeyboardPresent);
        }

        public bool CheckNewControlSelected()
        {
            return _newControlsMenuItem.IsSelected;
        }

        private void AddNavigationMenuItems()
        {
            foreach (var group in ControlInfoDataSource.Instance.Groups)
            {
                var item = new NavigationViewItem() { Content = group.Title, Tag = group.UniqueId, DataContext = group };
                AutomationProperties.SetName(item, group.Title);
                if (group.ImagePath.ToLowerInvariant().EndsWith(".png"))
                {
                    item.Icon = new BitmapIcon() { UriSource = new Uri(group.ImagePath, UriKind.RelativeOrAbsolute) };
                }
                else
                {
                    item.Icon = new FontIcon()
                    {
                        FontFamily = new FontFamily("Segoe MDL2 Assets"),
                        Glyph = group.ImagePath
                    };
                }
                NavigationViewControl.MenuItems.Add(item);
            }

            this._allControlsMenuItem = (NavigationViewItem)NavigationViewControl.MenuItems[0];

            _allControlsMenuItem.Loaded += OnAllControlsMenuItemLoaded;

            this._newControlsMenuItem = (NavigationViewItem)NavigationViewControl.MenuItems[1];
        }

        private void SetDeviceFamily()
        {
            var familyName = AnalyticsInfo.VersionInfo.DeviceFamily;

            if (!Enum.TryParse(familyName.Replace("Windows.", string.Empty), out DeviceType parsedDeviceType))
            {
                parsedDeviceType = DeviceType.Other;
            }

            DeviceFamily = parsedDeviceType;
        }

        private void OnAllControlsMenuItemLoaded(object sender, RoutedEventArgs e)
        {
            if (IsFocusSupported)
            {
                _allControlsMenuItem.Focus(FocusState.Keyboard);
            }
        }

        private void OnGamepadRemoved(object sender, Gamepad e)
        {
            _isGamePadConnected = Gamepad.Gamepads.Any();
        }

        private void OnGamepadAdded(object sender, Gamepad e)
        {
            _isGamePadConnected = Gamepad.Gamepads.Any();
        }
        
        private void OnNavigationRootPageKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (!e.Handled && e.Key == Windows.System.VirtualKey.E)
            {
                AutoSuggestBox box = this.GetDescendantsOfType<PageHeader>().First().controlsSearchBox;
                var f = box.Focus(FocusState.Programmatic);
            }
        }

        private void OnNavigationViewItemInvoked(NavigationView sender, NavigationViewItemInvokedEventArgs args)
        {
            if (args.IsSettingsInvoked)
            {
                if (NavigationViewControl.SelectedItem != NavigationViewControl.SettingsItem)
                {
                    rootFrame.Navigate(typeof(SettingsPage));
                }
            }
            else
            {
                var invokedItem = NavigationView.MenuItems.Cast<NavigationViewItem>().Single(i => i.Content == args.InvokedItem);
                
                if (invokedItem == _allControlsMenuItem)
                {
                    rootFrame.Navigate(typeof(AllControlsPage));
                }
                else if (invokedItem == _newControlsMenuItem)
                {
                    rootFrame.Navigate(typeof(NewControlsPage));
                }
                else
                {
                    var itemId = ((ControlInfoDataGroup)invokedItem.DataContext).UniqueId;
                    rootFrame.Navigate(typeof(SectionPage), itemId);
                }
            }
        }
        
        private void OnRootFrameNavigated(object sender, NavigationEventArgs e)
        {
            if (e.SourcePageType == typeof(AllControlsPage) ||
                e.SourcePageType == typeof(NewControlsPage))
            {
                NavigationViewControl.AlwaysShowHeader = false;
            }
            else
            {
                NavigationViewControl.AlwaysShowHeader = true;

                bool isFilteredPage = e.SourcePageType == typeof(SectionPage) || e.SourcePageType == typeof(SearchResultsPage);
                PageHeader?.UpdateBackground(isFilteredPage);
            }
        }
    }

    public enum DeviceType
    {
        Desktop,
        Mobile,
        Other,
        Xbox
    }
}