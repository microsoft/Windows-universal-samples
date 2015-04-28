using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
using AppUIBasics.Data;
using System.Threading.Tasks;
using Windows.UI.Xaml.Media.Imaging;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AutoSuggestBoxPage : Page
    {
        public ObservableCollection<string> Suggestions { get; private set; }

        public AutoSuggestBoxPage()
        {
            this.Suggestions = new ObservableCollection<string>();

            this.InitializeComponent();
        }

        private void AutoSuggestBox_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            if (args.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
            {
                Suggestions.Clear();
                Suggestions.Add(sender.Text + "1");
                Suggestions.Add(sender.Text + "2");
            }
            Control1.ItemsSource = Suggestions;
        }
        private void AutoSuggestBox_SuggestionChosen(AutoSuggestBox sender, AutoSuggestBoxSuggestionChosenEventArgs args)
        {
            SuggestionOutput.Text = args.SelectedItem.ToString();
        }

        /// <summary>
        /// This event gets fired anytime the text in the TextBox gets updated.
        /// It is recommended to check the reason for the text changing by checking against args.Reason
        /// </summary>
        /// <param name="sender">The AutoSuggestBox whose text got changed.</param>
        /// <param name="args">The event arguments.</param>
        private async void Control2_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            //We only want to get results when it was a user typing, 
            //otherwise we assume the value got filled in by TextMemberPath 
            //or the handler for SuggestionChosen
            if (args.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
            {
                var suggestions = await SearchControls(sender.Text);

                if (suggestions.Count > 0)
                    sender.ItemsSource = suggestions;
                else
                    sender.ItemsSource = new string[] { "No results found" };
            }
        }

        /// <summary>
        /// This event gets fired when:
        ///     * a user presses Enter while focus is in the TextBox
        ///     * a user clicks or tabs to and invokes the query button (defined using the QueryIcon API)
        ///     * a user presses selects (clicks/taps/presses Enter) a suggestion
        /// </summary>
        /// <param name="sender">The AutoSuggestBox that fired the event.</param>
        /// <param name="args">The args contain the QueryText, which is the text in the TextBox, 
        /// and also ChosenSuggestion, which is only non-null when a user selects an item in the list.</param>
        private async void Control2_QuerySubmitted(AutoSuggestBox sender, AutoSuggestBoxQuerySubmittedEventArgs args)
        {
            if (args.ChosenSuggestion != null && args.ChosenSuggestion is ControlInfoDataItem)
            {
                //User selected an item, take an action
                SelectControl(args.ChosenSuggestion as ControlInfoDataItem);
            }
            else if (!string.IsNullOrEmpty(args.QueryText))
            {
                //Do a fuzzy search based on the text
                var suggestions = await SearchControls(sender.Text);
                if(suggestions.Count > 0)
                {
                    SelectControl(suggestions.FirstOrDefault());
                }
            }
        }

        /// <summary>
        /// This event gets fired as the user keys through the list, or taps on a suggestion.
        /// This allows you to change the text in the TextBox to reflect the item in the list.
        /// Alternatively you can use TextMemberPath.
        /// </summary>
        /// <param name="sender">The AutoSuggestBox that fired the event.</param>
        /// <param name="args">The args contain SelectedItem, which contains the data item of the item that is currently highlighted.</param>
        private void Control2_SuggestionChosen(AutoSuggestBox sender, AutoSuggestBoxSuggestionChosenEventArgs args)
        {
            var control = args.SelectedItem as ControlInfoDataItem;

            //Don't autocomplete the TextBox when we are showing "no results"
            if(control != null)
            {
                sender.Text = control.Title;
            }
        }

        /// <summary>
        /// This 
        /// </summary>
        /// <param name="contact"></param>
        private void SelectControl(ControlInfoDataItem control)
        {
            if (control != null)
            {
                ControlDetails.Visibility = Visibility.Visible;

                BitmapImage image = new BitmapImage(new Uri(control.ImagePath));
                ControlImage.Source = image;

                ControlTitle.Text = control.Title;
                ControlSubtitle.Text = control.Subtitle;
            }
        }

        private async Task<List<ControlInfoDataItem>> SearchControls(string query)
        {
            var groups = await AppUIBasics.Data.ControlInfoDataSource.GetGroupsAsync();
            var suggestions = new List<ControlInfoDataItem>();

            foreach (var group in groups)
            {
                var matchingItems = group.Items.Where(
                    item => item.Title.IndexOf(query, StringComparison.CurrentCultureIgnoreCase) >= 0);

                foreach (var item in matchingItems)
                {
                    suggestions.Add(item);
                }
            }
            return suggestions.OrderByDescending(i => i.Title.StartsWith(query, StringComparison.CurrentCultureIgnoreCase)).ThenBy(i => i.Title).ToList();
        }
    }
}
