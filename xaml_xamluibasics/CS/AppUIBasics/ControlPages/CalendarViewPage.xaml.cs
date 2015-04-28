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
using System.Reflection;
using Windows.Globalization;
using Windows.UI.Popups;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class CalendarViewPage : Page
    {
        public CalendarViewPage()
        {
            this.InitializeComponent();

            List<string> calendarIdentifiers = new List<string>();
            var cidmethods = typeof(CalendarIdentifiers).GetRuntimeMethods();
            foreach (var m in cidmethods)
            {
                if (m.Name.Substring(0, 4) == "get_")
                {
                    calendarIdentifiers.Add(m.Invoke(null, null).ToString());
                }
            }

            calendarIdentifier.ItemsSource = calendarIdentifiers;
            calendarIdentifier.SelectedItem = CalendarIdentifiers.Gregorian;
        }

        private void SelectionMode_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            CalendarViewSelectionMode selectionMode;
            if (Enum.TryParse<CalendarViewSelectionMode>((sender as ComboBox).SelectedItem.ToString(), out selectionMode))
            {
                Control1.SelectionMode = selectionMode;
            }
        }

        private async void setLanguage_Click(object sender, RoutedEventArgs e)
        {
            if(Windows.Globalization.Language.IsWellFormed(language.Text))
            {
                Control1.Language = language.Text;
            }
            else
            {
                await new MessageDialog("That language is not valid.  Please check the language and try again.").ShowAsync();
            }
        }
    }
}
