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
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using Windows.ApplicationModel.Core;
using Windows.Devices.Input;
using Windows.Foundation.Metadata;
using Windows.Gaming.Input;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class NavigationRootPage : Page
    {
        public static NavigationRootPage Current;
        public static Frame RootFrame = null;
        private PageHeader header;
        private bool allnewControlsPage = false;
        private bool isGamePadConnected;
        private bool isKeyboardConnected;

        private IList<ControlInfoDataGroup> _groups;
        public IList<ControlInfoDataGroup> Groups
        {
            get { return _groups; }
        }

        public string CurrentGroupTitle
        {
            get
            {
                return Groups.First().Title;
            }
        }

        public NavigationView NavigationView
        {
            get
            {
                return NavigationViewControl;
            }
        }


        public DeviceType DeviceFamily
        {
            get; set;

        }

        public bool IsFocusSupported
        {
            get
            {
                return DeviceFamily == DeviceType.Xbox || isGamePadConnected || isKeyboardConnected;
            }
        }

        public PageHeader PageHeader
        {
            get
            {
                return header ?? (header = UIHelper.GetDescendantsOfType<PageHeader>(NavigationViewControl).FirstOrDefault());
            }
        }

        public event EventHandler GroupsLoaded;

        private RootFrameNavigationHelper rootFrameNavigationHelper;

        private NavigationMenuItem AllControlsMenuItem;
        public NavigationMenuItem NewControlsMenuItem;

        public NavigationRootPage()
        {
            this.InitializeComponent();
            this.rootFrameNavigationHelper = new RootFrameNavigationHelper(rootFrame);
            LoadGroups();
            Current = this;
            RootFrame = rootFrame;

            this.KeyDown += NavigationRootPage_KeyDown;
            this.SizeChanged += NavigationRootPage_SizeChanged;
            this.GotFocus += (object sender, RoutedEventArgs e) =>
            {
                // helpful for debugging focus problems w/ keyboard & gamepad
                FrameworkElement focus = FocusManager.GetFocusedElement() as FrameworkElement;
                if (focus != null)
                    Debug.WriteLine("got focus: " + focus.Name + " (" + focus.GetType().ToString() + ")");
            };
            this.Loaded += (sender, args) =>
            {
                NavigationViewControl.GetDescendantsOfType<Button>().Where(c => c.Name == "TogglePaneButton").First().VerticalAlignment = VerticalAlignment.Bottom;
                NavigationViewControl.GetDescendantsOfType<Grid>().Where(c => c.Name == "PaneContentGrid").First().RowDefinitions[0].Height = new GridLength(88);
                Grid g = NavigationViewControl.GetDescendantsOfType<Button>().Where(b => b.Name == "TogglePaneButton").First().Parent as Grid;
                g.Width = 80;
                g.Height = 80;
                ContentControl headerControl = NavigationViewControl.GetDescendantsOfType<ContentControl>().Where(c => c.Name == "HeaderContent").First();
                headerControl.HorizontalContentAlignment = HorizontalAlignment.Stretch;
                headerControl.Height = 70;
                headerControl.IsTabStop = false;
            };

            CoreApplicationViewTitleBar coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility = AppViewBackButtonVisibility.Collapsed;
            coreTitleBar.IsVisibleChanged += CoreTitleBar_IsVisibleChanged;
            Gamepad.GamepadAdded += Gamepad_GamepadAdded;
            Gamepad.GamepadRemoved += Gamepad_GamepadRemoved;
            isKeyboardConnected = Convert.ToBoolean(new KeyboardCapabilities().KeyboardPresent);
        }

        private void Gamepad_GamepadRemoved(object sender, Gamepad e)
        {
            isGamePadConnected = Gamepad.Gamepads.Any();
        }

        private void Gamepad_GamepadAdded(object sender, Gamepad e)
        {
            isGamePadConnected = Gamepad.Gamepads.Any();
        }

        private void NavigationRootPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            UpdateControlButtonForeground();
        }

        private void AddNavigationMenuItems()
        {
            foreach (var g in Groups)
            {
                var item = new NavigationMenuItem() { Text = g.Title, Tag = g.UniqueId, DataContext = g };
                AutomationProperties.SetName(item, g.Title);
                item.Invoked += NavigationMenuItem_Invoked;
                if (g.ImagePath.ToLowerInvariant().EndsWith(".png"))
                {
                    item.Icon = new BitmapIcon() { UriSource = new Uri(g.ImagePath, UriKind.RelativeOrAbsolute) };
                }
                else
                {
                    item.Icon = new FontIcon()
                    {
                        FontFamily = new Windows.UI.Xaml.Media.FontFamily("Segoe MDL2 Assets"),
                        Glyph = g.ImagePath
                    };
                }
                NavigationViewControl.MenuItems.Add(item);
            }
            this.AllControlsMenuItem = (NavigationMenuItem)NavigationViewControl.MenuItems[0];
            AllControlsMenuItem.Loaded += AllControlsMenuItem_Loaded;

            this.NewControlsMenuItem = (NavigationMenuItem)NavigationViewControl.MenuItems[1];
        }

        private void NavigationRootPage_KeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (!e.Handled && e.Key == Windows.System.VirtualKey.E)
            {
                AutoSuggestBox box = this.GetDescendantsOfType<PageHeader>().First().controlsSearchBox;
                var f = box.Focus(FocusState.Programmatic);
            }
        }

        private void CoreTitleBar_IsVisibleChanged(CoreApplicationViewTitleBar sender, object args)
        {
            //desktop
            CoreApplication.GetCurrentView().TitleBar.ExtendViewIntoTitleBar = true;
            ApplicationViewTitleBar titleBar = ApplicationView.GetForCurrentView().TitleBar;
            titleBar.ButtonBackgroundColor = Colors.Transparent;
            titleBar.ButtonInactiveBackgroundColor = Colors.Transparent;
            BackButtonGrid.Visibility = Visibility.Visible;
            DeviceFamily = DeviceType.Desktop;
            XboxContentSafeRect.Visibility = Visibility.Collapsed;
            NavigationViewControl.GetDescendantsOfType<NavigationMenuItem>().Where(c => c.Name == "SettingsNavPaneItem").First().Margin = new Thickness(0);

        }

        void updateTitleBar()
        {
            //phone
            if (ApiInformation.IsTypePresent("Windows.UI.ViewManagement.StatusBar"))
            {
                DeviceFamily = DeviceType.Mobile;
                BackButtonGrid.Visibility = Visibility.Collapsed;
                XboxContentSafeRect.Visibility = Visibility.Collapsed;
                NavigationViewControl.GetDescendantsOfType<NavigationMenuItem>().Where(c => c.Name == "SettingsNavPaneItem").First().Margin = new Thickness(0);

            }
            //xbox
            else
            {
                DeviceFamily = DeviceType.Xbox;
                BackButtonGrid.Visibility = Visibility.Collapsed;
                XboxContentSafeRect.Visibility = Visibility.Visible;
                NavigationViewControl.GetDescendantsOfType<NavigationMenuItem>().Where(c => c.Name == "SettingsNavPaneItem").First().Margin = new Thickness(0, 0, 0, 40);

            }
        }

        private async void LoadGroups()
        {
            _groups = (await ControlInfoDataSource.GetGroupsAsync()).ToList();

            AddNavigationMenuItems(); // Currently due to a Windows bug this can only be done in code not markup

            if (NavigationRootPage.Current != null)
            {
                var menuItem = (NavigationMenuItem)NavigationRootPage.Current.NavigationView.MenuItems.FirstOrDefault();
                if (menuItem != null)
                    menuItem.IsSelected = true;
            }

            if (GroupsLoaded != null)
                GroupsLoaded(this, new EventArgs());
        }

        private void NavigationView_SettingsInvoked(NavigationView sender, object args)
        {
            rootFrame.Navigate(typeof(SettingsPage));
        }

        private void NavigationMenuItem_Invoked(NavigationMenuItem sender, object args)
        {
            if (sender == AllControlsMenuItem)
            {
                rootFrame.Navigate(typeof(MainPage));
            }
            else if (sender == NewControlsMenuItem)
            {
                rootFrame.Navigate(typeof(NewControlsPage));
            }
            else
            {
                var itemId = ((ControlInfoDataGroup)sender.DataContext).UniqueId;
                rootFrame.Navigate(typeof(SectionPage), itemId);
            }
        }
        private void rootFrame_Navigated(object sender, Windows.UI.Xaml.Navigation.NavigationEventArgs e)
        {
            SetCustomBackButtonVisibility();
            if (e.SourcePageType == typeof(MainPage) || e.SourcePageType == typeof(NewControlsPage))
            {
                allnewControlsPage = true;
                UpdateControlButtonForeground();

            }
            else
            {
                allnewControlsPage = false;
                if (App.Current.RequestedTheme != ApplicationTheme.Dark)
                {
                    ApplicationView.GetForCurrentView().TitleBar.ButtonForegroundColor = rootFrame.CanGoBack ? Colors.Black : Colors.White;
                }
                else
                {
                    ApplicationView.GetForCurrentView().TitleBar.ButtonForegroundColor = rootFrame.CanGoBack ? Colors.White : Colors.Black;
                }
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (rootFrame != null && rootFrame.CanGoBack)
            {
                rootFrame.GoBack();
            }
        }

        private void SetCustomBackButtonVisibility()
        {
            if (rootFrame != null)
            {
                CustomBackButton.Visibility = rootFrame.CanGoBack ? Visibility.Visible : Visibility.Collapsed;
            }
        }

        private void AllControlsMenuItem_Loaded(object sender, RoutedEventArgs e)
        {
            if (IsFocusSupported)
            {
                AllControlsMenuItem.Focus(FocusState.Keyboard);
            }
        }

        private void UpdateControlButtonForeground()
        {
            if (App.Current.RequestedTheme != ApplicationTheme.Dark)
            {
                if (allnewControlsPage)
                {
                    ApplicationView.GetForCurrentView().TitleBar.ButtonForegroundColor = Window.Current.Bounds.Width > 640 ? Colors.White : Colors.Black;
                }
                else
                {
                    ApplicationView.GetForCurrentView().TitleBar.ButtonForegroundColor = Colors.Black;
                }
            }

            else
            {
                ApplicationView.GetForCurrentView().TitleBar.ButtonForegroundColor = Colors.White;
            }
        }
        private void NavigationViewControl_Loaded(object sender, RoutedEventArgs e)
        {
            updateTitleBar();
        }

        private void BackButtonGrid_Loaded(object sender, RoutedEventArgs e)
        {
            CoreApplicationViewTitleBar coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            Window.Current.SetTitleBar(CustomTitleGrid);
        }

        public bool CheckNewControlSelected()
        {
            return NewControlsMenuItem.IsSelected;
        }
    }

    public enum DeviceType
    {
        Desktop,
        Mobile,
        Xbox
    }
}
