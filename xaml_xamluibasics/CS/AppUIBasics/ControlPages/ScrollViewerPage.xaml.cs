//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScrollViewerPage : Page
    {
        public ScrollViewerPage()
        {
            this.InitializeComponent();
        }

        private void ZoomModeComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (Control1 != null && ZoomSlider != null)
            {
                ComboBox cb = sender as ComboBox;
                if (cb != null)
                {
                    switch (cb.SelectedIndex)
                    {
                        case 0: // Enabled
                            Control1.ZoomMode = ZoomMode.Enabled;
                            ZoomSlider.IsEnabled = true;
                            break;
                        case 1: // Disabled
                            Control1.ZoomMode = ZoomMode.Disabled;
                            Control1.ChangeView(null, null, (float)1.0);
                            ZoomSlider.Value = 1;
                            ZoomSlider.IsEnabled = false;
                            break;
                        default: // Disabled
                            Control1.ZoomMode = ZoomMode.Disabled;
                            Control1.ChangeView(null, null, (float)1.0);
                            ZoomSlider.Value = 1;
                            ZoomSlider.IsEnabled = false;
                            break;
                    }
                }
            }
        }

        private void ZoomSlider_ValueChanged(object sender, RangeBaseValueChangedEventArgs e)
        {
            if (Control1 != null)
            {
                Control1.ChangeView(null, null, (float)e.NewValue);
            }
        }

        private void hsmCombo_SelectionChanged_1(object sender, SelectionChangedEventArgs e)
        {
            if (Control1 != null)
            {
                ComboBox cb = sender as ComboBox;
                if (cb != null)
                {
                    switch (cb.SelectedIndex)
                    {
                        case 0: // Auto
                            Control1.HorizontalScrollMode = ScrollMode.Auto;
                            break;
                        case 1: //Enabled
                            Control1.HorizontalScrollMode = ScrollMode.Enabled;
                            break;
                        case 2: // Disabled
                            Control1.HorizontalScrollMode = ScrollMode.Disabled;
                            break;
                        default:
                            Control1.HorizontalScrollMode = ScrollMode.Enabled;
                            break;
                    }
                }
            }
        }

        private void hsbvCombo_SelectionChanged_1(object sender, SelectionChangedEventArgs e)
        {
            if (Control1 != null)
            {
                ComboBox cb = sender as ComboBox;
                if (cb != null)
                {
                    switch (cb.SelectedIndex)
                    {
                        case 0: // Auto
                            Control1.HorizontalScrollBarVisibility = ScrollBarVisibility.Auto;
                            break;
                        case 1: //Visible
                            Control1.HorizontalScrollBarVisibility = ScrollBarVisibility.Visible;
                            break;
                        case 2: // Hidden
                            Control1.HorizontalScrollBarVisibility = ScrollBarVisibility.Hidden;
                            break;
                        case 3: // Disabled
                            Control1.HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled;
                            break;
                        default:
                            Control1.HorizontalScrollBarVisibility = ScrollBarVisibility.Disabled;
                            break;
                    }
                }
            }
        }

        private void vsmCombo_SelectionChanged_1(object sender, SelectionChangedEventArgs e)
        {
            if (Control1 != null)
            {
                ComboBox cb = sender as ComboBox;
                if (cb != null)
                {
                    switch (cb.SelectedIndex)
                    {
                        case 0: // Auto
                            Control1.VerticalScrollMode = ScrollMode.Auto;
                            break;
                        case 1: //Enabled
                            Control1.VerticalScrollMode = ScrollMode.Enabled;
                            break;
                        case 2: // Disabled
                            Control1.VerticalScrollMode = ScrollMode.Disabled;
                            break;
                        default:
                            Control1.VerticalScrollMode = ScrollMode.Enabled;
                            break;
                    }
                }
            }
        }

        private void vsbvCombo_SelectionChanged_1(object sender, SelectionChangedEventArgs e)
        {
            if (Control1 != null)
            {
                ComboBox cb = sender as ComboBox;
                if (cb != null)
                {
                    switch (cb.SelectedIndex)
                    {
                        case 0: // Auto
                            Control1.VerticalScrollBarVisibility = ScrollBarVisibility.Auto;
                            break;
                        case 1: //Visible
                            Control1.VerticalScrollBarVisibility = ScrollBarVisibility.Visible;
                            break;
                        case 2: // Hidden
                            Control1.VerticalScrollBarVisibility = ScrollBarVisibility.Hidden;
                            break;
                        case 3: // Disabled
                            Control1.VerticalScrollBarVisibility = ScrollBarVisibility.Disabled;
                            break;
                        default:
                            Control1.VerticalScrollBarVisibility = ScrollBarVisibility.Visible;
                            break;
                    }
                }
            }
        }

        private void Page_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (Grid.GetColumnSpan(Control1) == 1)
            {
                Control1.Width = (e.NewSize.Width / 2) - 50;
            }
            else
            {
                Control1.Width = e.NewSize.Width - 50;
            }

        }

        private void Control1_ViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
        {
            ZoomSlider.Value = Control1.ZoomFactor;
        }

        private void Control1_ManipulationCompleted(object sender, ManipulationCompletedRoutedEventArgs e)
        {
            ZoomSlider.Value = Control1.ZoomFactor;
        }
    }
}
