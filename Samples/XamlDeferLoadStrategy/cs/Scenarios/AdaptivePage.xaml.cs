using System;
using System.Collections.ObjectModel;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace xDeferLoadStrategy
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AdaptivePage : Page
    {
        public ObservableCollection<Account> Accounts { get; set; }
        public ObservableCollection<Email> Emails { get; set; }

        public AdaptivePage()
        {
            this.InitializeComponent();

            Accounts = new ObservableCollection<Account>()
            {
                new Account() { AccountName = "test@outlook.com", IconUri = @"ms-appx:///Assets/Icons/outlook.png" },
                new Account() { AccountName = "test@gmail.com", IconUri = @"ms-appx:///Assets/Icons/gmail.png" },
                new Account() { AccountName = "test@yahoo.com", IconUri = @"ms-appx:///Assets/Icons/yahoo.png" },
                new Account() { AccountName = "test@mydomain.com", IconUri = @"ms-appx:///Assets/Icons/email.png" }
            };

            Emails = new ObservableCollection<Email>()
            {
                new Email() { From = "manager@company.com", Subject = "Weekend work", Body = "I'm going to need you to come in on Sunday too to cover for Bill.", ReceivedDate = DateTime.Now.Subtract(TimeSpan.FromMinutes(1)).ToString("g")},
                new Email() { From = "employee@company.com", Subject = "Project link", Body = "Hey, can I get a link to the project you told me about?", ReceivedDate = DateTime.Now.Subtract(TimeSpan.FromMinutes(2)).ToString("g")},
                new Email() { From = "spouse@home.com", Subject = "Groceries", Body = "Can you pick up some milk on your way home?", ReceivedDate = DateTime.Now.Subtract(TimeSpan.FromMinutes(5)).ToString("g")},
                new Email() { From = "coworker@company.com", Subject = "PTO", Body = "I heard you're heading to the Bahamas soon?  So jealous!", ReceivedDate = DateTime.Now.Subtract(TimeSpan.FromMinutes(15)).ToString("g")},
                new Email() { From = "hr@company.com", Subject = "PTO approved", Body = "Your PTO request has been approved.  Please remember to log your time.", ReceivedDate = DateTime.Now.Subtract(TimeSpan.FromMinutes(30)).ToString("g")}
            };
            MailList.ItemsSource = Emails;

            ApplicationView.GetForCurrentView().VisibleBoundsChanged += AdaptivePage_VisibleBoundsChanged;
        }

        private void AdaptivePage_VisibleBoundsChanged(ApplicationView sender, object args)
        {
            //Here we are hiding panes when we get to certain thresholds
            if (ApplicationView.GetForCurrentView().VisibleBounds.Width < 768 && ReadingPane != null)
                ReadingPane.Visibility = Visibility.Collapsed;

            if (ApplicationView.GetForCurrentView().VisibleBounds.Width < 1024 && AccountsList != null)
                AccountsList.Visibility = Visibility.Collapsed;
        }

        private void AccountsList_Loading(FrameworkElement sender, object args)
        {
            //We need to do this in Loading for AccountsList, since it's possible that it will be created later
            AccountsList.ItemsSource = Accounts;
        }
    }

    public class Account
    {
        public string AccountName { get; set; }
        public string IconUri { get; set; }
    }

    public class Email
    {
        public string From { get; set; }
        public string Subject { get; set; }
        public string Body { get; set; }
        public string ReceivedDate { get; set; }
    }
}
