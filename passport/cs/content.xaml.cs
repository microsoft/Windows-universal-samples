using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Security.Credentials;

namespace SDKTemplate
{
    public sealed partial class Content : Page
    {
        private MainPage rootPage;
        private Account activeAccount;

        public Content()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            activeAccount = (Account) e.Parameter;
        }

        private void Button_Restart_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(UserSelect));
        }

        private void Button_Forget_Click(object sender, RoutedEventArgs e)
        {
            PassportDelete();

            UserSelect.accountList.Remove(activeAccount);
            AccountsHelper.SaveAccountList(UserSelect.accountList);
        }

        /// <summary>
        /// Checks the KeyCredentialManager to see if there is a Passport for the current user and
        /// sends the informaton to the server to unregister it.
        /// 
        /// Then deletes the local key associated with the Passport.
        /// </summary>
        private async void PassportDelete()
        {
            KeyCredentialRetrievalResult keyOpenResult = await KeyCredentialManager.OpenAsync(activeAccount.Email);

            if (keyOpenResult.Status == KeyCredentialStatus.Success)
            {
                var userKey = keyOpenResult.Credential;
                var publicKey = userKey.RetrievePublicKey();

                // Send key information to server to unregister it
                DeletePassportServerSide();
            }

            await KeyCredentialManager.DeleteAsync(activeAccount.Email);

            rootPage.NotifyUser("User " + activeAccount.Email + " deleted.", NotifyType.StatusMessage);
            button_Forget.IsEnabled = false;
            button_Forget.Content = "User Forgotten.";
        }

        /// <summary>
        /// Function used to unregister user's Passport on the server side
        /// </summary>
        /// <returns></returns>
        private async Task<bool> DeletePassportServerSide()
        {
            return true;
        }
    }
}
