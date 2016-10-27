using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Input;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    public sealed partial class Scenario2_SystemDefaultMenu : Page
    {
        RadialControllerConfiguration config;
        RadialController controller;
        RadialControllerMenuItem customItem;

        public Scenario2_SystemDefaultMenu()
        {
            this.InitializeComponent();

            controller = RadialController.CreateForCurrentView();
            controller.RotationChanged += Controller_RotationChanged;

            customItem = RadialControllerMenuItem.CreateFromKnownIcon("Item1", RadialControllerMenuKnownIcon.InkColor);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            controller.Menu.Items.Clear();
        }

        private void Controller_RotationChanged(RadialController sender, RadialControllerRotationChangedEventArgs args)
        {
            log.Text += "\nRotation Changed Delta = " + args.RotationDeltaInDegrees;
            if (args.Contact != null)
            {
                log.Text += "\nBounds = " + args.Contact.Bounds.ToString();
                log.Text += "\nPosition = " + args.Contact.Position.ToString();
            }

            Slider1.Value += args.RotationDeltaInDegrees;
        }

        private void ModifySystemDefaults(object sender, RoutedEventArgs e)
        {
            config = RadialControllerConfiguration.GetForCurrentView();
            config.SetDefaultMenuItems(new[] { RadialControllerSystemMenuItemKind.Volume, RadialControllerSystemMenuItemKind.Scroll });
        }

        private void Select_Volume(object sender, RoutedEventArgs e)
        {
            config = RadialControllerConfiguration.GetForCurrentView();
            config.TrySelectDefaultMenuItem(RadialControllerSystemMenuItemKind.Volume);
        }

        private void SelectPreviouslySelectedItem(object sender, RoutedEventArgs e)
        {
            if (controller.Menu.TrySelectPreviouslySelectedMenuItem())
            {
                PrintSelectedItem();
            }
        }

        private void OnLogSizeChanged(object sender, object e)
        {
            logViewer.ChangeView(null, logViewer.ExtentHeight, null);
        }

        private void PrintSelectedItem()
        {
            RadialControllerMenuItem selectedMenuItem = controller.Menu.GetSelectedMenuItem();

            if (selectedMenuItem == customItem)
            {
                log.Text += "\n Selected : " + selectedMenuItem.DisplayText;
            }
            else
            {
                log.Text += "\n Selected System Item";
            }
        }

        private void Reset_ToDefault(object sender, RoutedEventArgs e)
        {
            config = RadialControllerConfiguration.GetForCurrentView();
            config.ResetToDefaultMenuItems();
        }

        private void AddCustomItem(object sender, RoutedEventArgs e)
        {
            if (!controller.Menu.Items.Contains(customItem))
            {
                controller.Menu.Items.Add(customItem);
            }
        }

        private void RemoveCustomItem(object sender, RoutedEventArgs e)
        {
            if (controller.Menu.Items.Contains(customItem))
            {
                controller.Menu.Items.Remove(customItem);
            }
        }

        private void SelectCustomItem(object sender, RoutedEventArgs e)
        {
            if (controller.Menu.Items.Contains(customItem))
            {
                controller.Menu.SelectMenuItem(customItem);
            }
        }

        private void Remove_Defaults(object sender, RoutedEventArgs e)
        {
            config = RadialControllerConfiguration.GetForCurrentView();
            config.SetDefaultMenuItems(new RadialControllerSystemMenuItemKind[] { });
        }
    }
}
