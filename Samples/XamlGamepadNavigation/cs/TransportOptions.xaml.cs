using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SDKTemplate
{
    public sealed partial class TransportOptions : UserControl
    {
        // For demonstration purposes, this control can simulate an old-style
        // control that does not support gamepad navigation. This mode
        // is used in Scenario 3.
        bool IsGamepadSupported = true;

        public TransportOptions()
        {
            this.InitializeComponent();
        }

        // When the user engages the control, put initial focus on the first item.
        void OnFocusEngaged(Control sender, FocusEngagedEventArgs e)
        {
            if (IsGamepadSupported)
            {
                InitialButton.Focus(FocusState.Programmatic);
            }
        }

        void OnLoaded(object sender, RoutedEventArgs e)
        {
            IsGamepadSupported = (Tag as string) != "Legacy";

            // In "legacy mode", remove the tab stops from all child controls.
            if (!IsGamepadSupported)
            {
                RemoveTabStops(this);
            }
        }

        void RemoveTabStops(DependencyObject root)
        {
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(root); i++)
            {
                DependencyObject child = VisualTreeHelper.GetChild(root, i);
                Control control = child as Control;
                if (control != null)
                {
                    control.IsTabStop = false;
                }
                RemoveTabStops(child);
            }
        }
    }
}
