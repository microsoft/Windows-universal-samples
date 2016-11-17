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
using System.Linq;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;

        public Scenario1()
        {
            this.InitializeComponent();
            var task = ContactSampleDataSource.CreateContactSampleDataAsync();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }
        
        /// <summary>
         /// This event gets fired anytime the text in the TextBox gets updated.
         /// It is recommended to check the reason for the text changing by checking against args.Reason
         /// </summary>
         /// <param name="sender">The AutoSuggestBox whose text got changed.</param>
         /// <param name="args">The event arguments.</param>
        private void asb_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            // We only want to get results when it was a user typing, 
            // otherwise we assume the value got filled in by TextMemberPath 
            // or the handler for SuggestionChosen
            if (args.Reason == AutoSuggestionBoxTextChangeReason.UserInput)
            {
                var matchingContacts = ContactSampleDataSource.GetMatchingContacts(sender.Text);

                sender.ItemsSource = matchingContacts.ToList();
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
        private void asb_QuerySubmitted(AutoSuggestBox sender, AutoSuggestBoxQuerySubmittedEventArgs args)
        {
            if (args.ChosenSuggestion != null)
            {
                // User selected an item, take an action on it here
                SelectContact((Contact)args.ChosenSuggestion);
            }
            else
            {
                // Do a fuzzy search on the query text
                var matchingContacts = ContactSampleDataSource.GetMatchingContacts(args.QueryText);

                // Choose the first match, or clear the selection if there are no matches.
                SelectContact(matchingContacts.FirstOrDefault());
            }
        }

        /// <summary>
        /// This event gets fired as the user keys through the list, or taps on a suggestion.
        /// This allows you to change the text in the TextBox to reflect the item in the list.
        /// Alternatively you can use TextMemberPath.
        /// </summary>
        /// <param name="sender">The AutoSuggestBox that fired the event.</param>
        /// <param name="args">The args contain SelectedItem, which contains the data item of the item that is currently highlighted.</param>
        private void asb_SuggestionChosen(AutoSuggestBox sender, AutoSuggestBoxSuggestionChosenEventArgs args)
        {
            var contact = (Contact)args.SelectedItem;

            sender.Text = contact.DisplayName;
        }

        /// <summary>
        /// Display details of the specified contact.
        /// </summary>
        /// <param name="contact"></param>
        private void SelectContact(Contact contact)
        {
            if (contact != null)
            {
                NoResults.Visibility = Visibility.Collapsed;
                ContactDetails.Visibility = Visibility.Visible;
                ContactName.Text = contact.FullName;
                ContactCompany.Text = contact.Company;
            }
            else
            {
                NoResults.Visibility = Visibility.Visible;
                ContactDetails.Visibility = Visibility.Collapsed;
            }
        }
    }
}
